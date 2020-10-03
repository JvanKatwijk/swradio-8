#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDR-J (JSDR).
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
#
#include	"reader.h"
#include	"drm-decoder.h"

//
//	A simple interface class to the ringBuffer
//	The methods are called from the drmdecoder

	Reader::Reader (RingBuffer<DSPCOMPLEX> *r,
	                int16_t s, 
	                drmDecoder *mr) {
	ringBuffer		= r;
	this	-> bufSize	= 4 * 8192;
	data			= new std::complex<float> [this -> bufSize];
	memset (data, 0, bufSize * sizeof (std::complex<float>));
	master			= mr;
	currentIndex		= 0;
	firstFreeCell		= 0;
	stopSignal		= false;
}

	Reader::~Reader (void) {
	delete []	data;
}

void	Reader::stop	() {
	stopSignal	= true;
}

uint32_t	Reader::Contents	(void) {
	if (firstFreeCell >= currentIndex)
	   return firstFreeCell - currentIndex;
	return (bufSize - currentIndex) + firstFreeCell;
}

//
//	The tricky point is that C++ does not have a decent facility
//	to "kill" a task. A task is supposed to be suicidal.
//	For this purpose, we poll the master - in this case the
//	drmdecoder itself - who will set a flag when we are supposed
//	to commit suicide.
//	Suicide will be executed by raising an exception.

//	The caller will be blocked until the amount
//	requested for is available.
void	Reader::waitfor (int32_t amount) {
uint32_t	tobeRead;
int32_t		contents	= Contents ();

	if (contents >= amount)
	   return;
	tobeRead	= amount - contents;
	while (ringBuffer -> GetRingBufferReadAvailable () < tobeRead) {
	   usleep (1000);
	   if (stopSignal)
	      throw (1);
	}
//
//	Ok, if the amount of samples to be read fits in a contiguous part
//	one read will suffice, otherwise it will be in two parts
	if (firstFreeCell + tobeRead <= bufSize) {
	   ringBuffer -> getDataFromBuffer (&data [firstFreeCell], tobeRead);
	}
	else {
	   ringBuffer -> getDataFromBuffer (&data [firstFreeCell],
	                                     bufSize - firstFreeCell);
	   ringBuffer -> getDataFromBuffer (&data [0],
	                                tobeRead - (bufSize - firstFreeCell));
	}
	   firstFreeCell = (firstFreeCell + tobeRead) % bufSize;
}

void	Reader::shiftBuffer (int16_t n) {
	if (n > 0)
	   waitfor (n + 20);
	currentIndex = (currentIndex + n) % bufSize;
}

