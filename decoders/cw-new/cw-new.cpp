#
/*
 *    Copyright (C) 2010, 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
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
#
#include	<QSettings>
#include	<QFrame>
#include	<QWidget>
#include	<qwt_dial.h>
#include	<qwt_dial_needle.h>
#include	"cw-new.h"
#include	"iir-filters.h"
#include	"shifter.h"
#include	"radio.h"

#define	USECS_PER_SEC		1000000
#define	MODE_IDLE		0100
#define	MODE_IN_TONE		0200
#define	MODE_AFTER_TONE		0300
#define	MODE_END_OF_LETTER	0400

#define	DOTLENGTH		1250
//
//	dotlength = 1.2 / speed (WPM) seconds.
//	So 20 words per minute (just an example) means 1.2 / 20 = 1200 / 20
//	60 milliseconds
//	For a 12000 samples/second signal this means app 5000 samples
//	If we want to be able to identiy dots where at least 4 successive
//	FFT elements carry a positive value, then we need an FFT taken
//	once per 1250 samples
//
#define	CW_DOT_REPRESENTATION	'.'
#define	CW_DASH_REPRESENTATION	'_'
#define	CW_IF			0
/*
 */

		cwDecoder::cwDecoder (RadioInterface *mir,
	                              int32_t rate,
	                              RingBuffer<std::complex<float> > *b,
	                              QSettings *s):
	                                   myFrame	(nullptr),
	                                   virtualDecoder (rate, b),
	                                   localShifter   (rate) {
	theRate		= rate;
	inputRate	= 12000;
	audioData	= b;
	cwSettings	= s;
	setupUi (&myFrame);
	myFrame. show ();

	fftBuffer	= (std::complex<float> *) fftwf_malloc (sizeof (std::complex<float>) * inputRate);
	 plan    = fftwf_plan_dft_1d (inputRate,
                                    reinterpret_cast <fftwf_complex *>(fftBuffer),
                                    reinterpret_cast <fftwf_complex *>(fftBuffer),
                                    FFTW_FORWARD, FFTW_ESTIMATE);

        x_axis          = new double [inputRate];
        for (int i = 0; i < inputRate; i ++)
	   x_axis [i] = - inputRate / 2 + i;
        y_values        = new double [inputRate];
        cwViewer       = new waterfallScope (cwScope,
                                             inputRate, 80);
	buffp		= 0;
}
//
//
	cwDecoder::~cwDecoder	() {
	fftwf_free (fftBuffer);
	fftwf_destroy_plan (plan);
}

int32_t	cwDecoder::rateOut		() {
	return theRate;
}

void	cwDecoder::process	(std::complex<float> s) {
	fftBuffer [buffp] = s;
	buffp ++;
	if (buffp > DOTLENGTH) { // one dot
	   for (int i = DOTLENGTH; i < inputRate; i ++)
	      fftBuffer [i] = std::complex<float> (0, 0);
	   addBuffer (fftBuffer);
	   buffp = 0;
	}
}

void	cwDecoder::addBuffer	(std::complex<float> *v) {
	fftwf_execute (plan);
	for (int i = 0; i < inputRate / 2; i ++) {
	   y_values [inputRate / 2 + i] = abs (fftBuffer [i]);
	   y_values [i] = abs (fftBuffer [inputRate / 2 + i]);
	}

	cwViewer	-> display (x_axis, y_values,
	                            amplitudeSlider -> value (), 0, 0);
}
