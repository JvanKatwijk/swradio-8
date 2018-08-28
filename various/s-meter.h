#
/*
 *    Copyright (C) 2010, 2011, 2012
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
 *
 */
#ifndef	__S_METER__
#define	__S_METER__

#include	"radio-constants.h"

class	S_Meter {
public:
		S_Meter (int32_t rate);
		~S_Meter	(void);
float		MeterValue	(std::complex<float> sample);
void		set_bitDepth	(int16_t);
	
private:
	float	s_attack;
	float	s_decay;
	float	S_attack;
	float	S_decay;
	int16_t	bitDepth;
	int32_t	zeroLevel;
	int16_t	fromDBtoS		(float db);
};
#endif

