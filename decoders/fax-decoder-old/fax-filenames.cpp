#
/*
 *
 *    Copyright (C) 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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

#include	"fax-filenames.h"
#include	<QString>
#include	<QDate>

		faxFilenames::faxFilenames (int16_t n) {
	base	= "sdr-j-fax-";
	base. append (QDate::currentDate (). toString ());
	counter	= n;
}

		faxFilenames::~faxFilenames (void) {
}

	QString	faxFilenames::newFileName	(void) {
QString temp = base;
	temp. append (QString::number (counter ++));
	temp. append (QString(".png"));
	return temp;
}


