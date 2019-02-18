#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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
 *
 *    viterbi.h  --  Viterbi decoder
 *
 */
#ifndef __VITERBI_DECODER_FAC_H
#define __VITERBI_DECODER_FAC_H
#include	<stdio.h>
#include	<string.h>
#include	<limits.h>
#include	<stdint.h>
#include	"basics.h"
//
//	experimental setting for 1/6 7 decoder
class viterbi_drm {
public:
		viterbi_drm	(int16_t);
		~viterbi_drm	(void);
	void	deconvolve	(metrics *, int16_t, uint8_t *);
	void	convolve	(uint8_t *, int16_t, uint8_t *);
private:
	float		costsFor		(uint16_t, metrics *);
	int32_t		**history;
	float		**Costs;
	uint8_t		*poly1_table;
	uint8_t		*poly2_table;
	uint8_t		*poly3_table;
	uint8_t		*poly4_table;
	uint8_t		*poly5_table;
	uint8_t		*poly6_table;
	int16_t		*predecessor_for_0;
	int16_t		*predecessor_for_1;
	int16_t		blockLength;
	uint8_t		bitFor		(uint16_t, uint16_t, uint8_t);
};
#endif
