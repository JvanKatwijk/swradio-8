#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRunoPLugin_drm
 *
 *    SDRunoPlugin_drm is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    SDRunoPlugin_drm is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with SDRunoPLugin_drm; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
//
//	the real msc work is to be done by descendants of the mscHandler

#include	<vector>
//#include	<windows.h>
#include	"msc-handler-qam16.h"
#include	"msc-streamer.h"
#include	"state-descriptor.h"
#include	"mapper.h"
#include	"basics.h"
#include	"prbs.h"
#include	"protlevels.h"
#include	"mer16-values.h"
#include	"drm-decoder.h"
//
//	For each of the "levels" (qam16 => 2 levels), we create a
//	separate handler. From "samples to bitstreams" is done here
//	as is the bit-deinterleaving
	QAM16_SM_Handler::QAM16_SM_Handler	(stateDescriptor *theState,
	                                         drmDecoder *m_form):
	                                             mscHandler (m_form,
	                                                           theState),
	                                             myDecoder () {
int16_t	RYlcm, i;
float	denom;
//
//	apply the formula from page 112 (section 7.2.1.1) to determine
//	the number of QAM cells in the A part, then the number of QAM cells
//	in the lower protected part (the B part) follows

	this	-> theState	= theState;
	lengthA			= 0;

	for (i = 0; i < theState -> numofStreams; i ++)
	   lengthA += theState	-> streams [i]. lengthHigh;

	lengthB		= 0;
	for (i = 0; i < theState -> numofStreams; i ++)
	   lengthB += theState	-> streams [i]. lengthLow;

	if (lengthA != 0) {	// two real levels
//	apply formula from section 7.2.1. to compute the number
//	of MSC cells for the higher protected part given in bytes
//	   RYlcm	= theState -> getRYlcm_16 (theState -> protLevelA);
	   RYlcm	= getRYlcm_16 (theState -> protLevelA);
	   denom	= 0;
	   for (i = 0; i < 2; i ++)
//	      denom += theState -> getRp (theState -> protLevelA, i);
//	      denom += theState -> getRp_qam16 (theState -> protLevelA, i);
	      denom += getRp_qam16 (theState -> protLevelA, i);
	   denom	*= 2 * RYlcm;
	   N1		= int16_t (ceil (8.0 * lengthA / denom) * RYlcm);
//	   fprintf (stderr, "N1 = %d (lengthA = %d)\n", N1, lengthA);
	   N2			= theState -> muxSize - N1;
	   Y13mapper_high	= new Mapper (2 * N1, 13);
	   Y21mapper_high	= new Mapper (2 * N1, 21);
	   Y13mapper_low	= new Mapper (2 * N2, 13);
	   Y21mapper_low	= new Mapper (2 * N2, 21);
	}
	else {
	   N1			= 0;
	   N2			= theState -> muxSize;
	   Y13mapper_high	= nullptr;
	   Y21mapper_high	= nullptr;
	   Y13mapper_low	= new Mapper (2 * N2, 13);
	   Y21mapper_low	= new Mapper (2 * N2, 21);
	}

//	we need two streamers:
//	The N1 indicates the number of OFDM cells for the
//	higher protected bits, N2 follows directly

	stream_0	= new MSC_streamer (theState, 0, N1,
	                                    Y13mapper_high, Y13mapper_low);
	stream_1	= new MSC_streamer (theState, 1, N1,
	                                    Y21mapper_high, Y21mapper_low);
	thePRBS		= new prbs (stream_0 -> highBits () +
	                            stream_1 -> highBits () +
	                            stream_0 -> lowBits  () +
	                            stream_1 -> lowBits  ());
	connect (this, SIGNAL (show_msc_mer (float)),
	         m_form, SLOT (show_msc_mer (float)));
}

	QAM16_SM_Handler::~QAM16_SM_Handler	(void) {
	delete	stream_0;
	delete	stream_1;
	delete	Y13mapper_high;
	delete	Y21mapper_high;
	delete	Y13mapper_low;
	delete	Y21mapper_low;
	delete	thePRBS;
}

void	QAM16_SM_Handler::process	(theSignal *v, uint8_t *o) {
int16_t	highProtectedbits	= stream_0 -> highBits () +
	                          stream_1 -> highBits ();
int16_t	lowProtectedbits	= stream_0 -> lowBits () +
	                          stream_1 -> lowBits ();
std::vector<uint8_t>	bitsOut;
std::vector<uint8_t>	bits_0;
std::vector<uint8_t>	bits_1;

std::vector<metrics> Y0;
std::vector<metrics> Y1;
std::vector<uint8_t> level_0;
std::vector<uint8_t> level_1;


mer16_compute computeMER;
float mer = 10 * log10 (computeMER. computemer (v, theState -> muxSize));
        show_msc_mer  (mer);

	bitsOut. resize (highProtectedbits + lowProtectedbits);
	bits_0. resize (stream_0 -> highBits () + stream_0 -> lowBits ());
	bits_1. resize (stream_1 -> highBits () + stream_1 -> lowBits ());
	Y0. resize (2 * theState -> muxSize);
	Y1. resize (2 * theState -> muxSize);
	level_0. resize (2 * theState -> muxSize);
	level_1. resize (2 * theState -> muxSize);

	for (int i = 0; i < 4; i ++) {
//	First the "normal" decoding. leading to two bit rows
	   myDecoder. computemetrics (v, theState -> muxSize, 0, Y0. data (),
//	                              false,
	                              i != 0,
	                              level_0. data (), level_1. data ());
	   stream_0	-> process	(Y0. data (),
	                                 bits_0. data (), level_0. data ());
	   myDecoder. computemetrics (v, theState -> muxSize, 1, Y1. data (),
//	                              false,
	                              true,
	                              level_0. data (), level_1. data ());
	   stream_1	-> process	(Y1. data (),
	                                 bits_1. data (), level_1. data ()); 
	}

	memcpy (&bitsOut [0],
	        &bits_0 [0],
	        stream_0 -> highBits ());
	memcpy (&bitsOut [stream_0 -> highBits ()],
	        &bits_1 [0],
	        stream_1 -> highBits ());
	memcpy (&bitsOut [stream_0 -> highBits () +
	                  stream_1 -> highBits ()],
	        &bits_0 [stream_0 -> highBits ()],
	        stream_0 -> lowBits ());
	memcpy (&bitsOut [stream_0 -> highBits () +
	                  stream_1 -> highBits () +
	                  stream_0 -> lowBits ()],
	        &bits_1 [stream_0 -> highBits ()],
	        stream_1 -> lowBits ());
	          
//	apply PRBS
//	m_form -> set_messageLabel ("sm16 process fase 2");
	thePRBS -> doPRBS (&bitsOut [0]);
	memcpy (o, bitsOut.data (), (lengthA + lengthB) * BITSPERBYTE);
}

