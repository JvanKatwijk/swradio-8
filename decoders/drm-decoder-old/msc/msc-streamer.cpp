#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the drm receiver
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
#include	"msc-streamer.h"
#include	"state-descriptor.h"
#include	"viterbi-drm.h"
#include	<stdio.h>
#include	"puncture-tables.h"
#include	"mapper.h"
#include	"protlevels.h"

//	streamhandler, invoked for each of the streams of
//	the Standard Method with QAM16 or QAM64
//
//	The streamer handles the depuncturing and deconvolving
//	of a single stream.
	MSC_streamer::MSC_streamer (stateDescriptor	*theState,
	                            int16_t	streamNumber,
	                            int16_t	N1,
	                            Mapper	*hpMapper,
	                            Mapper	*lpMapper) {
	this	-> theState	= theState;
	this	-> streamNumber	= streamNumber;
	this	-> N1		= N1;
	this	-> N2		= theState -> muxSize - N1;
	this	-> hpMapper	= hpMapper;
	this	-> lpMapper	= lpMapper;
//
//	just a handle:
//	
//	From the msc description, we extract the protection levels
//	for A and B parts
//	theState	-> get_protLevel (theState -> protLevelA, streamNumber,
//	                                                    &RX_High, &RY_High);
//	punctureHigh	= dummy. getPunctureTable (RX_High, RY_High);
////
//	theState	-> get_protLevel (theState -> protLevelB, streamNumber,
//	                                                     &RX_Low, &RY_Low);
	if (theState -> QAMMode == QAM16) {
	   protLevel_qam16 (theState -> protLevelA, streamNumber,
	                                                    &RX_High, &RY_High);
	   punctureHigh	= dummy. getPunctureTable (RX_High, RY_High);
//
	   protLevel_qam16 (theState -> protLevelB, streamNumber,
	                                                     &RX_Low, &RY_Low);
	}
	else {		// assume QAM64
	   protLevel_qam64 (theState -> protLevelA, streamNumber,
	                                                    &RX_High, &RY_High);
	   punctureHigh	= dummy. getPunctureTable (RX_High, RY_High);
//
	   protLevel_qam64 (theState -> protLevelB, streamNumber,
	                                                     &RX_Low, &RY_Low);
	}
	   
//	fprintf (stderr, "RX_High = %d, RY_High = %d, RX_Low = %d, RY_High = %d\n",
//	                  RX_High, RY_High, RX_Low, RY_Low);
	punctureLow	= dummy. getPunctureTable (RX_Low,  RY_Low);
//
//	The residu tables relate to the Lower protected part
	residuTable	= dummy. getResiduTable   (RX_Low, RY_Low, N2);
	residuBits	= dummy. getResiduBits    (RX_Low, RY_Low, N2);
//
//	formulas of section 7.2.1.1 are used to determine the
//	total number of bits resulting from this stream
//	As computed earlier, N1 is the number of constellations in the
//	higher protected part, N2 the number in the lower protected part
	highProtectedbits	= 2 * N1 * ((float)RX_High/ RY_High);
	lowProtectedbits	= RX_Low  * ((2 * N2 - 12) / RY_Low);
	deconvolver		= new viterbi_drm (highProtectedbits +
	                                           lowProtectedbits);
}

	MSC_streamer::~MSC_streamer	(void) {
	delete deconvolver;
}

int16_t	MSC_streamer::highBits (void) {
	return highProtectedbits;
}

