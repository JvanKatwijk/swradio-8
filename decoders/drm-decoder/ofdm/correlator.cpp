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
#include	"correlator.h"
#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif

static inline
std::complex<DRM_FLOAT>	valueFor (DRM_FLOAT amp, int16_t phase) {
	return std::complex<DRM_FLOAT> (amp * cos (2 * M_PI * phase / 1024),
	                            amp * sin (2 * M_PI * phase / 1024));
}

struct workCells CellsforModeA [] = {
	{17, 973},
	{18, 205},
	{19, 717},
	{21, 264},
	{28, 357},
	{29, 357},
	{32, 952},
	{33, 440},
	{39, 856},
	{40,  88},
	{41,  88},
	{53,  68},
	{54, 836},
	{55, 836},
	{56, 836},
	{60, 1008},
	{61, 1008},
	{63, 752},
	{71, 215},
	{72, 215},
	{73, 727},
	{-1,  -1}
};

struct workCells CellsforModeB [] = {
	{14, 304},
	{16, 331},
	{18, 108},
	{20, 620},
	{24, 192},
	{26, 704},
	{32,  44},
	{36, 432},
	{42, 588},
	{44, 844},
	{48, 651},
	{49, 651},
	{50, 651},
	{54, 460},
	{56, 460},
	{62, 944},
	{64, 555},
	{66, 940},
	{68, 428},
	{-1,  -1}
};

struct workCells CellsforModeC [] = {
	{ 8, 722},
	{10, 466},
	{11, 214},
	{12, 214},
	{14, 479},
	{16, 516},
	{18, 260},
	{22, 577},
	{24, 662},
	{28,   3},
	{30, 771},
	{32, 392},
	{33, 392},
	{36,  37},
	{38,  37},
	{42, 474},
	{44, 242},
	{45, 242},
	{46, 754},
	{-1,  -1}
};

struct workCells CellsforModeD [] = {
	{ 5, 636},
	{ 6, 124},
	{ 7, 788},
	{ 8, 788},
	{ 9, 200},
	{11, 688},
	{12, 152},
	{14, 920},
	{15, 920},
	{17, 644},
	{18, 388},
	{20, 652},
	{21, 1014},
	{23, 176},
	{24, 176},
	{26, 752},
	{27, 496},
	{28, 332},
	{29, 432},
	{30, 964},
	{32, 452},
	{-1, -1}
};
	

	correlator::correlator (smodeInfo *s) {
	theMode		= s -> Mode;
	theSpectrum	= s -> Spectrum;
	Tu		= Tu_of (theMode);
	K_min		= Kmin (theMode, theSpectrum);
	corrTable. resize (symbolsperFrame (theMode));
	switch (theMode) {
	   default:
	   case Mode_A:
	      refTable	= CellsforModeA;
	      break;
	   case Mode_B:
	      refTable	= CellsforModeB;
	      break;
	   case Mode_C:
	      refTable	= CellsforModeC;
	      break;
	   case Mode_D:
	      refTable	= CellsforModeD;
	      break;
	}
}

	correlator::~correlator	() {
	corrTable. resize (0);
}

void	correlator::cleanUp	() {
	for (int i = 0; i <  corrTable. size (); i ++)
	   corrTable [i] = 0;
}

void	correlator::correlate (std::complex<DRM_FLOAT> *input, int wordNo) {
DRM_FLOAT	sum	= 0;
DRM_FLOAT	sqrt_2	= sqrt (2);

	for (int i = 0; refTable [i]. index != -1; i ++)
	   sum +=  (real (input [refTable [i]. index - K_min] *
	                     conj (valueFor (sqrt_2, refTable [i]. phase))));
	corrTable [wordNo] = sum;
}

int	correlator::maxIndex	() {
int	maxIndex	= 0;
DRM_FLOAT	max		= corrTable [0];

	for (int i = 1 ; i < corrTable. size (); i ++) {
	   if (corrTable [i] > max) {
	      max = corrTable [i];
	      maxIndex	= i;
	   }
	}
	return maxIndex;
}

bool	correlator::bestIndex	(int ind) {
DRM_FLOAT	maxV	= corrTable [ind];

	for (int i = 0; i < corrTable. size (); i ++) 
	   if ((i != ind) && (corrTable [i] > maxV))
	      return false;
	return true;
}


