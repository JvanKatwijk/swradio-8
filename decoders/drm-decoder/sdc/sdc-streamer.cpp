#
/*
 *    Copyright (C) 2014
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
//
//	A single module for handling the output of the decoders (i.e.
//	raw bit streams), demapping, depuncturing and deconvolving the
//	data

#include	"sdc-streamer.h"
#include	"mapper.h"
#include	"viterbi-drm.h"

//	nrCells indicates - as suggested - the number of QAM cells
//	so the number of "raw" bits can be derived from that
	SDC_streamer::SDC_streamer (uint8_t RX, uint8_t RY, 
	                            Mapper	*myMapper,
	                            int16_t	nrCells):
	                               deconvolver (RX * ((2 * nrCells - 12) / RY)) {
	this	-> RX		= RX;
	this	-> RY		= RY;
	this	-> demapper	= myMapper;
	this	-> nrCells	= nrCells;
	this	-> outLength	= RX * ((2 * nrCells - 12) / RY);
	this	-> deconvolveLength	= 6 * (outLength + 6);
	punctureTable		= pt. getPunctureTable (RX, RY);
	residuTable		= pt. getResiduTable (RX, RY, nrCells);
	residuBits		= pt. getResiduBits (RX, RY, nrCells);
	punctureSize		= 6 * RX;
}

	SDC_streamer::~SDC_streamer	(void) {
}

int16_t	SDC_streamer::lengthOut (void) {
	return outLength;
}

void	SDC_streamer::handle_stream (metrics *softBits,
	                             uint8_t *reconstr,
	                             uint8_t *out, bool flag) {
int16_t	i, Cnt	= 0;
metrics	unmappedBits [2 * nrCells];
metrics	theBits [deconvolveLength + 600];
uint8_t temp [deconvolveLength + 600];
uint8_t	recomputedBits [2 * nrCells];

	for (i = 0; i < 2 * nrCells; i ++)
	   unmappedBits [demapper -> mapIn (i)] = softBits [i];

//	first the regular segment
	for (i = 0; i < 6 * (outLength + 6) - 36; i ++)
	   if (punctureTable [i % punctureSize] == 1)
	      theBits [i] = unmappedBits [Cnt ++];
	   else {
	      theBits [i]. rTow0 = 0;
	      theBits [i]. rTow1 = 0;
	   }
//
	for (i = 0; i < 36; i ++)
	   if (residuTable [i] == 1)
	      theBits [6 * outLength + i] = unmappedBits [Cnt ++];
	   else {
	      theBits [6 * outLength + i]. rTow0 = 0;
	      theBits [6 * outLength + i]. rTow1 = 0;
	   }
//
//	At this stage Cnt should be equal to 2 * nrCells
	deconvolver. deconvolve (theBits, outLength, out);
//
//	Now the other way around, but only if the flag says so
	if (!flag)
	   return;

	Cnt	= 0;		// start all over with counting
	deconvolver. convolve (out, outLength, temp);
	for (i = 0; i < 6 * (outLength + 6) - 36; i ++)
	   if (punctureTable [i % punctureSize] == 1)
	      recomputedBits [Cnt ++] = temp [i];
//	and the residu bits
	for (i = 6 * (outLength + 6) - 36;
	     i < 6 * (outLength + 6); i ++)
	   if (residuTable [(i - (6 * (outLength + 6) - 36))] == 1)
	      recomputedBits [Cnt ++] = temp [i];

	for (i = 0; i < 2 * nrCells; i ++)
	   reconstr [i] = recomputedBits [demapper -> mapIn (i)];
}

