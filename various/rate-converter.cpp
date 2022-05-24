#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DRM+ Decoder
 *
 *    DRM+ Decoder is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DRM+ Decoder is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DRM+ Decoder; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	"rate-converter.h"

	rateConverter::rateConverter (int32_t inRate, int32_t outRate, 
	                              int32_t inSize) {
int	err;
	this	-> inRate	= inRate;
	this	-> outRate	= outRate;
	inputLimit		= inSize;
	ratio			= double(outRate) / inRate;
	fprintf (stderr, "ratio = %f\n", ratio);
	outputLimit		= inSize * ratio;
//	converter		= src_new (SRC_SINC_BEST_QUALITY, 2, &err);
//	converter		= src_new (SRC_LINEAR, 2, &err);
	converter		= src_new (SRC_SINC_MEDIUM_QUALITY, 2, &err);
	src_data		= new SRC_DATA;
	inBuffer		= new float [2 * inputLimit + 20];
	outBuffer		= new float [2 * outputLimit + 20];
	src_data-> data_in	= inBuffer;
	src_data-> data_out	= outBuffer;
	src_data-> src_ratio	= ratio;
	src_data-> end_of_input	= 0;
	inp			= 0;
}

	rateConverter::~rateConverter (void) {
	src_delete	(converter);
	delete []	inBuffer;
	delete []	outBuffer;
	delete		src_data;
}

bool	rateConverter::convert (std::complex<float> v,
	                        std::complex<float> *out, int32_t *amount) {
int32_t	i;
int32_t	framesOut;
int	res;

	inBuffer [2 * inp]	= real (v);
	inBuffer [2 * inp + 1]	= imag (v);
	inp ++;
	if (inp < inputLimit)
	   return false;

	src_data	-> input_frames		= inp;
	src_data	-> output_frames	= outputLimit + 10;
	res		= src_process (converter, src_data);
	if (res != 0) {
	   fprintf (stderr, "error %s\n", src_strerror (res));
	   return false;
	}
	inp		= 0;
	framesOut	= src_data -> output_frames_gen;
	for (i = 0; i < framesOut; i ++)
	   out [i] = std::complex<float> (outBuffer [2 * i], outBuffer [2 * i + 1]);
	*amount		= framesOut;
	return true;
}

int32_t	 rateConverter::getOutputsize (void) {
	return outputLimit;
}

