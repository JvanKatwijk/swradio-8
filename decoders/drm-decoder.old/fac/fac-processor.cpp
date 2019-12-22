#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDR-J (JSDR).
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
#include	"fac-processor.h"
#include	"state-descriptor.h"
#include	"drm-decoder.h"
#include	"basics.h"
#include	"mapper.h"
#include	"qam4-metrics.h"
#include	"prbs.h"
#include	"checkcrc.h"

#define	FAC_BITS	64
#define	FAC_CRC		8
#define	FAC_SAMPLES	65
//
//	for the FAC bits we have the following crc polynome
static
const uint16_t crcPolynome [] = {
	0, 0, 0, 1, 1, 1, 0	// MSB .. LSB x⁸ + x⁴ + x³ + x² + 1
};

//	Since we do not support mode E (for now at least), it is
//	relatively simple: assume as input a vector with the 65
//	complex values, constituing the FAC code in the current frame.
//	If no valid FAC can be found, return false
//
	facProcessor::facProcessor ():
	                            myMapper (2 * FAC_SAMPLES, 21),
	                            thePRBS   (FAC_BITS + FAC_CRC),
	                            theCRC   (8, crcPolynome),
	                            deconvolver (FAC_BITS + FAC_CRC) {
}

	facProcessor::~facProcessor (void) {
}
//
//	The puncture table is the 3/5 table from the standard
//	it is defined as 3 subsequent 6 bits of output
//	with 110000, 100000, 110000 as puncture patterns
//	I.e. 5 bits in and 3 "symbols" of 6 bits out
static
uint8_t punctureTable [] = {
1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0
};
//
//	Mode is obvious, bank points to array of vectors,
//	lc is the index in this array (circular buffer style)
//
//	On the receiving side we therefore should do the reverse:
//	1. extract the FAC cells into a vector
//	2. 4QAM demodulation
//	3. bit-deinterleaving
//	4. depuncturing and convolutional decoding
//	5. dispersion
//	6. CRC checking, resulting in 64 nice bits
//
bool	facProcessor:: processFAC (theSignal *facVector,
	                           stateDescriptor *theState) {
uint8_t	 facBits [2 * (FAC_BITS + FAC_CRC)];

	fromSamplestoBits (facVector, facBits);
//	first: dispersion
	thePRBS. doPRBS (facBits);

//	next CRC
	if (!theCRC. doCRC (facBits, FAC_BITS + FAC_CRC)) {
	   return false;
	}

	interpretFac (facBits, theState);
	return true;
}

//	de-interleaving is done inline. The function's main concern is
//	preparation for the deconvolution
void	facProcessor::fromSamplestoBits (theSignal *v, uint8_t *outBuffer) {
metrics rawBits 	[2 * FAC_SAMPLES];
metrics demappedBits 	[2 * FAC_SAMPLES];
int16_t	bufferSize	= 6 * (FAC_BITS + FAC_CRC + 6);
metrics	deconvolveBuffer [bufferSize];
int16_t	deconvolveCnt   = 0;
int16_t	inputCnt	= 0;
int16_t	i;
//
	myMetrics. computemetrics (v, FAC_SAMPLES, rawBits);
//
//	The puncturing table is given, it is a 3 * 6 entry table
//	so coded in-line, together with de-interleaving
//
	for (i = 0; i < 2 * FAC_SAMPLES; i ++)
	   demappedBits [myMapper. mapIn (i)] = rawBits [i];

	while (deconvolveCnt < bufferSize) {
	   if (punctureTable [deconvolveCnt % (3 * 6)] == 1) {
	      deconvolveBuffer [deconvolveCnt] = demappedBits [inputCnt ++];
	   }
	   else {	// add a zero costs for both
	      deconvolveBuffer [deconvolveCnt]. rTow0 = 0;
	      deconvolveBuffer [deconvolveCnt]. rTow1 = 0;
	   }

	   deconvolveCnt ++;
	}
//
//	This "viterbi" understands the defined metrics
	deconvolver. deconvolve (deconvolveBuffer,
	                             FAC_BITS + FAC_CRC, outBuffer);
}

