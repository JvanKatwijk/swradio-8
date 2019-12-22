#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair programming
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
#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>
#include	"freqsyncer.h"
#include	"reader.h"
#include	"drm-decoder.h"
#include	"basics.h"
#include	"referenceframe.h"

//	The freqsyncer will handle segments of size Ts,
//	and do all kinds of frequency correction (timecorrection
//	is done in the syncer) and it will give an estimate of
//	the "coarse" //	frequency offset and a spectrum
//
//	The frequency shifter is in steps of 0.01 Hz
	freqSyncer::freqSyncer (Reader		*b,
	                        smodeInfo	*m,
	                        int32_t		sampleRate,	
	                        drmDecoder	*mr):
	                                 theShifter (100 * sampleRate) {
int16_t	i;
	this	-> buffer	= b;
	this	-> Mode		= m -> Mode;
	this	-> Spectrum	= m -> Spectrum;
	this	-> sampleRate	= sampleRate;
	this	-> theAngle	= 0;
	this	-> Tu		= Tu_of (Mode);
	this	-> Ts		= Ts_of (Mode);
	this	-> Tg		= Tg_of (Mode);
	this	-> displayCount	= 0;
	this	-> N_symbols	= symbolsperFrame (Mode);

//	for detecting pilots:
	int16_t k_pilot [3];
	int16_t cnt	= 0;

	for (i = 0; i < Tu / 2; i ++) {
	   if (isFreqCell (Mode, 0, i)) {
	      k_pilot [cnt ++] = i;
	      if (cnt > 3)
	         fprintf (stderr, "Hellup\n");	// does not happen
	   }
	}

	this	-> k_pilot1 = k_pilot [0] + Tu / 2;
	this	-> k_pilot2 = k_pilot [1] + Tu / 2;
	this	-> k_pilot3 = k_pilot [2] + Tu / 2;

	this	-> bufferIndex	= 0;
	this	-> symbolBuffer	= new std::complex<float> *[N_symbols];
	for (i = 0; i < N_symbols; i ++)
	   symbolBuffer [i] = new std::complex<float> [Tu_of (Mode)];
	fft_vector		= (std::complex<float> *)
	                               fftwf_malloc (Tu_of (Mode) *
	                                            sizeof (fftwf_complex));
	hetPlan			= fftwf_plan_dft_1d (Tu_of (Mode),
	                    reinterpret_cast <fftwf_complex *>(fft_vector),
	                    reinterpret_cast <fftwf_complex *>(fft_vector),
	                    FFTW_FORWARD, FFTW_ESTIMATE);
	connect (this, SIGNAL (show_fineOffset (float)),
	         mr, SLOT (show_fineOffset (float)));
	connect (this, SIGNAL (show_coarseOffset (float)),
	         mr, SLOT (show_coarseOffset (float)));
	connect (this, SIGNAL (show_timeDelay	(float)),
	         mr, SLOT (show_timeDelay (float)));
	connect (this, SIGNAL (show_timeOffset	(float)),
	         mr, SLOT (show_timeOffset (float)));
	connect (this, SIGNAL (show_clockOffset (float)),
	         mr, SLOT (show_clockOffset (float)));
	connect (this, SIGNAL (show_angle (float)),
	         mr, SLOT (show_angle (float)));
}

		freqSyncer::~freqSyncer (void) {
int16_t	i;
	fftwf_free (fft_vector);
	fftwf_destroy_plan (hetPlan);
	for (i = 0; i < N_symbols; i ++)
	   delete[]  symbolBuffer [i];
	delete[] symbolBuffer;
}

