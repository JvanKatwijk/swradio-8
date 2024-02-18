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
#include	"ssb-decoder.h"
#include	"radio.h"
#include	"fir-filters.h"

#define	USB_MODE	0100
#define	LSB_MODE	0101

	ssbDecoder::ssbDecoder (int32_t		inRate,
	                        RingBuffer<DSPCOMPLEX> *buffer,
	                        QSettings		*settings):
	                           virtualDecoder (inRate, buffer) {
	this	-> inRate	= inRate;
	(void)settings;
	myFrame			= new QFrame;
	setupUi (myFrame);
	myFrame	-> show ();
	lowpassFilter		= new LowPassFIR (5, 0.45 * inRate, inRate);
	adaptiveFiltersize	= 15;		/* the default	*/
	adaptive		= new adaptiveFilter (15, 0.2);
	adaptiveFiltering 	= false;
	connect (adaptiveFilterButton, SIGNAL (clicked (void)),
	         this, SLOT (set_adaptiveFilter (void)));
	connect (adaptiveFilterSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_adaptiveFiltersize (int)));
	connect (lowpassFilterslider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_lowpassFilter (int)));
	connect (submodeSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (set_mode (const QString &)));
	adaptiveFiltering	= false;
	adaptiveFilterSlider	-> hide ();	
	counter			= 0;
	mode			= USB_MODE;
}

	ssbDecoder::~ssbDecoder (void) {
	myFrame	-> hide ();
	delete	adaptive;
	delete	lowpassFilter;
	delete	myFrame;
}

void	ssbDecoder::set_adaptiveFilter (void) {
	adaptiveFiltering	= !adaptiveFiltering;
	if (adaptiveFiltering) {
	   adaptiveFilterButton -> setText ("on");
	   adaptiveFilterSlider	-> show ();
	}
	else {
	   adaptiveFilterButton	-> setText ("filter");
	   adaptiveFilterSlider	-> hide ();
	}
}

void	ssbDecoder::set_adaptiveFiltersize (int d) {
	if (d < 2)
	   return;
	adaptiveFiltersize	= d;
	delete	adaptive;
	adaptive	= new adaptiveFilter	(adaptiveFiltersize, 0.2);
}

void	ssbDecoder::processBuffer	(DSPCOMPLEX *buffer, int32_t amount) {
int32_t	i;

	for (i = 0; i < amount; i ++) 
	   process (buffer [i]);
}

void	ssbDecoder::process (std::complex<float> z) {
	std::complex<float> temp	= z;
	if (mode == USB_MODE)
	   temp = std::complex<float> (real (temp) - imag (temp),
	                               real (temp) - imag (temp));
	else
	   temp = std::complex<float> (real (temp) + imag (temp),
	                               real (temp) + imag (temp));
	   
	temp		= lowpassFilter -> Pass (temp);
	if (adaptiveFiltering)
	   temp =  adaptive -> Pass (temp);
	audioOut	-> putDataIntoBuffer (&temp, 1);

	if (++counter > inRate / 10) {
           setDetectorMarker (0);
           counter      = 0;
	   emit	audioAvailable (inRate / 10, inRate);
        }
}

void	ssbDecoder::set_lowpassFilter (int v) {

	delete lowpassFilter;
	lowpassFilter		= new LowPassFIR (5, 
	                                          float (v) * inRate / 101,
	                                          inRate);
}

void	ssbDecoder::set_mode	(const QString &s) {
	if (s == "mode usb")
	   mode = USB_MODE;
	else
	   mode = LSB_MODE;
}

