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
 *
 */
#include "viterbi-drm.h"
/*
 *
 *	Note that in this - very straighforward - implementation
 *	the polynomes are the same as in the coder (i.e. from
 *	the viewpoint of the receiver, reversed).
 */
#define		K	7	// constraint length
#define		N_POLYS	6
#define		Poly1	0133
#define		Poly2	0171
#define		Poly3	0145
#define		Poly4	0133
#define		Poly5	0171
#define		Poly6	0145

#define	NumofStates	(1 << (K - 1))

	viterbi_drm::viterbi_drm (int16_t blockLength) {
int16_t	i, j;

	this	-> blockLength	= blockLength;


	history = new int32_t *[blockLength + N_POLYS + 1];
	Costs	= new float   *[blockLength + N_POLYS + 1];

	for (i = 0; i < blockLength + N_POLYS + 1; i ++) {
	   history [i]	= new int32_t [NumofStates];
	   Costs [i]	= new float [NumofStates];
	}
	predecessor_for_0	= new int16_t [NumofStates];
	predecessor_for_1	= new int16_t [NumofStates];

//	indexTable	= new float [2 * NumofStates];
//
//	These tables give a mapping from (state * bit * Poly -> outputbit)
	poly1_table	= new uint8_t [2 * NumofStates];
	for (i = 0; i < 2; i ++)
	   for (j = 0; j < NumofStates; j ++)
	      poly1_table [i * NumofStates + j] = bitFor (j, Poly1, i);

	poly2_table	= new uint8_t [2 * NumofStates];
	for (i = 0; i < 2; i ++)
	   for (j = 0; j < NumofStates; j ++)
	      poly2_table [i * NumofStates + j] = bitFor (j, Poly2, i);

	poly3_table	= new uint8_t [2 * NumofStates];
	for (i = 0; i < 2; i ++)
	   for (j = 0; j < NumofStates; j ++)
	      poly3_table [i * NumofStates + j] = bitFor (j, Poly3, i);


	poly4_table	= new uint8_t [2 * NumofStates];
	for (i = 0; i < 2; i ++)
	   for (j = 0; j < NumofStates; j ++)
	      poly4_table [i * NumofStates + j] = bitFor (j, Poly4, i);

	poly5_table	= new uint8_t [2 * NumofStates];
	for (i = 0; i < 2; i ++)
	   for (j = 0; j < NumofStates; j ++)
	      poly5_table [i * NumofStates + j] = bitFor (j, Poly5, i);

	poly6_table	= new uint8_t [2 * NumofStates];
	for (i = 0; i < 2; i ++)
	   for (j = 0; j < NumofStates; j ++)
	      poly6_table [i * NumofStates + j] = bitFor (j, Poly6, i);
//
	for (i = 0; i < NumofStates; i ++) {
	   predecessor_for_0 [i] = ((i << 1) + 00) & (NumofStates - 1);
	   predecessor_for_1 [i] = ((i << 1) + 01) & (NumofStates - 1);
	}

}

	viterbi_drm::~viterbi_drm (void) {
int	i;

	delete	[]	predecessor_for_0;
	delete	[]	predecessor_for_1;
	delete	[]	poly1_table;
	delete	[]	poly2_table;
	delete	[]	poly3_table;
	delete	[]	poly4_table;
	delete	[]	poly5_table;
	delete	[]	poly6_table;
	for (i = 0; i < blockLength + N_POLYS + 1; i ++)  {
	   delete [] history [i];
	   delete [] Costs [i];
	}
	delete [] history;
	delete [] Costs;
}
//
void	viterbi_drm::deconvolve (metrics *sym,
	                         int16_t blockLength,
	                         uint8_t *out) {
uint16_t	cState, bestState;
int32_t 	i;
uint16_t	prev_0, prev_1;
float	 	costs_0, costs_1;
float		minimalCosts;
int32_t		sequence  [blockLength + N_POLYS + 1];

	if (this -> blockLength != blockLength)
	   fprintf (stderr, "expected %d, got %d\n",
	                      this -> blockLength, blockLength);


//int32_t	**history = new int32_t *[blockLength + N_POLYS + 1];
//float	**Costs	= new float   *[blockLength + N_POLYS + 1];
//
//	for (i = 0; i < blockLength + N_POLYS + 1; i ++) {
//	   history [i]	= new int32_t [NumofStates];
//	   Costs [i]	= new float [NumofStates];
//	}
	memset (Costs [0], 0, NumofStates * sizeof (float));

//	first step is to "pump" the soft bits into the state machine
//	and compute the cost matrix.
//	we assume the overall costs for state 0 are zero
//	and remain zero
	for (i = 1; i <= blockLength + N_POLYS; i ++) {
	   for (cState = 0; cState < NumofStates / 2; cState ++) {
	      prev_0	= predecessor_for_0 [cState];
	      prev_1	= predecessor_for_1 [cState];

//	we compute the minimal costs, based on the costs of the
//	prev states, and the additional costs of arriving from
//	the previous state to the current state with the symbol "sym"
//
//	entrybit = 0, so the index for the cost function is prev_xx
	      costs_0 = Costs [i - 1][prev_0] +
	                costsFor (prev_0, &sym [N_POLYS * (i - 1)]);
	      costs_1 = Costs [i - 1][prev_1] +
	                costsFor (prev_1, &sym [N_POLYS * (i - 1)]);
	      if (costs_0 < costs_1) {
	         Costs [i][cState] = costs_0;
	         history [i][cState] = prev_0;
	      } else {
	         Costs [i][cState] = costs_1;
	         history [i][cState] = prev_1;
	      }
	   }

	   for (cState = NumofStates / 2; cState < NumofStates; cState ++) {
	      prev_0	= predecessor_for_0 [cState];
	      prev_1	= predecessor_for_1 [cState];

//	we compute the minimal costs, based on the costs of the
//	prev states, and the additional costs of arriving from
//	the previous state to the current state with the symbol "sym"
//
//	entrybit is here "1", so the index is id cost function
//	is prev_xx + NumofStates
	      costs_0 = Costs [i - 1][prev_0] +
	                costsFor (prev_0 + NumofStates, &sym [N_POLYS * (i - 1)]);
	      costs_1 = Costs [i - 1][prev_1] +
	                costsFor (prev_1 + NumofStates, &sym [N_POLYS * (i - 1)]);
	      if (costs_0 < costs_1) {
	         Costs [i][cState] = costs_0;
	         history [i][cState] = prev_0;
	      } else {
	         Costs [i][cState] = costs_1;
	         history [i][cState] = prev_1;
	      }
	   }
	}
//
//	Once all costs are computed, we can look for the minimal cost
//	Our "end state" is somewhere in column blockLength + N_POLYS
	minimalCosts	= INT_MAX;
	bestState	= 0;
	for (i = 0; i < NumofStates; i++) {
	   if (Costs [blockLength][i] < minimalCosts) {
	      minimalCosts = Costs [blockLength][i];
	      bestState = i;
	   }
	}
	sequence [blockLength] = bestState;
/*
 *	Trace back goes back to state 0, and builds up the
 *	sequence of decoded symbols
 */
	for (i = blockLength; i > 0; i --) 
	   sequence [i - 1] = history [i][sequence[i]];

	for (i = 1; i <= blockLength; i++) 
	   out [i - 1] = (sequence [i] >= NumofStates / 2) ? 01 : 00;

//	for (i = 0; i < blockLength + N_POLYS + 1; i ++)  {
//	   delete [] history [i];
//	   delete [] Costs [i];
//	}
//	delete [] history;
//	delete [] Costs;
}

