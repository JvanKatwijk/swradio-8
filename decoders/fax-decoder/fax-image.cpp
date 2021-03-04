#
/*
 *    Copyright (C) 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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

#include	"fax-image.h"

	faxImage::faxImage (int w, int h, QWidget *p) {
int	i, j;

	(void)p;
	image	= QImage (w, h, QImage::Format_RGB32);
	for (i = 0; i < w; i ++)
	   for (j = 0; j < h; j ++)
	      image. setPixel (QPoint(i, j), qRgb (i % 256, i % 256, j % 256));
}

	faxImage::~faxImage () {
}

void	faxImage::clear		(void) {
int i, j;

	for (i = 0; i < image. width (); i ++)
	   for (j = 0; j < image. height (); j ++)
	      image. setPixel (QPoint(i, j), qRgb (i % 256, i % 256, j % 256));
}

QImage	faxImage::getImage	(void) {
//	return image. scaled (QSize (576, 300), Qt::IgnoreAspectRatio);
	return image;
}

bool	faxImage::setPixel	(int col, int row, int value, int rgbg) {
QRgb	oldColor;
QRgb	color;

	if (col >= image. width ())
	   return false;
	if (row >= image. height () + 1)
	   return false;

	if (row >= image. height ())
 	   resizeHeight (50);

	oldColor	= image.  pixel (col, row);
	switch (rgbg) {
	   case 0:
	      color	= qRgb (value, qGreen (oldColor), qBlue (oldColor));
	      break;

	   case 1:
	      color	= qRgb (qRed (oldColor), value, qBlue (oldColor));
	      break;

	   case 2:
	      color	= qRgb (qRed (oldColor), qGreen (oldColor), value);
	      break;

	   default:
	      color	= qRgb (value, value, value);
	      break;
	}

	image. setPixel (QPoint(col, row), (uint)color);

	return true;
}

void	faxImage::newSize	(int a, int b, int c, int d) {
	(void)a;
	(void)b;
	(void)c;
	(void)d;
}

void	faxImage::resizeHeight	(int h) {
int	imageW	= image.  width ();
int	imageH	= image.  height ();
	image	= image. copy (0, 0, imageW, imageH + h);

}

int	faxImage::getCols	() {
	return	image. width ();
}

int	faxImage::getHeight	() {
	return	image. height ();
}

void	faxImage::correctWidth	(int w) {
	image	= QImage (w, image.height (), QImage::Format_RGB32);
}

