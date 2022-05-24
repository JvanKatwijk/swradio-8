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
#include	"referenceframe.h"
#include	"estimator-2.h"
#include	"matrix2.h"
#include	"basics.h"

static	inline
int16_t	Minimum (int16_t a, int16_t b) {
	return a < b ? a : b;
}

static inline
complex<double> createExp (float s) {
	return complex<double> (cos (s), - sin (s));
}

static inline
complex<double> lcdiv (std::complex<double> a, float b) {
	return std::complex<double> (real (a) / b, imag (a) / b);
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

std::complex<double> ** S_p;
std::complex<double> ** A_p;

        this    -> refFrame     = refFrame;
        this    -> Mode         = Mode;
        this    -> Spectrum     = Spectrum;
        this    -> refSymbol    = refSymbol;
        this    -> K_min        = Kmin (Mode, Spectrum);
        this    -> K_max        = Kmax (Mode, Spectrum);
	fftSize			= Tu_of (Mode);
	numberofCarriers	= K_max - K_min + 1;
	numberofPilots		= getnrPilots (refSymbol);
	numberofTaps		= numberofPilots;
//	numberofTaps		= Tg_of (Mode) - 1;
	F_p		= createMatrix (numberofPilots, numberofTaps);
	S_p		= createMatrix (numberofPilots, numberofPilots);
	A_p		= createMatrix (numberofPilots, numberofTaps);
	A_p_inv		= createMatrix (numberofTaps, numberofPilots);
	pilotTable	= new int16_t [numberofPilots];

	for (int row = 0; row < numberofPilots; row ++)
	   for (int col = 0; col < numberofPilots; col ++)
	      S_p [row][col] = std::complex<float> (0, 0);

//	S_p is a diagonal matrix with the pilot values as they should be
	for (carrier = K_min; carrier <= K_max; carrier ++)
	   if (isPilotCell (Mode, refSymbol, carrier)) {
	      pilotTable [next] = carrier;
	      S_p [next][next] =
	               getPilotValue (Mode, Spectrum, refSymbol, carrier);
	      next ++;
	   }
//
//	F_p is initialized with the precomputed values and is
//	matrix filled with the (pilot, tap) combinations, where for the
//	pilots, their carrier values (relative to 0) are relevant
//	Basically the FFT transformer, with positions related to
//	the positions of the taps
//
//	The approach for channel estimation is as follows:
//	we call the channel in the time domain h_td,
//	in the frequency domain that will then be f = F x h_td
// 	we observe the values of the pilots at the recoever
// 	side, X_p, It is known that they were transmittes as s_p
// 	We know then that
//	X_p = diag (s_p) * f, where f = F_p * h_td
//	so, X_p = S_p * F_p * h_td
//	with S_p is diag (s_p), and F_p is the FFT transform of the channel
//	S_p en F_p can be computed, since they are known, regardless
//	A_p = S_p * F_p
//	the equation then is X_p = A_p * h_td
//	we solve that as h_td = A_p ^ -1 x X_p
//	recall that h_td is the chaannel in the time domain, so
//	h_fd = F_p * h_td is the solution
	for (pilotIndex = 0; pilotIndex < numberofPilots; pilotIndex ++) 
	   for (tap = 0; tap < numberofTaps; tap ++) 
	      F_p [pilotIndex][tap] = 
	              lcdiv (createExp (2 * M_PI *
	                          (fftSize / 2 + pilotTable [pilotIndex]) *
	                            tap / fftSize), sqrt (fftSize));

	for (int row = 0; row < numberofPilots; row ++) {
	   for (int col = 0; col < numberofTaps; col ++) {
	      A_p [row][col] = std::complex<float> (0, 0);
	      for (int k = 0; k < numberofPilots; k ++)
	         A_p [row][col] += S_p [row][k] * F_p [k][col];
	   }
	}
	for (int row = 0; row < numberofPilots; row ++)
	   for (int col = 0; col < numberofTaps;  col ++)
	      A_p_inv [row][col] = A_p [row][col];
	C_inverse (A_p_inv, numberofPilots);

	deleteMatrix (S_p, numberofPilots, numberofPilots);
	deleteMatrix (A_p, numberofTaps, numberofPilots);
}

	estimator_2::~estimator_2 (void) {
	delete[]	pilotTable;
	deleteMatrix (F_p, numberofPilots, numberofTaps);
	deleteMatrix (A_p_inv, numberofTaps, numberofPilots);
}

