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
#include	"qam4-metrics.h"

static
const DRM_FLOAT rTableQAM4[2][2] = {
    { 0.7071067811f,  0.7071067811f},
    {-0.7071067811f, -0.7071067811f}
};

	qam4_metrics::qam4_metrics	(void) {
}

	qam4_metrics::~qam4_metrics	(void) {
}

void	qam4_metrics::computemetrics	(theSignal *signalVector,
	                                 int32_t amount,
	                                 metrics *outputMetrics) {
int32_t	i;
	for (i = 0; i < amount; i++) {
/* Real part , i.e. y_0,0------------------------------------------- */
/* Distance to "0"						     */
                 outputMetrics [2 * i]. rTow0 =
	                        Minimum1 (real (signalVector [i]. signalValue),
	                                  rTableQAM4 [0][0],
	                                  signalVector [i]. rTrans);
/* Distance to "1" */
                 outputMetrics [2 * i]. rTow1 =
	                        Minimum1 (real (signalVector [i]. signalValue),
	                                  rTableQAM4 [1][0],
	                                  signalVector [i]. rTrans);

/* Imaginary part, i.e. y_0,1---------------------------------------- */
/* Distance to "0" */
	         outputMetrics [2 * i + 1]. rTow0 =
	                        Minimum1 (imag (signalVector [i]. signalValue),
	                                  rTableQAM4[0][1],
	                                  signalVector [i]. rTrans);

/* Distance to "1" */
	         outputMetrics [2 * i + 1]. rTow1 =
	                        Minimum1 (imag (signalVector [i]. signalValue),
                                          rTableQAM4 [1][1],
	                                  signalVector [i]. rTrans);
	}
}

