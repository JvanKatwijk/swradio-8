#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRuno rtty decoder
 *
 *    rtty decoder is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    rtty decoder is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with rtty decoder; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef	__UTILITY_FUNCTIONS__
#define	__UTILITY_FUNCTIONS__

#include	<stdint.h>
#include	"basics.h"

DRM_FLOAT	decayingAverage (DRM_FLOAT, DRM_FLOAT, DRM_FLOAT);
DRM_FLOAT	clamp (DRM_FLOAT, DRM_FLOAT, DRM_FLOAT);
class	rttyAverage {
public:
			rttyAverage	(int16_t);
			~rttyAverage	();
	DRM_FLOAT		filter		(DRM_FLOAT);
	void		clear		(DRM_FLOAT);
private:
        int16_t		size;
        DRM_FLOAT		*vec;
        int16_t		filp;
};

#endif


