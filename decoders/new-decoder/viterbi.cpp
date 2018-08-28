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
 *    viterbi.c  --  Viterbi decoder
 *
 *    Copyright (C) 2001, 2002, 2003
 *      Tomi Manninen (oh2bns@sral.fi)
 *
 *    This file is part of gMFSK.
 *	It is adapted for use in the JSDR
 *    software.
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
#include "viterbi.h"
/*
 *	The viterbi algorithm has been recoded for fitness in the
 *	SDR-J framwork.
 *	In this recoding, emphasis was on improved readability
 */
	viterbi::viterbi (int32_t k, int32_t poly1, int32_t poly2) {
int32_t	i, j;

	v_traceback		= PATHMEM - 1;
	v_chunksize		= 8;
	NumberofStates		= 1 << (k - 1);
	LengthofShiftregister	= k - 1;
	Poly1			= poly1;
	Poly2			= poly2;

	dibitTable [0]	= new int32_t [NumberofStates];
	dibitTable [1]	= new int32_t [NumberofStates];
	for (i = 0; i < NumberofStates; i ++) {
	   dibitTable [0] [i] = Dibitsfor (i, 0);
	   dibitTable [1] [i] = Dibitsfor (i, 1);
	}

	metrics		= new int32_t *[PATHMEM];
	history		= new int32_t *[PATHMEM];
	sequence	= new int32_t  [PATHMEM];

	for (i = 0; i < PATHMEM; i++) {
	   metrics [i]	= new int32_t [NumberofStates];
	   history [i]	= new int32_t [NumberofStates];
	   sequence [i] = 0;
	   for (j = 0; j < NumberofStates; j ++) {
	      metrics [i][j] = 0;
	      history [i][j] = 0;
	   }
	}
/*
 *	Initialize metrics table 
 *	Assume a symbol value of 0 is the strongest possible 0
 *	and a symbol value of 255 is the strongest possible 1
 *	a symbol of 128 is an erasure.
 */
	for (i = 0; i < 256; i++) {
	   mettab [0][i] = 128 - i;
	   mettab [1][i] = i - 128;
	}

	ptr = 0;
}

	viterbi::~viterbi () {
int32_t i;
	delete[]	dibitTable [0];
	delete[]	dibitTable [1];
	for (i = 0; i < PATHMEM; i++) {
	   delete[] metrics [i];
	   delete[] history [i];
	}

	delete[]	sequence;
	delete[]	metrics;
	delete[]	history;
}
/*
 *	when in state "state" and we receive bit "bit"
 *	we output two dibits
 *	one using the "top" register and polynome,
 *	the other one using the "bottom"  register and polynome
 */
int32_t	viterbi::Dibitsfor (uint32_t state, int32_t bit) {
int32_t	i;
uint32_t	topRegister, bottomRegister;
uint32_t	firstDibit = 0;
uint32_t	secondDibit = 0;

	topRegister = ((state << 1) | (bit & 01)) & Poly1;
	bottomRegister = ((state << 1) | (bit & 01)) & Poly2;
/*
 *	now xor the individual bits
 */
	for (i = 0; i < LengthofShiftregister + 1; i++) {
	   firstDibit ^= topRegister & 01;
	   topRegister >>= 1;
	}

	for (i = 0; i < LengthofShiftregister + 1; i ++) {
	   secondDibit ^= bottomRegister & 01;
	   bottomRegister >>= 1;
	}

	return (secondDibit << 1) | firstDibit;
}

