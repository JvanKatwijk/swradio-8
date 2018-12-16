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
#ifndef	__VIRTUAL_READER__
#define	__VIRTUAL_READER__

#include	<stdint.h>
#include	<stdio.h>
#include	"ringbuffer.h"
#include	"swradio-constants.h"
#include	"simple-converter.h"

class	ExtioHandler;
//	The virtualReader is the mother of the readers.
//	The cardReader is slighty different, however
//	made fitting the framework
class	Reader_base {
protected:
RingBuffer<DSPCOMPLEX>	*theBuffer;
int32_t	blockSize;
public:
		Reader_base	(ExtioHandler *, int32_t);
virtual		~Reader_base	(void);
virtual void	restartReader	(int32_t s);
virtual void	stopReader	(void);
virtual void	processData	(float IQoffs, void *data, int cnt);
virtual	int16_t	bitDepth	(void);
virtual	void	setInputrate	(int32_t);
	int32_t	store_and_signal		(DSPCOMPLEX *, int32_t);
protected:
	ExtioHandler	*myMaster;
	int32_t		previousAmount;
	int32_t		base;
	int32_t		inputRate;
private:
	int32_t		outputRate;
	converter	*myConverter;
};

#endif

