#
/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001-2014
 *
 * Author(s):
 *  Volker Fischer
 *
 * Description:
 *
 * The metric is calculated as follows:
 * Using ML: M = |r - s * h| = |h| * |r / h - s|
 * Using MAP: M = |r - s * h|^2 = |h|^2 * |r / h - s|^2
 *
 * Subset definition:
 * [1 2 3]  -> ([vecSubsetDef1 vecSubsetDef2 vecSubsetDef3])
 *
 ******************************************************************************
 *
 *    Copyright (C) 2015
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR). It is a rewrite of (part of)
 *    the file "Metrics.cpp" of the dream software. All rights acknowledged.
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
 */

#include	"qam64-metrics.h"
#include	"basics.h"

const float rTableQAM64SM [8][2] = {
    { 1.0801234497f,  1.0801234497f},
    {-0.1543033499f, -0.1543033499f},
    { 0.4629100498f,  0.4629100498f},
    {-0.7715167498f, -0.7715167498f},
    { 0.7715167498f,  0.7715167498f},
    {-0.4629100498f, -0.4629100498f},
    { 0.1543033499f,  0.1543033499f},
    {-1.0801234497f, -1.0801234497f}
};

#define BI_000  0 /* Three bits */
#define BI_001  1
#define BI_010  2
#define BI_011  3
#define BI_100  4
#define BI_101  5
#define BI_110  6
#define BI_111  7

//
//	64QAM SM
//
//	Definitions
//	Input bits are collected in bytes separately for imaginary
//	and real part. The order is:
//	[i_0, i_1, i_2] and [q_0, q_1, q_2] -> {i, q}
//	 All entries are normalized according to the DRM-standard */
//
//	(i_0  i_1  i_2  q_0  q_1  q_2) =
//	             (y_0,0  y_1,0  y_2,0  y_0,1  y_1,1  y_2,1) 

	qam64_metrics::qam64_metrics	(void) {
}

	qam64_metrics::~qam64_metrics	(void) {
}

