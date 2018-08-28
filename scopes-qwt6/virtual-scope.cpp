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
 *    along with JSDR; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"virtual-scope.h"

	virtualScope::virtualScope	(QwtPlot *scope, int16_t displaysize) {
	plotgrid	= scope;
	displaySize	= displaysize;
	bitDepth	= 24;
}

	virtualScope::~virtualScope	(void) {
}

void	virtualScope::display		(double	*x_axis,
	                                 double	*buffer,
	                                 double	amp,
	                                 int32_t marker,
	                                 int32_t theBin) {
	(void)x_axis; (void)buffer; (void)amp; (void)marker; (void)theBin;
}

void	virtualScope::set_bitDepth	(int16_t b) {
	(void)b;
}

