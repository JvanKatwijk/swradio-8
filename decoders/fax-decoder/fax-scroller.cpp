#
/*
 *    Copyright (C) 2008, 2009, 2010
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

#include	"fax-scroller.h"
#include	<QPoint>
#include	<stdio.h>

	faxScroller::faxScroller (QWidget *parent): QScrollArea (parent) {
}

	faxScroller::~faxScroller (void) {
}

void	faxScroller::mousePressEvent (QMouseEvent *m) {
QPoint	pos	= widget () -> mapFromParent (m -> pos ());

	emit fax_Clicked ((int)(pos. x()), (int)(pos. y()));
}

