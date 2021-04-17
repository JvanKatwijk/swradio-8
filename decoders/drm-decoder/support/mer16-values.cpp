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
/* Definitions ****************************************************************/
/* Definitions for binary numbers (BI). On the left is most sigificant bit */
#define BI_00   0 /* Two bits */
#define BI_01   1
#define BI_10   2
#define BI_11   3

/**********************************************************************/
/* 16QAM **************************************************************/
/**********************************************************************/
/*		(i_0  i_1  q_0  q_1) = (y_0,0  y_1,0  y_0,1  y_1,1)   */

#include	"mer16-values.h"

static
const float rTableQAM16 [4][2] = {
    { 0.9486832980,  0.9486832980},
    {-0.3162277660, -0.3162277660},
    { 0.3162277660,  0.3162277660},
    {-0.9486832980, -0.9486832980}
};
	mer16_compute::mer16_compute	() {
}

	mer16_compute::~mer16_compute	() {
}
//
float	mer16_compute::computemer	(theSignal	*signalVector,
	                                 int32_t	amount) {
double	sumIQ	= 0;
double	sumdIdQ	= 0;

	   for (int i = 0; i < amount; i ++) {
	      std::complex<float> val = signalVector [i]. signalValue;
	      float	theI, theQ, thedI, thedQ;
	      val =  std::complex<float> (fabs (real (val)), fabs (imag (val)));
	      if (abs (val) < 0.5) {
	         theI = rTableQAM16 [2][0];
	         theQ = rTableQAM16 [2][0];
	         thedI = real (val) - theI;
	         thedQ = imag (val) - theQ;
	      }
	      else {
	         theI = rTableQAM16 [0][0];
	         theQ = rTableQAM16 [0][0];
	         thedI = real (val) - theI;
	         thedQ = imag (val) - theQ;
	      }
	      sumIQ	+= theI * theI + theQ * theQ;
	      sumdIdQ	+= thedI * thedI + thedQ * thedQ;
	}
	return sumIQ / sumdIdQ;
}

