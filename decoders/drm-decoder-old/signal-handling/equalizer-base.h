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
#ifndef	__EQUALIZER_BASE__
#define	__EQUALIZER_BASE__

#include	<QObject>
#include	<QString>
#include	"basics.h"
#include	"radio-constants.h"

class	equalizer_base: public QObject {
Q_OBJECT
public:
			equalizer_base 	(uint8_t Mode, uint8_t Spectrum);
virtual			~equalizer_base	(void);
virtual	bool		equalize	(std::complex<float> *,
	                                 int16_t, theSignal **);
virtual	bool		equalize	(std::complex<float> *,
	                                 int16_t, theSignal **,
	                                 int16_t *, float *, float *, float *);
	void		getEq		(int16_t, DSPCOMPLEX *);
	int16_t		indexFor	(int16_t);
	DSPCOMPLEX	**getChannels	(void);
	float		getMeanEnergy	(void);
signals:
	void		show_eqsymbol	(int);
protected:
	DSPCOMPLEX	**testFrame;
	DSPCOMPLEX	**refFrame;
	uint8_t		Mode;
	uint8_t		Spectrum;
	int16_t		K_min;
	int16_t		K_max;
	int16_t		symbolsinFrame;
	int16_t		carriersinSymbol;
	int16_t		actualRow		(int16_t, int16_t);
	float		meanEnergy;
	int16_t		nrCells;
	void		init_gain_ref_cells	(void);
//	int16_t		gain_ref_cells_k	[750];
//	DSPCOMPLEX	gain_ref_cells_v	[750];
};

#endif

