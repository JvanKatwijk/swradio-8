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

#ifndef	__REFERENCE_FRAME__
#define	__REFERENCE_FRAME__

#include	"basics.h"
#include	<stdint.h>
#include	<complex>


#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif

bool		isTimeCell	(uint8_t, int16_t, int16_t);
bool		isPilotCell	(uint8_t, int16_t, int16_t);
bool		isBoostCell	(uint8_t, uint8_t, int16_t);
bool		isFreqCell	(uint8_t, int16_t, int16_t);
std::complex<float>	getFreqRef	(uint8_t, int16_t, int16_t);
std::complex<float>	getTimeRef	(uint8_t, int16_t, int16_t);
std::complex<float>	getGainRef	(uint8_t, int16_t, int16_t);
std::complex<float>	getPilotValue	(uint8_t, uint8_t, int16_t, int16_t);
float		init_gain_ref_cells (int16_t *cells_k,
	                             std::complex<float> *cells_v, int16_t *cnt);
#endif


