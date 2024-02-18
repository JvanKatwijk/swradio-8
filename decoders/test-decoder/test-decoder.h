#
/*
 *    Copyright (C) 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the swradio
 *
 *    swradio is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    swradio is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with swradio; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef	__TEST_DECODER__
#define	__TEST_DECODER__
#include	<QFrame>
#include 	"virtual-decoder.h"
#include	"ui_test-decoder.h"
#include	"fir-filters.h"
#include	"fft-filters.h"
#include	"fft.h"
#include	"spectrum-scope.h"
#include	"hilbertfilter.h"

class	QSettings;

class testDecoder: public virtualDecoder, public Ui_test_decoder {
Q_OBJECT
public:
		testDecoder	(int32_t,
	                   	 RingBuffer<DSPCOMPLEX> *,
	                         QSettings	*);
		~testDecoder	(void);
	void	processBuffer	(std::complex<float> *, int32_t);
	void    process		(std::complex<float> z);

private:
	LowPassFIR 	baseFilter;
	fftFilter	usbFilter;
	fftFilter	lsbFilter;
	hilbertFilter	s2Filter;
	hilbertFilter	s3Filter;
	int		fillP;
	QFrame		*myFrame;
	int32_t		inputRate;
	int32_t		outputRate;
	common_fft	*the_fft;
	std::complex<float> *fftBuffer;
	spectrumScope	*Viewer;
	int		counter;
	double		x_axis [512];
	double		y_values [512];;
	RingBuffer<std::complex<float>> *audioBuffer;
private	slots:
	void		handleClick	(int);
};

#endif

