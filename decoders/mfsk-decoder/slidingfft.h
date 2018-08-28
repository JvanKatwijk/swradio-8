#
/*
 *    Copyright (C) 2008, 2009, 2010
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J.
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
 */

#ifndef __SLIDING_FFT__
#define	__SLIDING_FFT__

#include	"radio-constants.h"

#define	STABILIZER	0.9999
/*
 *	a sliding FFT
 */

class	slidingFFT {
public:
		slidingFFT	(int32_t, int32_t, int32_t);
		~slidingFFT	();
	void	do_FFT		(DSPCOMPLEX, DSPCOMPLEX *);
	DSPCOMPLEX	getBin	(int32_t);
	int32_t	first		();
private:
	DSPCOMPLEX	*fftBase;
	DSPCOMPLEX	*Bins;		/* this is where it happens */
	DSPCOMPLEX	*Past;
	float		Corrector;
	int32_t		FFTlen;
	int32_t		First;
	int32_t		Last;
	int32_t		Amount;
	int32_t		Ptr;
};

#endif

