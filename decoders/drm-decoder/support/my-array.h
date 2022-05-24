#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRunoPlugin_drm
 *
 *    drm plugin is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm plugin is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with drm plugin; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    viterbi.h  --  Viterbi decoder
 *
 */
#ifndef	__MY_ARRAY__
#define	__MY_ARRAY__

#include	<stdint.h>

template <class elementtype>
class	myArray {
public:
	myArray			(int nrows, int ncolumns) {
	this    -> rows         = nrows;
        this    -> columns      = ncolumns;
        table   = new elementtype *[rows];
        for (int i = 0; i < rows; i ++)
           table [i] = new elementtype [columns];
}

	~myArray		() {
	for (int i = 0; i < rows; i ++)
	   delete [] table [i];
	delete [] table;
}

elementtype	*element	(int row) {
	if ((row < 0) || (row >= rows))
	   throw (22);
	return table [row];
}

elementtype elementValue (int row, int column) {
	if ((row < 0) || (row >= rows))
	   throw (22);
	if ((column < 0) || (column >= columns))
	   throw (32);
	return table [row][column];
}


private:
elementtype	**table;
int	rows;
int	columns;
};

#endif

