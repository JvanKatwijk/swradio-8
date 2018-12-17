#
/*
 *    Copyright (C)  2017, 2018
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J.
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
#include	"spectrum-scope.h"
#include	<qwt_picker_machine.h>

	spectrumScope::spectrumScope (QwtPlot *plot, int16_t displaySize):
	                                      virtualScope (plot, displaySize) {

	plotgrid	-> setCanvasBackground (Qt::black);
	grid		= new QwtPlotGrid;
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid		-> setMajPen (QPen(Qt::white, 0, Qt::DotLine));
#else
	grid		-> setMajorPen (QPen(Qt::white, 0, Qt::DotLine));
#endif
	grid		-> enableXMin (true);
	grid		-> enableYMin (true);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid		-> setMinPen (QPen(Qt::white, 0, Qt::DotLine));
#else
	grid		-> setMinorPen (QPen(Qt::white, 0, Qt::DotLine));
#endif
	grid		-> attach (plotgrid);

	SpectrumCurve	= new QwtPlotCurve ("");
   	SpectrumCurve	-> setPen (QPen(Qt::white));
	SpectrumCurve	-> setOrientation (Qt::Horizontal);
	SpectrumCurve	-> setBaseline	(get_db (0));
	ourBrush	= new QBrush (Qt::white);
	ourBrush	-> setStyle (Qt::Dense3Pattern);
	SpectrumCurve	-> setBrush (*ourBrush);
	SpectrumCurve	-> attach (plotgrid);
	
	Marker		= new QwtPlotMarker ();
	Marker		-> setLineStyle (QwtPlotMarker::VLine);
	Marker		-> setLinePen (QPen (Qt::white));
	Marker		-> attach (plotgrid);
	QwtText MarkerLabel             = QwtText ("label");
        MarkerLabel. setColor (Qt::white);
        Marker          -> setLabel (MarkerLabel);
        Marker          -> attach (plotgrid);
	oldmarkerValue	= -1;
	plotgrid	-> enableAxis (QwtPlot::yLeft);
	counter		= 10;
	lm_picker	= new QwtPlotPicker (plot -> canvas ());
	QwtPickerClickPointMachine *lpickerMachine =
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

	spectrumScope::~spectrumScope () {
//	plotgrid	-> enableAxis (QwtPlot::yLeft, false);
	Marker		-> detach ();
	SpectrumCurve	-> detach ();
	grid		-> detach ();
	delete		Marker;
	delete		SpectrumCurve;
	delete		grid;
	delete		lm_picker;
	delete		rm_picker;
	delete		ourBrush;
}

void	spectrumScope::leftMouseClick (const QPointF &point) {
	clickedwithLeft ((int)(point. x()) - IndexforMarker);
}

void	spectrumScope::rightMouseClick (const QPointF &point) {
	clickedwithRight ((int)(point. x()) - IndexforMarker);
}

void	spectrumScope::display (double *X_axis,
		                double *Y1_value,
	                        double amp,
	                        int32_t markerValue,
	                        int32_t theBin) {
int16_t	i;
double binVal = 0;

	for (i = -10; i < 10; i ++)
	   if (theBin + i >= 0)
	      binVal += Y1_value [theBin + i];
	binVal /= 20;

	amp		= amp / 100 * (-get_db (0));
	IndexforMarker	= markerValue;
	plotgrid	-> setAxisScale (QwtPlot::xBottom,
				         (double)X_axis [0],
				         X_axis [displaySize - 1]);
	plotgrid	-> enableAxis (QwtPlot::xBottom);
	plotgrid	-> setAxisScale (QwtPlot::yLeft,
				         get_db (0), get_db (0) + amp);
	for (i = 0; i < displaySize; i ++) 
	   Y1_value [i] = get_db (Y1_value [i]); 

	SpectrumCurve	-> setBaseline (get_db (0));
	Y1_value [0]	= get_db (0);
	Y1_value [displaySize - 1] = get_db (0);

	SpectrumCurve	-> setSamples (X_axis, Y1_value, displaySize);
//      iff the marker is changed, create a new one,
//      otherwise, the old one will do
	if ((markerValue != oldmarkerValue) || (--counter < 0)) {
           const QString help   = QString::number (get_db (binVal));
           QwtText MarkerLabel  =  QwtText (help);
	   counter = 10;
	   QFont *font1	= new QFont ("Courier New");
	   font1	-> setPixelSize (30);;
	   MarkerLabel.    setFont (*font1);
	   MarkerLabel.    setColor (Qt::white);
	   Marker       -> detach ();
	   Marker       = new QwtPlotMarker ();
	   Marker       -> setLineStyle (QwtPlotMarker::VLine);
	   Marker       -> setLinePen (QPen (Qt::white, 3.0));
	   Marker       -> setLabel (MarkerLabel);
	   Marker       -> attach (plotgrid);
	   Marker       -> setXValue (markerValue);
	   oldmarkerValue   = markerValue;
	}

	plotgrid	-> replot(); 
}

float	spectrumScope::get_db (float x) {
	return 20 * log10 ((x + 1) / (float)(normalizer));
}

void	spectrumScope::set_bitDepth	(int16_t d) {

	bitDepth	= d;
	if (d < 0 || d > 32)
	   d = 24;

	normalizer	= 1;
	while (-- d >= 0) 
	   normalizer <<= 1;
}

