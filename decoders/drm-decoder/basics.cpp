#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRunoPlugin_drm

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
#include	"basics.h"
#include	<math.h>
#define	KK	1

#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif

int16_t	Ts_of (uint8_t Mode) {
	switch (Mode) {
	   default:
	   case Mode_A:
	   case Mode_B:
	      return KK * 320;

	   case Mode_C:
	      return KK * 240;

	   case Mode_D:
	      return KK * 200;
	}
}

int16_t	Tg_of (uint8_t Mode) {
	switch (Mode) {
	   default:
	   case Mode_A:
	      return KK * 32;

	   case Mode_B:
	      return KK * 64;

	   case Mode_C:
	      return KK * 64;

	   case Mode_D:
	      return KK * 88;
	}
}

int16_t	Tu_of (uint8_t Mode) {
	return Ts_of (Mode) - Tg_of (Mode);
}

int16_t	symbolsperFrame	(uint8_t Mode) {
	switch (Mode) {
	   default:
	   case Mode_A:
	   case Mode_B:
	      return 15;
	   case Mode_C:
	      return 20;
	   case Mode_D:
	      return 24;
	}
}

int16_t	groupsperFrame	(uint8_t Mode) {
	switch (Mode) {
	   default:
	   case Mode_A:
	      return 5;
	   case Mode_B:
	      return 3;
	   case Mode_C:
	      return 2;
	   case Mode_D:
	      return 3;
	}
}

int16_t	pilotDistance	(uint8_t Mode) {
	switch (Mode) {
	   default:
	   case Mode_A:
	      return 20;
	   case Mode_B:
	      return 6;
	   case Mode_C:
	      return 4;
	   case Mode_D:
	      return 3;		// wrong
	}
}

int16_t	symbolsperGroup (uint8_t Mode) {
	return symbolsperFrame (Mode) / groupsperFrame (Mode);
}

int16_t	Kmin	(uint8_t Mode, uint8_t s) {
	switch (Mode) {
	   case Mode_A:
	      default:
	      return s == 0 ? 2 : 
	             s == 1 ? 2 :
	             s == 2 ? -102:
	             s == 3 ? -114:
	             s == 4 ? -98 : -110;
	   case Mode_B:
	      return s == 0 ? 1 :
	             s == 1 ? 1 :
	             s == 2 ? -91 :
	             s == 3 ? -103 :
	             s == 4 ? -87 : -99;
	   case Mode_C:
	      return s == 3 ? -69 : -67;
	   case Mode_D:
	      return s == 3 ? -44 : -43;
	}
}

int16_t	Kmax	(uint8_t Mode, uint8_t s) {
	switch (Mode) {
	   case Mode_A:
	      default:
	      return s == 0 ? 102 : 
	             s == 1 ? 114 :
	             s == 2 ? 102:
	             s == 3 ? 114:
	             s == 4 ? 314: 350;
	   case Mode_B:
	      return s == 0 ? 91 :
	             s == 1 ? 103 :
	             s == 2 ? 91 :
	             s == 3 ? 103 :
	             s == 4 ? 279 : 311;
	   case Mode_C:
	      return s == 3 ? 69 : 213;
	   case Mode_D:
	      return s == 4 ? 44 : 135;
	}
}

int16_t	ususedCarriers	(uint8_t Mode) {
	switch (Mode) {
	   case 1:
	      return 3;
	   default:
	      return 1;
	}
}

#define	DBL_EPSILON	(0.000015)
DRM_FLOAT	sinc	(DRM_FLOAT f) {
	if (fabs (f) < DBL_EPSILON)
	   return 1.0;
	return sin (M_PI * f) / (M_PI * f);
}