bool freqSyncer::frequencySync (smodeInfo *m) {
int16_t	i;
int32_t	localIndex	= 0;
float	occupancyIndicator [6];
uint8_t	spectrum;

	buffer	-> waitfor (N_symbols * Ts + Ts);
//
//	first, load spectra for the first N_symbol symbols
//	into the (circular) buffer "symbolBuffer",
//	However: do not move the "currentIndex" in the Reader,
//	i.e. do not consume the words from the input
	for (i = 0; i < N_symbols; i ++) {
	 int16_t  time_offset_integer =
	          getWord (buffer -> data,
	                   buffer -> bufSize,
	                   buffer -> currentIndex + localIndex,
	                   i,
	                   m -> timeOffset_fractional);
	   localIndex += Ts + time_offset_integer;
	}
//
//	our version of get_zeroBin returns the "bin" number
//	of the bin with the highest energy level.
	int32_t	binNumber;
	binNumber = get_zeroBin (0);
	m -> freqOffset_integer	= binNumber * sampleRate / Tu;
//
	binNumber = binNumber < 0 ? binNumber + Tu : binNumber;
	for (i = 0; i < 4; i ++) 
	   occupancyIndicator [i] = get_spectrumOccupancy (i, binNumber);

	float tmp1	= 0.0;
	for (spectrum = 1; spectrum < 4; spectrum ++) {	
	   if (occupancyIndicator [spectrum] >= tmp1) {
	      tmp1 = occupancyIndicator [spectrum];
	      m -> Spectrum = spectrum;
	   }
	}
	return true;
}

//
//	get_zeroBin cooperates with the local version of getWord.
//	It uses the two dimensional array "symbolBuffer" with
//	in the rows the computed spectra of the last N_symbols ofdm words,
//	starting at start (i.e. circular)
//
int32_t	freqSyncer::get_zeroBin (int16_t start) {
int16_t i, j;
std::complex<float> correlationSum [Tu];
float	abs_sum [Tu];

//	accumulate phase diffs of all carriers 
//	we know that the correlation for the (frequency) pilot values
//	in subsequent symbols should be high, while
//	random correlations have low correlation.
//	Alternatively, we could add up the abs vales of the "bin"s
//	and look for the highest (near the carriers where the pilots
//	are assumed to be). Still not sure what is the best
	memset (correlationSum, 0, Tu * sizeof (std::complex<float>));
	memset (abs_sum, 0, Tu * sizeof (float));

//	accumulate phase diffs of all carriers in subsequent symbols
	for (j = start + 1; j < start + N_symbols; j++) {
	   int16_t jmin1 	= (j - 1) % N_symbols;
	   int16_t jj		= j % N_symbols;
	   for (i = 0; i < Tu; i++) {
	      std::complex<float> tmp1 = symbolBuffer [jmin1][i] *
	                                        conj (symbolBuffer [jj][i]);
	      correlationSum [i] += tmp1;
	   }
	}
//
	for (i = 0; i < Tu; i++) 
	   abs_sum [i] = abs (correlationSum [i]);

	float	highest		= -1.0E20;
	int	dcOffset	= 0;
//
//	recall that the pilots are relative to -Tu / 2
	for (i = - Tu / 10; i < Tu / 10; i ++) {
	   float sum = abs_sum [k_pilot1 + i] +
	               abs_sum [k_pilot2 + i] +
	               abs_sum [k_pilot3 + i];
	   if (sum > highest) {
	      dcOffset	= i;
	      highest	= sum;
	   }
	}

	return dcOffset;
}

