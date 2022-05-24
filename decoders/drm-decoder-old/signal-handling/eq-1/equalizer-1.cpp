//#
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%                                                                         %
//%%  University of Kaiserslautern, Institute of Communications Engineering  %
//%%  Copyright (C) 2004 Andreas Dittrich                                    %
//%%                                                                         %
//%%  Author(s)    : Andreas Dittrich (dittrich@eit.uni-kl.de)               %
//%%  Project start: 27.05.2004                                              %
//%%  Last change: 02.05.2005, 11:30                                         %
//%%  Changes      : |                                                       %
//%%                                                                         %
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*************************************************************************
*
*                           PA0MBO
*
*    COPYRIGHT (C)  2009  M.Bos 
*
*    This file is part of the distribution package RXAMADRM
*
*    This package is free software and you can redistribute is
*    and/or modify it under the terms of the GNU General Public License
*
*    More details can be found in the accompanying file COPYING
*************************************************************************/
/*
 *    Copyright (C) 2015
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDR-J
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
 *	The Wiener algorithm used in this implementation is written 
 *	in C++  and is a translation of the algorithm as given in
 *	diorama 1.1, and inspired by the translitteration
 *	of the Matlab specification of that algorithm as done in RXAMADRM
 */
#
#include	<QDebug>
#include	"referenceframe.h"
#include	"basics.h"
#include	"equalizer-1.h"
#include	"drm-decoder.h"
#include	"estimator-base.h"
#ifdef	ESTIMATOR_1
#include	"estimator-1.h"
#elif	ESTIMATOR_2
#include	"estimator-2.h"
#else
#include	"estimator-3.h"
#endif
#include	"matrix2.h"

