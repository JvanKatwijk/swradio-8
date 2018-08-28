#
/*
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
 */
#
#include	"checkcrc.h"
#include	<float.h>
#include	<math.h>

//	Straightforward CRC implementation

	checkCRC::checkCRC	(int16_t size, const uint16_t *polynome) {
	this	-> size		= size;
	this	-> polynome	= polynome;
}

	checkCRC::~checkCRC	(void) {
}
//
//	length in bits, assumption: 1 bit per byte
//	assumption: CRC included at the end
bool	checkCRC::doCRC		(uint8_t *in, int16_t length) {
int16_t	i, j;
uint16_t Sum	= 0;
uint8_t	Register [16];		//either 8 or 16, so it would be enough

	memset (Register, 1, size);
//
//	One's complement has been passed for the check bits
	for (i = length - size; i < length; i ++)
	   in [i] ^= 1;

//	Most significant bit left
	for (i = 0; i < length; i ++) {
	   if ((Register [0] ^ in [i]) == 1) {
	      for (j = 0; j < size - 1; j ++)
	         Register [j] = polynome [j] ^ Register [j + 1];
	      Register [size - 1] = 1;
	   }
	   else {
	      memmove (&Register [0], &Register [1],
	                   (size - 1) * sizeof (uint8_t));
	      Register [size - 1] = 0;
	   }
	}

	for (i = 0; i < size; i ++)
	   Sum += Register [i];
	return Sum == 0;
}

