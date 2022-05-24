#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    lazy Chair Computing
 *
 *    This file is part of the SDRuno plugin for drm
 *
 *    drm decoder is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm decoder is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with drm decoder; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#ifndef	__READER__
#define	__READER__

#include	"ringbuffer.h"
#include	"basics.h"

class	drmDecoder;
//
//	For now we have a simple abstraction layer over a ringbuffer
//	that provides a suitable buffer.  It acts as a - more or
//	less regular - array, however, how the data gets in is kept
//	a secret. In a next version we eliminate the ringbuffer.
class	Reader {
public:
			Reader (RingBuffer<std::complex<float>> *, 
	                        uint32_t, drmDecoder *);
			~Reader		();
	void		waitfor		(int32_t);
	void		shiftBuffer	(int32_t);
	void		stop		();
	uint32_t	bufSize;
	uint32_t	bufMask;
	std::complex<float>	*data;
	uint32_t	currentIndex;
	bool		stopSignal;
	uint32_t	firstFreeCell;
private:
	drmDecoder	*m_form;
	uint32_t	Contents	();
	
	RingBuffer<std::complex<float>> * ringBuffer;
};

#endif

