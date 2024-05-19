#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRuno drm decoder
 *
 *    drm decoder is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm decoder is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with drm decoder; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
//
//	just some very general stuff
#ifndef	__BASICS__
#define	__BASICS__
#include	<stdint.h>
#include	<complex>
#define  _USE_MATH_DEFINES
#include	<math.h>


typedef	float DRM_FLOAT;

//static inline
//std::complex<DRM_FLOAT> cmul (std::complex<DRM_FLOAT> a, DRM_FLOAT b) {
//	return std::complex<DRM_FLOAT> (real (a) * b, imag (a) * b);
//}
//
//static inline
//std::complex<DRM_FLOAT> cdiv(std::complex<DRM_FLOAT> a, DRM_FLOAT b) {
//	return std::complex<DRM_FLOAT>(real(a) / b, imag (a) / b);
//}

enum    {
        QAM4, QAM16, QAM64
};

struct	modeinfo {
	int16_t		Mode;
	uint8_t		Spectrum;
	DRM_FLOAT	sampleRate_offset;
	DRM_FLOAT	freqOffset_fractional;
	int		freqOffset_integer;
	int16_t		timeOffset_integer;
	DRM_FLOAT	timeOffset_fractional;
};

typedef	struct modeinfo smodeInfo;

typedef struct {
        int     symbol;
        int     carrier;
} sdcCell;

typedef struct	ourSignal {
	std::complex<DRM_FLOAT>	signalValue;
	double		rTrans;
} theSignal;

struct metrics_struct {
	DRM_FLOAT	rTow0;
	DRM_FLOAT	rTow1;
};

typedef struct metrics_struct metrics;

//
//	rChan was computed by the equalizer,
//	in the current setting, the best results are
//	with a plain rDist (or squared)
static inline
DRM_FLOAT computeMetric (const DRM_FLOAT rDist, const DRM_FLOAT rChan) {
	return rDist * rDist * rChan;
//	return rDist * rChan;
//	return rDist * rDist;
	return rDist;
#ifdef USE_MAX_LOG_MAP
/* | r / h - s | ^ 2 * | h | ^ 2 */
	return rDist * rDist * rChan;
#else
/* | r / h - s | * | h | */
	return rDist * rChan;
#endif
}

static inline
DRM_FLOAT Minimum1 (const DRM_FLOAT rA, const DRM_FLOAT rB, const DRM_FLOAT rChan) {
//	The minimum in case of only one parameter is trivial 
	return computeMetric (fabs(rA - rB), rChan);
}

static inline
DRM_FLOAT	Minimum2 (const DRM_FLOAT rA,	// value to consider
	          const DRM_FLOAT rB1,	// reference 1
	          const DRM_FLOAT rB2,	// reference 2
	          const DRM_FLOAT rChan) {
        /* First, calculate all distances */
const DRM_FLOAT rResult1 = fabs (rA - rB1);
const DRM_FLOAT rResult2 = fabs (rA - rB2);

/* Return smallest one */
        return  (rResult1 < rResult2) ?
	              computeMetric (rResult1, rChan) :
	              computeMetric (rResult2, rChan);
}

static inline
DRM_FLOAT	Minimum2 (const DRM_FLOAT rA,
	          const DRM_FLOAT rX0,
	          const DRM_FLOAT rX1,
                  const DRM_FLOAT rChan,
	          const DRM_FLOAT rLVal0) {
        /* X0: L0 < 0
           X1: L0 > 0 */

/* First, calculate all distances */
DRM_FLOAT rResult1	= computeMetric (fabs (rA - rX0), rChan);
DRM_FLOAT rResult2	= computeMetric (fabs (rA - rX1), rChan);

/* Add L-value to metrics which to not correspond to correct hard decision */
	if (rLVal0 > 0.0)
            rResult1 += rLVal0;
        else
            rResult2 -= rLVal0;

/* Return smallest one */
        if (rResult1 < rResult2)
            return rResult1;
        else
            return rResult2;
}

static inline 
DRM_FLOAT	Minimum4 (const DRM_FLOAT rA, const DRM_FLOAT rB1, const DRM_FLOAT rB2,
	          const DRM_FLOAT rB3, DRM_FLOAT rB4, const DRM_FLOAT rChan) {
/* First, calculate all distances */
const DRM_FLOAT rResult1 = fabs (rA - rB1);
const DRM_FLOAT rResult2 = fabs (rA - rB2);
const DRM_FLOAT rResult3 = fabs (rA - rB3);
const DRM_FLOAT rResult4 = fabs (rA - rB4);

        /* Search for smallest one */
	DRM_FLOAT rReturn = rResult1;
        if (rResult2 < rReturn)
            rReturn = rResult2;
        if (rResult3 < rReturn)
            rReturn = rResult3;
        if (rResult4 < rReturn)
            rReturn = rResult4;

        return computeMetric (rReturn, rChan);
}

#define		BITSPERBYTE	8

#define	Mode_A	1
#define	Mode_B	2
#define	Mode_C	3
#define	Mode_D	4

int16_t		Ts_of		(uint8_t);
int16_t		Tu_of		(uint8_t);
int16_t		Tg_of		(uint8_t);
int16_t		symbolsperFrame	(uint8_t);
int16_t		groupsperFrame	(uint8_t);
int16_t		pilotDistance	(uint8_t);
int16_t		symbolsperGroup	(uint8_t);
int16_t		Kmin		(uint8_t, uint8_t);
int16_t		Kmax		(uint8_t, uint8_t);
int16_t		ususedCarriers	(uint8_t);
DRM_FLOAT	sinc		(DRM_FLOAT);

#define		SHOW_CHANNEL	0100
#define		SHOW_PILOTS	0200
#define		SHOW_ERROR	0400

#endif
