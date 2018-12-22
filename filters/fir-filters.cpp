#
/*
 *
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
 */

#include	"fir-filters.h"
#ifndef	__MINGW32__
#include	"alloca.h"
#endif

//FIR LowPass

	LowPassFIR::LowPassFIR (int16_t firsize,
	                        int32_t Fc, int32_t fs):Basic_FIR (firsize) {
	sampleRate	= fs;
	newKernel (Fc);
}

void	LowPassFIR::newKernel (int32_t Fc) {
int16_t	i;
float	tmp [filterSize];
float	f	= (float)Fc / sampleRate;
float	sum	= 0.0;

	for (i = 0; i < filterSize; i ++) {
	   if (i == filterSize / 2)
	      tmp [i] = 2 * M_PI * f;
	   else 
	      tmp [i] = sin (2 * M_PI * f * (i - filterSize/2))/ (i - filterSize/2);
//
//	Blackman window
	   tmp [i]  *= (0.42 -
		    0.5 * cos (2 * M_PI * (float)i / filterSize) +
		    0.08 * cos (4 * M_PI * (float)i / filterSize));

	   sum += tmp [i];
	}

	for (i = 0; i < filterSize; i ++)
	   filterKernel [i] = DSPCOMPLEX (tmp [i] / sum, 0);
}

	LowPassFIR::~LowPassFIR () {
}

DSPCOMPLEX	*LowPassFIR::getKernel (void) {
	return filterKernel;
}

//=====================================================================
//FIR HighPass

	HighPassFIR::HighPassFIR (int16_t firsize,
	                          int32_t Fc, int32_t fs):Basic_FIR (firsize) {
	sampleRate	= fs;
	newKernel (Fc);
}

void	HighPassFIR::newKernel (int32_t Fc) {
int16_t	i;
float 	tmp [filterSize];
float f	= (float)Fc / sampleRate;
float sum	= 0.0;

	for (i = 0; i < filterSize; i ++) {
	   if (i == filterSize / 2)
	      tmp [i] = 2 * M_PI * f;
	   else 
	      tmp [i] = sin (2 * M_PI * f * (i - filterSize/2))/ (i - filterSize/2);

	   tmp [i]  *= (0.42 -
		    0.5 * cos (2 * M_PI * (float)i / (float)filterSize) +
		    0.08 * cos (4 * M_PI * (float)i / (float)filterSize));

	   sum += tmp [i];
	}

	for (i = 0; i < filterSize; i ++)
	   if (i == filterSize / 2)
	      filterKernel [i] = DSPCOMPLEX (1.0 - tmp [i] / sum, 0);
	   else  
	      filterKernel [i] = DSPCOMPLEX (- tmp [i] / sum, 0);
}

	HighPassFIR::~HighPassFIR () {
}

//===============================================================

/*
 *	for the major bandpass filter, we use a simple basis,
 *	the FIR based on a blackman filter.
 *	The filter itself is somewhat more complex
 *	since we take negative frequencies into account
 *	based on the assumption that since we have I and Q
 *	signals, the spectrum is essentially a 0 .. 2N one
 */
static
float	Blackman (float *v, int fsize, float f) {
int	i;
float		sum = 0;

	for (i = 0; i < fsize; i ++) {
	   if (i == fsize / 2)
	      v [i] = 2 * M_PI * f;
	   else 
	      v [i] = sin (2 * M_PI * f * (i - fsize/2))/ (i - fsize/2);

	   v [i] *= (0.42 -
		        0.5 * cos (2 * M_PI * (float)i / (float)fsize) +
		        0.08 * cos (4 * M_PI * (float)i / (float)fsize));

	   sum += v [i];
	}

	return sum;
}

