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
 *    viterbi.h  --  Viterbi decoder
 *
 *    Copyright (C) 2001, 2002, 2003
 *      Tomi Manninen (oh2bns@sral.fi)
 *
 *    This file is adapted from gMFSK for use in the ESDR software.
 *
 *    gMFSK is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    gMFSK is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with gMFSK; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef __VITERBI_DECODER_H__
#define __VITERBI_DECODER_H__
#include	<stdio.h>
#include	<string.h>
#include	<limits.h>
#include	<stdint.h>

#define PATHMEM 64

class viterbi {
public:
		viterbi			(int32_t, int32_t, int32_t);
		~viterbi 		();
	int32_t	viterbi_set_traceback	(int32_t);
	int32_t	viterbi_set_chunksize	(int32_t);
	int32_t	viterbi_decode		(unsigned char *, int32_t *);
	void	viterbi_reset		(void);
private:
	int32_t	traceback		(uint32_t, int32_t, int32_t *);
	int32_t	v_traceback;
	int32_t	LengthofShiftregister;
	int32_t	NumberofStates;
	int32_t	Poly1;
	int32_t	Poly2;
	int32_t	*dibitTable		[2];
	int32_t	Dibitsfor		(uint32_t, int32_t);
	int32_t	v_chunksize;
	int32_t	**metrics;
	int32_t	**history;
	int32_t	*sequence;
	int32_t	mettab   [2][256];
	uint32_t ptr;
};
#endif
