#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair programming
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
 *
 *	base class for the estimators
 */
#include	"estimator-base.h"
#include	"basics.h"
#include	"referenceframe.h"

//
//	We create a single "estimator" for each of the symbols
//	of a frame.
//
	estimatorBase::estimatorBase (std::complex<float> **refFrame,
	                              uint8_t	Mode,
	                              uint8_t	Spectrum,
	                              int16_t	refSymbol) {
	this	-> refFrame	= refFrame;
	this	-> Mode		= Mode;
	this	-> Spectrum	= Spectrum;
	this	-> refSymbol	= refSymbol;
	this	-> K_min	= Kmin (Mode, Spectrum);
	this	-> K_max	= Kmax (Mode, Spectrum);
}

	estimatorBase::~estimatorBase (void) {
}
//
void	estimatorBase::estimate (std::complex<float> *testRow,
	                         std::complex<float> *resultRow) {
	(void)testRow; (void)resultRow;
}

int16_t	estimatorBase::indexFor (int16_t carrier) {
	return carrier - K_min;
}


int16_t	estimatorBase::getnrPilots (int16_t symbolno) {
int16_t		carrier;
int16_t		amount = 0;

	for (carrier = K_min; carrier <= K_max; carrier ++) {
	   if (carrier == 0)
	      continue;
	   if ((Mode == 1) && (carrier == -1 || carrier == 1))
	      continue;
	   if (isPilotCell (Mode, symbolno, carrier)) amount ++;
	}
	return amount;
}

