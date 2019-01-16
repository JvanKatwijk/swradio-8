#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the swradio
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

#ifndef __FIR_FILTERS__
#define __FIR_FILTERS__

#include	"radio-constants.h"

class	HilbertFilter;

class	Basic_FIR	{
	public:
	int16_t		filterSize;
	DSPCOMPLEX	*filterKernel;
	DSPCOMPLEX	*Buffer;
	int16_t		ip;
	int32_t		sampleRate;

			Basic_FIR (int16_t size) {
	int16_t		i;
	filterSize	= size;
	filterKernel	= new DSPCOMPLEX [filterSize];
	Buffer		= new DSPCOMPLEX [filterSize];
	ip		= 0;

	for (i = 0; i < filterSize; i ++) {
	   filterKernel [i]	= 0;
	   Buffer [i]		= 0;
	}
}

			~Basic_FIR (void) {
	delete[]	filterKernel;
	delete[]	Buffer;
}
//
//	we process the samples backwards rather than reversing
//	the kernel
	DSPCOMPLEX		Pass (DSPCOMPLEX z) {
int16_t	i;
DSPCOMPLEX	tmp	= 0;

	Buffer [ip]	= z;
	for (i = 0; i < filterSize; i ++) {
	   int16_t index = ip - i;
	   if (index < 0)
	      index += filterSize;
	   tmp		+= Buffer [index] * filterKernel [i];
	}

	ip = (ip + 1) % filterSize;
	return tmp;
}

	float	Pass (float v) {
int16_t		i;
float	tmp	= 0;

	Buffer [ip] = DSPCOMPLEX (v, 0);
	for (i = 0; i < filterSize; i ++) {
	   int16_t index = ip - i;
	   if (index < 0)
	      index += filterSize;
	   tmp += real (Buffer [index]) * real (filterKernel [i]);
	}

	ip = (ip + 1) % filterSize;
	return tmp;
}

	void		setTaps (int16_t len,
	                         float *itaps, float *qtaps) {
int16_t	i;

	if (len != filterSize)
	   fprintf (stderr, "H E L P\n");

	for (i = 0; i < len; i ++) {
	   float re = 0;
	   float im = 0;

	   if (itaps != NULL)
	      re = itaps [i];
	   if (qtaps != NULL)
	      im = qtaps [i];
	   filterKernel [i] = DSPCOMPLEX (re, im);
	}
}
	      
	
};

class	LowPassFIR : public Basic_FIR {
public:
			LowPassFIR (int16_t,	// order
	                            int32_t, 	// cutoff frequency
	                            int32_t	// samplerate
	                           );
			~LowPassFIR (void);
	DSPCOMPLEX	*getKernel	(void);
	void		newKernel	(int32_t);	// cutoff
};
//
//	Both for lowpass band bandpass, we provide:
class	decimatingFIR: public Basic_FIR {
public:
		         decimatingFIR	(int16_t, int32_t, int32_t, int16_t);
	                 decimatingFIR	(int16_t, int32_t, int32_t,
	                                                   int32_t, int16_t);
			~decimatingFIR	(void);
	void		newKernel	(int32_t);
	void		newKernel	(int32_t, int32_t);
	bool		Pass	(DSPCOMPLEX, DSPCOMPLEX *);
	bool		Pass	(float, float *);
private:
	int16_t	decimationFactor;
	int16_t	decimationCounter;
};

class	HighPassFIR: public Basic_FIR {
public:
			HighPassFIR	(int16_t, int32_t, int32_t);
			~HighPassFIR	(void);
	void		newKernel	(int32_t);
};

class	bandpassFIR: public Basic_FIR {
public:
			bandpassFIR	(int16_t, int32_t, int32_t, int32_t);
			~bandpassFIR	(void);
	DSPCOMPLEX	*getKernel	(void);
	void		newKernel	(int32_t, int32_t);
private:
};

class	BasicBandPass: public Basic_FIR {
public:
			BasicBandPass	(int16_t, int32_t, int32_t, int32_t);
			~BasicBandPass	(void);
	DSPCOMPLEX	*getKernel	(void);
private:
};

class	adaptiveFilter {
public:
		adaptiveFilter	(int16_t, float);
		~adaptiveFilter	();
	DSPCOMPLEX	Pass		(DSPCOMPLEX);

private:
	int16_t		ip;
	float		err;
	float		mu;
	int16_t		firsize;
	float	*Kernel;
	DSPCOMPLEX	*Buffer;
};

class HilbertFilter {
public:
		HilbertFilter	(int16_t, float, int32_t);
		~HilbertFilter	();
	DSPCOMPLEX	Pass		(DSPCOMPLEX);
	DSPCOMPLEX	Pass		(float, float);
private:
	int16_t		ip;
	int16_t		firsize;
	int32_t		rate;
	float	*cosKernel;
	float	*sinKernel;
	DSPCOMPLEX	*Buffer;
	void		adjustFilter	(float);
};

#endif

