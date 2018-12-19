#
/*
 *
 *    Copyright (C) 2008, 2009, 2010
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    The interleaver is a reimplementation (adapted for use
 *    in SDR-J), of the interleaver appearing in the gmfsk0.6 code.
 *    All copyrights of the original authors are recognized.
 *
 *    This file is part of the SDR-J.
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
#include	<stdio.h>
#include	"interleave.h"
#include	<string.h>
#ifdef UNIX
#include	<alloca.h>
#endif
/*
 *	the interleaver is a repetative interleaver, using
 *	the diagonal of the size * size matrix as output.
 *	in order to overcome the limitations of size (in the
 *	case of mfsk, it is 4), it is applied repetative
 */
	Interleaver::Interleaver (int16_t rep, int16_t size, uint8_t dir) {

	this	-> dir	= dir;
	this	-> size	= size;
	this	-> rep	= rep;
	this	-> table	= new uint8_t [rep * size * size];
	memset (table, 0, rep * size * size * sizeof (uint8_t));
}

	Interleaver::~Interleaver () {
	delete[]	table;
}

uint8_t Interleaver::get (int16_t i, int16_t j, int16_t k) {
	return table [size * size * i + size * j + k];
}

void	Interleaver::set (int16_t i, int16_t j, int16_t k, uint8_t v) {
	table [size * size * i + size * j + k] = v;
}

void	Interleaver::InterleaveSyms (uint8_t *syms) {
int16_t i, j, k;

	for (k = 0; k < rep; k ++) {
/*
 *	move all colums:
 *	M [i, j] := M [i, j + 1];
 */
	   for (i = 0; i < size; i ++) 
	      for (j = 0; j < size - 1; j ++) 
	         set (k, i, j, get (k, i, j + 1));
/*
 *	... and then shift in the new symbols
 *
 *	M [i, size] := syms [i];
 */
	   for (i = 0; i < size; i ++)
	      set (k, i, size - 1, syms [i]);
/*
 *	Now the result is formed by the diagonal elements
 */
	   for (i = 0; i < size; i ++)
	      if (dir == INTERLEAVE_FWD)
	         syms [i] = get (k, i, size - i - 1);
	      else
	         syms [i] = get (k, i, i);
	}
}

void 	Interleaver::InterleaveBits (uint32_t *bits) {
uint8_t	*syms;
int16_t	i;

	syms = (uint8_t *)alloca (size * sizeof (uint8_t));

	for (i = 0; i < size; i ++)
	   syms [i] = (*bits >> (size - i - 1)) & 1;

	InterleaveSyms (syms);

	*bits = 0;
	for (i = 0; i < size; i ++ )
	   *bits = (*bits << 1) | syms [i];
}

