#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the drm receiver
 *
 *    drm receiver is free software; you can redistribute it and/or modify
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
 *    along with drm receiver; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#include	"referenceframe.h"
#include	"basics.h"
#include	<cstring>
//
//	support functions
//
static inline
std::complex<float>	valueFor (float amp, int16_t phase) {
	return std::complex<float> (amp * cos (2 * M_PI * phase / 1024),
	                            amp * sin (2 * M_PI * phase / 1024));
}

struct timeCells {
	int16_t	index;
	int16_t	phase;
};

struct timeCells timeCellsforModeA [] = {
	{17, 973},
	{18, 205},
	{19, 717},
	{21, 264},
	{28, 357},
	{29, 357},
	{32, 952},
	{33, 440},
	{39, 856},
	{40, 88},
	{41, 88},
	{53, 68},
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

struct timeCells timeCellsforModeB [] = {
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

struct timeCells timeCellsforModeC [] = {
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

struct timeCells timeCellsforModeD [] = {
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
	
struct timeCells *tableforMode (uint8_t Mode) {
	switch (Mode) {
	   default:
	   case 1:
	      return timeCellsforModeA;
	   case 2:
	      return timeCellsforModeB;
	   case 3:
	      return timeCellsforModeC;
	   case 4:
	      return timeCellsforModeD;
	}
}

bool	isTimeCell (uint8_t Mode, int16_t symbol, int16_t carrier) {
struct timeCells *table = tableforMode (Mode);
int16_t	i;

	if (symbol != 0)
	   return false;
	for (i = 0; table [i]. index != -1; i ++)
	   if (table [i]. index == carrier)
	      return true;

	return false;
}

std::complex<float> getTimeRef (uint8_t Mode,
	                        int16_t symbol, int16_t carrier) {
struct timeCells *table = tableforMode (Mode);
int16_t i;

	if (symbol != 0)
	   return 0;
	for (i = 0; table [i]. index != -1; i ++)
	   if (table [i]. index == carrier)
	      return valueFor (sqrt (2), table [i]. phase);

	return 0;
}

struct timeCells freqCellsforModeA [] = {
	{18, 205},
	{54, 836},
	{72, 215},
	{-1, -1}
};

struct timeCells freqCellsforModeB [] = {
	{16, 331},
	{48, 651},
	{64, 555},
	{-1, -1}
};

struct timeCells freqCellsforModeC [] = {
	{11, 214},
	{33, 392},
	{44, 242},
	{-1, -1}
};

struct timeCells freqCellsforModeD [] = {
	{ 7, 788},
	{21, 1014},
	{28, 332},
	{-1, -1}
};

bool	isFreqCell (uint8_t Mode, int16_t symbol, int16_t carrier) {
struct timeCells *table;
int16_t	i;

	(void)symbol;
	switch (Mode) {
	   default:
	   case Mode_A: table = freqCellsforModeA; break;
	   case Mode_B: table = freqCellsforModeB; break;
	   case Mode_C: table = freqCellsforModeC; break;
	   case Mode_D: table = freqCellsforModeD; break;
	}

	for (i = 0; table [i]. index != -1; i ++)
	   if (table [i]. index == carrier)
	      return true;

	return false;
}

std::complex<float> getFreqRef (uint8_t Mode,
	                        int16_t symbol, int16_t carrier) {
struct timeCells *table;
int16_t	i;

	(void)symbol;
	switch (Mode) {
	   default:
	   case Mode_A: table = freqCellsforModeA; break;
	   case Mode_B: table = freqCellsforModeB; break;
	   case Mode_C: table = freqCellsforModeC; break;
	   case Mode_D: table = freqCellsforModeD; break;
	}

	for (i = 0; table [i]. index != -1; i ++)
	   if (table [i]. index == carrier)
	      return valueFor (sqrt (2), table [i]. phase);

	return 0;
}

bool	isPilotCell (uint8_t Mode, int16_t symbol, int16_t carrier) {
int16_t	p;

	if (carrier == 0)
	   return false;

	switch (Mode) {
	   default:
	   case Mode_A:
	      p = carrier - 2 - 4 * (symbol % 5);
	      if (p < 0) p = -p;
	      return (p % 20 == 0);

	   case Mode_B:
	      p = carrier - 1 - 2 * (symbol % 3);
	      if (p < 0) p = -p;
	      return (p % 6 == 0);

	   case Mode_C:
	      p = carrier - 1 - 2 * (symbol % 2);
	      if (p < 0) p = -p;
	      return (p % 4 == 0);

	   case Mode_D:
	      p = carrier - 1 - (symbol % 3);
	      if (p < 0) p = -p;
	      return (p % 3 == 0);
	}
}

int16_t	Q_1024 (uint8_t Mode) {
	switch (Mode) {
	   default:
	   case Mode_A: return 36;
	   case Mode_B: return 12;
	   case Mode_C: return 12;
	   case Mode_D: return 14;
	}
}

int16_t	W_1024_A (int16_t n, int16_t m) {
static	int16_t temp [] = {
	228, 341, 455,
	455, 569, 683,
	683, 796, 910,
	910,   0, 114,
	114, 228, 341};
	return temp [3 * n + m];
}

int16_t W_1024_B (int16_t n, int16_t m) {
static int16_t temp [] = {
	512,   0, 512,   0, 512,
	  0, 512,   0, 512,   0,
	512,   0, 512,   0, 512};
	return temp [5 * n + m];
}

int16_t	W_1024_C (int16_t n, int16_t m) {
static	int16_t	temp [] = {
	465, 372, 279, 186,  93,   0, 931, 838, 745, 652,
	931, 838, 745, 652, 559, 465, 372, 279, 186,  93};
	return temp [n * 10 + m];
};

int16_t	W_1024_D (int16_t n, int16_t m) {
static	int16_t	temp [] = {
	366, 439, 512, 585, 658, 731, 805, 676,
	731, 805, 979, 951,   0,  73, 146, 219,
	 73, 146, 219, 293, 366, 439, 512, 585};

	return temp [n * 8 + m];
}

int16_t	W_1024 (uint8_t Mode, int16_t n, int16_t m) {
	switch (Mode) {
	   default:
	   case Mode_A:	return W_1024_A (n, m);
	   case Mode_B:	return W_1024_B (n, m);
	   case Mode_C:	return W_1024_C (n, m);
	   case Mode_D:	return W_1024_D (n, m);
	}
}

int16_t	Z_256_A (int16_t n, int16_t m) {
static int16_t temp [] = {
	  0,  81, 248,
	 18, 106, 106,
	122, 116,  31,
	129, 129,  39,
	 33,  32, 111};
	return temp [n * 3 + m];
}

int16_t Z_256_B (int16_t n, int16_t m) {
static int16_t temp [] = {
	  0,  57, 164,  64,  12,
	168, 255, 161, 106, 118,
	 25, 232, 132, 233,  38};
	return temp [n * 5 + m];
}

int16_t	Z_256_C (int16_t n, int16_t m) {
static int16_t temp [] = {
	  0,  76,  29,  76,   9, 190, 161, 248,  33, 108,
	179, 178,  83, 253, 127, 105, 101, 198, 250, 145};
	return temp [n * 10 + m];
}

int16_t	Z_256_D (int16_t n, int16_t m) {
static int16_t	temp [] = {
	  0, 240,  17,  60, 220,  38, 151, 101,
	110,   7,  78,  82, 175, 150, 106,  25,
	165,   7, 252, 124, 253, 177, 197, 142};
	return temp [n * 8 + m];
}

int16_t	Z_256 (uint8_t Mode, int16_t n, int16_t m) {
	switch (Mode) {
	   default:
	   case Mode_A: return Z_256_A (n, m);
	   case Mode_B: return Z_256_B (n, m);
	   case Mode_C: return Z_256_C (n, m);
	   case Mode_D: return Z_256_D (n, m);
	}
}

bool	isBoostCell (uint8_t Mode, uint8_t spectrum,
	                     int16_t symbol, int16_t carrier) {
	switch (Mode) {
	   case Mode_A:
	      switch (spectrum) {
	         default:
	         case 0:
	               return (carrier == 2) || (carrier == 98) ||
	                      (carrier == 6) || (carrier == 102);
	         case 1:
	               return (carrier == 2) || (carrier == 110) ||
	                      (carrier == 6) || (carrier == 114);
	         case 2:
	               return (carrier == -102) || (carrier == 98) ||
	                      (carrier == -98) || (carrier == 102);
	         case 3:
	               return (carrier == -114) || (carrier == 110) ||
	                      (carrier == -110) || (carrier == 114);
	      }

	   case Mode_B:
	      switch (spectrum) {
	         case 0:
	               return (carrier == 1) || (carrier == 89) ||
	                      (carrier == 3) || (carrier == 91);
	         case 1:
	               return (carrier == 1) || (carrier == 101) ||
	                      (carrier == 3) || (carrier == 103);
	         case 2:
	               return (carrier == -91) || (carrier == 89) ||
	                      (carrier == -89) || (carrier == 91);
	         case 3:
	         default:
	               return (carrier == -103) || (carrier == 101) ||
	                      (carrier == -101) || (carrier == 103);
	      }

	   case Mode_C:
	      switch (spectrum) {
	         default:
	            if ((symbol & 01) == 0)
	               return (carrier == -69) || (carrier == 67);
	            else
	               return (carrier == -67)  || (carrier == 69);
	      }

	   case Mode_D:
	      switch (spectrum) {
	         default:
	            return carrier == -44 || carrier == -43 ||
	                   carrier == 43  || carrier == 44;
	      }
	   default:
	       // does not happen
	      return false;
	}
}

std::complex<float>	getPilotValue (uint8_t Mode,
	                       uint8_t Spectrum, int16_t symbol, int16_t carrier) {
int16_t	x	= Mode == Mode_A ? 4 :
	          Mode == Mode_B ? 2 :
	          Mode == Mode_C ? 2 : 1;
int16_t	y	= Mode == Mode_A ? 5 :
	          Mode == Mode_B ? 3 :
	          Mode == Mode_C ? 2 : 3;
int16_t	k0	= Mode == Mode_A ? 2 : 1;

int16_t	n	= symbol % y;
int16_t	m	= floor (symbol / y);
int16_t	p	= int16_t((float(carrier - k0 - n * x)) / (x * y));

int16_t	phase	= (4 * Z_256 (Mode, n , m) + p * W_1024 (Mode, n, m) +
	          (p * p) * (1 + symbol) * Q_1024 (Mode)) % 1024;

	if (isBoostCell (Mode, Spectrum, symbol, carrier))
	   return valueFor (sqrt (4.0), phase);
	else
	   return valueFor (sqrt (2.0), phase);

}
