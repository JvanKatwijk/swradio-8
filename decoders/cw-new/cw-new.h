#
/*
 *    Copyright (C) 2010, 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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

#ifndef __NEW_CW__
#define	__NEW_CW__
#

#include	<QFrame>
#include	"radio-constants.h"
#include	"virtual-decoder.h"
#include	"ui_cw-new.h"
#include	"shifter.h"
#include	<fftw3.h>
#include	"waterfall-scope.h"
#include	<vector>

class	LowPassIIR;
class	QSettings;
class	shifter;
class	RadioInterface;

class cwDecoder: public virtualDecoder, public Ui_new_cw {
Q_OBJECT
public:
			cwDecoder		(RadioInterface *,
	                                         int32_t,
	                                         RingBuffer<std::complex <float> > *,
	                                         QSettings *);
			~cwDecoder		();
	void		process			(std::complex<float>);
	int32_t		rateOut			();
private:
	QFrame		myFrame;
	shifter		localShifter;
	QSettings	*cwSettings;
	double		*x_axis;
	double		*y_values;
	int		buffp;
	std::complex<float>	*fftBuffer;
	fftwf_plan	plan;
	waterfallScope	*cwViewer;
	int		inputRate;
	void		addBuffer		(std::complex<float> *);
	int32_t		theRate;
	RingBuffer<std::complex<float> > *audioData;
};

#endif


