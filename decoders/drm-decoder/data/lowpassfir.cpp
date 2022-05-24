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

#include	"lowpassfir.h"
//#include	<Windows.h>
#define  _USE_MATH_DEFINES
#include <math.h>


	lowpassFIR::lowpassFIR (int16_t firSize,
	                        int32_t Fc, int32_t fs):
	                           kernel (firSize),
	                           buffer (firSize) {
float	*tmp	= (float *)alloca (firSize *  sizeof (float));

	filterSize	= firSize;
	sampleRate	= fs;
	ip		= 0;
	float	f	= (float)Fc / sampleRate;
	float	sum	= 0.0;

	for (int i = 0; i < firSize; i ++)
	   buffer [i] = std::complex<float> (0, 0);
	for (int i = 0; i < filterSize; i ++) {
	   if (i == filterSize / 2)
	      tmp [i] = 2 * M_PI * f;
	   else 
	      tmp [i] = sin (2 * M_PI * f * (i - filterSize/2))/ (i - filterSize/2);
//
//	Blackman window
	   tmp [i]  *= (0.42 -
		    0.5 * cos (2 * M_PI * (float)i / filterSize) +
		    0.08 * cos (4 * M_PI * (float)i / filterSize));

	   sum += tmp [i];
	}

	for (int i = 0; i < filterSize; i ++)
	   kernel [i] = std::complex<float> (tmp [i] / sum, 0);
}

	lowpassFIR::~lowpassFIR () {
}

std::complex<float>
	lowpassFIR::Pass	(std::complex<float> z) {
std::complex<float>	tmp	= 0;

	buffer [ip]	= z;
	for (int i = 0; i < filterSize; i ++) {
	   int16_t index = ip - i;
	   if (index < 0)
	      index += filterSize;
	   tmp		+= buffer [index] * kernel [i];
	}

	ip = (ip + 1) % filterSize;
	return tmp;
}

