#
/*
 *    Copyright (C) 2022
 *    Karlis Goba
 *    YL33G
 *
 *    This file is imported from the ft8 library of Karlis Goba
 *    and the rights obviously remain to him
 *    The file is included in the ft8 decoder of swradio-8
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
// LDPC decoder for FT8.
//
// given a 174-bit codeword as an array of log-likelihood of zero,
// return a 174-bit corrected codeword, or zero-length array.
// last 87 bits are the (systematic) plain-text.
// this is an implementation of the sum-product algorithm
// from Sarah Johnson's Iterative Error Correction book.
// codeword [i] = log (P (x=0) / P(x=1) )
//

#include "ldpc.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include	<stdint.h>

// Each row describes one LDPC parity check.
// Each number is an index into the codeword (1-origin).
// The codeword bits mentioned in each row must XOR to zero.
const uint8_t kFTX_LDPC_Nm[FTX_LDPC_M][7] = {
    { 4, 31, 59, 91, 92, 96, 153 },
    { 5, 32, 60, 93, 115, 146, 0 },
    { 6, 24, 61, 94, 122, 151, 0 },
    { 7, 33, 62, 95, 96, 143, 0 },
    { 8, 25, 63, 83, 93, 96, 148 },
    { 6, 32, 64, 97, 126, 138, 0 },
    { 5, 34, 65, 78, 98, 107, 154 },
    { 9, 35, 66, 99, 139, 146, 0 },
    { 10, 36, 67, 100, 107, 126, 0 },
    { 11, 37, 67, 87, 101, 139, 158 },
    { 12, 38, 68, 102, 105, 155, 0 },
    { 13, 39, 69, 103, 149, 162, 0 },
    { 8, 40, 70, 82, 104, 114, 145 },
    { 14, 41, 71, 88, 102, 123, 156 },
    { 15, 42, 59, 106, 123, 159, 0 },
    { 1, 33, 72, 106, 107, 157, 0 },
    { 16, 43, 73, 108, 141, 160, 0 },
    { 17, 37, 74, 81, 109, 131, 154 },
    { 11, 44, 75, 110, 121, 166, 0 },
    { 45, 55, 64, 111, 130, 161, 173 },
    { 8, 46, 71, 112, 119, 166, 0 },
    { 18, 36, 76, 89, 113, 114, 143 },
    { 19, 38, 77, 104, 116, 163, 0 },
    { 20, 47, 70, 92, 138, 165, 0 },
    { 2, 48, 74, 113, 128, 160, 0 },
    { 21, 45, 78, 83, 117, 121, 151 },
    { 22, 47, 58, 118, 127, 164, 0 },
    { 16, 39, 62, 112, 134, 158, 0 },
    { 23, 43, 79, 120, 131, 145, 0 },
    { 19, 35, 59, 73, 110, 125, 161 },
    { 20, 36, 63, 94, 136, 161, 0 },
    { 14, 31, 79, 98, 132, 164, 0 },
    { 3, 44, 80, 124, 127, 169, 0 },
    { 19, 46, 81, 117, 135, 167, 0 },
    { 7, 49, 58, 90, 100, 105, 168 },
    { 12, 50, 61, 118, 119, 144, 0 },
    { 13, 51, 64, 114, 118, 157, 0 },
    { 24, 52, 76, 129, 148, 149, 0 },
    { 25, 53, 69, 90, 101, 130, 156 },
    { 20, 46, 65, 80, 120, 140, 170 },
    { 21, 54, 77, 100, 140, 171, 0 },
    { 35, 82, 133, 142, 171, 174, 0 },
    { 14, 30, 83, 113, 125, 170, 0 },
    { 4, 29, 68, 120, 134, 173, 0 },
    { 1, 4, 52, 57, 86, 136, 152 },
    { 26, 51, 56, 91, 122, 137, 168 },
    { 52, 84, 110, 115, 145, 168, 0 },
    { 7, 50, 81, 99, 132, 173, 0 },
    { 23, 55, 67, 95, 172, 174, 0 },
    { 26, 41, 77, 109, 141, 148, 0 },
    { 2, 27, 41, 61, 62, 115, 133 },
    { 27, 40, 56, 124, 125, 126, 0 },
    { 18, 49, 55, 124, 141, 167, 0 },
    { 6, 33, 85, 108, 116, 156, 0 },
    { 28, 48, 70, 85, 105, 129, 158 },
    { 9, 54, 63, 131, 147, 155, 0 },
    { 22, 53, 68, 109, 121, 174, 0 },
    { 3, 13, 48, 78, 95, 123, 0 },
    { 31, 69, 133, 150, 155, 169, 0 },
    { 12, 43, 66, 89, 97, 135, 159 },
    { 5, 39, 75, 102, 136, 167, 0 },
    { 2, 54, 86, 101, 135, 164, 0 },
    { 15, 56, 87, 108, 119, 171, 0 },
    { 10, 44, 82, 91, 111, 144, 149 },
    { 23, 34, 71, 94, 127, 153, 0 },
    { 11, 49, 88, 92, 142, 157, 0 },
    { 29, 34, 87, 97, 147, 162, 0 },
    { 30, 50, 60, 86, 137, 142, 162 },
    { 10, 53, 66, 84, 112, 128, 165 },
    { 22, 57, 85, 93, 140, 159, 0 },
    { 28, 32, 72, 103, 132, 166, 0 },
    { 28, 29, 84, 88, 117, 143, 150 },
    { 1, 26, 45, 80, 128, 147, 0 },
    { 17, 27, 89, 103, 116, 153, 0 },
    { 51, 57, 98, 163, 165, 172, 0 },
    { 21, 37, 73, 138, 152, 169, 0 },
    { 16, 47, 76, 130, 137, 154, 0 },
    { 3, 24, 30, 72, 104, 139, 0 },
    { 9, 40, 90, 106, 134, 151, 0 },
    { 15, 58, 60, 74, 111, 150, 163 },
    { 18, 42, 79, 144, 146, 152, 0 },
    { 25, 38, 65, 99, 122, 160, 0 },
    { 17, 42, 75, 129, 170, 172, 0 }
};

// Each row corresponds to a codeword bit.
// The numbers indicate which three LDPC parity checks (rows in Nm) refer to the codeword bit.
// 1-origin.
const uint8_t kFTX_LDPC_Mn[FT8_LDPC_BITS][3] = {
    { 16, 45, 73 },
    { 25, 51, 62 },
    { 33, 58, 78 },
    { 1, 44, 45 },
    { 2, 7, 61 },
    { 3, 6, 54 },
    { 4, 35, 48 },
    { 5, 13, 21 },
    { 8, 56, 79 },
    { 9, 64, 69 },
    { 10, 19, 66 },
    { 11, 36, 60 },
    { 12, 37, 58 },
    { 14, 32, 43 },
    { 15, 63, 80 },
    { 17, 28, 77 },
    { 18, 74, 83 },
    { 22, 53, 81 },
    { 23, 30, 34 },
    { 24, 31, 40 },
    { 26, 41, 76 },
    { 27, 57, 70 },
    { 29, 49, 65 },
    { 3, 38, 78 },
    { 5, 39, 82 },
    { 46, 50, 73 },
    { 51, 52, 74 },
    { 55, 71, 72 },
    { 44, 67, 72 },
    { 43, 68, 78 },
    { 1, 32, 59 },
    { 2, 6, 71 },
    { 4, 16, 54 },
    { 7, 65, 67 },
    { 8, 30, 42 },
    { 9, 22, 31 },
    { 10, 18, 76 },
    { 11, 23, 82 },
    { 12, 28, 61 },
    { 13, 52, 79 },
    { 14, 50, 51 },
    { 15, 81, 83 },
    { 17, 29, 60 },
    { 19, 33, 64 },
    { 20, 26, 73 },
    { 21, 34, 40 },
    { 24, 27, 77 },
    { 25, 55, 58 },
    { 35, 53, 66 },
    { 36, 48, 68 },
    { 37, 46, 75 },
    { 38, 45, 47 },
    { 39, 57, 69 },
    { 41, 56, 62 },
    { 20, 49, 53 },
    { 46, 52, 63 },
    { 45, 70, 75 },
    { 27, 35, 80 },
    { 1, 15, 30 },
    { 2, 68, 80 },
    { 3, 36, 51 },
    { 4, 28, 51 },
    { 5, 31, 56 },
    { 6, 20, 37 },
    { 7, 40, 82 },
    { 8, 60, 69 },
    { 9, 10, 49 },
    { 11, 44, 57 },
    { 12, 39, 59 },
    { 13, 24, 55 },
    { 14, 21, 65 },
    { 16, 71, 78 },
    { 17, 30, 76 },
    { 18, 25, 80 },
    { 19, 61, 83 },
    { 22, 38, 77 },
    { 23, 41, 50 },
    { 7, 26, 58 },
    { 29, 32, 81 },
    { 33, 40, 73 },
    { 18, 34, 48 },
    { 13, 42, 64 },
    { 5, 26, 43 },
    { 47, 69, 72 },
    { 54, 55, 70 },
    { 45, 62, 68 },
    { 10, 63, 67 },
    { 14, 66, 72 },
    { 22, 60, 74 },
    { 35, 39, 79 },
    { 1, 46, 64 },
    { 1, 24, 66 },
    { 2, 5, 70 },
    { 3, 31, 65 },
    { 4, 49, 58 },
    { 1, 4, 5 },
    { 6, 60, 67 },
    { 7, 32, 75 },
    { 8, 48, 82 },
    { 9, 35, 41 },
    { 10, 39, 62 },
    { 11, 14, 61 },
    { 12, 71, 74 },
    { 13, 23, 78 },
    { 11, 35, 55 },
    { 15, 16, 79 },
    { 7, 9, 16 },
    { 17, 54, 63 },
    { 18, 50, 57 },
    { 19, 30, 47 },
    { 20, 64, 80 },
    { 21, 28, 69 },
    { 22, 25, 43 },
    { 13, 22, 37 },
    { 2, 47, 51 },
    { 23, 54, 74 },
    { 26, 34, 72 },
    { 27, 36, 37 },
    { 21, 36, 63 },
    { 29, 40, 44 },
    { 19, 26, 57 },
    { 3, 46, 82 },
    { 14, 15, 58 },
    { 33, 52, 53 },
    { 30, 43, 52 },
    { 6, 9, 52 },
    { 27, 33, 65 },
    { 25, 69, 73 },
    { 38, 55, 83 },
    { 20, 39, 77 },
    { 18, 29, 56 },
    { 32, 48, 71 },
    { 42, 51, 59 },
    { 28, 44, 79 },
    { 34, 60, 62 },
    { 31, 45, 61 },
    { 46, 68, 77 },
    { 6, 24, 76 },
    { 8, 10, 78 },
    { 40, 41, 70 },
    { 17, 50, 53 },
    { 42, 66, 68 },
    { 4, 22, 72 },
    { 36, 64, 81 },
    { 13, 29, 47 },
    { 2, 8, 81 },
    { 56, 67, 73 },
    { 5, 38, 50 },
    { 12, 38, 64 },
    { 59, 72, 80 },
    { 3, 26, 79 },
    { 45, 76, 81 },
    { 1, 65, 74 },
    { 7, 18, 77 },
    { 11, 56, 59 },
    { 14, 39, 54 },
    { 16, 37, 66 },
    { 10, 28, 55 },
    { 15, 60, 70 },
    { 17, 25, 82 },
    { 20, 30, 31 },
    { 12, 67, 68 },
    { 23, 75, 80 },
    { 27, 32, 62 },
    { 24, 69, 75 },
    { 19, 21, 71 },
    { 34, 53, 61 },
    { 35, 46, 47 },
    { 33, 59, 76 },
    { 40, 43, 83 },
    { 41, 42, 63 },
    { 49, 75, 83 },
    { 20, 44, 48 },
    { 42, 49, 57 }
};

const uint8_t kFTX_LDPC_Num_rows[FTX_LDPC_M] = {
    7, 6, 6, 6, 7, 6, 7, 6, 6, 7, 6, 6, 7, 7, 6, 6,
    6, 7, 6, 7, 6, 7, 6, 6, 6, 7, 6, 6, 6, 7, 6, 6,
    6, 6, 7, 6, 6, 6, 7, 7, 6, 6, 6, 6, 7, 7, 6, 6,
    6, 6, 7, 6, 6, 6, 7, 6, 6, 6, 6, 7, 6, 6, 6, 7,
    6, 6, 6, 7, 7, 6, 6, 7, 6, 6, 6, 6, 6, 6, 6, 7,
    6, 6, 6
};

	ldpc::ldpc	() {}
	ldpc::~ldpc	() {}
// codeword is 174 log-likelihoods.
// plain is a return value, 174 ints, to be 0 or 1.
// max_iters is how hard to try.
// ok == 87 means success.
void	ldpc::ldpc_decode (float codeword [], int max_iters,
	                                   uint8_t plain [], int* ok) {
//float m [FTX_LDPC_M][FTX_LDPC_N]; // ~60 kB
//float e [FTX_LDPC_M][FTX_LDPC_N]; // ~60 kB

float	**m = new float *[FTX_LDPC_M];
float	**e = new float *[FTX_LDPC_M];
	for (int i = 0; i < FTX_LDPC_M; i ++) {
	   m [i] = new float [FTX_LDPC_N];
	   e [i] = new float [FTX_LDPC_N];
	}
	   
int min_errors = FTX_LDPC_M;
//
//	we maken M kopien van het codewoord
	for (int j = 0; j < FTX_LDPC_M; j++) {
	   for (int i = 0; i < FTX_LDPC_N; i++) {
	      m [j][i] = codeword [i];
	      e [j][i] = 0.0f;
	   }
	}

	for (int iter = 0; iter < max_iters; iter++) {
//	prepare for decoding, compute the errorss
	   for (int j = 0; j < FTX_LDPC_M; j++) {
	      for (int ii1 = 0; ii1 < kFTX_LDPC_Num_rows [j]; ii1++) {
	         int i1 = kFTX_LDPC_Nm [j] [ii1] - 1;
	         float a = 1.0f;
	         for (int ii2 = 0; ii2 < kFTX_LDPC_Num_rows [j]; ii2++) {
	            int i2 = kFTX_LDPC_Nm [j][ii2] - 1;
	            if (i2 != i1) {
	               a *= tanh (-m [j][i2] / 2.0f);
	            }
	         }
	         e [j][i1] = -2.0f * atanh (a);
	      }
	   }
//
//	new guess for plain values
	   for (int i = 0; i < FTX_LDPC_N; i++) {
	      float l = codeword [i];
	      for (int j = 0; j < 3; j++)
	         l += e [kFTX_LDPC_Mn [i][j] - 1][i];
	      plain [i] = (l > 0) ? 1 : 0;
	   }

           int errors = ldpc_check (plain);

	   if (errors < min_errors) {
//	Update the current best result
	      min_errors = errors;
	      if (errors == 0) {
	         break; // Found a perfect answer
	      }
	   }

//	and update the nodes
           for (int i = 0; i < FTX_LDPC_N; i++) {
	      for (int ji1 = 0; ji1 < 3; ji1++) {
	         int j1 = kFTX_LDPC_Mn [i][ji1] - 1;
	         float l = codeword [i];
	         for (int ji2 = 0; ji2 < 3; ji2++) {
	            if (ji1 != ji2) {
	               int j2 = kFTX_LDPC_Mn [i][ji2] - 1;
	               l += e [j2] [i];
	            }
	         }
	         m [j1] [i] = l;
	      }
	   }
	}

	for (int i = 0; i < FTX_LDPC_M; i ++) {
	   delete []  m [i];
	   delete []  e [i];
	}
	delete [] m;
	delete [] e;
	*ok = min_errors;
}

void	ldpc::bp_decode (float codeword [], int max_iters,
	                              uint8_t plain [], int* ok) {
float tov [FTX_LDPC_N][3];
float toc [FTX_LDPC_N][7];
//float toc [FTX_LDPC_M][7];

int min_errors = FTX_LDPC_M;

//	initialize message data
	for (int n = 0; n < FTX_LDPC_N; ++n) {
	   tov [n][0] = tov [n][1] = tov [n][2] = 0;
	}

	for (int iter = 0; iter < max_iters; iter ++) {
//	Do a hard decision guess (tov = 0 in iter 0)
	   int plain_sum = 0;
	   for (int n = 0; n < FTX_LDPC_N; ++n) {
	      plain [n] = ((codeword[n] + tov[n][0] + tov[n][1] + tov[n][2]) > 0) ? 1 : 0;
	      plain_sum += plain[n];
	   }

	   if (plain_sum == 0) {
//	message converged to all-zeros, which is prohibited
	      break;
	   }

//	Check to see if we have a codeword (check before we do any iter)
	   int errors = ldpc_check (plain);

	   if (errors < min_errors) {
//	we have a better guess - update the result
	      min_errors = errors;
	      if (errors == 0) {
	         break; // Found a perfect answer
	      }
	   }

//	Send messages from bits to check nodes
	   for (int m = 0; m < FTX_LDPC_M; ++m) {
	      for (int n_idx = 0; n_idx < kFTX_LDPC_Num_rows [m]; n_idx ++) {
	         int n = kFTX_LDPC_Nm [m][n_idx] - 1;
//	for each (n, m)
	         float Tnm = codeword [n];
	         for (int m_idx = 0; m_idx < 3; m_idx ++) {
	            if ((kFTX_LDPC_Mn [n][m_idx] - 1) != m) {
	               Tnm += tov [n][m_idx];
	            }
	         }
	         toc [m][n_idx] = tanh (-Tnm / 2);
	      }
	   }

//	send messages from check nodes to variable nodes
	   for (int n = 0; n < FTX_LDPC_N; ++n) {
	      for (int m_idx = 0; m_idx < 3; m_idx ++) {
	         int m = kFTX_LDPC_Mn [n][m_idx] - 1;
//	for each (n, m)
	         float Tmn = 1.0f;
	         for (int n_idx = 0; n_idx < kFTX_LDPC_Num_rows[m]; ++n_idx) {
	            if ((kFTX_LDPC_Nm[m][n_idx] - 1) != n) {
	               Tmn *= toc [m][n_idx];
	            }
	         }
	         tov [n][m_idx] = -2 * atanh (Tmn);
	      }
	   }
	}

	*ok = min_errors;
}
//
// does a 174-bit codeword pass the FT8's LDPC parity checks?
// returns the number of parity errors.
// 0 means total success.
//
int	ldpc::ldpc_check (uint8_t codeword[]) {
int errors = 0;

	for (int m = 0; m < FTX_LDPC_M; ++m) {
	   uint8_t x = 0;
	   for (int i = 0; i < kFTX_LDPC_Num_rows[m]; ++i) {
	      x ^= codeword [kFTX_LDPC_Nm[m][i] - 1];
	   }
	   if (x != 0) {
	      errors ++;
	   }
	}
	return errors;
}

