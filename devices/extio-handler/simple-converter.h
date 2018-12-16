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
#ifndef	__RATE_CONVERTER__
#define	__RATE_CONVERTER__

#include	<swradio-constants.h>
#include	<ringbuffer.h>
#include	"samplerate.h"

//	Very simple converter, the basic idea is that the
//	different (high speed) usb readers put their data
//	in, and the converter fills the outputbuffer with
//	data with the right (i.e. lower for the sw receiver)
//	samplerate
class converter {
public:
		converter	(int32_t,
	                         int32_t,
	                         RingBuffer<DSPCOMPLEX> *);
		~converter	(void);
	int32_t	convert		(DSPCOMPLEX *, int32_t);
private:
	int32_t	rateIn;
	int32_t	rateOut;
	RingBuffer<DSPCOMPLEX> *resultBuffer;
	SRC_STATE*	rabbit;
	SRC_DATA	rabbit_data;
};

#endif


