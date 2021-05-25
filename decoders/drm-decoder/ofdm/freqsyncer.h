#
/*
 *    Copyright (C) 2014
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

#ifndef	__FREQ_SYNCER__
#define	__FREQ_SYNCER__

#include	<QObject>
#include	"basics.h"
#include	"radio-constants.h"
#include	<stdio.h>
#include	<stdlib.h>
#include	<cstring>
#include	<math.h>
#include	<fftw3.h>
#include	"shifter.h"

class	Reader;
class	drmDecoder;

class	freqSyncer:public QObject {
Q_OBJECT
public:
			freqSyncer 		(Reader *,
	                                         smodeInfo *,
	                                         int32_t,
	                                         drmDecoder *);
			~freqSyncer 		(void);
	bool		frequencySync 		(smodeInfo *);
private:
	int16_t		getWord			(DSPCOMPLEX *,
	                                         int32_t,
	                                         int32_t,
	                                         int16_t,
	                                         int,
	                                         float);
	int32_t		get_zeroBin 		(int16_t);
	float		get_spectrumOccupancy	(uint8_t,
	                                         int16_t);
	Reader		* buffer;
	int32_t		sampleRate;
	uint8_t		Mode;
	uint8_t		Spectrum;
	int16_t		k_pilot1;
	int16_t		k_pilot2;
	int16_t		k_pilot3;
	float		theAngle;
	int16_t		Tu;
	int16_t		Ts;
	int16_t		Tg;
	int16_t		displayCount;
	int16_t		N_symbols;
	int16_t		bufferIndex;
	DSPCOMPLEX	**symbolBuffer;
	DSPCOMPLEX	*fft_vector;
	fftwf_plan	hetPlan;
	shifter		theShifter;
//
signals:
	void		show_coarseOffset	(float);
	void		show_fineOffset		(float);
	void		show_timeOffset		(float);
	void		show_timeDelay		(float);
	void		show_clockOffset	(float);
	void		show_angle		(float);
};

#endif

