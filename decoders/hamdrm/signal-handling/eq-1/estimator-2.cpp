#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDR-J 
 *    SDR-J is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    SDR-J is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with SDR-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	Simple channel estimator for DRM based on private communication
 *	with Geert Leus
 */
#include	<QDebug>
#include	"estimator-2.h"
#include	"basics.h"

static	inline
int16_t	Minimum (int16_t a, int16_t b) {
	return a < b ? a : b;
}

static inline
complex<float> createExp (float s) {
	return complex<float> (cos (s), - sin (s));
}

//	The basic idea is to create an estimator with an instance
//	of the class for each of the N symbols of a frame. 

//
//	I thought to be clever and look at the DFT of the computed
//	channel. However, the channel is only a rough approximation
//	of the channel as applied to the pilots.
	estimator_2::estimator_2 (DSPCOMPLEX 	**refFrame,
	                          uint8_t	Mode,
	                          uint8_t	Spectrum,
	                          int16_t	refSymbol):
	                           estimatorBase (refFrame,
	                                          Mode,
	                                          Spectrum,
	                                          refSymbol) {
int16_t	carrier;
int16_t	next	= 0;
int16_t	pilotIndex, tap;

	fftSize			= Tu_of (Mode);
	numberofCarriers	= K_max - K_min + 1;
	numberofPilots		= getnrPilots (refSymbol);
	numberofTaps		= Tg_of (Mode) - 1;
	F_p			= cx_fmat (numberofPilots, numberofTaps);
	S_p			= zeros<cx_fmat> (numberofPilots,
	                                          numberofPilots);
	F_p_inv			= cx_fmat (numberofTaps, numberofPilots);
//
	pilotTable		= new int16_t [numberofPilots];
//	S_p is a diagonal matrix with the pilot values as they should be
	for (carrier = K_min; carrier <= K_max; carrier ++)
	   if (isPilotCell (Mode, refSymbol, carrier)) {
	      pilotTable [next] = carrier;
	      S_p (next, next) = getPilotValue (Mode, Spectrum, refSymbol, carrier);
	      next ++;
	   }
//
//	F_p is initialized with the precomputed values and is
//	matrix filled with the (pilot, tap) combinations, where for the
//	pilots, their carrier values (relative to 0) are relevant
	for (pilotIndex = 0; pilotIndex < numberofPilots; pilotIndex ++) 
	   for (tap = 0; tap < numberofTaps; tap ++) 
	      F_p (pilotIndex, tap) = 
	              cdiv (createExp (2 * M_PI *
	                          (fftSize / 2 + pilotTable [pilotIndex]) *
	                            tap / fftSize), sqrt (fftSize));

	F_p_inv			= pinv (S_p * F_p);
}

	estimator_2::~estimator_2 (void) {
	delete[]	pilotTable;
}

//
void	estimator_2::estimate (DSPCOMPLEX *testRow, DSPCOMPLEX *resultRow) {
int16_t		index;
cx_fvec	h_td (numberofTaps);
cx_fvec	H_fd (numberofPilots);
cx_fvec	X_p  (numberofPilots);

	for (index = 0; index < numberofPilots; index ++) 
	   X_p (index) = testRow [indexFor (pilotTable [index])];

//	Ok, the matrices are filled, now computing the channelvalues
	h_td	= F_p_inv *  X_p;
	H_fd	= F_p * h_td;

	for (index = 0; index < numberofPilots; index ++)
	   resultRow [indexFor (pilotTable [index])] =
	                  H_fd [index];
}

