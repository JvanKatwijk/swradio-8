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
#ifndef	__UP_CONVERTER__
#define	__UP_CONVERTER__

#include	<stdint.h>
#include	<samplerate.h>
#include	"radio-constants.h"

class	audioSink;

class	upConverter {
public:
		upConverter	(int32_t, int32_t, audioSink *);
		~upConverter	(void);
	void	handle		(DSPCOMPLEX *buffer, int amount);
private:
	int32_t         inRate;
	int32_t         outRate;
	audioSink	*client;
	double          ratio;
	int32_t         outputLimit;
	int32_t         inputLimit;
	SRC_STATE       *src_converter;
	SRC_DATA        *src_data;
	float           *inBuffer;
	float           *outBuffer;
	int32_t         inp;
};
#endif

