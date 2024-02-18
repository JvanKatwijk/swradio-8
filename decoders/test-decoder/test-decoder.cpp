#
/*
 *    Copyright (C) 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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
	                           virtualDecoder (inRate, buffer),
	                           baseFilter (55, 4500, inRate),
	                           s2Filter	(8192),
	                           s3Filter	(8192),
	                           usbFilter	(8192, 511),
	                           lsbFilter	(8192, 511) {

	myFrame			= new QFrame;
	setupUi (myFrame);
	myFrame	-> show ();
	this	-> inputRate	= inRate;
	this	-> outputRate	= 12000;
	this	-> audioBuffer	= buffer;
	for (int i = 0; i < 512; i ++)
	   x_axis [i] = i;
	the_fft			= new common_fft (1024);
	fftBuffer		= the_fft	-> getVector ();
	Viewer			= new spectrumScope (testSpectrum, 512);
	counter			= 30;
	connect (Viewer, SIGNAL (clickedwithLeft (int)),
	         this, SLOT (handleClick (int)));
	usbFilter. setBand (40, 4000, inRate);
	lsbFilter. setBand (-4000, -40, inRate);
}

	testDecoder::~testDecoder (void) {
	delete	myFrame;
}

void	testDecoder::processBuffer	(std::complex<float> *buffer,
	                                                 int32_t amount) {
int	i;

	for (i = 0; i < amount; i ++) 
	   process (buffer [i]);
}
//
//	
void    testDecoder::process      (std::complex<float> z) {
static int counter = 0;
static std::complex<float> old_usb	= std::complex<float> (0, 0);
static std::complex<float> old_lsb	= std::complex<float> (0, 0);

	z	= baseFilter. Pass (z);
	z	= std::complex<float> (real (z) / abs (z),
	                               imag (z) / abs (z));
	std::complex<float> usbBand	= usbFilter. Pass (z);
	std::complex<float> lsbBand	= lsbFilter. Pass (z);
	std::complex<float> USB = s2Filter. Pass (real (usbBand) - imag (usbBand));
	std::complex<float> LSB = s3Filter. Pass (real (lsbBand) + imag (lsbBand));

	float plusSignal	= arg (USB);
	float minusSignal	= arg (LSB);
//	float plusSignal	= arg (USB * conj (old_usb));
	old_usb			= USB;
	old_lsb			= LSB;

	std::complex<float> combined =
	                      std::complex<float> (minusSignal, minusSignal);
	audioBuffer -> putDataIntoBuffer (&combined, 1);
	counter ++;
        if (counter > inputRate / 10) {
           setDetectorMarker (0);
           counter      = 0;
           emit audioAvailable (audioOut -> GetRingBufferReadAvailable () / 10,
                                                        inputRate);
        }
}

void    testDecoder::handleClick (int a) {
	fprintf (stderr, "handling click for %d\n", a);
        adjustFrequency (a);
}

