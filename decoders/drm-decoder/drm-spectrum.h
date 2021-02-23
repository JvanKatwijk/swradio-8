#
/*
 *    Copyright (C) 2014 .. 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB.
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

//
//	Simple spectrum scope object
//	Shows the spectrum of the incoming data stream 
//
#ifndef		__DRM_SPECTRUM__
#define		__DRM_SPECTRUM__

#include	<fftw3.h>
#include	<complex>
#include	"ringbuffer.h"
#include	<qwt.h>
#include	<qwt_plot.h>
#include	<qwt_plot_marker.h>
#include	<qwt_plot_grid.h>
#include	<qwt_plot_curve.h>
#include	<qwt_plot_marker.h>

class	drmSpectrum {
public:
			drmSpectrum	(QwtPlot *);
			~drmSpectrum	();
	void		showSpectrum	(std::complex<float> *, int);
	void		setBitDepth	(int16_t);
private:
	int16_t		displaySize;
	int16_t		spectrumSize;
	std::complex<float>	*spectrum;
	std::vector<double>	displayBuffer;
	std::vector<float>	Window;
	fftwf_plan	plan;
	QwtPlot		*plotgrid;
	QwtPlotGrid	*grid;
	QwtPlotCurve	*spectrumCurve;
	QBrush		*ourBrush;
	void		ViewSpectrum	(double *, double *, int);
	float		get_db 		(float);
	int32_t		normalizer;
};

#endif

