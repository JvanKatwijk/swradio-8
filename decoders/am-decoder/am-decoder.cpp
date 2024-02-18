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
#include	"am-decoder.h"
#include	"radio.h"
#include	"fir-filters.h"

#define	DC_ALPHA	0.98

	amDecoder::amDecoder (int32_t		inRate,
	                      RingBuffer<DSPCOMPLEX> *buffer,
	                      QSettings		*settings) :
	                           virtualDecoder (inRate, buffer) {
	this	-> inRate	= inRate;
	(void)settings;
	audioOut		= buffer;
	myFrame			= new QFrame;
	setupUi (myFrame);
	myFrame	-> show ();
	m_w1			= 0;
	lowpassFilter		= new LowPassFIR (5, 0.45 * inRate, inRate);
	adaptiveFiltersize	= 15;		/* the default	*/
	adaptive		= new adaptiveFilter (15, 0.2);
	adaptiveFiltering 	= false;
	connect (adaptiveFilterButton, SIGNAL (clicked ()),
	         this, SLOT (set_adaptiveFilter ()));
	connect (adaptiveFilterSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_adaptiveFiltersize (int)));
	connect (lowpassFilterslider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_lowpassFilter (int)));
	adaptiveFiltering	= false;
	adaptiveFilterSlider	-> hide ();	
	counter			= 0;
	
}

	amDecoder::~amDecoder () {
	delete	adaptive;
	delete	lowpassFilter;
	delete	myFrame;
}

void	amDecoder::set_adaptiveFilter () {
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

void	amDecoder::set_adaptiveFiltersize (int d) {
	if (d < 2)
	   return;
	adaptiveFiltersize	= d;
	delete	adaptive;
	adaptive	= new adaptiveFilter	(adaptiveFiltersize, 0.2);
}

void	amDecoder::processBuffer	(DSPCOMPLEX *buffer, int32_t amount) {
int32_t	i;

	for (i = 0; i < amount; i ++)
	   process (buffer [i]);
}

void	amDecoder::process	(std::complex<float> z) {
float	mag	= abs (z);

//	HF filtering according to CuteSDR
        float w0	= mag + (DC_ALPHA * m_w1);
        float res	= w0 - m_w1;
        m_w1		= w0;

	std::complex<float> t =
	           lowpassFilter -> Pass (std::complex<float> (res, res));
	audioOut	-> putDataIntoBuffer (&t, 1);

	counter ++;
	if (counter > inRate / 10) { 
	   setDetectorMarker (0);
	   counter	= 0;
	   emit audioAvailable (audioOut -> GetRingBufferReadAvailable () / 10,
	                                                inRate);
	}
}

void	amDecoder::set_lowpassFilter (int v) {

	delete lowpassFilter;
	lowpassFilter		= new LowPassFIR (5, 
	                                          float (v) * inRate / 101,
	                                          inRate);
}

