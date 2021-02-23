#
/*
 *
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
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
#
#
//	Simple class for crc checking.
//	It is worthwhile to see whether we can include the
//	polynomes here
//
#ifndef	__CHECK_CRC
#define	__CHECK_CRC

#include	<stdint.h>
#include	<cstring>

class	checkCRC {
public:
		checkCRC	(int16_t size, const uint16_t *polynome);
		~checkCRC	(void);
	bool	doCRC		(uint8_t *, int16_t);
private:
	int16_t	size;
const	uint16_t	*polynome;
};

#endif

