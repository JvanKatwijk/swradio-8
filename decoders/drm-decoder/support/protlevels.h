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
 *
 *    viterbi.h  --  Viterbi decoder
 *
 */

#ifndef	__PROT_LEVELS__
#include	<stdint.h>
void		protLevel_qam16	(uint8_t, int16_t,
	                                           int16_t *, int16_t *);
void		protLevel_qam64	(uint8_t, int16_t,
	                                           int16_t *, int16_t *);
float		getRp_qam16	(uint8_t, int16_t);
float		getRp_qam64	(uint8_t, int16_t);
int16_t		getRYlcm_16	(int16_t);
int16_t		getRYlcm_64	(int16_t);
#endif