static
float	*BandPassKernel (float *v,
	                         int16_t fsize,
	                         float Fcl,
	                         float Fch) {
float	sumA	= 0.0;
float	sumB	= 0.0;
int16_t	i;
float	tmp1 [fsize];
float	tmp2 [fsize];

	if ((Fcl > 0.5) || (Fch <= Fcl) || (Fch > 0.5)) {
            fprintf (stderr, "bandpasskernel ??? (%f, %f) %d\n",
	                  (float)Fcl, (float)Fch, fsize);
	   Fcl	= 0.2;
	   Fch	= 0.4;
	}
	   
	sumA	= Blackman (tmp1, fsize, Fcl);
	sumB	= Blackman (tmp2, fsize, Fch);
	/* normalize	*/
	for (i = 0; i < fsize; i ++) {
	   tmp1 [i] =   tmp1 [i] / sumA;
	   tmp2 [i] = - tmp2 [i] / sumB;
	   v [i] = - (tmp1 [i] + tmp2 [i]);
	}

	return v;
}

	BasicBandPass::BasicBandPass (int16_t firsize,
	                              int32_t low, int32_t high,
	                              int32_t rate):Basic_FIR (firsize) {
float	t1 [firsize];
int16_t		i;

	sampleRate	= rate;
	(void) BandPassKernel (t1, firsize, (float) low / rate,
	                                     (float) high / rate);
	for (i = 0; i < filterSize; i ++)  
	   filterKernel [i] = DSPCOMPLEX (t1 [i], t1 [i]);
}

	BasicBandPass::~BasicBandPass () {
}

DSPCOMPLEX	*BasicBandPass::getKernel () {
	return filterKernel;
}

/*
 *	The bandfilter is for the complex domain. 
 *	We create a lowpass filter, which stretches over the
 *	positive and negative half, then shift this filter
 *	to the right position to form a nice bandfilter.
 *	For the real domain, we use the Simple BandPass version.
 */
	bandpassFIR::bandpassFIR (int16_t firSize,
	                          int32_t low, int32_t high,
	                          int32_t fs):Basic_FIR (firSize) {
	sampleRate	= fs;
	newKernel (low, high);
}
//
//	For the kernel we compute the size of the "equivalent"
//	low pass filter (lo), and the amount of the shift
void	bandpassFIR::newKernel (int32_t low, int32_t high) {
float	tmp [filterSize];
float	lo	= (float)((high - low) / 2) / sampleRate;
float	shift	= (float) ((high + low) / 2) / sampleRate;
float	sum	= 0.0;
int16_t	i;

	for (i = 0; i < filterSize; i ++) {
	   if (i == filterSize / 2)
	      tmp [i] = 2 * M_PI * lo;
	   else 
	      tmp [i] = sin (2 * M_PI * lo * (i - filterSize /2)) / (i - filterSize/2);
//
//	windowing, according to Blackman
	   tmp [i]  *= (0.42 -
		    0.5 * cos (2 * M_PI * (float)i / (float)filterSize) +
		    0.08 * cos (4 * M_PI * (float)i / (float)filterSize));

	   sum += tmp [i];
	}

	for (i = 0; i < filterSize; i ++) {	// shifting
	   float v = (i - filterSize / 2) * (2 * M_PI * shift);
	   filterKernel [i] = DSPCOMPLEX (tmp [i] * cos (v) / sum, 
	                                  tmp [i] * sin (v) / sum);
	}
}

	bandpassFIR::~bandpassFIR () {
}

DSPCOMPLEX	*bandpassFIR::getKernel () {
	return filterKernel;
}

//=====================================================================
/*
 *	adaptive (noise reduction) filter a la
 *	doug smith (Signal, Samples and stuff 3, qex 
 *	july 1998.
 *	basic: h(k+ 1) = lambda h(k) + 2 * u * err * x (k)
 *	it really works
 */
	adaptiveFilter::adaptiveFilter (int16_t fsize, float mu) {
int16_t	i;
	if (fsize < 2)
	   fsize = 2;
	else
	if (fsize > 20)
	   fsize = 20;
	firsize		= fsize;
	Kernel		= new float [fsize];
	Buffer		= new DSPCOMPLEX [fsize];
// 	we start with a small mu
	this	-> mu = 0.20;
	if (mu > 0 && mu < 1)
	   this -> mu = mu;
	ip		= 0;
//
//	we start with  kernel such that error determination
//	results in a (small) error for the first token
	Kernel [0] 	= 0.95; Buffer [0]	= 0;
	Kernel [1]	= 0.05; Buffer [1]	= 0;

	for (i = 2; i < firsize; i ++) {
	   Kernel [i]	= 0;
	   Buffer [i]	= 0;
	}
	err	= 0.5;
}

	adaptiveFilter::~adaptiveFilter () {
	delete[]	Kernel;
	delete[]	Buffer;
}