int32_t	MSC_streamer::lowBits (void) {
	return lowProtectedbits;
}
//
//	The streamer does 2 things:
//	- first of all, it maps the incoming softbits
//	  to segments of HPP bits and LPP bits
//	- second, it returns - in the vector "better" - the
//	  viterbi encoded bits that - with costs 0 - map into
//	  the output.
int16_t	MSC_streamer::process (metrics *softBits,
	                       uint8_t *out, uint8_t *better) {
metrics	temp	[2 * (N1 + N2)];	// incoming bits
uint8_t	xxTemp	[2 * (N1 + N2)];	// recomputed input
metrics	*temp1;
metrics	*temp2;
int32_t	deconvolveLength = 6 * (highProtectedbits + lowProtectedbits + 6);
metrics	UTemp	[deconvolveLength];
uint8_t	hulpVec [deconvolveLength];
int32_t	pntr	= 0;
int32_t	next	= 0;
int16_t	i;

//	first step: deinterleaving
//	According to the standard (7.3.3.3), mapping is within the section
//	i.e. HP bits are mapped separately from LP bits
//	we first map the HP part
	if (hpMapper == NULL)	// just an eep stream or stream 0
	   memcpy (temp, softBits, (2 * N1) * sizeof (metrics));
	else
	   for (i = 0; i < 2 * N1; i ++)
	      temp [hpMapper -> mapIn (i)] = softBits [i];
//
	temp1	= &softBits [2 * N1];
	temp2	= &temp     [2 * N1];
	
//	Next the LP bits
	if (lpMapper == NULL)	// stream 0 for SM 64
	   memcpy (temp2, temp1, (2 * N2) * sizeof (metrics));
	else
	   for (i = 0; i < 2 * N2; i ++)
	      temp2 [lpMapper -> mapIn (i)] = temp1 [i];

//	second step: get the higher protected bits
	for (pntr = 0; pntr < 6 * highProtectedbits; pntr ++)
	   if (punctureHigh [pntr % (6 * RX_High)] == 1) {
	      UTemp [pntr]. rTow0 = temp [next]. rTow0;
	      UTemp [pntr]. rTow1 = temp [next]. rTow1;
	      next ++;
	   }
	   else {
	      UTemp [pntr]. rTow0 = 0;
	      UTemp [pntr]. rTow1 = 0;
	   }

//	A simple correctness check is to look here at the
//	number of consumed soft bits, which should be equal to
//	2 * N1
//	... then the lower protected part, just until the tailbits
	for (pntr = 6 * highProtectedbits;
	     pntr < 6 * (highProtectedbits + lowProtectedbits); pntr ++) {
	   if (punctureLow [(pntr - 6 * highProtectedbits) % (6 * RX_Low)] == 1) {
	      UTemp [pntr]. rTow0 = temp [next] . rTow0;
	      UTemp [pntr]. rTow1 = temp [next] . rTow1;
	      next ++;
	   }
	   else {
	      UTemp [pntr]. rTow0 = 0;
	      UTemp [pntr]. rTow1 = 0;
	   }
	}
//
//	We should have eaten 2 * muxSize - residu bits here
//	The residual bits, to form the last 36 bits in the
//	deconvolver
	for (pntr = 6 * (highProtectedbits + lowProtectedbits);
	     pntr < 6 * (highProtectedbits + lowProtectedbits) + 36; pntr ++)
	   if (residuTable [pntr -
	            6 * (highProtectedbits + lowProtectedbits)] == 1) {
	      UTemp [pntr]. rTow0 = temp [next] . rTow0;
	      UTemp [pntr]. rTow1 = temp [next] . rTow1;
	      next ++;
	   }
	   else {
	      UTemp [pntr]. rTow0 = 0;
	      UTemp [pntr]. rTow1 = 0;
	   }

	deconvolver	-> deconvolve (UTemp,
	                               highProtectedbits + lowProtectedbits,
	                               out);
//
//	what we do next is try to reconstruct what should have been
//	in the input vector, such that that may be used for improving
//	the decoding process
	deconvolver	-> convolve (out,
	                             highProtectedbits + lowProtectedbits,
	                             hulpVec);
//
//	Now back, 
//	second step: get the higher protected bits
	next	= 0;
	for (pntr = 0; pntr < 6 * highProtectedbits; pntr ++)
	   if (punctureHigh [pntr % (6 * RX_High)] == 1)
	      xxTemp [next ++] = hulpVec [pntr];
//	A simple correctness check is to look here at the
//	number of consumed soft bits, which should be equal to
//	2 * N1
//	... then the lower protected part, just until the tailbits
	for (pntr = 6 * highProtectedbits;
	     pntr < 6 * (highProtectedbits + lowProtectedbits); pntr ++) {
	   if (punctureLow [(pntr - 6 * highProtectedbits) % (6 * RX_Low)] == 1)
	      xxTemp [next ++] = hulpVec [pntr];
	   }
//
//	We should have eaten 2 * muxSize - residu bits here
//	The residual bits, to form the last 36 bits in the
//	deconvolver
	for (pntr = 6 * (highProtectedbits + lowProtectedbits);
	     pntr < 6 * (highProtectedbits + lowProtectedbits) + 36; pntr ++)
	   if (residuTable [pntr -
	            6 * (highProtectedbits + lowProtectedbits)] == 1)
	      xxTemp [next ++] =  hulpVec [pntr];

	if (hpMapper != NULL)	// hp part to be mappered
	   for (i = 0; i < 2 * N1; i ++)
	      better [i] = xxTemp [hpMapper -> mapIn (i)];
	else
	   memcpy (better, xxTemp, 2 * N1 * sizeof (uint8_t));

//	Next the LP bits
	if (lpMapper != NULL)	// stream > 0 for SM 64
	   for (i = 0; i < 2 * N2; i ++) 
	      better [2 * N1 + i] = xxTemp [2 * N1 + lpMapper -> mapIn (i)];
	else
	   memcpy (&better [2 * N1],
	           &xxTemp [2 * N1], 2 * N2 * sizeof (uint8_t));

	return highProtectedbits + lowProtectedbits;
}

