#
/*
 *    Copyright (C) 2008
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

#ifndef __INTERLEAVER__
#define	__INTERLEAVER__
#
#include	"radio-constants.h"

class	Interleaver {
public:
		Interleaver (int16_t, int16_t, uint8_t);
		~Interleaver (void);
	void	InterleaveSyms (uint8_t	*syms);
	void	InterleaveBits (uint32_t  *bits);

	enum Mode {
	INTERLEAVE_FWD	=	0,
	INTERLEAVE_REV	=	1
	};

private:
	uint8_t get (int16_t, int16_t, int16_t);
	void	set (int16_t, int16_t, int16_t, uint8_t);
	int16_t	size;
	uint8_t	dir;
	int16_t	rep;
	uint8_t	*table;
};
#endif