//	The settings for decoding the ofdm frame follow from the
//	bits in the FAC, so lets go
void	facProcessor::interpretFac (uint8_t *v, stateDescriptor *st) {
	set_channelParameters (v, st);
	set_serviceParameters (&v [20], st);
}

void	facProcessor::set_channelParameters (uint8_t *v, stateDescriptor *st) {
	st	-> FrameIdentity = (v [1] << 1) | v [2];
	st	-> RMflag	= v [3]& 01;
	st	-> spectrumBits	= (v [4] << 2) | (v [5] << 1) | v [6];
	if (st -> RMflag) 
	   st -> interleaverDepth	=  (v [7] == 0) ? 5 : 1;
	else
	   if (v [0] == 0)
	   st -> interleaving = (v [7] == 0) ? 600 : -1;

	if (st -> RMflag == 0) {
	   switch ((v [8] << 1) | (v [9])) {
	      default:		// cannot happen
	      case 0:
	         st -> QAMMode	= stateDescriptor::QAM64;
	         st -> mscMode	= stateDescriptor::SM;
	         break;
	      case 1:
	         st -> QAMMode	= stateDescriptor::QAM64;
	         st -> mscMode	= stateDescriptor::HMI;
	         break;
	      case 2:
	         st -> QAMMode	= stateDescriptor::QAM64;
	         st -> mscMode	= stateDescriptor::HMIQ;
	         break;
	      case 3:
	         st -> QAMMode	= stateDescriptor::QAM16;
	         st -> mscMode	= stateDescriptor::SM;
	   }
	}
	else {		// RMflag == 1
	   switch ((v [8] << 1) | (v [9])) {
	      default:		// cannot happen
	      case 0:
	         st -> QAMMode	= stateDescriptor::QAM16;
	         st -> mscMode	= stateDescriptor::SM;
	      case 1: ;
	   }
	}

	st -> SDCMode		= v [10] & 01;

	uint8_t val = (v [11] << 3) | (v [12] << 2) | (v [13] << 1) | v [14];
	switch (val) {
	   case 0000:
	      st -> audioServices = 4; st -> dataServices = 0; break;
	   case 0001:
	      st -> audioServices = 0; st -> dataServices = 1; break;
	   case 0002:
	      st -> audioServices = 0; st -> dataServices = 2; break;
	   case 0003:
	      st -> audioServices = 0; st -> dataServices = 3; break;
	   case 0004:
	      st -> audioServices = 1; st -> dataServices = 0; break;
	   case 0005:
	      st -> audioServices = 1; st -> dataServices = 1; break;
	   case 0006:
	      st -> audioServices = 1; st -> dataServices = 2; break;
	   case 0007:
	      st -> audioServices = 1; st -> dataServices = 3; break;
	   case 0010:
	      st -> audioServices = 2; st -> dataServices = 0; break;
	   case 0011:
	      st -> audioServices = 2; st -> dataServices = 1; break;
	   case 0012:
	      st -> audioServices = 2; st -> dataServices = 2; break;
	   case 0013:	// cannot happen
	   case 0014:
	      st -> audioServices = 3; st -> dataServices = 0; break;
	   case 0015:
	      st -> audioServices = 3; st -> dataServices = 1; break;
	   case 0016:		// cannot happen
	   case 0017:
	      st -> audioServices = 0; st -> dataServices = 4; break;
	}
}

void	facProcessor::set_serviceParameters (uint8_t *v, stateDescriptor *st) {
        set_serviceLanguage (&v [27], st);
        st    -> AudioService = v [31] == 0;
        set_programType (&v [32], st);
}

