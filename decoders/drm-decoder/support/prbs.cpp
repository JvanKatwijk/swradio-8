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
#include	"prbs.h"
#include	<cstring>
//
//	Simple PRBS-er. I decided that computing each time the PRBS
//	is to be applied is OK, so no need to maintain pretty
//	lengthy vectors.
	prbs::prbs	(int32_t length) {
	this	-> length	= length;
}

	prbs::~prbs	(void) {
}

void	prbs::doPRBS	(uint8_t *v) {
int16_t	i, j;
uint8_t	shiftRegister [9];

	memset (shiftRegister, 1, 9);
	for (i = 0; i < length; i ++) {
	   uint8_t t = shiftRegister [8] ^ shiftRegister [4];
	   v [i] ^= t;
	   for (j = 8; j > 0; j --)
	      shiftRegister [j] = shiftRegister [j - 1];
	   shiftRegister [0] = t;
	}
}


