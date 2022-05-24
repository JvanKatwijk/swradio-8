#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRunoPlugin_drm
 *
 *    drm plugin is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm plugin is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with drm plugin; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	"estimator" by just dividing the obtained values by
 *	the predefined values.
 */
#include	"basics.h"
#include	"estimator-1.h"
#include	"referenceframe.h"

//
//	We create a single "estimator" for each of the symbols
//	of a frame. We do, however, share he knowledge gained
//	from one symbol to the next one by passing on the channel
//	obtained in the time domain
//
	estimator_1::estimator_1 (std::complex<float> 	**refFrame,
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

	estimator_1::~estimator_1 (void) {
}
//
void	estimator_1::estimate (std::complex<float> *testRow,
	                               std::complex<float> *resultRow) {
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

int16_t estimator_1::indexFor (int16_t carrier) {
        return carrier - K_min;
}

int16_t estimator_1::getnrPilots (int16_t symbolno) {
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

