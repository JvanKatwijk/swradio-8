#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRunoplugin_drm
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

#ifndef	__CORRELATOR__
#define	__CORRELATOR__

#include	<stdint.h>
#include	<complex>
#include	<vector>
#include	"basics.h"

struct workCells {
        int16_t index;
        int16_t phase;
};

class	correlator {
public:
	correlator	(smodeInfo *s);
	~correlator	();
void	correlate	(std::complex<DRM_FLOAT> *, int);
int	maxIndex	();
void	cleanUp		();
bool	bestIndex	(int);
private:
	uint8_t	theMode;
	uint8_t	theSpectrum;
	std::vector<DRM_FLOAT>	corrTable;
	struct workCells	*refTable;
	int			Tu;
	int			K_min;
};
#endif

