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

#ifndef	__SCOPES__
#define	__SCOPES__

#include	"radio-constants.h"
#include	<QObject>
#include	<qwt.h>
#include	<qwt_plot.h>
#include	<qwt_plot_curve.h>
#include	<qwt_plot_marker.h>
#include	<qwt_plot_grid.h>
#include	<qwt_color_map.h>
#include	<qwt_plot_picker.h>
#include	<QBrush>
#include	<QTimer>
#include	<stdint.h>

class	SpectrumViewer;
class	QwtLinearColorMap;
class	QwtScaleWidget;
/*
 *	The stacked object is called the scope, it is built as a wrapper
 *	around the actual display mechanisms the waterfall and the
 *	spectrumviewer.
 */


class Scope: public QObject {
Q_OBJECT
public:
	Scope					(QwtPlot *, uint16_t);
	~Scope 					(void);
void	Display 				(double *,
	                                         double *,
	                                         double, int32_t);
void	set_bitDepth	(int16_t);
void	switch_ViewMode	(int);
private:
	QwtPlot		*Plotter;
	uint16_t	Displaysize;
	SpectrumViewer	*Spectrum;
	int16_t		bitDepth;
	float		get_db			(float);
private slots:
	void		leftClicked		(int);
	void		rightClicked		(int);
signals:
	void		clickedwithLeft		(int);
	void		clickedwithRight	(int);
};

/*
 *	The spectrumDisplay
 */
class	SpectrumViewer: public QObject {
Q_OBJECT
public:
	SpectrumViewer 	(QwtPlot *, uint16_t);
	~SpectrumViewer	(void);
void	ViewSpectrum	(double *, double *, double, int32_t);
void	setBitDepth	(int16_t);
private:
	QwtPlot		*plotgrid;
	uint16_t	Displaysize;
	QwtPlotGrid	*grid;
	QwtPlotCurve	*SpectrumCurve;
	QwtPlotMarker	*Marker;
	uint32_t	IndexforMarker;
	QwtPlotPicker	*lm_picker;
	QwtPlotPicker	*rm_picker;
	QBrush		*ourBrush;
	int16_t		bitDepth;
	int32_t		normalizer;
	float		get_db		(float);
private slots:
	void	leftMouseClick (const QPointF &);
	void	rightMouseClick (const QPointF &);
signals:
	void	leftClicked	(int);
	void	rightClicked	(int);
};
#endif