// 	Passing here is more complex since we 
// 	adapt the filtercoeeficients at the same time
DSPCOMPLEX adaptiveFilter::Pass (DSPCOMPLEX z) {
DSPCOMPLEX	tmp = 0;
int16_t		i;
float	sum	= 0.0;
DSPCOMPLEX	refSymbol	= Buffer [ip];
/*
 *	first filter with delayed elements
 *	Buffer is used in a circular way, with insertion and reading
 *	differing by firsize
 */
	Buffer [ip] = z;

	for (i = 0; i < firsize; i ++) {
	   int16_t index = (ip - i);
	   if (index < 0)
	      index += firsize;
	   tmp += cmul (Buffer [index], Kernel [i]);
	}

	ip = (ip + 1);
	if (ip >= firsize)
	   ip = 0;
//
//	determine the error
	err = abs (refSymbol) - abs (tmp);
//	err = (real (z) - real (tmp)) + (imag (z) - imag (tmp));
/*
 *	... and adapt the kernel elements accordingly
 */
	for (i = 0; i < firsize; i ++) {
	   Kernel [i] = 0.99 * Kernel [i] + 2.0 * mu * err * real (z);
	   sum += Kernel [i];
	}

	for (i = 0; i < firsize; i ++) 
	   Kernel [i] /= sum;
	return tmp;
}
//
//====================================================================
//
//	decimationFilter.
//	Useful for e.g. reduction of the samplerate with dabsticks
//	Two tastes are within the same class: the lowpass and
//	the bandpass variant.

	decimatingFIR::decimatingFIR (int16_t firSize,
	                              int32_t low,
	                              int32_t fs,
	                              int16_t Dm):Basic_FIR (firSize) {

	sampleRate		= fs;
	decimationFactor	= Dm;
	decimationCounter	= 0;
	newKernel (low);
}

void	decimatingFIR::newKernel (int32_t low) {
int16_t	i;
float	*tmp	= (float *)alloca (filterSize * sizeof (float));
float	f	= (float)low / sampleRate;
float	sum	= 0.0;

	for (i = 0; i < filterSize; i ++) {
	   if (i == filterSize / 2)
	      tmp [i] = 2 * M_PI * f;
	   else 
	      tmp [i] = sin (2 * M_PI * f * (i - filterSize/2))/ (i - filterSize/ 2);

	   tmp [i]  *= (0.42 -
		    0.5 * cos (2 * M_PI * (float)i / (float)filterSize) +
		    0.08 * cos (4 * M_PI * (float)i / (float)filterSize));

	   sum += tmp [i];
	}

	for (i = 0; i < filterSize; i ++)
	   filterKernel [i] = DSPCOMPLEX (tmp [i] / sum, 0);
}

	decimatingFIR::decimatingFIR (int16_t firSize,
	                              int32_t low,
	                              int32_t high,
	                              int32_t fs,
	                              int16_t Dm):Basic_FIR (firSize) {

	sampleRate		= fs;
	decimationFactor	= Dm;
	decimationCounter	= 0;
	newKernel (low, high);
}

