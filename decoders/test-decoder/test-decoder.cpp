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
	this	-> samplesperSymbol	= 60;

	x_axis			= new double [samplesperSymbol];
	for (int i = 0; i < samplesperSymbol; i ++)
	   x_axis [i] = (i * 0.73 - samplesperSymbol / 2 * 0.73);
	y_values		= new double [samplesperSymbol];
	theFilter		= new decimatingFIR (55, 100, inputRate, inputRate / 750);
	the_fft			= new common_fft (1024);
	fftBuffer		= the_fft	-> getVector ();
	theCache		= new Cache (samplesperSymbol,
	                                     4 * 162);
	Viewer			= new waterfallScope (testSpectrum,
	                                              samplesperSymbol, 30);
	counter			= 30;
	cacheLineP		= 0;
	fillP			= 0;
	cacheSize		= 4 * 162;

	connect (Viewer, SIGNAL (clickedwithLeft (int)),
	         this, SLOT (handleClick (int)));
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
//
//	The tone distance is 1.4648 Hz, the tone duration is
//	1/1.4648 = 0.68 seconds.
//	we want to be able to separate half tones, i.e. two bins
//	per tone. This means a bin-width of 0.732 Hz.
//	If we take an FFT of 1024 bins, that means a samplerate
//	of 750. In order to get 4 lines for each tone instance, we
//	need 750 * 0.68 * 0.25 samples to create an FFT, i.e. 127/128
//	samples.
//	
void    testDecoder::process      (std::complex<float> z) {
int i, j;
std::complex<float> res;
std::complex<float> v [1024];
static	bool switcher	= false;

	if (theFilter -> Pass (z, &res)) {
	   fftBuffer [fillP++] = cmul (res, 2);
	   if (fillP <  (switcher ? 128 : 127)) 
	      return;

	   switcher = !switcher;
	   for (i = fillP; i < 1024; i ++)
	      fftBuffer [i] = std::complex<float> (0, 0);
	   the_fft -> do_FFT ();

	   for (i = 0; i < 1024 / 2; i ++) {
	      v [i] = fftBuffer [1024 / 2 + i];
	      v [1024 / 2 + i] = fftBuffer [i];
	   }

	   for (i = 0; i < samplesperSymbol; i ++)
	      (theCache -> cacheLine (cacheLineP)) [i] =
	                      v [1024 / 2 - samplesperSymbol / 2 + i];
	   for (i = 0; i < samplesperSymbol; i ++)
	      y_values [i] = abs (theCache -> cacheElement (cacheLineP, i));

	   Viewer -> display (x_axis, y_values,
	                      amplitudeSlider -> value (),
	                      0, 0);
	   cacheLineP ++;

	   if (cacheLineP % 20 == 0) {
	      int maxLine = -1;
	      float maxLineSum = 0;
	      for (i = 10; i < samplesperSymbol - 10; i ++) {
	         float sum = 0;
	         for (j = 0; j < 50; j ++)
	            for (int k = 0; k < 8; k ++)
	               sum += abs (theCache -> cacheElement (j, i + k));
	         if (sum > maxLineSum) {
	            maxLineSum = sum;
	            maxLine    = i;
	         }
	      }
	      fprintf (stderr, "maxLine = %d\n", maxLine);
	   }
	
	   if (cacheLineP >= cacheSize) 
	      cacheLineP = 0;
	   fillP = 0;
	   
	}
}

void    testDecoder::handleClick (int a) {
	fprintf (stderr, "handling click for %d\n", a);
        adjustFrequency (a);
}

