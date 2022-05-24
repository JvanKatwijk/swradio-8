#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRuno drm decoder
 *
 *    drm decoder is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm decoder is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with drm decoder; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __BANDPASS_FILTER_H
#define __BANDPASS_FILTER_H

#include	"basics.h"
#include	<complex>
#include	<vector>

class	drmBandfilter {
public:
			drmBandfilter	(int16_t, int32_t, int32_t);
			~drmBandfilter	();
	std::complex<DRM_FLOAT> Pass (std::complex<DRM_FLOAT>);
	void		modulate	(int);
	int		offset	();
private:
	std::vector<DRM_FLOAT> filterBase;
	std::vector<std::complex <DRM_FLOAT>> filterKernel;
	std::vector<std::complex<DRM_FLOAT>> Buffer;
	int	sampleRate;
	int	ip;
	int	filterSize;
	int	centerFreq;
};

#endif

