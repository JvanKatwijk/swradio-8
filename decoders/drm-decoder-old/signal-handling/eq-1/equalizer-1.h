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
 */
#
#ifndef	__EQUALIZER_1
#define	__EQUALIZER_1

#include	"equalizer-base.h"
#include	<fftw3.h>
class		estimatorBase;

typedef	struct {
	int16_t	symbol;
	int16_t	carrier;} trainer;

class	equalizer_1: public equalizer_base {
public:
			equalizer_1 	(uint8_t	Mode,
	                                 uint8_t	Spectrum,
	                                 int8_t		strength = 2);
			~equalizer_1 	(void);
	bool		equalize	(DSPCOMPLEX *,
	                                 int16_t,
	                                 theSignal **,
	                                 int16_t *,
	                                 float *,
	                                 float *,
	                                 float *);
	bool		equalize	(DSPCOMPLEX *,
	                                 int16_t,
	                                 theSignal **);
private:
	void		getRelAddress	(int16_t, int16_t *, int16_t *);
	int16_t		buildTrainers	(int16_t, trainer *);
	int16_t		rndcnt;
	estimatorBase	**Estimators;
	int16_t		windowsinFrame;
	int16_t		periodforPilots;
	int16_t		periodforSymbols;
	double		**W_symbol_blk [10];
	float		f_cut_t;
	float		f_cut_k;
	trainer		**theTrainers;
	DSPCOMPLEX	**pilotEstimates;
	int16_t		trainers_per_window [10];
	int16_t		symbols_to_delay;
	int16_t		symbols_per_window;
	void		processSymbol	(int16_t, theSignal *);
	int16_t		Ts;
	int16_t		Tu;
	int16_t		Tg;
};

#endif