//
void	estimator_2::estimate (std::complex<float> *testRow,
	                            std::complex<float> *resultRow) {
int16_t		index;
std::complex<double> h_td [numberofTaps];
std::complex<double> H_fd [numberofPilots];
std::complex<double> X_p  [numberofPilots];

//	X_p are the observed values, and we have to "solve"
//	X_p	= A_P * h
//	h_td	= A_p_inv *  X_p;
//	H_fd	= F_p * h_td;
	for (index = 0; index < numberofPilots; index ++) 
	   X_p [index] = testRow [indexFor (pilotTable [index])];

//	Ok, the matrices are filled, now computing the channelvalues

//	h_td	= F_p_inv *  X_p;
//	h_td	is the channel in the time domain
	for (int i = 0; i < numberofPilots; i ++) {
	   h_td [i] = 0;
	   for (int j = 0; j < numberofPilots; j ++)
	      h_td [i] += A_p_inv [i][j] * X_p [j];
	}
//
//	and we make it into the channel of the frequency domain
	for (int i = 0; i < numberofPilots; i ++) {
	   H_fd [i] = 0;
	   for (int j = 0; j < numberofPilots; j ++)
	      H_fd [i] += F_p [i][j] * h_td [j];
	}

	for (index = 0; index < numberofPilots; index ++)
	   resultRow [indexFor (pilotTable [index])] =
	                  H_fd [index];
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

std::complex<double> ** estimator_2::createMatrix (int rows, int cols) {
std::complex<double> **res = new std::complex<double> *[rows];

	for (int i = 0; i < rows; i ++)
	   res [i] = new std::complex<double> [cols];
	return res;
}

void	estimator_2::deleteMatrix (std::complex<double> **M,
	                                       int rows, int cols) {
	for (int i = 0; i < rows; i ++) 
	   delete [] M [i];
	delete [] M;
}

//	(pilots X pilots) X (pilots X Taps) -> (pilots X Taps)
//	we assume Taps >= pilots
void	estimator_2::multiply_d (
	           std::complex<double> **Left, int l_rows, int l_cols,
	           std::complex<double> **Right, int r_rows, int r_cols,
	           std::complex<double> **Out) {
//	
//	for the first l_rows colums
	if (l_cols != r_rows)
	   fprintf (stderr, "PIEP\n");
	for (int row = 0; row < l_rows;  row ++) {
	   for (int col = 0; col < l_cols; col ++) {
	      Out [row][col] = std::complex<float> (0, 0);
	      for (int k = 0; k < l_cols; k ++)
	         Out [row][col] += Left [row][k] * Right [k][col];
	   }
	}
//
//	for the columns in the range l_rows .. r_cols 
	for (int row = 0; row < r_cols - l_rows ; row ++) {
	   for (int col = l_rows; col < r_cols; col ++) {
	      Out [row][col] = std::complex<float> (0, 0);
	      for (int k = 0; k < l_cols; k ++)
	         Out [row][col] +=
	                         Left [row][k] * Right [k][col];
	   }
	}
}

void	estimator_2::transpose (
	           std::complex<double> **In, int rows, int cols,
	           std::complex<double> **Out) {
	for (int row = 0; row < rows; row ++)
	   for (int col = 0; col < cols; col ++)
	      Out [col][row] = In [row][col];
}

//	(Taps X pilots) X (pilots X Taps) -> (Taps X Taps)
void	estimator_2::multiply_e (
	          std::complex<double> **Left, int l_rows, int l_cols,
	          std::complex<double> **Right, int r_rows, int r_cols,
	          std::complex<double> **Out) {
	for (int row = 0; row < l_rows; row ++) {
	   for (int col = 0; col < r_cols; col ++) {
	      Out [row][col] = std::complex<float> (0, 0);
	      for (int k = 0; k < l_cols; k ++)
	         Out [row][col] += Left [row][k] * Right [k][col];
	   }
	}
}
	   
//	(Taps X Taps) X (Taps X pilots) -> (Taps X pilots)
void	estimator_2::multiply_f (
	          std::complex<double> **Left, int l_rows, int l_cols,
	          std::complex<double> **Right, int r_rows, int r_cols,
	          std::complex<double> **Out) { 
	for (int row = 0; row < l_rows; row ++) {
	   for (int col = 0; col < r_cols; col ++) {
	      Out [row][col] = std::complex<float> (0, 0);
	      for (int k = 0; k < l_cols; k ++)
	         Out [row][col] += Left [row][k] * Right [k][col];
	   }
	}
}


