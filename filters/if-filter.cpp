
#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the  SDR-J series.
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
#include	"if-filter.h"


	ifFilter::ifFilter (int32_t rate, int32_t width) {
	theRate		= rate;
	theWidth	= width;
	theMiddle	= 0;

	theFilter	= new fftFilter (1024, 255);
	theFilter	-> setBand (-width / 2, width / 2, rate);
}

	ifFilter::~ifFilter	(void) {
	delete theFilter;
}

void	ifFilter::setMiddle	(int32_t m) {
	theFilter	-> setBand (m - theWidth / 2,
	                            m + theWidth / 2, theRate);
}

void	ifFilter::Pass		(DSPCOMPLEX *buffer, int32_t bufSize) {
int32_t i;
	for (i = 0; i < bufSize; i ++) 
	   buffer [i] = theFilter -> Pass (buffer [i]);
}

