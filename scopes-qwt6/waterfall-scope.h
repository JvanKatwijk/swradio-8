#
/*
 *    Copyright (C) 2008, 2009, 2010
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

#ifndef	__WATERFALLSCOPE__
#define	__WATERFALLSCOPE__

#include	"virtual-scope.h"
#include        <qwt.h>
#include        <qwt_plot.h>
#include        <qwt_plot_curve.h>
#include        <qwt_plot_marker.h>
#include        <qwt_plot_grid.h>
#include        <qwt_color_map.h>
#include        <qwt_plot_spectrogram.h>
#include        <qwt_plot_zoomer.h>
#include        <qwt_plot_panner.h>
#include        <qwt_plot_layout.h>
#include        <qwt_scale_widget.h>
#include        <QBrush>
#include        "spectrogramdata.h"
#include        <QTimer>
#include        <stdint.h>

class waterfallScope: public virtualScope, public QwtPlotSpectrogram {
Q_OBJECT
public:
		waterfallScope	(QwtPlot *, int16_t, int16_t);
		~waterfallScope	(void);
	void	set_BitDepth	(int16_t);
	void	display		(double *, double *,
	                         double, int32_t, int32_t);
private:
	SpectrogramData	*WaterfallData;
	int16_t		rasterSize;
	double		*plotData;
	QwtPlotMarker	*Marker;
	int16_t		indexforMarker;
	int16_t		old_marker;
	QwtPlotPicker	*lm_picker;
	QwtPlotPicker	*rm_picker;
	uint8_t		OneofTwo;
	QwtLinearColorMap	*colorMap;
	QwtScaleWidget		*rightAxis;
private slots:
	void	leftMouseClick (const QPointF &);
	void	rightMouseClick (const QPointF &);
};

#endif
