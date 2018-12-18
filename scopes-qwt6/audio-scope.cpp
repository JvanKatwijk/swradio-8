#
/*
 *    Copyright (C)  2017, 2018
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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
#include	"audio-scope.h"
//

static inline
float   get_db (float x) {
        return 20 * log10 ((x + 1) / (float)(64));
}
//
	audioScope::audioScope (QwtPlot	*plotGrid,
	                        int16_t	displaySize,
	                        int32_t	sampleRate) {
int16_t	i;
double	temp;
	this	-> plotGrid		= plotGrid;
	this	-> displaySize		= displaySize;
	spectrumSize			= 4 * displaySize;
	this	-> displayBuffer	= new double [displaySize];
	this	-> averageBuffer	= new double [displaySize];
	this    -> spectrum_fft         = new common_fft (spectrumSize);
        this    -> spectrumBuffer       = spectrum_fft  -> getVector ();
	Window				= new float [spectrumSize];
	this	-> X_axis		= new double [displaySize];
	for (i = 0; i < spectrumSize; i ++) 
	   Window [i] = 0.42 - 0.5 * cos ((2.0 * M_PI * i) / (spectrumSize - 1)) +
	                      0.08 * cos ((4.0 * M_PI * i) / (spectrumSize - 1));

	temp	= (double)sampleRate / 200 / displaySize;
	for (i = 0; i < displaySize / 2; i ++)
	   X_axis [i] =
	        (double)((i) * (double) temp);

	for (i = 0; i < displaySize / 2; i ++)
	   averageBuffer [i] = 0;
	plotGrid        -> setCanvasBackground (Qt::black);
        grid            = new QwtPlotGrid;
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
        grid            -> setMajPen (QPen(Qt::white, 0, Qt::DotLine));
#else
        grid            -> setMajorPen (QPen(Qt::white, 0, Qt::DotLine));
#endif
        grid            -> enableXMin (true);
        grid            -> enableYMin (true);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
        grid            -> setMinPen (QPen(Qt::white, 0, Qt::DotLine));
#else
        grid            -> setMinorPen (QPen(Qt::white, 0, Qt::DotLine));
#endif
        grid            -> attach (plotGrid);

        SpectrumCurve   = new QwtPlotCurve ("");
        SpectrumCurve   -> setPen (QPen(Qt::white));
        SpectrumCurve   -> setOrientation (Qt::Horizontal);
        SpectrumCurve   -> setBaseline  (get_db (0));
        ourBrush        = new QBrush (Qt::white);
        ourBrush        -> setStyle (Qt::Dense3Pattern);
	SpectrumCurve   -> setBrush (*ourBrush);
        SpectrumCurve   -> attach (plotGrid);
	averageCounter	= 0;
	fillPointer	= 0;
}

	audioScope::~audioScope (void) {
	delete[]	this	-> displayBuffer;
	delete[]	this	-> averageBuffer;
	delete[]	this	-> X_axis;
	delete[]	this	-> Window;
	delete		this	-> spectrum_fft;
}

void	audioScope::setRate (int newRate) {
double temp;
int16_t	i;

	if (sampleRate == newRate)
	   return;

	fprintf (stderr, "audiorate set to %d\n", newRate);
	sampleRate = newRate;
	temp	= (double)sampleRate / 200 / displaySize;
	for (i = 0; i < displaySize / 2; i ++)
	   X_axis [i] =
	        (double)((i) * (double) sampleRate / displaySize);
}

void	audioScope::addElement (std::complex<float> x) {
int32_t	i, j;
//	
	spectrumBuffer [fillPointer] = x;
	if (++fillPointer < spectrumSize) {
	   return;
	}

	fillPointer	= 0;
	for (i = 0; i < spectrumSize; i ++) 
	   spectrumBuffer [i] = cmul (spectrumBuffer [i], Window [i]); 
//
//	we show only the positive half of the spectrum
	spectrum_fft	-> do_FFT ();
	for (i = 0; i < displaySize / 2; i ++) {
	   displayBuffer [i] = 0;
	   for (j = 0; j < 4; j ++) {
	      displayBuffer [i] +=
	           0.25 * abs (spectrumBuffer [4 * i + j]);
	   }
	}

#define	FREQ 3
//	for (i = 0; i < displaySize / 2; i ++) {
//	   if (displayBuffer [i] != displayBuffer [i])
//	      displayBuffer [i] = 0;
//	   averageBuffer [i] = 
//	      ((double)(FREQ - 1)) / (FREQ) * averageBuffer [i] +
//	                1.0f / (FREQ) * displayBuffer [i];
//	   displayBuffer [i] = averageBuffer [i];
//	}

	if (averageCounter < 5) {
	   averageCounter ++;
	   return;
	}

        plotGrid        -> setAxisScale (QwtPlot::xBottom,
                                         (double)X_axis [0],
                                         X_axis [displaySize / 2 - 1]);
        plotGrid        -> enableAxis (QwtPlot::xBottom);
        plotGrid        -> setAxisScale (QwtPlot::yLeft,
                                         get_db (0), get_db (0) + 50);

        for (i = 0; i < displaySize / 2; i ++)
	   displayBuffer [i] = get_db (displayBuffer [i]);
        displayBuffer [0]		= get_db (0);
        displayBuffer [displaySize / 2 - 1] = get_db (0);
        SpectrumCurve   -> setSamples (X_axis, displayBuffer, displaySize / 2);
	plotGrid	-> replot ();
}



