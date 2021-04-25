#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair programming
 *
 *    This file is part of the drm decoder
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
 *
 *	The "getMode" function is a reimplementation of the
 *	MMSE algorithm (described in e.g. Tsai), some parts of it
 *	are taken over (or at least inspired by) the RXAMADRM software
 *	and its origin, diorama. 
 */
#include	"timesync.h"
#include	"reader.h"
#include	"drm-decoder.h"
#include	"basics.h"


	timeSyncer::timeSyncer (Reader *theReader,
	                        int32_t	sampleRate, int16_t nSymbols) {

	this	-> theReader	= theReader;
	this	-> sampleRate	= sampleRate;
	this	-> nSymbols	= nSymbols;
//	for mode determination:
	this	-> nSamples	= nSymbols * Ts_of (Mode_A);
}

	timeSyncer::~timeSyncer (void) {
}
//
//	Formula 5.16 from Tsai reads
//	Phi (m) = sum (r=0, R-1, abs (z[m - r] ^ 2)) +
//	          sum (r=0, R-1, abs (z[m - r - L]) ^ 2) -
//	       2 * abs (sum (r=0, R-1, z [m - r] * conj (z [m - r - L])));
//	The start of the symbol is there where
//	Phi (m) is minimal
//
void	timeSyncer::getMode (smodeInfo	*result) {
float	gammaRelative;
float	list_gammaRelative 	[]	= {0.0, 0.0, 0.0, 0.0};
float	list_epsilon		[]	= {0.0, 0.0, 0.0, 0.0};
int16_t	list_Offsets 		[]	= {0,   0,   0,   0};
int16_t mode;
int32_t i;
int16_t b [nSymbols];
int16_t	averageOffset	= 0;
int16_t	theMode;

	theReader -> waitfor (nSamples + 100);

//	just try all modes
	for (mode = Mode_A; mode <= Mode_D; mode++) {
	   compute_gammaRelative (mode,
	                          &list_gammaRelative	[mode - Mode_A],
	                          &list_epsilon		[mode - Mode_A],
	                          &list_Offsets		[mode - Mode_A]);
	}
//
//	we have collected for all modes the gamma and the theta
//	now decide for the mode to be detected 
	theMode		= Mode_B;		// default
	gammaRelative	= -1.0E20;
	for (i = Mode_A; i <= Mode_D; i++) {
	   if (list_gammaRelative [i - Mode_A] > gammaRelative) {
	      gammaRelative = list_gammaRelative [i - Mode_A];
	      theMode = i;
	   }
	}

//	check if result is reliable 
	bool	maxOK = true;			/* assume reliable */
	if (gammaRelative < 0.1)
//	if (gammaRelative < 0.3)
//	if (gammaRelative < 0.5)
	   maxOK = false;
	else
	for (i = Mode_A; i <= Mode_D; i++) {
	   if ((i != theMode) && (list_gammaRelative [i - Mode_A] >
	                           0.50 * gammaRelative))
	      maxOK = false;
	}
	
	if (!maxOK)	{		// no reliable mode found
	   result	-> Mode		= -1;
	   result	-> sampleRate_offset	= 0.0;
	   result	-> timeOffset_integer	= 0;
	   result	-> timeOffset_fractional	= 0;
	   result	-> freqOffset_integer	= 0;
	   result	-> freqOffset_fract	= 0.0;
	   return;
	}
//
//
//	OK, we seem to have a mode "theMode". The offset indicates the
//	offset in the buffer, and from that we compute the timeoffset.

	averageOffset = list_Offsets [theMode - 1];
//

	result	-> Mode			= theMode;
	result	-> sampleRate_offset	= 0;
	result	-> timeOffset_integer	= list_Offsets [theMode - Mode_A];
	result	-> timeOffset_fractional = 0;
	result	-> freqOffset_integer	= 0;
	result -> freqOffset_fract	= list_epsilon [theMode - Mode_A];
}

void	timeSyncer::compute_gammaRelative (uint8_t	mode,
	                                   float	*gammaRelative,
	                                   float	*Epsilon,
	                                   int16_t	*Offsets) {
int16_t	Ts		= Ts_of (mode);
int16_t Tu		= Tu_of (mode);
int16_t Tg		= Ts - Tu;
std::complex<float> gamma	[Ts];	// large enough
float	squareTerm	[Ts];
int32_t i, j, k, theOffset;

	memset (gamma,	0, Ts * sizeof (std::complex<float>));
	memset (squareTerm, 0, Ts * sizeof (float));

	for (i = 0; i < Ts; i ++) {
	   gamma [i]	= std::complex<float> (0, 0);
	   squareTerm [i] = float (0);
	   int32_t base = theReader -> currentIndex + i;
	   int32_t mask = theReader -> bufSize - 1;
	   for (j = 0; j < nSymbols; j ++) {
	      for (k = 0; k < Tg; k ++) {
	         std::complex<float> f1	=
	               theReader -> data [(base + j * Ts + k     ) & mask];
	         std::complex<float> f2	=
	               theReader -> data [(base + j * Ts + Tu + k) & mask];
	         gamma [i]	+=  f1 * conj (f2);
	         squareTerm [i] += (real (f1 * conj (f1)) +
	                                  real (f2 * conj (f2)));
	      }
	   }
	}

	theOffset		= 0;
	float minValue		= 10000.0;
	for (i = 0; i < Ts; i ++) {
	   float mmse = abs (squareTerm [i] - 2 * abs (gamma [i]));
	   if (mmse < minValue) {
	      minValue = mmse;
	      theOffset = i;
	   }
	}
	*gammaRelative	= abs (gamma [theOffset]) / squareTerm [theOffset];
	*Epsilon	= (float) arg (gamma [theOffset]);
	*Offsets	= theOffset;
}

