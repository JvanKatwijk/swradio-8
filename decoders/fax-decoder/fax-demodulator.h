#
/*
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
#ifndef	__FAX_DEMODULATOR__
#define	__FAX_DEMODULATOR__

#include	"radio-constants.h"

class	faxDemodulator {
public:
			faxDemodulator	(int8_t, int32_t, int32_t);
			~faxDemodulator	(void);
	int16_t		demodulate	(std::complex<float>);
	void		setMode		(int8_t);
	void		setDeviation	(int16_t);
private:
	int8_t		mode;
	int32_t		Rate;
	int32_t		deviation;
	std::complex<float>	prevSample;
};

#endif

