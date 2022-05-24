#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the drm receover and is based on
 *    parts of the file "Metrics.cpp" of the dream software.
 *    All rights are acknowledged.
 *
 *    drm  receiver is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm receiver is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with SDR-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#

#include	"mer4-values.h"

static
const double rTableQAM4[2][2] = {
    { 0.7071067811f,  0.7071067811f},
    {-0.7071067811f, -0.7071067811f}
};

	mer4_compute::mer4_compute	() {
}

	mer4_compute::~mer4_compute	() {
}
//
DRM_FLOAT	mer4_compute::computemer	(theSignal	*signalVector,
	                                 int32_t	amount) {
double	sumIQ	= 0;
double	sumdIdQ	= 0;

	   for (int i = 0; i < amount; i ++) {
	      std::complex<DRM_FLOAT> val = signalVector [i]. signalValue;
	      DRM_FLOAT	theI, theQ, thedI, thedQ;
	      val =  std::complex<DRM_FLOAT> (fabs (real (val)), fabs (imag (val)));
	      theI = rTableQAM4 [0][0];
	      theQ = rTableQAM4 [0][0];
	      thedI = real (val) - theI;
	      thedQ = imag (val) - theQ;
	      sumIQ	+= theI * theI + theQ * theQ;
	      sumdIdQ	+= thedI * thedI + thedQ * thedQ;
	}
	return sumIQ / sumdIdQ;
}

