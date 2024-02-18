#
/*
 *    Copyright (C) 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the sw receiver
 *
 *    sw receiver is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    swreceiver is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with swreceiver; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef	__VIRTUAL_SCOPE_H
#define	__VIRTUAL_SCOPE_H

#include	"radio-constants.h"
#include	<QObject>
#include	<qwt.h>
#include        <qwt_plot.h>
#include	<stdint.h>

class virtualScope: public QObject {
Q_OBJECT
public:
		virtualScope		(QwtPlot *, int16_t);
virtual		~virtualScope		(void);
virtual	void	display 		(double *, double *,
	                                 double, int32_t, int32_t);
virtual	void	set_bitDepth		(int16_t);
protected:
	QwtPlot		*plotgrid;
	int16_t		displaySize;
	int16_t		bitDepth;


signals:
	void		clickedwithLeft		(int);
	void		clickedwithRight	(int);
};

#endif
