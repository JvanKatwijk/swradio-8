#
/*
 *    Copyright (C) 2015
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDR-J (JSDR).
 *
 *    SDR-J is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    SDR-J is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with SDR-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
//
#include	"drm-decoder.h"
#include	"msc-handler-qam64.h"
#include	"msc-streamer.h"
#include	"state-descriptor.h"
#include	<cmath>
#include	<stdio.h>
#include	"mapper.h"
#include	"basics.h"
#include	"prbs.h"
#include	"protlevels.h"

#include	"mer64-values.h"

//	In order to handle the A and B levels, we create a
//	separate processor ("streamer") for the A and B parts of the stream.
	QAM64_SM_Handler::QAM64_SM_Handler	(drmDecoder *m,
	                                         stateDescriptor *theState,
	                                         int8_t		qam64Roulette):
	                                          mscHandler (m, theState),
	                                          myDecoder () {
int16_t	N1, N2;
int16_t	RYlcm, i;
float	denom	= 0;
int32_t	highProtected, lowProtected;
//
	this	-> theState		= theState;
	this	-> qam64Roulette	= qam64Roulette;
	lengthA				= 0;
	for (i = 0; i < theState -> numofStreams; i ++)
	   lengthA += theState	-> streams [i]. lengthHigh;
	lengthB		= 0;
	for (i = 0; i < theState -> numofStreams; i ++)
	   lengthB += theState	-> streams [i]. lengthLow;

	fprintf (stderr, "LengthA = %d, lengthB = %d\n", lengthA, lengthB);
	if (lengthA != 0) {	// two real levels
	   RYlcm = getRYlcm_64 (theState -> protLevelA);
//	   RYlcm = theState -> getRYlcm_64 (theState -> protLevelA);
	   for (i = 0; i < 3; i ++)
	      denom += getRp_qam64 (theState -> protLevelA, i);
//	      denom += theState -> getRp_qam64 (theState -> protLevelA, i);
//	      denom += theState -> getRp (theState -> protLevelA, i);
	   denom *= 2 * RYlcm;
	   N1	= int16_t (ceil ((8.0 * lengthA) / denom)) * RYlcm;
	   N2	= theState	-> muxSize - N1;
	   Y13mapper_high	= new Mapper (2 * N1, 13);
	   Y21mapper_high	= new Mapper (2 * N1, 21);
	   Y13mapper_low	= new Mapper (2 * N2, 13);
	   Y21mapper_low	= new Mapper (2 * N2, 21);
	}
	else {
	   N1	= 0;
	   N2	= theState	-> muxSize;
	   Y13mapper_high	= NULL;
	   Y21mapper_high	= NULL;
	   Y13mapper_low	= new Mapper (2 * N2, 13);
	   Y21mapper_low	= new Mapper (2 * N2, 21);
	}
//
//	Note that N2 is (re)computed in the streamer
	stream_0	= new MSC_streamer (theState, 0, N1, NULL,  NULL);
	stream_1	= new MSC_streamer (theState, 1, N1,
	                              Y13mapper_high, Y13mapper_low);
	stream_2	= new MSC_streamer (theState, 2, N1,
	                              Y21mapper_high, Y21mapper_low);

	highProtected	= stream_0 -> highBits () +
	                  stream_1 -> highBits () +
	                  stream_2 -> highBits ();
	lowProtected	= stream_0 -> lowBits () +
	                  stream_1 -> lowBits () +
	                  stream_2 -> lowBits ();
	thePRBS		= new prbs (lowProtected + highProtected);
}

	QAM64_SM_Handler::~QAM64_SM_Handler	(void) {
	delete	stream_0;
	delete	stream_1;
	delete	stream_2;
	delete	Y13mapper_high;
	delete	Y13mapper_low;
	delete	Y21mapper_high;
	delete	Y21mapper_low;
}
//
//	we return the part of the Mux comprising the A and B parts
//	of the logical streams

metrics	QAM64_SM_Handler::makeMetrics (uint8_t v) {
metrics m;
	if (v) {
	   m. rTow0 = 1.0;
	   m. rTow1 = 0.0;
	}
	else {
	   m. rTow0 = 0.0;
	   m. rTow1 = 1.0;
	}
	return m;
}
	
void	QAM64_SM_Handler::process	(theSignal *v, uint8_t *o) {
int16_t	highProtectedbits	= stream_0 -> highBits () +
	                          stream_1 -> highBits () +
	                          stream_2 -> highBits ();
int16_t	lowProtectedbits	= stream_0 -> lowBits () +
	                          stream_1 -> lowBits () +
	                          stream_2 -> lowBits ();
uint8_t	bitsOut [highProtectedbits + lowProtectedbits];
uint8_t	bits_0  [stream_0 -> highBits () + stream_0 -> lowBits ()];
//uint8_t	bits_0a  [stream_0 -> highBits () + stream_0 -> lowBits ()];
uint8_t	bits_1  [stream_1 -> highBits () + stream_1 -> lowBits ()];
uint8_t	bits_2  [stream_2 -> highBits () + stream_2 -> lowBits ()];
metrics Y0 [2 * theState -> muxSize];
metrics Y1 [2 * theState -> muxSize];
metrics Y2 [2 * theState -> muxSize];
int32_t	i;
//
uint8_t	level_0 [2 * theState -> muxSize];
uint8_t	level_1 [2 * theState -> muxSize];
uint8_t	level_2 [2 * theState -> muxSize];

mer64_compute computeMER;
float mer = 10 * log10 (computeMER. computemer (v, theState -> muxSize));
        show_mer (mer);
	
	for (i = 0; i < qam64Roulette; i ++) {
	   myDecoder. computemetrics (v, theState -> muxSize,
	                                   0, Y0,
	                                   i > 0,
	                                   level_0, level_1, level_2);
	   stream_0	-> process (Y0, bits_0, level_0);
	   myDecoder. computemetrics (v, theState -> muxSize,
	                                   1, Y1, 
	                                   i > 0,
	                                   level_0, level_1, level_2);
	   stream_1	-> process (Y1, bits_1, level_1);
	   myDecoder. computemetrics (v, theState -> muxSize,
	                                   2, Y2,
	                                   true,
	                                   level_0, level_1, level_2);	
	   stream_2	-> process (Y2, bits_2, level_2);
	}
//	now the big move
	memcpy (&bitsOut [0],
	        &bits_0 [0],
	        stream_0 -> highBits ());
	memcpy (&bitsOut [stream_0 -> highBits ()],
	        &bits_1 [0],
	        stream_1 -> highBits ());
	memcpy (&bitsOut [stream_0 -> highBits () +
	                  stream_1 -> highBits ()],
	        &bits_2 [0],
	        stream_2 -> highBits ());
	memcpy (&bitsOut [stream_0 -> highBits () +
	                  stream_1 -> highBits () +
	                  stream_2 -> highBits ()],
	        &bits_0 [stream_0 -> highBits ()],
	        stream_0 -> lowBits ());
	memcpy (&bitsOut [stream_0 -> highBits () +
	                  stream_1 -> highBits () +
	                  stream_2 -> highBits () +
	                  stream_0 -> lowBits ()],
	        &bits_1 [stream_1 -> highBits ()], stream_1 -> lowBits ());
	memcpy (&bitsOut [stream_0 -> highBits () +
	                  stream_1 -> highBits () +
	                  stream_2 -> highBits () +
	                  stream_0 -> lowBits ()  +
	                  stream_1 -> lowBits ()],
	        &bits_2 [stream_2 -> highBits ()], stream_2 -> lowBits ());
//
//	still one bit per uint
	thePRBS	-> doPRBS (&bitsOut [0]);
	memcpy (o, bitsOut, (lengthA + lengthB) * BITSPERBYTE);
}
//
