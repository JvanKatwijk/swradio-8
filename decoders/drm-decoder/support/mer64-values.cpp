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

#include	"mer64-values.h"

const DRM_FLOAT rTableQAM64SM [8][2] = {
    { 1.0801234497f,  1.0801234497f},
    {-0.1543033499f, -0.1543033499f},
    { 0.4629100498f,  0.4629100498f},
    {-0.7715167498f, -0.7715167498f},
    { 0.7715167498f,  0.7715167498f},
    {-0.4629100498f, -0.4629100498f},
    { 0.1543033499f,  0.1543033499f},
    {-1.0801234497f, -1.0801234497f}
};

	mer64_compute::mer64_compute	() {
}

	mer64_compute::~mer64_compute	() {
}

DRM_FLOAT	N (DRM_FLOAT f) {
	return sqrt (2 * f * f);
}

DRM_FLOAT	mer64_compute::computemer	(theSignal	*signalVector,
	                                 int32_t	amount) {
double	sumIQ	= 0;
double	sumdIdQ	= 0;
DRM_FLOAT	diff1	= (0.4629100498 * sqrt (2) -
                                         0.1543033499 * sqrt (2)) / 2; 
DRM_FLOAT	diff2	= (0.7715167498 * sqrt (2) -
                                         0.4629100498 * sqrt (2)) / 2;
DRM_FLOAT	diff3	= (1.0801234497 * sqrt (2) -
                                         0.7715167498 * sqrt (2)) / 2;

	   for (int i = 0; i < amount; i ++) {
	      std::complex<DRM_FLOAT> val = signalVector [i]. signalValue;
	      DRM_FLOAT	theI, theQ, thedI, thedQ;
	      val =  std::complex<DRM_FLOAT> (fabs (real (val)), fabs (imag (val)));
	      if (abs (val) < 0.4629100498 * sqrt (2) - diff1)  {
	         theI = 0.1543033499;
	         theQ = 0.1543033499;;
	         thedI = real (val) - theI;
	         thedQ = imag (val) - theQ;
	      }
	      else 
	      if (abs (val) < 0.7715167498 * sqrt (2) - diff2) {
	         theI = 0.4629100498;
	         theQ = 0.4629100498;
	         thedI = real (val) - theI;
	         thedQ = imag (val) - theQ;
	      }
	      else
	      if (abs (val) < 1.0801234497 * sqrt (2) - diff3) {
	         theI = 0.7715167498;
	         theQ = 0.7715167498;
	         thedI = real (val) - theI;
	         thedQ = imag (val) - theQ;
	      }
	      else {
	         theI = 1.0801234497;
	         theQ = 1.0801234497;
	         thedI = real (val) - theI;
	         thedQ = imag (val) - theQ;
	      }

	      sumIQ	+= theI * theI + theQ * theQ;
	      sumdIdQ	+= thedI * thedI + thedQ * thedQ;
	}
	return sumIQ / sumdIdQ;
}

