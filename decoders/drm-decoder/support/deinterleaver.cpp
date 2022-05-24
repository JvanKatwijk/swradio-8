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
 */
#

#include	"deinterleaver.h"
#include	<stdio.h>
//
//	for the short one, the class is simple
//	and essentially equal to the mapper

	deInterleaver::deInterleaver (int16_t length):mapper (length, 5) {
	this	-> Length	= length;
}

	deInterleaver::~deInterleaver (void) {
}

void	deInterleaver::deInterleave (theSignal *inp, theSignal *out) {
int16_t	i;
	for (i = 0; i < Length; i ++)
	   out [mapper. mapIn (i)] = inp [i];
}

//	long interleaver is a real extension
	deInterleaver_long::deInterleaver_long	(int16_t length,
	                                         int16_t depth):
	                                         deInterleaver (length) {
int16_t	i;

	this	-> depth	= depth;
	this	-> buffer	= new theSignal *[depth];

	for (i = 0; i < depth; i ++)
	   buffer [i] = new theSignal [length];
}

deInterleaver_long::~deInterleaver_long	(void) {
int16_t	i;
	for (i = 0; i < depth; i ++)
	   delete [] buffer [i];
	delete[] buffer;
}
//
void	deInterleaver_long::deInterleave	(theSignal *in,
	                                         theSignal *out) {
int16_t	j, k;
//
//	step 1: add the incoming samples to the buffer
	for (j = 0; j < Length; j ++)
	   buffer [(j % depth)][mapper. mapIn (j)] = in [j];
//
//	step 2: copy the bottom row of the buffer to the output
	for (j = 0; j < Length; j ++)
	   out [j] = buffer [depth - 1][j];
//
//	step 3:	shift the buffer elements
	for (k = depth - 1; k > 0; k --)
	   for (j = 0; j < Length; j ++)
	      buffer [k][j] = buffer [k - 1][j];
}