#define	realSym(x)	((x + symbolsinFrame)% symbolsinFrame)

		equalizer_1::equalizer_1 (drmDecoder	*parent,
	                                  uint8_t	Mode,
	                                  uint8_t	Spectrum,
	                                  int8_t	strength,
	                                  RingBuffer<std::complex<float>> *b):
	                                     equalizer_base (Mode, Spectrum) {
int16_t	i, window;
float	sigmaq_noise_list [] = {16.0, 14.0, 14.0, 12.0};
float	sigmaq_noise	= pow (10.0, - sigmaq_noise_list [Mode - Mode_A] / 10.0);
float	**PHI;
float	*THETA;

	this	-> parent	= parent;
	this	-> eqBuffer	= b;
	connect	(this, SIGNAL (show_eqsymbol (int)),
	         parent, SLOT (show_eqsymbol (int)));
//	Based on table 92 ETSI ES 201980

//	Just for experimentation, we added some alternatives
int16_t		symbols_per_window_list_0 []	= {6, 4, 4, 6};
int16_t		symbols_per_window_list_1 []	= {10, 6, 8, 6};
int16_t		symbols_per_window_list_2 []	= {12, 8, 8, 6};
int16_t		symbols_per_window_list_3 []	= {14, 10, 8, 6};
int16_t		symbols_per_window_list_4 []	= {15, 12, 10, 6};
int16_t		symbols_per_window_list_5 []	= {15, 15, 15, 6};
//
//	first shorthands 
	symbols_per_window	=  strength == 0 ?
	                              symbols_per_window_list_0 [Mode - Mode_A]:
	                           strength == 1 ?
	                              symbols_per_window_list_1 [Mode - Mode_A]:
	                           strength == 2 ?
	                              symbols_per_window_list_2 [Mode - Mode_A]:
	                           strength == 3 ?
	                              symbols_per_window_list_3 [Mode - Mode_A]:
	                           strength == 4 ?
	                              symbols_per_window_list_4 [Mode - Mode_A]:
	                              symbols_per_window_list_5 [Mode - Mode_A];

	symbols_to_delay	= floor (symbols_per_window / 2);
	periodforSymbols	= groupsperFrame (Mode);
	periodforPilots		= pilotDistance (Mode);
	windowsinFrame		= groupsperFrame (Mode);
	Ts			= Ts_of (Mode);
	Tu			= Tu_of (Mode);
	Tg			= Tg_of	(Mode);
//
//	we kunnen het aantal trainers redelijk schatten door
//	het aantal pilots per symbol te benaderen (carriers / afstand)
//	en te vermenigvuldigen met  het aantal symbols per window

	theTrainers		. resize (windowsinFrame);
	pilotEstimates		. resize (symbolsinFrame);
	for (i = 0; i < symbolsinFrame; i ++)
	   pilotEstimates. at (i). resize (carriersinSymbol);

//	precompute 2-D-Wiener filter-matrix w.r.t. power boost
//	Reference: Peter Hoeher, Stefan Kaiser, Patrick Robertson:
//	"Two-Dimensional Pilot-Symbol-Aided Channel Estimation By
//	Wiener Filtering",
//	ISIT 1997, Ulm, Germany, June 29 - July 4
//	PHI	= auto-covariance-matrix
//	THETA	= cross-covariance-vector
//	f_cut_t	% two-sided maximum doppler frequency
//	                       (normalized w.r.t symbol duration Ts) 
//	f_cut_k % two-sided maximum echo delay
//	                       (normalized w.r.t useful symbol duration Tu)
//
//	values taken from diorama
	f_cut_t = 0.0675 / symbols_to_delay;
	f_cut_k = 1.75 * (float) Tg / (float) Tu;
	f_cut_k = 2.0 * (float) Tg / (float) Tu;
//
//	This code is based on the diorama Matlab code, and a
//	(complete)rewrite of the C translation of this Matlab code by Ties Bos.
//
//	first: create the vector of gain reference cells for the
//	whole frame(s) of this mode
//	Note that there is a periodicity such that the pilot structure
//	can be described in just a few "windows"
//	Now the handling: two loops, in the first one we create
//	"trainers" for the particular window
//	In the second one - a large loop - we build the filters
//	
//	We build up the "trainers" as relative addresses of the pilots
	for (window = 0; window < windowsinFrame; window ++) {
	   buildTrainers (window);
	   trainers_per_window [window] = theTrainers. at (window). size ();

	   int16_t	trainers_in_window	= trainers_per_window [window];
	   trainer	*currentTrainers	= theTrainers [window] . data ();
	   int16_t	trainer_1, trainer_2, carrier;
//
//	   W_symbol_blk [window]	= new double  *[carriersinSymbol];
//	   for (i = 0; i < carriersinSymbol; i ++)
//	      W_symbol_blk [window][i] = new double [trainers_in_window];
	   W_symbol_blk [window]. resize (carriersinSymbol);
	   for (i = 0; i < carriersinSymbol; i ++)
	      W_symbol_blk [window][i]. resize (trainers_in_window);

	   PHI		= new float *[trainers_in_window];
	   int cc;
	   for (cc = 0; cc < trainers_in_window; cc ++) {
	      PHI [cc] = new float [trainers_in_window];
	   }
	   THETA	= new float [trainers_in_window];
//
//	No need to set the PHI and THETA to zero, first access
//	is a normal assigment
//	Calculating PHI for the current "window"
//	Note that the symbol positions here are the positions
//	within the window (sym, car), sym relative
           for (trainer_1 = 0;
                trainer_1 < trainers_in_window; trainer_1 ++) {
	      int16_t sym_1	= currentTrainers [trainer_1]. symbol;
	      int16_t car_1	= currentTrainers [trainer_1]. carrier;
              for (trainer_2 = 0;
                   trainer_2 < trainers_in_window; trainer_2 ++) {
	         int16_t sym_2	= currentTrainers [trainer_2]. symbol;
	         int16_t car_2	= currentTrainers [trainer_2]. carrier;
	         PHI [trainer_1][trainer_2] = sinc ((car_1 - car_2) * f_cut_k)
	                                    * sinc ((sym_1 - sym_2) * f_cut_t);
              }
	   }	// end of trainer_1 loop

//	add the noise to the diagonals
	   for (trainer_1 = 0; trainer_1 < trainers_in_window; trainer_1++) {
	      int16_t sym	= currentTrainers [trainer_1]. symbol;
	      int16_t car	= currentTrainers [trainer_1]. carrier;
	      std::complex<float> v = getPilotValue (Mode, Spectrum,
	                                             sym + window, car);
	      float amp = real (v * conj (v));
	      PHI [trainer_1][trainer_1] += sigmaq_noise * 2.0 / amp;
	   }
//
	   inverse (PHI, trainers_in_window);
//
//	Note that from now on, PHI is actually PHI_INV. A "local"
//	implementation of inverse is used to avoid linking libraries
//	Now for Theta's
	   for (carrier = K_min; carrier <= K_max; carrier++) {
	      int16_t j, k;
	      if (carrier == 0)
	         continue;
	// first a new THETA
	      for (trainer_1 = 0;
	           trainer_1 < trainers_in_window; trainer_1 ++) {
	         int16_t pilotSymbol, pilotCarrier;
	         pilotSymbol	= currentTrainers [trainer_1]. symbol;
	         pilotCarrier	= currentTrainers [trainer_1]. carrier;
//	THETA = cross covariance-vector
	         THETA [trainer_1] = sinc ((carrier - pilotCarrier) * f_cut_k)
	                           * sinc ((symbols_to_delay - pilotSymbol) * f_cut_t);
	      }		// end of trainer_1 loop

//      calc matrix product  W_symbol_blk [w] [car] = THETA * PHI_INV
//	in-line coded to avoid libraries
	      for (j = 0; j < trainers_in_window; j++) {
	         W_symbol_blk [window][indexFor (carrier)][j] = 0.0;
	         for (k = 0; k < trainers_in_window; k++)
	            W_symbol_blk [window][indexFor (carrier)][j] +=
	                          THETA [k] * PHI [k][j];
	      }
	   } /* end carrier-loop, W_symbol_blk done for this carrier */
//
//	   done with the THETA and the PHI
	   delete [] THETA;
	   for (i = 0; i < trainers_in_window; i ++) 
	      delete [] PHI [i];

	   delete [] PHI;
	}
//
//	The W_symbol_blk filters are ready now
//
//	and finally, the estimators
	Estimators	= new estimatorBase *[symbolsinFrame];
	for (i = 0; i < symbolsinFrame; i ++)
#ifdef	ESTIMATOR_1
	   Estimators [i] = new estimator_1 (refFrame, Mode, Spectrum, i);
#elif	ESTIMATOR_2
	   Estimators [i] = new estimator_2 (refFrame, Mode, Spectrum, i);
#else
	   Estimators [i] = new estimator_3 (refFrame, Mode, Spectrum, i);
#endif
}

		equalizer_1::~equalizer_1 (void) {
int16_t	i;
	for (i = 0; i < symbolsinFrame; i ++)
	   delete Estimators [i];
	delete [] Estimators;
//
//	W_symbol_blk is a matrix with three dimensions
//	for (int window = 0; window < windowsinFrame; window ++) { 
//	   for (i = 0; i < carriersinSymbol; i ++)
//	      delete W_symbol_blk [window][i];
//	   delete W_symbol_blk [window];
//	}
}
//
//	The "trainers" are built over the "regular" pilots, i.e.
//	those pilots that appear in the regular pilot pattern.
//	"Trainers" are encoded as a pair, with the symbol relative
//	to the start of the window
int16_t		equalizer_1::buildTrainers (int16_t window) {
int16_t symbol, carrier;
int16_t	myCount	= 0;
	theTrainers. at (window). resize (0);
	for (symbol = window;
	     symbol < window + symbols_per_window; symbol ++) {
	   for (carrier = K_min; carrier <= K_max; carrier ++) {
	      if (isPilotCell (Mode, symbol, carrier)) {
	         trainer temp;
	         temp. symbol	= symbol - window;
	         temp. carrier	= carrier;
	         theTrainers. at (window). push_back (temp);
	         myCount ++;
	      }
	   }
	}
	return myCount;
}

