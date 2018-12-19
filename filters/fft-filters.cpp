#
/*
 *    Copyright (C) 2008, 2009, 2010
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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
 */

#include	"fft-filters.h"
#include	<cstring>

//
//	Straight forward implementation of lowpass and bandpass
//	filters using an DFT.
	fftFilter::fftFilter (int32_t size,
	                      int16_t degree) {
int32_t	i;

	fftSize		= size;
	filterDegree	= degree;
	OverlapSize	= filterDegree;
	NumofSamples	= fftSize - OverlapSize;

	MyFFT		= new common_fft	(fftSize);
	FFT_A		= MyFFT		-> 	getVector ();
	MyIFFT		= new common_ifft	(fftSize);
	FFT_C		= MyIFFT	->	getVector ();

	FilterFFT	= new common_fft	(fftSize);
	filterVector	= FilterFFT	->	getVector ();
	RfilterVector	= new float [fftSize];

	Overloop	= new DSPCOMPLEX [OverlapSize];
	inp		= 0;
	for (i = 0; i < fftSize; i ++) {
	   FFT_A [i] = 0;
	   FFT_C [i] = 0;
	   filterVector [i] = 0;
	   RfilterVector [i] = 0;
	}
	blackman	= new float [filterDegree];
	for (i = 0; i < filterDegree; i ++)
	   blackman [i] = (0.42 -
		    0.5 * cos (2 * M_PI * (float)i / (float)filterDegree) +
		    0.08 * cos (4 * M_PI * (float)i / (float)filterDegree));
}

	fftFilter::~fftFilter () {
	delete		MyFFT;
	delete		MyIFFT;
	delete		FilterFFT;
	delete[]	RfilterVector;
	delete[]	Overloop;
	delete[]	blackman;
}

//
//	set the band to a new value, i.e. create a new kernel
void	fftFilter::setBand (int32_t low, int32_t high, int32_t rate) {
float	tmp [filterDegree];
float	lo	= (float)((high - low) / 2) / rate;
float	shift	= (float) ((high + low) / 2) / rate;
float	sum	= 0.0;
int16_t	i;

	for (i = 0; i < filterDegree; i ++) {
	   if (i == filterDegree / 2)
	      tmp [i] = 2 * M_PI * lo;
	   else 
	      tmp [i] = sin (2 * M_PI * lo *
	                  (i - filterDegree /2)) / (i - filterDegree / 2);
//
//	windowing, according to Blackman
	   tmp [i]  *= blackman [i];
	   sum += tmp [i];
	}
//
//	Now the modulation
	for (i = 0; i < filterDegree; i ++) {	// shifting
	   float v = (i - filterDegree / 2) * (2 * M_PI * shift);
	   filterVector [i] = DSPCOMPLEX (tmp [i] * cos (v) / sum, 
	                                  tmp [i] * sin (v) / sum);
	}

	memset (&filterVector [filterDegree], 0,
	                (fftSize - filterDegree) * sizeof (DSPCOMPLEX));
	FilterFFT	-> do_FFT ();
	inp		= 0;
}

void	fftFilter::setLowPass (int32_t low, int32_t rate) {
int32_t	i;
LowPassFIR	*LowPass	= new LowPassFIR ((int16_t)filterDegree,
	                                          low,
	                                          rate);

	for (i = 0; i < filterDegree; i ++)
	   filterVector [i] = (LowPass -> getKernel ()) [i];
	memset (&filterVector [filterDegree], 0,
	                (fftSize - filterDegree) * sizeof (DSPCOMPLEX));
	FilterFFT	-> do_FFT ();
	inp	= 0;
	delete LowPass;
}

float	fftFilter::Pass (float x) {
int32_t		j;
float	sample;

	sample	= real (FFT_C [inp]);
	FFT_A [inp] = x;

	if (++inp >= NumofSamples) {
	   inp = 0;
	   memset (&FFT_A [NumofSamples], 0,
	               (fftSize - NumofSamples) * sizeof (DSPCOMPLEX));
	   MyFFT	-> do_FFT ();

	   for (j = 0; j < fftSize; j ++) {
	      FFT_C [j] = FFT_A [j] * filterVector [j];
              FFT_C [j] = DSPCOMPLEX (real (FFT_C [j]) * 10,
                                      imag (FFT_C [j]) * 10);
	   }

	   MyIFFT	-> do_IFFT ();
	   for (j = 0; j < OverlapSize; j ++) {
	      FFT_C [j] += Overloop [j];
	      Overloop [j] = FFT_C [NumofSamples + j];
	   }
	}

	return sample;
}

DSPCOMPLEX	fftFilter::Pass (DSPCOMPLEX z) {
DSPCOMPLEX	sample;
int16_t		j;

	sample	= FFT_C [inp];
	FFT_A [inp] = z;

	if (++inp >= NumofSamples) {
	   inp = 0;
	   memset (&FFT_A [NumofSamples], 0,
	               (fftSize - NumofSamples) * sizeof (DSPCOMPLEX));
	   MyFFT	-> do_FFT ();

	   for (j = 0; j < fftSize; j ++) 
	      FFT_C [j] = FFT_A [j] * filterVector [j];

	   MyIFFT	-> do_IFFT ();
	   for (j = 0; j < OverlapSize; j ++) {
	      FFT_C [j] += Overloop [j];
	      Overloop [j] = FFT_C [NumofSamples + j];
	   }
	}

	return sample;
}


