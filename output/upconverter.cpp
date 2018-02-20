#
/*
 *    Copyright (C) 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the  SDR-J 
 *    Many of the ideas as implemented in the SDR-J are derived from
 *    other work, made available through the (a) GNU general Public License. 
 *    All copyrights of the original authors are recognized.
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
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"upconverter.h"
#include	"audiosink.h"


	upConverter::upConverter (int32_t inRate, int32_t outRate,
	                                    audioSink *client) {
	int err;
	this	-> inRate	= inRate;
	this	-> outRate	= outRate;
	this	-> client	= client;

	inputLimit              = inRate / 10;
        ratio                   = double(outRate) / inRate;
        outputLimit             = inputLimit * ratio;
//	src_converter		= src_new (SRC_SINC_BEST_QUALITY, 2, &err);
	src_converter		= src_new (SRC_LINEAR, 2, &err);
//	src_converter		= src_new (SRC_SINC_MEDIUM_QUALITY, 2, &err);
        src_data                = new SRC_DATA;
        inBuffer                = new float [2 * inputLimit + 20];
        outBuffer               = new float [2 * outputLimit + 20];
        src_data-> data_in      = inBuffer;
        src_data-> data_out     = outBuffer;
        src_data-> src_ratio    = ratio;
        src_data-> end_of_input = 0;
        inp                     = 0;
}

	upConverter::~upConverter (void) {
}

void	upConverter::handle	(DSPCOMPLEX *buffer, int32_t amount) {
int16_t i;

	for (i = 0; i < amount; i ++) {
	   inBuffer [2 * inp]      = real (buffer [i]);
	   inBuffer [2 * inp + 1]  = imag (buffer [i]);
	   inp ++;
	   if (inp >= inputLimit) {
	      src_data	-> input_frames         = inp;
	      src_data  -> output_frames        = outputLimit + 10;
	      int res       = src_process (src_converter, src_data);
	      inp       = 0;
	      int framesOut       = src_data -> output_frames_gen;
	      for (i = 0; i < framesOut; i ++)
	         client -> putSample (DSPCOMPLEX (outBuffer [2 * i],
	                                          outBuffer [2 * i + 1]));
	   }
	}
}