bool	equalizer_1::equalize (std::complex<float> *testRow,
	                       int16_t	newSymbol,
	                       myArray<theSignal>*outFrame,
	                       float	*offset_fractional,
	                       float	*delta_freq_offset,
	                       float	*sampleclockOffset) {
int16_t	carrier;
int16_t	symbol_to_process;
int16_t	i;

//	First, we copy the incoming vector to the appropriate vector
//	in the testFrame. Next we compute the estimates for the
//	channels of the pilots.
//
//	Tracking the freqency offset is done by looking at the
//	phase difference of frequency pilots in subsequent words
	std::complex<float>	offs1	= std::complex<float> (0, 0);
	std::complex<float>	offs2	= std::complex<float> (0, 0);
	float		offsa	= 0;
	int		offs3	= 0;
	int		offsb	= 0;
	std::complex<float>	offs7	= std::complex<float> (0, 0);
	
	for (carrier = K_min; carrier <= K_max; carrier ++) {
	   if (carrier == 0)
	      continue;
	   std::complex<float> oldValue	= 
	                  testFrame [newSymbol][indexFor (carrier)];
	   testFrame [newSymbol][indexFor (carrier)] = 
	                                 testRow [indexFor (carrier)];
//
//	apply formula 5.40 from the Tsai book to get the SCO
	   if (isPilotCell (Mode, newSymbol, carrier)) {
	      offsa	+= arg (oldValue * conj (testRow [indexFor (carrier)]))  / symbols_to_delay * indexFor (carrier);
	      offsb	+= indexFor (carrier) * indexFor (carrier); 
	   }
//
//	For an estimate of the residual frequency offset, we
//	look at the average phase difference in the
//	frequency pilots of the last N symbols
	   if (isFreqCell (Mode, newSymbol, carrier)) {
	      for (i = 1; i < symbolsinFrame; i ++) {
	         offs1 += conj (testFrame [realSym (newSymbol - 1)]
	                                             [indexFor (carrier)]) *
	                   (testFrame [realSym (newSymbol)]
	                                             [indexFor (carrier)]);
	      }
	   }
//
//	alternatively, use the phase differences between successive
//	symbols with the same pilot layout
	   if (isPilotCell (Mode, newSymbol, carrier)) {
	      int16_t helpme = realSym (newSymbol - periodforSymbols);
	      std::complex<float> f1 =
	               testFrame [newSymbol][indexFor (carrier)] *
	                   conj (getPilotValue (Mode, Spectrum, newSymbol, carrier));
	      std::complex<float> f2 =
	               testFrame [helpme][indexFor (carrier)] *
	                   conj (getPilotValue (Mode, Spectrum, helpme, carrier));
	      offs7 += f1 * conj (f2);
	   }
	}

//	For an estimate of the residual sample time offset (includes
//	the phase offset of the LO), we look at the average of the
//	phase offsets of the subsequent pilots in the current symbol
	std::complex<float> prev_1 = std::complex<float> (0, 0);
	std::complex<float> prev_2 = std::complex<float> (0, 0);
	for (carrier = K_min; carrier <= K_max; carrier ++) {
	   if (isPilotCell (Mode, newSymbol, carrier)) {
//	Formula 5.26 (page 99, Tsai et al), average phase offset
	      if (offs3 > 0) 
	         offs2 += (testRow [indexFor (carrier)] * 
	                      conj (getPilotValue (Mode, Spectrum, newSymbol, carrier))) *
	                   conj (prev_1 * conj (prev_2));
	         
	      offs3 += 1;
	      prev_1 = testRow [indexFor (carrier)];
	      prev_2 = getPilotValue (Mode, Spectrum, newSymbol, carrier);
	   }
	}
//
//	the SCO is then
//	arg (offsa) / symbolsinFrame / (2 * M_PI * Ts / Tu * offsb)) * Ts;
//	The measured offset is in radials
	*sampleclockOffset = arg (offsa) / (2 * M_PI * (float (Ts) / Tu) * offsb);

//	still wondering about the scale
	*offset_fractional	= arg (offs2) / (2 * M_PI * periodforPilots);
//	the frequency error we measure in radials
//	we may choose here between two ways of computing
//	offs1 means using the frequency pilots over N symbols
//	offs7 means using all pilots over two near symbols with the same
//	pilot layout
//	*delta_freq_offset	=  arg (offs1) / (3 * (symbolsinFrame - 1));
//	*delta_freq_offset	=  arg (offs7) / periodforSymbols;
	*delta_freq_offset	= (arg (offs1) + arg (offs7) / periodforSymbols) / 2;
//	fprintf (stderr, "freq error: freq pilots = %f, all pilots  = %f\n",
//	                 arg (offs1) / (3 * (symbolsinFrame - 1)),
//	                 arg (offs7) / periodforSymbols);
//
	Estimators [newSymbol] ->
	              estimate (testFrame [newSymbol],
	                        pilotEstimates [newSymbol]. data ());

//	For equalizing symbol X, we need the pilotvalues
//	from the symbols X - symbols_to_delay .. X + symbols_to_delay - 1
//
//	We added the symbol at loc newSymbol, so we can equalize
//	the symbol "symbols_to_delay" back.
//	
	symbol_to_process = realSym (newSymbol - symbols_to_delay);
	processSymbol (symbol_to_process,
	                      outFrame -> element (symbol_to_process));

//	If we have a frame full of output: return true
	return symbol_to_process == symbolsinFrame - 1;
}
//
bool	equalizer_1::equalize (std::complex<float> *testRow,
	                       int16_t newSymbol,
	                       myArray<theSignal>*outFrame) {
int16_t	carrier;
int16_t	symbol_to_process;

//	First, we copy the incoming vector to the appropriate vector
//	in the testFrame. Next we compute the estimates for the
//	channels of the pilots.
//	This is the most simple approach: the channels of the pilots
//	are obtained by dividing the value observed by the pilot value.
	for (carrier = K_min; carrier <= K_max; carrier ++) {
	   if (carrier == 0)
	      continue;
	   testFrame [newSymbol][indexFor (carrier)] =
	                         testRow [indexFor (carrier)];
	   if (isPilotCell (Mode, newSymbol, carrier))
	      pilotEstimates [newSymbol][indexFor (carrier)] =
	                  testRow [indexFor (carrier)] /
	                    getPilotValue (Mode, Spectrum, newSymbol, carrier);
	}

//
//	For equalizing symbol X, we need the pilotvalues
//	from the symbols X - symbols_to_delay .. X + symbols_to_delay - 1
//
//	We added the symbol at loc newSymbol, so we can equalize
//	the symbol "symbols_to_delay - 1" back.
//	
	symbol_to_process = realSym (newSymbol - symbols_to_delay);
	processSymbol (symbol_to_process,
	                      outFrame -> element (symbol_to_process));
//	If we have a frame full of output: return true
	return symbol_to_process == symbolsinFrame - 1;
}