float	freqSyncer::get_spectrumOccupancy (uint8_t spectrum,
	                                   int16_t baseBin) {
int16_t	i, j;
int16_t K_min_ = Kmin (Mode, spectrum);
int16_t K_max_ = Kmax (Mode, spectrum);

	if (K_min_ == K_max_) 	// should not happen
	   return 0;

//	now we go

//	The indices of the "lowest" and the "highest" carrier.
	int K_min_indx		= (Tu / 2 + baseBin + K_min_) % (Tu);
	int K_max_indx		= (Tu / 2 + baseBin + K_max_) % (Tu);

	float tmp3	= 0;
	float tmp4	= 0;
	float tmp5	= 0;
	float tmp6	= 0;

	for (i = 0; i < N_symbols; i ++) {
//	near the carrier with the lowest index
	   for (j = 0; j < 5; j ++) {
	      tmp3 += real (symbolBuffer [i][K_min_indx - 4 - j] *
	                    conj (symbolBuffer [i][K_min_indx - 4 - j]));
	      tmp4 += real (symbolBuffer [i][K_min_indx + 4 + j] *
	                    conj (symbolBuffer [i][K_min_indx + 4 + j]));
	   }
//	near the carrier with the highest index
	   for (j = 0; j < 5; j ++) {
	      tmp5 += real (symbolBuffer [i][K_max_indx - 4 - j] *
	                    conj (symbolBuffer [i][K_max_indx - 4 - j]));
	      tmp6 += real (symbolBuffer [i][K_max_indx + 4 + j] *
	                    conj (symbolBuffer [i][K_max_indx + 4 + j]));
	   }
	}

	float energy_ratio_K_min_to_K_min_p4;
	float energy_ratio_K_max_to_K_max_p4;
//
//	some safety measure, we assume that the data that is to
//	supposed to be in the carriers with energy has substantially
//	higher value that the "outside" data
	if (tmp4 < 3 * tmp3)
	   energy_ratio_K_min_to_K_min_p4 = 0;
	else
	   energy_ratio_K_min_to_K_min_p4 = tmp4 / tmp3;
	if (tmp5 < 3 * tmp6)
	   energy_ratio_K_max_to_K_max_p4 = 0;
	else
	   energy_ratio_K_max_to_K_max_p4 = tmp5 / tmp6;
	return energy_ratio_K_max_to_K_max_p4 +
	       energy_ratio_K_min_to_K_min_p4;
}
//
//	In this local version of getWord, the input buffer is only looked
//	at, not read!! It is called by the frequency synchronizer
//	No reduction of the output to the Kmin .. Kmax useful
//	carriers is made, but the order of the low-high freqencies
//	is change to reflect the "lower .. higher" frequencies in order.
int16_t	freqSyncer::getWord (std::complex<float> *buffer,
	                     int32_t		bufSize,
	                     int32_t		theIndex,
	                     int16_t		wordNumber,
	                     float		offsetFractional) {
std::complex<float> temp [Ts];
int16_t		i;
int16_t		bufMask	= bufSize - 1;
std:;complex<float> angle	= std::complex<float> (0, 0);

//	To take into account the fractional timing difference,
//	we do some interpolation between samples in the time domain
	int f	= (int)(floor (theIndex)) & bufMask;
	if (offsetFractional < 0) {
	   offsetFractional = 1 + offsetFractional;
	   f -= 1;
	}
	for (i = 0; i < Ts; i ++) {
	   std::complex<float> een = buffer [(f + i) & bufMask];
	   std::complex<float> twee = buffer [(f + i + 1) & bufMask];
	   temp [i] = cmul (een, 1 - offsetFractional) +
	              cmul (twee, offsetFractional);
	}

//	Now: estimate the fine grain offset.
	for (i = 0; i < Tg; i ++)
	   angle += conj (temp [Tu + i]) * temp [i];
//	simple averaging:
	theAngle	= 0.9 * theAngle + 0.1 * arg (angle);

//	offset in Hz / 100
	float offset	= theAngle / (2 * M_PI) * 100 * sampleRate / Tu;
	if (++displayCount > 20) {
	   displayCount = 0;
	   show_coarseOffset	(0);
	   show_fineOffset	(- offset / 100);
	   show_angle		(arg (angle));
	   show_timeDelay	(offsetFractional);
	}

	if (abs (offset) > 2300) {
	   fprintf (stderr, "angle = %f %f, offset = %f (sampleRate %d, Tu %d)\n",
	             arg (angle), theAngle, offset, sampleRate, Tu);
	   offset = offset < 0 ? -23 : 23;
	}
	   
	if (offset == offset)	// precaution to handle undefines
	   theShifter. do_shift (temp, Ts, -offset);

//	and extract the Tu set of samples for fft processsing
	memcpy (fft_vector, &temp [Tg], Tu * sizeof (std::complex<float>));

	fftwf_execute (hetPlan);
	memcpy (symbolBuffer [wordNumber],
	        &fft_vector [Tu / 2], Tu / 2 * sizeof (std::complex<float>));
	memcpy (&symbolBuffer [wordNumber] [Tu / 2],
	        fft_vector , Tu / 2 * sizeof (std::complex<float>));
	return 0;
}