int32_t	viterbi::viterbi_decode (unsigned char *sym, int32_t *metric) {
uint32_t currptr, prevptr;
uint32_t CurrentState, bestState;
int32_t i, j, metricsAddition [4], bestMetric;

	currptr = ptr;
	prevptr = (ptr - 1) % PATHMEM;

	metricsAddition [0] = mettab [0][sym [1]] + mettab [0][sym [0]];
	metricsAddition [1] = mettab [0][sym [1]] + mettab [1][sym [0]];
	metricsAddition [3] = mettab [1][sym [1]] + mettab [1][sym [0]];
	metricsAddition [2] = mettab [1][sym [1]] + mettab [0][sym [0]];

	for (CurrentState = 0;
             CurrentState < (uint32_t)NumberofStates; CurrentState ++) {
	   int32_t predecessorState_0, predecessorState_1;
	   int32_t metrics_0;
	   int32_t metrics_1;
	   int32_t entrybit	= CurrentState & 01;
/*
 *	we know that each state has two predecessors, to be obtained
 *	by shifting the "youngest" bit out and adding "oldest" bits
 *	zero or one.
 */
	   predecessorState_0 = (CurrentState >> 1) |
	                           (0 << (LengthofShiftregister - 1));
	   predecessorState_1 = (CurrentState >> 1) |
	                           (1 << (LengthofShiftregister - 1));
/*
 *	we look for the additional metrics, obtained
 *	by looking at the metricsAddition applied to the 
 *	dibits being outputted in the predecessorstate
 */
	   metrics_0 = metrics [prevptr][predecessorState_0] +
	            metricsAddition [dibitTable [entrybit][predecessorState_0]];
	   metrics_1 = metrics [prevptr][predecessorState_1] +
	            metricsAddition [dibitTable [entrybit][predecessorState_1]];

	   if (metrics_0 > metrics_1) {
	      metrics [currptr][CurrentState] = metrics_0;
	      history [currptr][CurrentState] = predecessorState_0;
	   } else {
	      metrics [currptr][CurrentState] = metrics_1;
	      history [currptr][CurrentState] = predecessorState_1;
	   }
	}
/*
 *	OK, now for this pointerposition, all states have
 *	got their "best" metrics and the predecessor to
 *	get here.
 */
	ptr = (ptr + 1) % PATHMEM;
	if ((ptr % v_chunksize) == 0) {
	   bestMetric	= INT_MIN;
	   bestState	= 0;

	   for (i = 0; i < NumberofStates; i++) {
	      if (metrics [currptr][i] > bestMetric) {
	         bestMetric = metrics [currptr][i];
	         bestState = i;
	      }
	   }

	   return traceback (currptr, bestState, metric);
	}

	if (metrics [currptr][0] > INT_MAX / 2) {
	   for (i = 0; i < PATHMEM; i++)
	      for (j = 0; j < NumberofStates; j++)
	         metrics [i][j] -= INT_MAX / 2;
	}

	if (metrics [currptr][0] < INT_MIN / 2) {
	   for (i = 0; i < PATHMEM; i++)
	      for (j = 0; j < NumberofStates; j++)
	         metrics [i][j] += INT_MIN / 2;
	}

	return -1;
}
/*
 *	Trace back 'traceback' steps, starting from the best state
 *	and decode the "chuncksize" amount of bits
 */
int32_t viterbi::traceback (uint32_t p, int32_t beststate, int32_t *metric) {
uint32_t	c;	// the result
uint32_t	prev;	// pointer
int32_t	localMetrics;
int32_t	i;

	sequence [p] = beststate;
	for (i = 0; i < v_traceback; i++) {
	   prev = (p - 1) % PATHMEM;
	   sequence [prev] = history [p][sequence[p]];
	   p = prev;
	}

	localMetrics = metrics [p][sequence[p]];
/*
 * Decode 'chunksize' bits
 */
	c = 0;
	for (i = 0; i < v_chunksize; i++) {
/*
 * low bit of state is the previous input bit
 */
	   c = (c << 1) | (sequence [p] & 1);
	   p = (p + 1) % PATHMEM;
	}

	localMetrics = metrics [p][sequence [p]] - localMetrics;
	if (metric)
	   *metric = localMetrics;

	return c;
}

int32_t	viterbi::viterbi_set_traceback (int32_t traceback) {
	if (traceback < 0 || traceback > PATHMEM - 1)
	   return -1;

	v_traceback = traceback;
	return 0;
}

int32_t	viterbi::viterbi_set_chunksize (int32_t chunksize) {
	if (chunksize < 1 || chunksize > v_traceback)
	   return -1;

	v_chunksize = chunksize;
	return 0;
}

void	viterbi::viterbi_reset() {
int32_t i;

	for (i = 0; i < PATHMEM; i++) {
	   memset (metrics [i], 0, NumberofStates * sizeof(int));
	   memset (history [i], 0, NumberofStates * sizeof(int));
	}

	ptr = 0;
}