//
//	process symbol "symbol", 
void	equalizer_1::processSymbol (int16_t symbol,
	                            theSignal *outVector) {
//	"ntwee" will indicate the "model" of the window, we deal with
//	while windowBase indicates the REAL window, i.e. the
//	the first symbol of the window as appearing in the frame.
int16_t	windowBase		= realSym (symbol +
	                                   symbols_to_delay -
	                                   symbols_per_window + 1);
int16_t	ntwee			= windowBase % windowsinFrame;
int16_t	nrTrainers		= trainers_per_window [ntwee];
trainer *currentTrainers	= theTrainers [ntwee]. data ();
int16_t	currentTrainer;
int16_t	carrier;

//	The trainers are over N subsequent symbols in the frame, starting
//	at windowBase. The "model" is to be found at ntwee.
//	we determine the REAL address in the frame by first computing the
//	relative address, and then adding the base of the window over
//	the frame we are looking at

//	The outer loop refers to the targets for the
//	filtering,
//	the inner loop loops over the pilots (aka trainers) for
//	the reference window
	for (carrier = K_min; carrier <= K_max; carrier ++) {
	   double sum = 0;
	   if (carrier == 0)
	      continue;

	   refFrame [symbol] [indexFor (carrier)] = 0;
	   for (currentTrainer = 0;
	        currentTrainer < nrTrainers;  currentTrainer ++) {
	      int16_t relSym	= currentTrainers [currentTrainer]. symbol;
	      int16_t pilotCarrier = currentTrainers [currentTrainer]. carrier;
	      int16_t actualSymbol = realSym (relSym + windowBase);

	      refFrame [symbol] [indexFor (carrier)] +=
	           cmul (pilotEstimates [actualSymbol][indexFor (pilotCarrier)],
	                 W_symbol_blk [ntwee][indexFor (carrier)]
	                                                    [currentTrainer]);
	      sum += W_symbol_blk [ntwee][indexFor (carrier)][currentTrainer];
	   }
//	and normalize the value
	   refFrame [symbol][indexFor (carrier)] =
	                cdiv (refFrame [symbol][indexFor (carrier)], sum);
	}

	if (symbol == 0) {
	   eqBuffer -> putDataIntoBuffer (refFrame [symbol],
	                                  K_max - K_min + 1);
	   show_eqsymbol (K_max - K_min + 1);
	}
	   
//	The transfer function is now there, stored in the appropriate
//	entry in the refFrame, so let us equalize
	for (carrier = K_min; carrier <= K_max; carrier ++) {
	   std::complex<float> temp	=
	                   refFrame [symbol] [indexFor (carrier)];
	   if (carrier == 0)
	      outVector [indexFor (0)]. signalValue = 
	                                     std::complex<float> (0, 0);
	   else {
	      std::complex<float> qq =
	                 testFrame [symbol][indexFor (carrier)] / temp;
	      outVector [indexFor (carrier)] . signalValue = qq;
	   }
	   outVector [indexFor (carrier)]. rTrans = abs (temp);
	}
}

