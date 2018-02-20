#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J.
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
#include	"agchandler.h"

#define	AGCSLOW		(sampleRate / 2)
#define	AGCFAST		(sampleRate / 8);

#define	RISE_ATTACKWEIGHT	(sampleRate / 500)
#define	FALL_ATTACKWEIGHT	(sampleRate / 100)
#define	RISE_DECAYWEIGHT	(sampleRate / 200)
#define	FALL_DECAYWEIGHT	(sampleRate / 100)
/*
 *	Handling agc, still rudimentary.
 *	The approach taken is derived from cuteSDR
 */
	agcHandler::agcHandler	(int32_t sampleRate, int16_t bitDepth) {
	this	-> sampleRate	= sampleRate;
	this	-> bitDepth	= bitDepth;
	if (bitDepth < 0 || bitDepth >= 24)
	   bitDepth = 24;

	normalizer      = 1;
	while (-- bitDepth >= 0)
	   normalizer <<= 1;

	agcMode			= AGC_OFF;
	currentPeak		= 0;
	attackAverage		= 1;
	decayAverage		= 1;
	bufferSize		= sampleRate / 50;	// 20 msec buffer
	buffer			= new float [bufferSize];
	bufferP			= 0;
	ThresholdValue		= -get_db (0) + 30;	// default
	fprintf (stderr, "Threshold = %f\n", ThresholdValue);
}

	agcHandler::~agcHandler	(void) {
	delete	buffer;
}

void	agcHandler::set_bitDepth (int bitDepth) {
	this	-> bitDepth	= bitDepth;
	if (bitDepth < 0 || bitDepth >= 24)
	   bitDepth = 24;

	normalizer      = 1;
	while (-- bitDepth >= 0)
	   normalizer <<= 1;
}

float   agcHandler::get_db (float x) {
	return 20 * log10 ((x + 1) / (float)(normalizer));
}

void	agcHandler::setMode	(uint8_t m) {
	agcMode		= m;
	agcHangtime	= 0;
	agcHangInterval	= agcMode == AGC_SLOW ? AGCSLOW : AGCFAST;
}

//
//	Note: in this revised version, the handler
void		agcHandler::doAgc	(DSPCOMPLEX *buffer, int32_t amount) {
int32_t i;
	if (agcMode == AGC_OFF)
	   return;

	for (i = 0; i < amount; i ++)
	   buffer [i] = cmul (buffer [i], doAgc (buffer [i]));
}

//	returns the amplification factor rather than the amplified value
float	agcHandler::doAgc	(DSPCOMPLEX z) {
float	currentMag;
float	oldest_Mag;
float	gain;
int16_t	i;

	oldest_Mag		= buffer [bufferP];
	currentMag		= abs (z);
	buffer [bufferP]	= currentMag;
	bufferP			= (bufferP + 1) % bufferSize;

	if (agcMode == AGC_OFF) 
	   return 1.0;

	if (currentMag >= currentPeak)
	   currentPeak = currentMag;
	else
	if (oldest_Mag >= currentPeak) { // largest one shifted out
	   currentPeak = currentMag;
	   for (i = 0; i < bufferSize; i ++) 
	      if (buffer [i] >= currentPeak)
	         currentPeak = buffer [i];
	}
//
//	now we have a valid currentPeak
//
	if (currentPeak > attackAverage)
	   attackAverage = decayingAverage (attackAverage,
	                                    currentPeak, RISE_ATTACKWEIGHT);
	else
	   attackAverage = decayingAverage (attackAverage,
	                                    currentPeak, FALL_ATTACKWEIGHT);

	if (currentPeak > decayAverage) {
	   decayAverage = decayingAverage (decayAverage,
	                                   currentPeak, RISE_DECAYWEIGHT);
	   agcHangtime = 0;
	}
	else {
	   if (agcHangtime < agcHangInterval)
	      agcHangtime ++;	// keep current decayAverage
	   else
	      decayAverage = decayingAverage (decayAverage,
	                                      currentPeak, FALL_DECAYWEIGHT);
	}

	if (attackAverage > decayAverage)
	   gain = gain_for (attackAverage);
	else
	   gain = gain_for (decayAverage);

	return gain;
}
//
//	set the threshold value for the agc in db
void	agcHandler::setThreshold (int16_t s) {
	if (s < get_db (0))
	   s = get_db (0);
	if (s > 0)
	   s = 0;

	ThresholdValue	= (float)s / 16;
}

float	agcHandler::gain_for (float v) {
float mag	= log10 (v / normalizer + 1e-8);

	if (mag < ThresholdValue)	// knee value
	   return 1.0;
	
	return  pow (10.0, - (mag - ThresholdValue));
}

float	agcHandler::decayingAverage (float old,
	                             float input,
	                             float weight) {
	if (weight <= 1)
	   return input;
	return input * (1.0 / weight) + old * (1.0 - (1.0 / weight));
}

