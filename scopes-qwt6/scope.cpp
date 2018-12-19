#
/*
 *    Copyright (C)  2012, 2013, 2014
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
 *    along with JSDR; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"scope.h"
#include	<qwt_picker_machine.h>
/*
 *	The "scope" combines the Qwt widgets and control for both
 *	the spectrumdisplay and the waterfall display.
 */
	Scope::Scope (QwtPlot		*scope,
	              uint16_t		displaysize) {
	Plotter			= scope;
	Displaysize		= displaysize;
	bitDepth		= 24;
	Spectrum	= new SpectrumViewer  (Plotter,
	                                       Displaysize);
	scope		-> setCanvasBackground (QColor (QString ("blue")));
	connect (Spectrum,
	         SIGNAL (leftClicked (int)),
	         this,
	         SLOT (leftClicked (int)));
}

	Scope::~Scope (void) {
	delete Spectrum;
}

void	Scope::leftClicked (int n) {
	clickedwithLeft (n);
}

void	Scope::rightClicked (int n) {
	clickedwithRight (n);
}

void	Scope::Display (double	*x_axis,
	                double	*buffer,
	                double	amp,
	                int32_t	marker) {
	Spectrum	-> ViewSpectrum (x_axis,
	                                 buffer,
	                                 amp,
	                                 marker);
}

void	Scope::set_bitDepth	(int16_t b) {
	bitDepth = b;
	Spectrum -> setBitDepth (b);
}
/*
 *	The spectrumDisplay
 */
	SpectrumViewer::SpectrumViewer (QwtPlot *plot, uint16_t displaysize) {

	plotgrid		= plot;
	this	-> Displaysize	= displaysize;
	plotgrid-> setCanvasBackground (Qt::blue);
	grid	= new QwtPlotGrid;
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid	-> setMajPen (QPen(Qt::white, 0, Qt::DotLine));
#else
	grid	-> setMajorPen (QPen(Qt::white, 0, Qt::DotLine));
#endif
	grid	-> enableXMin (true);
	grid	-> enableYMin (true);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid	-> setMinPen (QPen(Qt::white, 0, Qt::DotLine));
#else
	grid	-> setMinorPen (QPen(Qt::white, 0, Qt::DotLine));
#endif
	grid	-> attach (plotgrid);

	SpectrumCurve	= new QwtPlotCurve ("");
   	SpectrumCurve	-> setPen (QPen(Qt::white));
//	SpectrumCurve	-> setStyle	(QwtPlotCurve::Sticks);
	SpectrumCurve	-> setOrientation (Qt::Horizontal);
	SpectrumCurve	-> setBaseline	(get_db (0));
	ourBrush	= new QBrush (Qt::white);
	ourBrush	-> setStyle (Qt::Dense3Pattern);
	SpectrumCurve	-> setBrush (*ourBrush);
	SpectrumCurve	-> attach (plotgrid);
	
	Marker		= new QwtPlotMarker ();
	Marker		-> setLineStyle (QwtPlotMarker::VLine);
	Marker		-> setLinePen (QPen (Qt::red));
	Marker		-> attach (plotgrid);
	plotgrid	-> enableAxis (QwtPlot::yLeft);

	lm_picker	= new QwtPlotPicker (plot -> canvas ());
	QwtPickerMachine *lpickerMachine =
	              new QwtPickerClickPointMachine ();
	lm_picker	-> setStateMachine (lpickerMachine);
	lm_picker	-> setMousePattern (QwtPlotPicker::MouseSelect1,
	                                    Qt::LeftButton);
	connect (lm_picker, SIGNAL (selected (const QPointF&)),
	         this, SLOT (leftMouseClick (const QPointF &)));

	rm_picker	= new QwtPlotPicker (plot -> canvas ());
	QwtPickerMachine *rpickerMachine =
	              new QwtPickerClickPointMachine ();
	rm_picker	-> setStateMachine (rpickerMachine);
	rm_picker	-> setMousePattern (QwtPlotPicker::MouseSelect1,
	                                    Qt::RightButton);
	connect (rm_picker, SIGNAL (selected (const QPointF&)),
	         this, SLOT (rightMouseClick (const QPointF &)));

	IndexforMarker	= 0;
}

	SpectrumViewer::~SpectrumViewer () {
	disconnect (lm_picker,
	            SIGNAL (selected (const QPointF &)),
	            this,
	            SLOT (leftMouseClick (const QPointF &)));
	disconnect (rm_picker,
	            SIGNAL (selected (const QPointF &)),
	            this,
	            SLOT (rightMouseClick (const QPointF &)));
	plotgrid	-> enableAxis (QwtPlot::yLeft, false);
	Marker		-> detach ();
	SpectrumCurve	-> detach ();
	grid		-> detach ();
	delete		Marker;
	delete		SpectrumCurve;
	delete		grid;
	delete		lm_picker;
	delete		rm_picker;
}

void	SpectrumViewer::leftMouseClick (const QPointF &point) {
	leftClicked ((int)(point. x()) - IndexforMarker);
}

void	SpectrumViewer::rightMouseClick (const QPointF &point) {
	rightClicked ((int)(point. x()) - IndexforMarker);
}

void	SpectrumViewer::ViewSpectrum (double *X_axis,
		                      double *Y1_value,
	                              double amp1,
	                              int32_t marker) {
uint16_t	i;

	double amp	= amp1 / 100 * (-get_db (0));
	IndexforMarker	= marker;
	plotgrid	-> setAxisScale (QwtPlot::xBottom,
				         (double)X_axis [0],
				         X_axis [Displaysize - 1]);
	plotgrid	-> enableAxis (QwtPlot::xBottom);
	plotgrid	-> setAxisScale (QwtPlot::yLeft,
				         get_db (0), get_db (0) + amp);
	for (i = 0; i < Displaysize; i ++) 
	   Y1_value [i] = get_db (amp1 / 100 * Y1_value [i]); 

	SpectrumCurve	-> setBaseline (get_db (0));
	Y1_value [0]	= get_db (0);
	Y1_value [Displaysize - 1] = get_db (0);

	SpectrumCurve	-> setSamples (X_axis, Y1_value, Displaysize);
	Marker		-> setXValue (marker);
	plotgrid	-> replot(); 
}

float	SpectrumViewer::get_db (float x) {
	return 20 * log10 ((x + 1) / (float)(normalizer));
}

void	SpectrumViewer::setBitDepth	(int16_t d) {

	if (d < 0 || d > 32)
	   d = 24;

	normalizer	= 1;
	while (-- d >= 0) 
	   normalizer <<= 1;
}

