#
/*
 *    Copyright (C) 2015 .. 2017
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

#ifndef	__WORD_COLLECTOR__
#define	__WORD_COLLECTOR__

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
class	simpleBuf;
class	drmDecoder;
class	wordCollector:public QObject {
Q_OBJECT
public:
			wordCollector 		(drmDecoder *,
	                                         Reader *,
	                                         smodeInfo *,
	                                         int32_t);
			~wordCollector 		(void);
	void		getWord			(std::complex<float> *,
	                                         int32_t,
	                                         float,
	                                         float);
	void		getWord			(std::complex<float> *,
	                                         int32_t,
	                                         bool,
	                                         float,
	                                         float,
	                                         float);
private:
	shifter		theShifter;

	smodeInfo	*modeInf;
	uint32_t	bufMask;
	void		fft_and_extract		(std::complex<float> *,
	                                         std::complex<float> *);
	Reader		* buffer;
	int32_t		sampleRate;
	uint8_t		Mode;
	uint8_t		Spectrum;
	drmDecoder	*master;
	float		theAngle;
	float		sampleclockOffset;
	int16_t		Tu;
	int16_t		Ts;
	int16_t		Tg;
	int16_t		K_min;
	int16_t		K_max;
	int16_t		displayCount;
	DSPCOMPLEX	*fft_vector;
	fftwf_plan	hetPlan;
	float		get_timeOffset		(int, int);
	int		get_intOffset		(int base,
                                                 int nrSymbols, int range);
	double		compute_mmse		(int, int);
signals:
	void		show_coarseOffset	(float);
	void		show_fineOffset		(float);
	void		show_timeOffset		(float);
	void		show_timeDelay		(float);
	void		show_clockOffset	(float);
	void		show_angle		(float);
};

#endif

