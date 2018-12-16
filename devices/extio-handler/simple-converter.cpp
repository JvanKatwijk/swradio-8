#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J.
 *    Many of the ideas as implemented in SDR-J are derived from
 *    other work, made available through the GNU general Public License. 
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
 *    along with SDR-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	<stdint.h>
#include	<simple-converter.h>

//	Very simple converter, the basic idea is that the
//	different (high speed) usb readers put their data
//	in, and the converter fills the outputbuffer with
//	data with the right (i.e. lower for the sw receiver)
//	samplerate
		converter::converter (int32_t rateIn,
	                              int32_t rateOut, RingBuffer<DSPCOMPLEX> *b) {
int	error;
	this	-> rateIn	= rateIn;
	this	-> rateOut	= rateOut;
	this	-> resultBuffer	= b;
	rabbit	= src_new (SRC_SINC_MEDIUM_QUALITY, 2, &error);
	(void)error;
}

		converter::~converter (void) {
	src_delete (rabbit);
	fprintf (stderr, "rabbit is deleted\n");
}

int32_t	converter::convert (DSPCOMPLEX *in, int32_t n) {
DSPCOMPLEX	temp [n];

	rabbit_data. data_in		= (float *)in;
	rabbit_data. data_out		= (float *)temp;
	rabbit_data. input_frames	= n;
	rabbit_data. output_frames	= int (float (rateOut) / rateIn * n) + 10;
	rabbit_data. end_of_input	= 0;
	rabbit_data. src_ratio		= double(rateOut) / rateIn;
	src_process (rabbit, &rabbit_data);
	resultBuffer	-> putDataIntoBuffer (rabbit_data. data_out,
	                                      rabbit_data. output_frames_gen);
	return rabbit_data. output_frames_gen;
}

