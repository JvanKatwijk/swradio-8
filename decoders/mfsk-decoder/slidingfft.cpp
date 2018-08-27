#
/*
 *
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J
 *    Many of the ideas as implemented in SDR-J are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
 *
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
 */
#include	<stdio.h>
#include	"slidingfft.h"
/*
 *	we slide with a goertzel derived algorithm, while
 *	the whole spectrum takes len bins, we are only
 *	interested in the first .. last bins.
 *	Very elegant algorithm, obviously not invented by me
 */
	slidingFFT::slidingFFT	(int32_t len, int32_t first, int32_t last) {
int32_t	i;

	fftBase	= new DSPCOMPLEX [len];
	Past	= new DSPCOMPLEX [len];
	Bins	= new DSPCOMPLEX [len];

	FFTlen	= len;
	First	= first;
	Amount	= last - first + 1;
	Last	= last;
	Ptr	= 0;

	for (i = 0; i < len; i ++) {
	   fftBase [i] = cmul (DSPCOMPLEX (cos (2.0 * i * M_PI / len),
				           sin (2.0 * i * M_PI / len)), 
		               STABILIZER);
	}

	Corrector = pow (STABILIZER, len);
}

	slidingFFT::~slidingFFT () {
	delete[]	fftBase;
	delete[]	Past;
	delete[]	Bins;
}

int32_t	slidingFFT::first (void) {
	return First;
}

void	slidingFFT::do_FFT (DSPCOMPLEX nnew, DSPCOMPLEX *out) {
DSPCOMPLEX	old;
DSPCOMPLEX	z;
int32_t	i;

	old = cmul (Past [Ptr], Corrector);
/*
 *	save the new sample
 */
	Past [Ptr] = nnew;
	Ptr = (Ptr + 1) % FFTlen;
/*
 *	calculate the wanted bins
 */
	for (i = First; i < Last; i ++) {
	   z = Bins [i] - old + nnew;
	   Bins [i] = z * fftBase [i];
	}

	for (i = 0; i < Amount; i ++) 
	   out [i] = Bins [First + i];
}

DSPCOMPLEX	slidingFFT::getBin (int32_t ind) {
	return Bins [ind];
}

