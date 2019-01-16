#
/*
 *    Copyright (C)  2012, 2013, 2014
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

#include	"waterfall-scope.h"
#include        <qwt_picker_machine.h>
#include        <qwt_text.h>
 
	waterfallScope::waterfallScope (QwtPlot       *plot,
	                                int16_t      displaySize,
	                                int16_t      rasterSize):
	                                   virtualScope (plot, displaySize),
	                                   QwtPlotSpectrogram () {
int     i, j;
	colorMap  = new QwtLinearColorMap (Qt::darkCyan, Qt::red);
	QwtLinearColorMap *c2 = new QwtLinearColorMap (Qt::darkCyan, Qt::red);
	this            -> rasterSize   = rasterSize;
	colorMap        -> addColorStop (0.1, Qt::cyan);
	colorMap        -> addColorStop (0.4, Qt::green);
	colorMap        -> addColorStop (0.7, Qt::yellow);
	c2              -> addColorStop (0.1, Qt::cyan);
	c2              -> addColorStop (0.4, Qt::green);
	c2              -> addColorStop (0.7, Qt::yellow);
	this            -> setColorMap (colorMap);
	OneofTwo                = 0;
	rightAxis	= plotgrid -> axisWidget (QwtPlot::yRight);
// A color bar on the right axis
	rightAxis	-> setColorBarEnabled (true);
	plotData	= new double [2 * displaySize * rasterSize];

	for (i = 0; i < rasterSize; i ++)
	   for (j = 0; j < displaySize; j ++)
	      plotData [i * displaySize + j] = (double)i / rasterSize;

	WaterfallData   = new SpectrogramData (plotData,
	                                       10000,
	                                       1000,
	                                       rasterSize,
	                                       displaySize,
	                                       50.0);
	this -> setData (WaterfallData);
	this -> setDisplayMode (QwtPlotSpectrogram::ImageMode, true);
	rightAxis -> setColorMap (this -> data () -> interval (Qt::YAxis),
	                          c2);
	plotgrid	-> setAxisScale (QwtPlot::yRight, 0, 50.0);
	plotgrid	-> enableAxis   (QwtPlot::yRight);
	plotgrid	-> setAxisScale (QwtPlot::xBottom,
	                                 10000,
	                                 11000);

	plotgrid	-> enableAxis (QwtPlot::xBottom);
	plotgrid	-> enableAxis (QwtPlot::yLeft);
	plotgrid	-> setAxisScale (QwtPlot::yLeft, 0, rasterSize);

	Marker          = new QwtPlotMarker ();
	Marker          -> setLineStyle (QwtPlotMarker::VLine);
	Marker          -> setLinePen (QPen (Qt::black, 2.0));
	Marker          -> attach (plotgrid);
	indexforMarker  = 0;

	lm_picker       = new QwtPlotPicker (plot -> canvas ());
	QwtPickerMachine *lpickerMachine =
	              new QwtPickerClickPointMachine ();
	lm_picker       -> setStateMachine (lpickerMachine);
	lm_picker       -> setMousePattern (QwtPlotPicker::MouseSelect1,
	                                    Qt::LeftButton);
	rm_picker       = new QwtPlotPicker (plot -> canvas ());
	QwtPickerMachine *rpickerMachine =
	              new QwtPickerClickPointMachine ();
	rm_picker       -> setStateMachine (rpickerMachine);
	rm_picker       -> setMousePattern (QwtPlotPicker::MouseSelect1,
	                                    Qt::RightButton);
	connect (lm_picker, SIGNAL (selected (const QPointF&)),
	         this, SLOT (leftMouseClick (const QPointF &)));
	connect (rm_picker, SIGNAL (selected (const QPointF&)),
	         this, SLOT (rightMouseClick (const QPointF &)));

	plotgrid        -> replot ();
}

	waterfallScope::~waterfallScope () {
	plotgrid        -> enableAxis (QwtPlot::yRight, false);
	plotgrid        -> enableAxis (QwtPlot::xBottom, false);
	plotgrid        -> enableAxis (QwtPlot::yLeft, false);
	delete	lm_picker;
	delete	rm_picker;
	this            -> detach ();
//	delete	colorMap;
//	delete	WaterfallData;
}

void    waterfallScope::leftMouseClick (const QPointF &point) {
	clickedwithLeft ((int)(point. x()) - indexforMarker);
}

void    waterfallScope::rightMouseClick (const QPointF &point) {
	clickedwithRight ((int)(point. x()) - indexforMarker);
}

void    waterfallScope::display (double *X_axis,
	                         double *Y1_value,
	                         double  amp,
	                         int32_t marker, int32_t theBin) {
int     orig    = (int)(X_axis [0]);
int     width   = (int)(X_axis [displaySize - 1] - orig);

	(void)theBin;
	indexforMarker  = marker;
	if (OneofTwo) {
	   OneofTwo = 0;
	   return;
	}
	OneofTwo        = 1;
/*
 *      shift one row, faster with memmove than writing out
 *      the loops. Note that source and destination overlap
 *      and we therefore use memmove rather than memcpy
 */
	memmove (&plotData [0],
	         &plotData [displaySize],
	         (rasterSize - 1) * displaySize * sizeof (double));
/*
 *      ... and insert the new line
 */
	memcpy (&plotData [(rasterSize - 1) * displaySize],
	        &Y1_value [0],
	        displaySize * sizeof (double));

	invalidateCache ();
	WaterfallData = new SpectrogramData (plotData,
	                                     orig,
	                                     width,
	                                     rasterSize,
	                                     displaySize,
	                                     amp);

	this            -> detach       ();
	this            -> setData      (WaterfallData);
	this            -> setDisplayMode (QwtPlotSpectrogram::ImageMode,
	                                                               true);

	plotgrid        -> setAxisScale (QwtPlot::xBottom,
	                                 orig,
	                                 orig + width);
	plotgrid        -> enableAxis (QwtPlot::xBottom);
	Marker          -> setXValue (marker);
	this            -> attach     (plotgrid);
	plotgrid        -> replot();
}

