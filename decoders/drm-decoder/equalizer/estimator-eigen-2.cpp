#
/*
 *    Copyright (C) 2014 .. 2017
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
 *
 *	Simple channel estimator for DRM based on private communication
 *	with Geert Leus
 */
#include	<QDebug>
#include	"referenceframe.h"
#include	"estimator-eigen-2.h"
#include	"matrix2.h"
#include	"basics.h"
#include	<Eigen/QR>
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
	estimator_2::estimator_2 (std::complex<float> 	**refFrame,
	                          uint8_t	Mode,
	                          uint8_t	Spectrum,
	                          int16_t	refSymbol) {
int16_t	carrier;
int16_t	next	= 0;
int16_t	pilotIndex, tap;

        this    -> refFrame     = refFrame;
        this    -> Mode         = Mode;
        this    -> Spectrum     = Spectrum;
        this    -> refSymbol    = refSymbol;
        this    -> K_min        = Kmin (Mode, Spectrum);
        this    -> K_max        = Kmax (Mode, Spectrum);
	fftSize			= Tu_of (Mode);
	numberofCarriers	= K_max - K_min + 1;
	numberofPilots		= getnrPilots (refSymbol);
	numberofTaps		= Tg_of (Mode);
//	numberofTaps		= numberofPilots;
	F_p			= MatrixXd (numberofPilots, numberofTaps);
	S_p			= MatrixXd (numberofPilots,
	                                          numberofPilots);
	A_p			= MatrixXd (numberofPilots, numberofTaps);
	A_p_inv			= MatrixXd (numberofTaps, numberofPilots);
//
	pilotTable		= new int16_t [numberofPilots];

//	S_p is a diagonal matrix with the pilot values as they should be
	for (int row = 0; row < numberofPilots; row ++)
	   for (int col = 0; col < numberofPilots; col ++)
              S_p (row, col) = std::complex<float> (0, 0);

	for (carrier = K_min; carrier <= K_max; carrier ++)
	   if (isPilotCell (Mode, refSymbol, carrier)) {
	      pilotTable [next] = carrier;
	      S_p (next, next) =
	               getPilotValue (Mode, Spectrum, refSymbol, carrier);
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

//
//	Note that A_p is not a square matrix, the number of taps
//	is larger than the number of pilots, so we have to apply 
//	the formula
//	A_p_inv = (A_p. transpose ()) * (A_p_transpose () * A_p). inverse ()
//	see: https://math.stackexchange.com/questions/1335693/invertible-matrix-of-non-square-matrix
	A_p	= S_p * F_p;
	A_p_inv = A_p. transpose () * (A_p * A_p. transpose ()). inverse ();
}

	estimator_2::~estimator_2 () {
	delete[]	pilotTable;
}

//
void	estimator_2::estimate (std::complex<float> *testRow,
	                            std::complex<float> *resultRow,
	                            std::vector<std::complex<float>> &channel) {
Vector	h_td (numberofTaps);
Vector  H_fd (numberofPilots);
Vector  X_p  (numberofPilots);
//	X_p are the observed values, and we have to "solve"
//	X_p     = A_P * h_td
//	h_td    = A_p_inv *  X_p;
//	H_fd    = F_p * h_td;


	for (int index = 0; index < numberofPilots; index ++) 
	   X_p (index) = testRow [indexFor (pilotTable [index])];
//
////	Ok, the matrices are filled, now computing the channelvalues
	h_td	= A_p_inv * X_p;
	H_fd	= F_p * h_td;
//
	for (int index = 0; index < numberofPilots; index ++)
	   resultRow [indexFor (pilotTable [index])] = H_fd [index];

	channel. resize (numberofPilots);
	for (int index = 0; index < numberofPilots; index ++)
	   channel. at (index) = h_td [index];
}

float	estimator_2::testQuality	(ourSignal *v) {
std::complex<float> res	= 0;
int count	= 0;
	
	for (int carrier = K_min; carrier <= K_max; carrier ++) {
	   if (carrier == 0)
	      continue;
	   if (isPilotCell (Mode, refSymbol, carrier)) {
	      count ++;
	      res += v [indexFor (carrier)]. signalValue / 
	                     getPilotValue (Mode, Spectrum, refSymbol, carrier);
	   }
	}
	fprintf (stderr, "%f %f\n", arg (res), abs (res)/ count);
	return abs (res);
}
int16_t estimator_2::indexFor (int16_t carrier) {
        return carrier - K_min;
}

int16_t estimator_2::getnrPilots (int16_t symbolno) {
int16_t         carrier;
int16_t         amount = 0;

        for (carrier = K_min; carrier <= K_max; carrier ++) {
           if (carrier == 0)
              continue;
           if ((Mode == 1) && (carrier == -1 || carrier == 1))
              continue;
           if (isPilotCell (Mode, symbolno, carrier)) amount ++;
        }
        return amount;
}