//
//	level_0, level_1 and level_2 are the "input as it should be"
//	for the (i0, q0), (i1, q1) and (i2, q2) sequences.
//
//	"retry" is true when we pass for the second or more time
//	such that we can use all data of previous rounds
void	qam64_metrics::computemetrics	(theSignal	*signalVector,
	                                 int32_t	amount,
	                                 uint8_t	iLevel,
	                                 metrics	*outputMetrics,	
	                                 bool		retry,
	                                 uint8_t	*level_0,
	                                 uint8_t	*level_1,
	                                 uint8_t	*level_2) {

int32_t	i;
	if (iLevel == 0 && !retry) {
	   for (i = 0; i < amount; i++) {
/* Real part -------------------------------------------- */
	         outputMetrics [2 * i]. rTow0 =
                        Minimum4 (real (signalVector [i]. signalValue),
                                 rTableQAM64SM[BI_000 /* [0 0 0] */][0],
                                 rTableQAM64SM[BI_001 /* [0 0 1] */][0],
                                 rTableQAM64SM[BI_010 /* [0 1 0] */][0],
                                 rTableQAM64SM[BI_011 /* [0 1 1] */][0],
                                 signalVector [i].rTrans);

	         outputMetrics [2 * i]. rTow1 =
                        Minimum4 (real (signalVector [i]. signalValue),
                                 rTableQAM64SM[BI_100 /* [1 0 0] */][0],
                                 rTableQAM64SM[BI_101 /* [1 0 1] */][0],
                                 rTableQAM64SM[BI_110 /* [1 1 0] */][0],
                                 rTableQAM64SM[BI_111 /* [1 1 1] */][0],
                                 signalVector [i].rTrans);
	
/* Imaginary part --------------------------------------- */
	         outputMetrics [2 * i + 1]. rTow0 =
                        Minimum4 (imag (signalVector [i]. signalValue),
                                 rTableQAM64SM[BI_000 /* [0 0 0] */][1],
                                 rTableQAM64SM[BI_001 /* [0 0 1] */][1],
                                 rTableQAM64SM[BI_010 /* [0 1 0] */][1],
                                 rTableQAM64SM[BI_011 /* [0 1 1] */][1],
                                 signalVector [i].rTrans);

	         outputMetrics [2 * i + 1]. rTow1 =
                        Minimum4 (imag (signalVector [i]. signalValue),
                                 rTableQAM64SM[BI_100 /* [1 0 0] */][1],
                                 rTableQAM64SM[BI_101 /* [1 0 1] */][1],
                                 rTableQAM64SM[BI_110 /* [1 1 0] */][1],
                                 rTableQAM64SM[BI_111 /* [1 1 1] */][1],
                                 signalVector [i].rTrans);
	      }
	}
	else
	if (iLevel == 1 && !retry) {
// first time here, 
// compute i1 and q1, i0 and q0 are known
	   for (i = 0; i < amount; i ++) {
	      int8_t iTabInd0;
//	real part
//	we know the "highest" bit, so that is being used
	      iTabInd0 = (level_0 [2 * i] & 1) << 2;
	      outputMetrics [2 * i]. rTow0 =
	            Minimum2 (real( signalVector [i]. signalValue),
	                      rTableQAM64SM[iTabInd0    ][0],
	                      rTableQAM64SM[iTabInd0 | 1][0],
	                      signalVector [i]. rTrans);

	      iTabInd0 = ((level_0 [2 * i] & 1) << 2) | (1 << 1);
	      outputMetrics [2 * i]. rTow1 =
	            Minimum2 (real (signalVector [i]. signalValue),
	                      rTableQAM64SM[iTabInd0    ][0],
	                      rTableQAM64SM[iTabInd0 | 1][0],
	                      signalVector [i]. rTrans);
//	imag part
	      iTabInd0 = ((level_0 [2 * i + 1] & 1) << 2);
	      outputMetrics [2 * i + 1]. rTow0 =
	            Minimum2 (imag (signalVector [i]. signalValue),
	                      rTableQAM64SM[iTabInd0    ][1],
	                      rTableQAM64SM[iTabInd0 | 1][1],
	                      signalVector [i]. rTrans);

	      iTabInd0 = ((level_0 [2 * i + 1] & 1) << 2) | (1 << 1);
	      outputMetrics [2 * i + 1]. rTow1 =
	            Minimum2 (imag (signalVector [i]. signalValue),
	                      rTableQAM64SM[iTabInd0    ][1],
	                      rTableQAM64SM[iTabInd0 | 1][1],
	                      signalVector [i].rTrans);
	   }
	}
	else
	if (iLevel == 2) {
//	first time or it does not matter anymore now, since we
//	know i0, i1, q0 and q1. So, compute i2, q2
	   for (i = 0; i < amount; i ++) {
	      uint8_t iTabInd0;
//	Highest bit defined by "level_0",
//	next bit defined  by "level_1" */
//	real part
	      iTabInd0 = ((level_0 [2 * i] & 1) << 2) |
	                 ((level_1 [2 * i] & 1) << 1);

              outputMetrics [2 * i]. rTow0 =
	            Minimum1 (real (signalVector [i]. signalValue),
	                      rTableQAM64SM [iTabInd0][0],
	                      signalVector [i]. rTrans);

	      outputMetrics [2 * i]. rTow1 =
	            Minimum1 (real (signalVector [i]. signalValue),
	                      rTableQAM64SM [iTabInd0 | 1][0],
	                      signalVector [i]. rTrans);
//
//	imag part
//	Highest bit defined by "level_0"
//	next bit defined by "level_1" 
	      iTabInd0 = ((level_0 [2 * i + 1] & 1) << 2) |
	                 ((level_1 [2 * i + 1] & 1) << 1);

	      outputMetrics [2 * i + 1]. rTow0 =
	            Minimum1 (imag (signalVector [i]. signalValue),
	                      rTableQAM64SM [iTabInd0][1], 
	                      signalVector [i]. rTrans);

	      outputMetrics [2 * i + 1]. rTow1 =
	            Minimum1 (imag (signalVector [i]. signalValue),
	                      rTableQAM64SM [iTabInd0 | 1][1],
	                      signalVector [i]. rTrans);
	   }
        }
	else
	if (iLevel == 0 && retry) {
//	from previous runs we have approximations for i1, i2, q1 and q2
	   for (i = 0; i < amount; i ++) {
	      uint8_t iTabInd0;
//	Lowest bit defined by "level_2"
//	next bit defined by "level_1"

//	first, the real part
	      iTabInd0 = (level_2 [2 * i] & 1) |
	                 ((level_1 [2 * i] & 1) << 1);

	      outputMetrics [2 * i]. rTow0 =
	            Minimum1 (real (signalVector [i]. signalValue),
	                      rTableQAM64SM[iTabInd0][0],
	                      signalVector [i]. rTrans);

	      outputMetrics [2 * i]. rTow1 =
	            Minimum1 (real (signalVector [i]. signalValue),
	                      rTableQAM64SM[iTabInd0 | (1 << 2)][0],
	                      signalVector [i]. rTrans);
//
//	the Imag part

	      iTabInd0 = ( level_2 [2 * i + 1] & 1) |
	                 ((level_1 [2 * i + 1] & 1) << 1);

	      outputMetrics [2 * i + 1]. rTow0 =
	            Minimum1 (imag (signalVector [i]. signalValue),
	                      rTableQAM64SM[iTabInd0][1],
	                      signalVector [i]. rTrans);

	      outputMetrics [2 * i + 1]. rTow1 =
	            Minimum1 (imag (signalVector [i]. signalValue),
	                      rTableQAM64SM[iTabInd0 | (1 << 2)][1],
	                      signalVector [i]. rTrans);
	   }
	}
	else
	if (iLevel == 1 && retry) {	
//	going for (i1, q1), we know (i0, q0) (level_0)
//	and have an approximation for (i2, q2) (level_2)
	   for (i = 0; i < amount; i ++) {
	      uint8_t iTabInd0;
// real
//	Lowest bit defined by "level_2",
//	highest defined by "level_0" */
	      iTabInd0 = ((level_0 [2 * i] & 1) << 2) |
                         ( level_2 [2 * i] & 1);

	      outputMetrics [2 * i]. rTow0 =
	            Minimum1 (real (signalVector [i]. signalValue),
	                      rTableQAM64SM[iTabInd0][0],
	                      signalVector [i]. rTrans);

	      outputMetrics [2 * i]. rTow1 =
	            Minimum1 (real (signalVector [i]. signalValue),
	                      rTableQAM64SM[iTabInd0 | (1 << 1)][0],
	                      signalVector [i]. rTrans);
//	imag
	      iTabInd0 = ((level_0 [2 * i + 1] & 1) << 2) |
	                 ( level_2 [2 * i + 1] & 1);

	      outputMetrics [2 * i + 1]. rTow0 =
	            Minimum1 (imag (signalVector [i]. signalValue),
	                      rTableQAM64SM[iTabInd0][1],
	                      signalVector [i]. rTrans);

	      outputMetrics [2 * i + 1]. rTow1 =
	            Minimum1 (imag (signalVector [i]. signalValue),
	                      rTableQAM64SM[iTabInd0 | (1 << 1)][1],
	                      signalVector [i]. rTrans);
	   }
	}
	else
//	   we should not be here
	   fprintf (stderr, "do not know what I am doing\n");
}