//	Language is a 4 bit field
void	facProcessor::set_serviceLanguage (uint8_t *v, stateDescriptor *st) {
uint16_t val = (v [0] << 3) | (v [1] << 2) | (v [2] << 1) | v [3];

	switch (val) {
	   default:		// cannot happen
	   case 0:
	      st -> theLanguage	= "No language specified"; break;
	   case 1:
	      st -> theLanguage	= "Arabic"; break;
	   case 2:
	      st -> theLanguage	= "Bengali"; break;
	   case 3:
	      st -> theLanguage	= "Chinese"; break;
	   case 4:
	      st -> theLanguage	= "Dutch"; break;
	   case 5:
	      st -> theLanguage	= "English"; break;
	   case 6:
	      st -> theLanguage	= "French"; break;
	   case 7:
	      st -> theLanguage	= "German"; break;
	   case 8:
	      st -> theLanguage	= "Hindi"; break;
	   case 9:
	      st -> theLanguage	= "Japanese"; break;
	   case 10:
	      st -> theLanguage	= "Javanese"; break;
	   case 11:
	      st -> theLanguage	= "Korean"; break;
	   case 12:
	      st -> theLanguage	= "Portugese"; break;
	   case 13:
	      st -> theLanguage	= "Russian"; break;
	   case 14:
	      st -> theLanguage	= "Spanish"; break;
	   case 15:
	      st -> theLanguage	= "Other Language"; break;
	}
//	fprintf (stderr, " Language = %s\n", theLanguage);
}

void	facProcessor::set_programType (uint8_t *v, stateDescriptor *st) {
uint8_t val	= 0;
int16_t	i;
	if (!st -> AudioService)
	   return;		// apparently data service, skip for now

	for (i = 0; i < 5; i ++)
	   val = (val << 1) | (v [i] & 01);

//fprintf (stderr, "programmetype = %d ", val);
	switch (val) {
	   default:		// cannot happen
	   case 0:
	      st -> theProgrammeType	= "no program type"; break;
	   case 1:
	      st -> theProgrammeType	= "News"; break;
	   case 2:
	      st -> theProgrammeType	= "Current Affairs"; break;
	   case 3:
	      st -> theProgrammeType	= "Information"; break;
	   case 4:
	      st -> theProgrammeType	= "Sport"; break;
	   case 5:
	      st -> theProgrammeType	= "Education"; break;
	   case 6:
	      st -> theProgrammeType	= "Drama"; break;
	   case 7:
	      st -> theProgrammeType	= "Culture"; break;
	   case 8:
	      st -> theProgrammeType	= "Science"; break;
	   case 9:
	      st -> theProgrammeType	= "Varied"; break;
	   case 10:
	      st -> theProgrammeType	= "Pop Music"; break;
	   case 11:
	      st -> theProgrammeType	= "Rock Music"; break;
	   case 12:
	      st -> theProgrammeType	= "Easy Listening"; break;
	   case 13:
	      st -> theProgrammeType	= "Light Classical"; break;
	   case 14:
	      st -> theProgrammeType	= "Serious Classical"; break;
	   case 15:
	      st -> theProgrammeType	= "Other Music"; break;
	   case 16:
	      st -> theProgrammeType	= "Wheather"; break;
	   case 17:
	      st -> theProgrammeType	= "Finance/Business"; break;
	   case 18:
	      st -> theProgrammeType	= "Children\'s programmes"; break;
	   case 19:
	      st -> theProgrammeType	= "Social Affairs"; break;
	   case 20:
	      st -> theProgrammeType	= "Religion"; break;
	   case 21:
	      st -> theProgrammeType	= "Phone In"; break;
	   case 22:
	      st -> theProgrammeType	= "Travel"; break;
	   case 23:
	      st -> theProgrammeType	= "Leisure"; break;
	   case 24:
	      st -> theProgrammeType	= "Jazz Music"; break;
	   case 25:
	      st -> theProgrammeType	= "Country Music"; break;
	   case 26:
	      st -> theProgrammeType	= "National Music"; break;
	   case 27:
	      st -> theProgrammeType	= "Oldies Music"; break;
	   case 28:
	      st -> theProgrammeType	= "Folk Music"; break;
	   case 29:
	      st -> theProgrammeType	= "Documentary"; break;
	   case 30:
	   case 31:
	      st -> theProgrammeType	= "   "; break;
	}
//	fprintf (stderr, "programme type = %s\n", theProgrammeType);
}

