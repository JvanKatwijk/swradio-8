#
/*
 *    Copyright (C) 2010, 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of swradio
 *
 *    swradio is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    swradio is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with swradio; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
//
//	to be included by the main program of the decoder

#include	<stdint.h>
#include	<complex>

class	Cache {
private:
	int	nrows;
	int	ncolums;
	std::complex<float> **data;

public:
       Cache (int16_t ncolums, int16_t nrows) {
int16_t i;

        this	-> ncolums	= ncolums;
	this	-> nrows	= nrows;
        data            = new std::complex<float> *[nrows];
        for (i = 0; i < nrows; i++)
           data [i] = new std::complex<float> [ncolums];
	fprintf (stderr, "new cache with %d rows and %d colums\n",
	                            nrows, ncolums);
}

        ~Cache (void) {
int     i;
        for (i = 0; i < nrows; i ++)
           delete[] data [i];

        delete[] data;
}

std::complex<float>     *cacheLine (int16_t n) {
        return data [n];
}

std::complex<float>	cacheElement (int16_t line, int16_t element) {
	return data [line] [element];
}
};

