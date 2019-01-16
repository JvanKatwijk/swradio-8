#
/*
 *    Copyright (C) 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the swradio
 *    swradio is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    swradio is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with swradio; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#include	"test-decoder.h"
#include	"radio.h"
#include	"cache.h"

	testDecoder::testDecoder (int32_t	inRate,
	                          RingBuffer<DSPCOMPLEX> *buffer,
	                          QSettings		*settings) :
	                           virtualDecoder (inRate, buffer) {
	myFrame			= new QFrame;
	setupUi (myFrame);
	myFrame	-> show ();
	this	-> inputRate	= inRate;
	this	-> outputRate	= 600;
	this	-> samplesperSymbol	= 64;

	x_axis			= new double [samplesperSymbol];
	for (int i = 0; i < samplesperSymbol; i ++)
	   x_axis [i] = i - samplesperSymbol / 2;
	y_values		= new double [samplesperSymbol];
	theFilter		= new decimatingFIR (155, 50, inputRate, inputRate / 120);
	theSlider		= new slidingFFT (240,
	                                           60 - samplesperSymbol / 2,
	                                           60 + samplesperSymbol / 2 - 1);
	the_fft			= new common_fft (240);
	fftBuffer		= the_fft	-> getVector ();
	theCache		= new Cache (samplesperSymbol,
	                                     samplesperSymbol);
	Viewer			= new waterfallScope (testSpectrum,
	                                              samplesperSymbol, 30);
	counter			= 30;
	cacheLineP		= 0;
	fillP			= 0;
	cacheSize		= 30;
}

	testDecoder::~testDecoder (void) {
	delete	myFrame;
	delete	theFilter;
}

void	testDecoder::processBuffer	(std::complex<float> *buffer,
	                                                 int32_t amount) {
int	i;

	for (i = 0; i < amount; i ++) 
	   process (buffer [i]);
}

void    testDecoder::process      (std::complex<float> z) {
int i, j;
std::complex<float> res;
std::complex<float> v [240];

	if (theFilter -> Pass (z, &res)) {
	   fftBuffer [fillP++] = res;
	   if (fillP < 60)
	      return;

	   for (i = fillP; i < 240; i ++)
	      fftBuffer [i] = std::complex<float> (0, 0);
	   the_fft -> do_FFT ();

	   for (i = 0; i < 120; i ++) {
	      v [i] = fftBuffer [120 + i];
	      v [120 + i] = fftBuffer [i];
	   }

	   for (i = 0; i < samplesperSymbol; i ++)
	      (theCache -> cacheLine (cacheLineP)) [i] =
	                      v [120 - samplesperSymbol / 2 + i];
	   for (i = 0; i < samplesperSymbol; i ++)
	      y_values [i] = abs (theCache -> cacheElement (cacheLineP, i));

	   Viewer -> display (x_axis, y_values,
	                         amplitudeSlider -> value (),
	                         0, 0);
	   cacheLineP ++;
	   if (cacheLineP >= cacheSize)
	      cacheLineP = 0;
	   fillP = 0;
	}
}

