#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRunoPlugin_drm
 *
 *    drm plugin is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm plugin is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with drm plugin; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#ifndef	__SDC_STREAMER__
#define	__SDC_STREAMER__

#include	<stdio.h>
#include	<stdint.h>
#include	"basics.h"
#include	"puncture-tables.h"
#include	"viterbi-drm.h"

class	Mapper;

class	SDC_streamer {
public:
			SDC_streamer	(uint8_t, uint8_t, Mapper *, int16_t);
			~SDC_streamer	(void);
	void		handle_stream	(metrics *, uint8_t *, uint8_t *, bool);
	int16_t		lengthOut	(void);
private:
	punctureTables	pt;
	uint8_t		*punctureTable;
	uint8_t		*residuTable;
	int16_t		punctureSize;
	viterbi_drm	deconvolver;
	Mapper		*demapper;
	uint8_t		RX, RY;
	int16_t		nrCells;
	int16_t		outLength;
	int16_t		deconvolveLength;
	int16_t		residuBits;
};

#endif

