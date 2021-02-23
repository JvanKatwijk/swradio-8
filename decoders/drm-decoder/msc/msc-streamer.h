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
#ifndef	__MSC_STREAMER__
#define	__MSC_STREAMER__

#include	"radio-constants.h"
#include	"puncture-tables.h"
#include	"basics.h"

class	stateDescriptor;
class	viterbi_drm;
class	Mapper;

class	MSC_streamer {
public:
		MSC_streamer	(stateDescriptor *,	// base for config
	                         int16_t,	// stream number
	                         int16_t,	// N1 (HPP ofdm cells)
	                         Mapper *,	// mapper used for HP
	                         Mapper *	// mapper used for LP
	                        );
		~MSC_streamer	(void);
	int16_t	highBits	(void);		// computed info
	int32_t	lowBits		(void);		// computed info
	int16_t	process		(metrics *,	// input soft bits
	                         uint8_t *, 	// output HPP + LPP bits
	                         uint8_t *);	// corrected input
private:
	punctureTables	dummy;
	stateDescriptor	*theState;
	int16_t		streamNumber;
	int16_t		N1;		// higher protected cells
	int32_t		N2;		// lower protected cells
	Mapper		*hpMapper;
	Mapper		*lpMapper;
	viterbi_drm	*deconvolver;
	int16_t		RX_High;
	int16_t		RX_Low;
	int16_t		RY_High;
	int16_t		RY_Low;
	uint8_t		*punctureHigh;
	uint8_t		*punctureLow;
	uint8_t		*residuTable;
	int16_t		highProtectedbits;
	int32_t		lowProtectedbits;
	int16_t		residuBits;
};

#endif

