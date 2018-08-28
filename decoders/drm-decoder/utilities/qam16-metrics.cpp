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
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR) and is a rewrite of
 *    parts of the file "Metrics.cpp" of the dream software.
 *    All rights are acknowledged.
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
/* Definitions ****************************************************************/
/* Definitions for binary numbers (BI). On the left is most sigificant bit */
#define BI_00   0 /* Two bits */
#define BI_01   1
#define BI_10   2
#define BI_11   3

/**********************************************************************/
/* 16QAM **************************************************************/
/**********************************************************************/
/*		(i_0  i_1  q_0  q_1) = (y_0,0  y_1,0  y_0,1  y_1,1)   */

#include	"qam16-metrics.h"
#include	"basics.h"

static
const float rTableQAM16 [4][2] = {
    { 0.9486832980,  0.9486832980},
    {-0.3162277660, -0.3162277660},
    { 0.3162277660,  0.3162277660},
    {-0.9486832980, -0.9486832980}
};
	qam16_metrics::qam16_metrics	(void) {
}

	qam16_metrics::~qam16_metrics	(void) {
}
//
void	qam16_metrics::computemetrics	(theSignal	*signalVector,
	                                 int32_t	amount,
	                                 uint8_t	level,
	                                 metrics	*outputMetrics,
	                                 bool		retry,
	                                 uint8_t	*level_0,
	                                 uint8_t	*level_1) {
int32_t	i;
/*	There are two possible points for each bit. Both have to   */
/*	be used. In the first step: {i_1}, {q_1} = 0               */
/*	In the second step: {i_1}, {q_1} = 1                       */
/*	Calculate distances */


	if (level == 0 && !retry) { // we go for i0, q0 for the first time
	   for (i = 0; i < amount; i ++) {
	   outputMetrics [2 * i]. rTow0 =
	                      Minimum2 (real (signalVector [i]. signalValue),
	                               rTableQAM16[BI_00 /* [0  0] */][0],
	                               rTableQAM16[BI_01 /* [0  1] */][0],
	                               signalVector [i]. rTrans);

	   outputMetrics [2 * i]. rTow1 =
	                     Minimum2 (real (signalVector [i]. signalValue),
	                               rTableQAM16[BI_10 /* [1  0] */][0],
	                               rTableQAM16[BI_11 /* [1  1] */][0],
	                               signalVector [i]. rTrans);

//	 Imaginary part, i.e. q0                                    */
	   outputMetrics [2 * i + 1]. rTow0 =
	                     Minimum2 (imag (signalVector [i]. signalValue),
	                               rTableQAM16[BI_00 /* [0  0] */][1],
	                               rTableQAM16[BI_01 /* [0  1] */][1],
	                               signalVector [i]. rTrans);

	   outputMetrics [2 * i + 1]. rTow1 =
	                     Minimum2 (imag (signalVector [i]. signalValue),
	                               rTableQAM16[BI_10 /* [1  0] */][1],
	                               rTableQAM16[BI_11 /* [1  1] */][1],
	                               signalVector [i]. rTrans);
	   }
	}
	else
	if (level == 0 && retry) { // we go for i0, q0, we have "level_1"
	   for (i = 0; i < amount; i ++) {
	      uint8_t iTabInd0;

//	the real part: Lowest bit defined by "level_1" 
	      iTabInd0 = level_1 [2 * i] & 1;
//	Distance to "0" */
//
//	Looking at the i1, knowing i0
//	suppose i0 == 0, then we refer to 3 / sq10 for the distance to 0
//	and to -1 / sq10 for the distance to 1 
//	suppose the i0 == 1, then distance to "0" refers to
//	 Table [1][0] (i.e. 1 / sq10) and the distance to "1"
//	is measured as x - Table [3] [0], i.e. -3 / sq10
//	
	      outputMetrics [2 * i]. rTow0 =
	            Minimum1 (real (signalVector [i]. signalValue),
	                      rTableQAM16 [iTabInd0][0],
	                      signalVector [i]. rTrans);

/* Distance to "1" */
	      outputMetrics [2 * i]. rTow1 =
	            Minimum1 (real (signalVector [i]. signalValue),
	                      rTableQAM16 [iTabInd0 | (1 << 1)][0],
	                      signalVector [i]. rTrans);
//
//	the imag part: Lowest bit defined by "level_1" 
             iTabInd0 = level_1 [2 * i + 1] & 1;
/* Distance to "0" */
	     outputMetrics [2 * i + 1]. rTow0 =
	           Minimum1 (imag (signalVector [i]. signalValue),
	                     rTableQAM16 [iTabInd0][1],
	                     signalVector [i]. rTrans);

/* Distance to "1" */
	     outputMetrics [2 * i + 1]. rTow1 =
	           Minimum1 (imag (signalVector [i]. signalValue),
	                     rTableQAM16 [iTabInd0 | (1 << 1)][1],
	                     signalVector [i]. rTrans);
	   }
	}
	else		// level_1, we obviously have level_0
	for (i = 0; i < amount; i ++) {
/* Highest bit defined by "vecSubsetDef1" */
//	for the real part
	   uint8_t iTabInd0 = ((level_0 [2 * i]) & 1) << 1;

/* Distance to "0" */
//	if the level_0 bit = 0, we refer to 3/sq10
//	if the level_0 bit = 1, we refer to -1 / sq10
	   outputMetrics [2 * i].rTow0 =
	                  Minimum1 (real (signalVector [i]. signalValue),
                                    rTableQAM16 [iTabInd0][0],
	                            signalVector [i]. rTrans);

/* Distance to "1" */
//	if the level_0 bit = 0, we refer to 1 / sq10
//	if the level_0 bit = 1, we refer to -3 / sq10

	   outputMetrics [2 * i].rTow1 =
	                  Minimum1 (real (signalVector [i]. signalValue),
	                            rTableQAM16 [iTabInd0 | 1][0],
	                            signalVector [i].rTrans);
//
//	if e.g. the value = 0.1 while the level_0 bit = 0
//	then the rTow0	= 3 / sq10 - 0.1
//	and the  rTow1	= 1 / sq10 - 0.1
//	for the impart
//	Highest bit defined by "vecSubsetDef1" 
	   iTabInd0 = ((level_0 [2 * i + 1]) & 1) << 1;

/* Distance to "0" */
	   outputMetrics [2 * i + 1]. rTow0 =
	                  Minimum1 (imag (signalVector [i]. signalValue),
	                            rTableQAM16 [iTabInd0][1],
	                            signalVector [i].rTrans);

/* Distance to "1" */
	   outputMetrics [2 * i + 1]. rTow1 =
	                  Minimum1 (imag (signalVector [i]. signalValue),
	                            rTableQAM16 [iTabInd0 | 1][1],
	                            signalVector [i].rTrans);
	}
}

