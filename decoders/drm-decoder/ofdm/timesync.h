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
#ifndef	__TIMESYNC__
#define	__TIMESYNC__
#include	"radio-constants.h"
#include	"ringbuffer.h"
#include	"reader.h"
#include	"basics.h"

class		drmDecoder;

class	timeSyncer {
public:
			timeSyncer	(Reader *, int32_t,  int16_t);
			~timeSyncer 	(void);
	void		getMode		(smodeInfo *);
private:
	void		compute_gammaRelative (uint8_t   mode,
                                               float    *gammaRelative,
                                               float    *Epsilon,
                                               int16_t  *Offsets);

	Reader		*theReader;
	int32_t		sampleRate;
	int16_t		nSymbols;
	int32_t		nSamples;
};
#endif

