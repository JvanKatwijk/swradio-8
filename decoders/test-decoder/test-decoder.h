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
#include	"slidingfft.h"
#include	"fft.h"
#include	"waterfall-scope.h"

class	Cache;
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
	int	fillP;
	QFrame		*myFrame;
	decimatingFIR	*theFilter;
	int32_t		inputRate;
	int32_t		outputRate;
	int32_t		samplesperSymbol;
	int32_t		counter;
	slidingFFT	*theSlider;
	common_fft	*the_fft;
	std::complex<float> *fftBuffer;
	Cache		*theCache;
	int		cacheLineP;
	int		cacheSize;
	waterfallScope	*Viewer;
	double		*x_axis;
	double		*y_values;
};

#endif

