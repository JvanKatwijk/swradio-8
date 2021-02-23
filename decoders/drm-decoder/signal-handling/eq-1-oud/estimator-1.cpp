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
 *	"estimator" by just dividing the obtained values by
 *	the predefined values.
 */
#include	<QDebug>
#include	"estimator-1.h"
#include	"basics.h"
#include	"referenceframe.h"

//
//	We create a single "estimator" for each of the symbols
//	of a frame. We do, however, share he knowledge gained
//	from one symbol to the next one by passing on the channel
//	obtained in the time domain
//
	estimator_1::estimator_1 (DSPCOMPLEX 	**refFrame,
	                          uint8_t	Mode,
	                          uint8_t	Spectrum,
	                          int16_t	refSymbol):
	                             estimatorBase (refFrame,
	                                            Mode,
	                                            Spectrum,
	                                            refSymbol) {
}

	estimator_1::~estimator_1 (void) {
}
//
void	estimator_1::estimate (DSPCOMPLEX *testRow, DSPCOMPLEX *resultRow) {
int16_t	carrier;

	for (carrier = K_min; carrier <= K_max; carrier ++) {
	   if (carrier == 0)
	      continue;
	   if (isPilotCell (Mode, refSymbol, carrier)) 
	      resultRow [indexFor (carrier)] =
	                  testRow [indexFor (carrier)] /
	                    getPilotValue (Mode, Spectrum, refSymbol, carrier);
	}
}

