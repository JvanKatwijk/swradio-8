#
/*
 *    Copyright (C) 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
 *    Many of the ideas as implemented in JSDR are derived from
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

//
//	For the different formats for input, we have
//	different readers, with one "mother" reader.
//	Note that the cardreader is quite different here
#ifndef	__COMMON_READERS
#define	__COMMON_READERS

class	ExtioHandler;

#include	"reader-base.h"

class	reader_16: public Reader_base {
public:
	reader_16	(ExtioHandler *, int32_t, int32_t);
	~reader_16	(void);
void	processData	(float IQoffs, void *data, int cnt);
int16_t bitDepth	(void);
};

class	reader_24: public Reader_base {
public:
	reader_24	(ExtioHandler *, int32_t, int32_t);
	~reader_24	(void);
void	processData	(float IQoffs, void *data, int cnt);
int16_t bitDepth	(void);
};

class	reader_32: public Reader_base {
public:
	reader_32	(ExtioHandler *, int32_t, int32_t);
	~reader_32	(void);
void	processData	(float IQoffs, void *data, int cnt);
int16_t	bitDepth	(void);
};


class	reader_float: public Reader_base {
public:
	reader_float	(ExtioHandler *, int32_t);
	~reader_float	(void);
void	processData	(float IQoffs, void *data, int cnt);
int16_t	bitDepth	(void);
};

#endif

