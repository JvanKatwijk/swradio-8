#
/*
 *    Copyright (C) 2017, 2018
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
#ifndef	__SPECTROGRAMDATA__
#define	__SPECTROGRAMDATA__

#include	<qwt_interval.h>
#include        <qwt_interval.h>
#include        <QPen>
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0602)
# include       <qwt_raster_data.h>
#else
# include       <qwt_matrix_raster_data.h>
#endif


#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0602)
class   SpectrogramData: public QwtRasterData {
#else
class   SpectrogramData: public QwtMatrixRasterData {
#endif
public:
	double	*data;		// pointer to actual data
	int	left;		// index of left most element in raster
	int	width;		// raster width
	int	height;		// rasterheigth
	int	datawidth;	// width of matrix
	int	dataheight;	// for now == rasterheigth
	double	max;

	SpectrogramData (double *data, int left, int width, int height,
	                 int datawidth, double max):
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0602)
        QwtRasterData () {
#else
        QwtMatrixRasterData () {
#endif

	this	-> data		= data;
	this	-> left		= left;
	this	-> width	= width;
	this	-> height	= height;
	this	-> datawidth	= datawidth;
	this	-> dataheight	= height;
	this	-> max		= max;

	setInterval (Qt::XAxis, QwtInterval (left, left + width));
	setInterval (Qt::YAxis, QwtInterval (0, height));
	setInterval (Qt::ZAxis, QwtInterval (0, max));
}

void	initRaster (const QRectF &x, const QSize &raster) {
	(void)x;
	(void)raster;
}

QwtInterval Interval (Qt::Axis x)const {
	if (x == Qt::XAxis)
	   return QwtInterval (left, left + width);
	return QwtInterval (0, max);
}

	~SpectrogramData () {
}

double value (double x, double y) const {
	if (x != x)
	   x = 0;
	if (y != y)
	   y = 0;
	x = x - left;
	x = x / width * (datawidth - 1);
        y = y / height * (dataheight - 1);
        return data [(int)y * datawidth + (int)x];

}

};

#endif

