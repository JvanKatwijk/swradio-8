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
 *    the Free Software Foundation as version 2 of the License.
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

#include	"lowpassfilter.h"
#define  _USE_MATH_DEFINES
#include <math.h>

	LowPassFIR::LowPassFIR (int16_t filterSize,
	                        int32_t Fc,
	                        int32_t sampleRate):
	                             filterKernel (filterSize),
	                             Buffer (filterSize) {
std::vector<float>tmp	(filterSize);
float	f	= (float)Fc / sampleRate;
float	sum	= 0.0;

	this	-> filterSize	= filterSize;
	this	-> sampleRate	= sampleRate;
	this	-> ip		= 0;
	for (int i = 0; i < filterSize; i ++) {
	   if (i == filterSize / 2)
	      tmp [i] = 2 * M_PI * f;
	   else 
	      tmp [i] = sin (2 * M_PI * f * (i - filterSize/2))/ (i - filterSize/2);
//
//	Blackman window
	   tmp [i]  *= (0.42 -
		    0.5 * cos (2 * M_PI * (float)i / (float)filterSize) +
		    0.08 * cos (4 * M_PI * (float)i / (float)filterSize));

	   sum += tmp [i];
	}

	for (int i = 0; i < filterSize; i ++)
	   filterKernel [i] = std::complex<float> (tmp [i] / sum, 0);
}

	LowPassFIR::~LowPassFIR () {
}

std::complex<float>	LowPassFIR::Pass (std::complex<float> z) {
int16_t	i;
std::complex<float>	tmp	= 0;

	Buffer [ip]	= z;
	for (i = 0; i < filterSize; i ++) {
	   int16_t index = ip - i;
	   if (index < 0)
	      index += filterSize;
	   tmp		+= Buffer [index] * filterKernel [i];
	}

	ip = (ip + 1) % filterSize;
	return tmp;
}

