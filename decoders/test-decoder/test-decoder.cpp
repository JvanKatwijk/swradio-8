#
/*
 *    Copyright (C) 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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
#
#include	"test-decoder.h"
#include	"radio.h"


	testDecoder::testDecoder (int32_t	inRate,
	                          RingBuffer<DSPCOMPLEX> *buffer,
	                          QSettings		*settings) :
	                           virtualDecoder (inRate, buffer) {
	myFrame			= new QFrame;
	setupUi (myFrame);
	myFrame	-> show ();
	this	-> inputRate	= inRate;
	this	-> outputRate	= 600;
	
	theFilter		= new decimating_filter (inputRate, 512, 50);
	testScope		= new fftScope (testSpectrum,
                                                128,
                                                  1,
	                                        512,
	                                         50,
                                                  8);
	testScope	-> setScope (0, 0);
//	testScope	-> switch_viewMode  ();
	connect (amplitudeSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_amplitude (int)));
}

	testDecoder::~testDecoder (void) {
	delete	testScope;
	delete	myFrame;
	delete	theFilter;
}

void	testDecoder::processBuffer	(std::complex<float> *buffer,
	                                                 int32_t amount) {
int	i;
std::complex<float> res;

	fprintf (stderr, "%d ", amount);
	for (i = 0; i < amount; i ++)
	   if (theFilter -> Pass (buffer [i], &res))
	      testScope	-> addElements (&res, 1);
}

void	testDecoder::process		(std::complex<float> v) {
std::complex<float> res = v;
static	int	count	= 0;
int	i;

	if (theFilter -> Pass (v, &res)) {
	   testScope -> addElements (&res, 1);
	}
}

void	testDecoder::handle_amplitude	(int a) {
	testScope	-> setLevel (a);
}

