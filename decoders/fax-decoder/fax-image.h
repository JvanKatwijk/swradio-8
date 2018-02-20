#
/*
 *
 *    Copyright (C) 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    The fax implementation is a rewrite of hamfax
 *    Copyright (c) 2001, 2002
 *	Christof Schmitt, DH1CS <cschmitt@users.sourceforge.net>
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

#ifndef	__FAXIMAGE__
#define	__FAXIMAGE__

#include	"radio-constants.h"
#include	<QImage>
#include	<QPoint>
#include	<QString>

class faxImage {

public:
		faxImage	(int, int, QWidget	*parent = NULL);
		~faxImage	(void);
	void	newSize		(int, int, int, int);
	bool	setPixel	(int, int, int, int);
	int	getCols		(void);
	int	getHeight	(void);
	QImage	getImage	();
	void	correctWidth	(int);
	void	clear		(void);
private:
	void	resizeHeight	(int);
	QImage	image;
};

#endif