void	decimatingFIR::newKernel (int32_t low, int32_t high) {
float	*tmp	= (float *)alloca (filterSize * sizeof (float));
float	lo	= (float)((high - low) / 2) / sampleRate;
float	shift	= (float) ((high + low) / 2) / sampleRate;
float	sum	= 0.0;
int16_t	i;

	for (i = 0; i < filterSize; i ++) {
	   if (i == filterSize / 2)
	      tmp [i] = 2 * M_PI * lo;
	   else 
	      tmp [i] = sin (2 * M_PI * lo * (i - filterSize /2)) / (i - filterSize/2);
//
//	windowing
	   tmp [i]  *= (0.42 -
		        0.5 * cos (2 * M_PI * (float)i / (float)filterSize) +
		        0.08 * cos (4 * M_PI * (float)i / (float)filterSize));

	   sum += tmp [i];
	}
//
//	and modulate the kernel
	for (i = 0; i < filterSize; i ++) {	// shifting
	   float v = (i - filterSize / 2) * (2 * M_PI * shift);
	   filterKernel [i] = DSPCOMPLEX (tmp [i] * cos (v) / sum, 
	                                  tmp [i] * sin (v) / sum);
	}
}

	decimatingFIR::~decimatingFIR () {
}
//
//	The real cpu killer: this function is called once for every
//	sample that comes from the dongle. So, it really should be
//	optimized.
bool	decimatingFIR::Pass (DSPCOMPLEX z, DSPCOMPLEX *z_out) {
int16_t		i;
DSPCOMPLEX	tmp	= 0;
int16_t		index;

	Buffer [ip] = z;
	if (++decimationCounter < decimationFactor) {
	   ip =  (ip + 1) % filterSize;
	   return false;
	}

	decimationCounter = 0;
//
//	we are working with a circular buffer, we take two steps
//	we move from ip - 1 .. 0 with i going from 0 .. ip -1
	for (i = 0; i <= ip; i ++) {
	   index =  ip - i;
	   tmp 	+= Buffer [index] * filterKernel [i];
	}
//	and then we take the rest
	for (i = ip + 1; i < filterSize; i ++) {
	   index =  filterSize + ip - i;
	   tmp 	+= Buffer [index] * filterKernel [i];
	}

	ip = (ip + 1) % filterSize;
	*z_out = tmp;
//	*z_out = cdiv (tmp, filterSize);
	return true;
}

bool	decimatingFIR::Pass (float z, float *z_out) {
	if (++decimationCounter < decimationFactor) {
	   Buffer [ip] = DSPCOMPLEX (z, 0);
	   ip = (ip + 1) % filterSize;
	   return false;
	}

	decimationCounter = 0;
	*z_out = Basic_FIR::Pass (z);
	return true;
}


//====================================================================
/*
 *	The Hilbertfilter is derived from QEX Mar/April 1998
 *	to perform a 90 degree phase shift needed for (a.o)
 *	USB and LSB detection.
 */
	HilbertFilter::HilbertFilter (int16_t fsize, float f, int32_t rate) {
	firsize		= fsize;
	this	-> rate	= rate;
	cosKernel	= new float [fsize];
	sinKernel	= new float [fsize];
	Buffer		= new DSPCOMPLEX [fsize];
	adjustFilter (f);
}

	HilbertFilter::~HilbertFilter () {
	delete[]	cosKernel;
	delete[]	sinKernel;
	delete[]	Buffer;
}
/*
 * 	the validity of the hilbertshift was validated
 * 	by computing (and displaying) 
 * 	arg (res - DSPCOMPLEX (real (res), real (res)))
 * 	where res = ... -> Pass (s1, s1)
 */
void	HilbertFilter::adjustFilter (float centre) {
float	*v1	= (float *)alloca (firsize * sizeof (float));
float	sum = Blackman (v1, firsize, centre);
int16_t	i;

	for (i = 0; i < firsize; i ++)
	   v1 [i] = v1 [i] / sum;

	for (i = 0; i < firsize; i ++) {
	   float omega = 2.0 * M_PI * centre;
	   cosKernel [i] = v1 [i] * 
	             cos (omega * (i - ((float)firsize - 1) / (2.0 * rate)));
	   sinKernel [i] = v1 [i] * 
	             sin (omega * (i - ((float)firsize - 1) / (2.0 * rate)));
	   Buffer [i] = 0;
	}

	ip = 0;
}

DSPCOMPLEX	HilbertFilter::Pass (float a, float b) {
	return Pass (DSPCOMPLEX (a, b));
}

DSPCOMPLEX	HilbertFilter::Pass (DSPCOMPLEX z) {
DSPCOMPLEX	tmp = 0;
float	re, im;
int	i;

	Buffer [ip] = z;
	ip = (ip + 1) % firsize;

	for (i = 0; i < firsize; i ++) {
	   int16_t index = ip - i;
	   if (index < 0)
	      index += firsize;
	   re = real (Buffer [index]);
	   im = imag (Buffer [index]);
	   tmp += DSPCOMPLEX (re * cosKernel [i], im * sinKernel [i]);
	}

	return tmp;
}

