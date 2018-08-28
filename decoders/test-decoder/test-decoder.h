#
/*
 *    Copyright (C) 2017
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

#ifndef	__TEST_DECODER__
#define	__TEST_DECODER__
#include	<QFrame>
#include 	"virtual-decoder.h"
#include	"ui_test-decoder.h"
#include	"decimating_filter.h"
#include	"fft-scope.h"

class	QSettings;

class testDecoder: public virtualDecoder, public Ui_test_decoder {
Q_OBJECT
public:
		testDecoder	(int32_t,
	                   	 RingBuffer<DSPCOMPLEX> *,
	                         QSettings	*);
		~testDecoder	(void);
	void	processBuffer	(std::complex<float> *, int32_t);
	void	process		(std::complex<float>);
private:
	QFrame		*myFrame;
	decimating_filter	*theFilter;
	int32_t		inputRate;
	int32_t		outputRate;
	fftScope	*testScope;
private slots:
	void		handle_amplitude	(int);
};

#endif

