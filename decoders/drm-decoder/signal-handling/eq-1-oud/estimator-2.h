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
 */
#
#ifndef	__ESTIMATOR_2__
#define	__ESTIMATOR_2__

#include	<armadillo>
#include	<QString>
#include	"radio-constants.h"
#include	"estimator-base.h"
#
using namespace	arma;

//	The processor for estimating the channel(s) of a single
//	symbol
class	estimator_2: public estimatorBase {
public:
		estimator_2 (DSPCOMPLEX **,
	                     uint8_t, uint8_t, int16_t);
		~estimator_2 (void);
	void	estimate	(DSPCOMPLEX *, DSPCOMPLEX *);
private:
	int16_t		numberofCarriers;
	int16_t		numberofPilots;
	int16_t		numberofTaps;
	int16_t		currentSymbol;
	int16_t		fftSize;
	cx_fmat		F_p;
	cx_fmat		S_p;
	cx_fmat		F_p_inv;
	DSPCOMPLEX	*pilotVector;
	int16_t		*pilotTable;
};

#endif