float	viterbi_drm::costsFor (uint16_t lIndex, metrics *sym) {
float	res1, res2, res3, res4, res5, res6;

uint8_t	targetBit	= 0;

//	still considering to make this code more general by
//	creating a loop-like construct
	targetBit = poly1_table [lIndex];
	if (targetBit == 0)
	   res1 = sym [0] .rTow0;
	else
	   res1 = sym [0]. rTow1;

	targetBit = poly2_table [lIndex];
	if (targetBit == 0)
	   res2 = sym [1]. rTow0;
	else
	   res2 = sym [1]. rTow1;

	targetBit = poly3_table [lIndex];
	if (targetBit == 0)
	   res3 = sym [2]. rTow0;
	else
	   res3 = sym [2]. rTow1;

	targetBit = poly4_table [lIndex];
	if (targetBit == 0)
	   res4 = sym [3]. rTow0;
	else
	   res4 = sym [3]. rTow1;

	targetBit = poly5_table [lIndex];
	if (targetBit == 0)
	   res5 = sym [4]. rTow0;
	else
	   res5 = sym [4]. rTow1;

	targetBit = poly6_table [lIndex];
	if (targetBit == 0)
	   res6 = sym [5]. rTow0;
	else
	   res6 = sym [5]. rTow1;

	return res1 + res2 + res3 + res4 +
	       res5 + res6;
}

/*
 *	as an aid, we give a function "bitFor" that, given
 *	the register state, the polynome and the bit to be inserted
 *	returns the bit coming from the engine
 */
uint8_t	viterbi_drm::bitFor (uint16_t state, uint16_t poly, uint8_t bit) {
uint16_t Register;
uint8_t	resBit = 0;
int16_t	i;
//
//	the register after shifting "bit" in would be:
	Register = bit == 0 ? state : (state + NumofStates);
	Register &= poly;
/*
 *	now for the individual bits
 */
	for (i = 0; i <= K; i++) {
	   resBit ^= (Register & 01);
	   Register >>= 1;
	}

	return resBit;
}
//
//	Given the output, we can construct the input (in bits)
//	that would have been the correct one
void	viterbi_drm::convolve (uint8_t *input,
	                       int16_t blockLength, uint8_t *out) {
int16_t	i;
uint8_t	a0, a1 = 0,
	    a2 = 0,
	    a3 = 0,
	    a4 = 0,
	    a5 = 0,
	    a6 = 0;

	for (i = 0; i < blockLength; i ++) {
	   a0 = input [i];
	   out [6 * i + 0] = a0 ^ a2 ^ a3 ^ a5 ^ a6;
	   out [6 * i + 1] = a0 ^ a1 ^ a2 ^ a3 ^ a6;
	   out [6 * i + 2] = a0 ^ a1 ^ a4 ^ a6;
	   out [6 * i + 3] = out [6 * i + 0];
	   out [6 * i + 4] = out [6 * i + 1];
	   out [6 * i + 5] = out [6 * i + 2];
//
//	now shift
	   a6 = a5; a5 = a4; a4 = a3; a3 = a2; a2 = a1; a1 = a0;
	}
//
//	Now the residu bits. Empty the registers by shifting in
//	zeros
	for (i = blockLength; i < blockLength + 6; i ++) {
	   a0 = 0;
	   out [6 * i + 0] = a0 ^ a2 ^ a3 ^ a5 ^ a6;
	   out [6 * i + 1] = a0 ^ a1 ^ a2 ^ a3 ^ a6;
	   out [6 * i + 2] = a0 ^ a1 ^ a4 ^ a6;
	   out [6 * i + 3] = out [6 * i + 0];
	   out [6 * i + 4] = out [6 * i + 1];
	   out [6 * i + 5] = out [6 * i + 2];
//
//	now shift
	   a6 = a5; a5 = a4; a4 = a3; a3 = a2; a2 = a1; a1 = a0;
	}
}

