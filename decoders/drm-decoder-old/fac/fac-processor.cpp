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
#include	"fac-data.h"
#include	"drm-decoder.h"
#include	"basics.h"
#include	"mapper.h"
#include	"qam4-metrics.h"
#include	"viterbi-drm.h"
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
	facProcessor::facProcessor (uint8_t	Mode,
	                            uint8_t	Spectrum,
	                            viterbi_drm	*theDecoder) {

	this	-> Mode		= Mode;
	this	-> Spectrum	= Spectrum;
	deconvolver		= theDecoder;
	myMapper		= new Mapper (2 * FAC_SAMPLES, 21);
	thePRBS			= new prbs (FAC_BITS + FAC_CRC);
	theCRC			= new checkCRC (8, crcPolynome);
	myMetrics		= new qam4_metrics ();
}

	facProcessor::~facProcessor (void) {
	delete 		thePRBS;
	delete		theCRC;
	delete		myMapper;
	delete		myMetrics;
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
bool	facProcessor:: processFAC (theSignal *facVector, facData *facDB) {
uint8_t	 facBits [2 * (FAC_BITS + FAC_CRC)];

	fromSamplestoBits (facVector, facBits);
//	first: dispersion
	thePRBS -> doPRBS (facBits);

//	next CRC
	if (!theCRC -> doCRC (facBits, FAC_BITS + FAC_CRC)) {
	   facDB -> set_FAC_crc (false);
	   return false;
	}

	interpretFac (facBits, facDB);
	facDB -> set_FAC_crc (true);
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
	myMetrics -> computemetrics (v, FAC_SAMPLES, rawBits);
//
//	The puncturing table is given, it is a 3 * 6 entry table
//	so coded in-line, together with de-interleaving
//
	for (i = 0; i < 2 * FAC_SAMPLES; i ++)
	   demappedBits [myMapper -> mapIn (i)] = rawBits [i];

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
//	This "viterbi" understands the defines metrics
	deconvolver -> deconvolve (deconvolveBuffer,
	                             FAC_BITS + FAC_CRC, outBuffer);
}

//	The settings for decoding the ofdm frame follow from the
//	bits in the FAC, so lets go
void	facProcessor::interpretFac (uint8_t *v, facData *drm) {
	drm -> FAC_channelParameters (v);
	drm -> FAC_serviceParameters (&v [20]);
}

