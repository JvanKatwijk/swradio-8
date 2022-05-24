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

#ifndef	__DRM_SHIFTER_H
#define	__DRM_SHIFTER_H
#include	<complex>
#include	"basics.h"

class	drmShifter {
public:
			drmShifter	(int32_t);
			~drmShifter	(void);
	std::complex<DRM_FLOAT>
			do_shift	(std::complex<DRM_FLOAT>, int32_t);
	void		do_shift	(std::complex<DRM_FLOAT> *, int32_t, int32_t);
private:
	int32_t		phase;
	int32_t		tableSize;
	std::complex<DRM_FLOAT>	*table;
};
#endif

