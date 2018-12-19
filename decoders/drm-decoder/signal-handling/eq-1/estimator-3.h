#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
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
#
#
#ifndef	__ESTIMATOR_3
#define	__ESTIMATOR_3

#include	"radio-constants.h"
#include	"referenceframe.h"
#include	"estimator-base.h"
#include	<armadillo>
#include	<QString>

using namespace	arma;


//	The processor for estimating the channel(s) of a single
//	symbol
class	estimator_3: public estimatorBase {
public:
		estimator_3	(DSPCOMPLEX **, uint8_t, uint8_t, int16_t);
		~estimator_3	(void);
	void	estimate	(DSPCOMPLEX *, DSPCOMPLEX *);
private:
	int16_t		numberofCarriers;
	int16_t		numberofPilots;
	int16_t		numberofTaps;
//
//	lots of matrices
	cx_fmat		D;
	cx_fmat		B;		// DFT for pilots
	cx_fmat		B_H;		// 
	cx_fmat		C_h;
	cx_fmat		S_p;
	int16_t		fftSize;
	int16_t		*pilotTable;
};

#endif

