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

#ifndef	__IF_FILTER__
#define	__IF_FILTER__

#include	"radio-constants.h"
#include	"fft-filters.h"


class	ifFilter {
public:
		ifFilter	(int32_t rate, int32_t width);
		~ifFilter	(void);
	void	setMiddle	(int32_t);
	void	Pass		(DSPCOMPLEX *, int32_t);
private:
	fftFilter	*theFilter;
	int32_t		theRate;
	int32_t		theWidth;
	int32_t		theMiddle;
};

#endif

