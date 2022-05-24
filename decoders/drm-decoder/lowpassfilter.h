#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRuno drm plugin
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

#ifndef	__LOWPASSPILTER_H
#define	__LOWPASSFILTER_H

#include	<complex>
#include	<vector>

class LowPassFIR {
public:
		LowPassFIR	(int16_t filterSize,
	                         int32_t Fc,
	                         int32_t sampleRate);
		~LowPassFIR	();
std::complex<float> Pass	(std::complex<float> z);
private:
	std::vector<std::complex<float>> filterKernel;
	std::vector<std::complex<float>> Buffer;
	int	filterSize;
	int	sampleRate;
	int	ip;
};
#endif

