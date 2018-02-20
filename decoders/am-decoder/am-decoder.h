#
/*
 *    Copyright (C) 2017
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

#ifndef	__AM_DECODER__
#define	__AM_DECODER__
#include	<QFrame>
#include 	"virtual-decoder.h"
#include	"ui_am-decoder.h"
#include	"fir-filters.h"

class	QSettings;
class	adaptiveFilter;
class	RadioInterface;

class amDecoder :public virtualDecoder, public Ui_am_decoder {
Q_OBJECT
public:
		amDecoder	(int32_t,
	                   	 RingBuffer<DSPCOMPLEX> *,
	                         QSettings	*);
		~amDecoder	(void);
	void	processBuffer	(DSPCOMPLEX *, int32_t);
	void	process		(std::complex<float>);
private:
	int32_t		inRate;
	QFrame		*myFrame;
	LowPassFIR	*lowpassFilter;
	int16_t		adaptiveFiltersize;
	adaptiveFilter	*adaptive;
	bool		adaptiveFiltering;
	int		counter;
	float		m_w1;
private slots:
	void		set_adaptiveFilter	(void);
	void		set_adaptiveFiltersize	(int);
	void		set_lowpassFilter	(int);
};

#endif

