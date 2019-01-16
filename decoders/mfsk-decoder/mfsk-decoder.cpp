#
/*
 *    Copyright (C) 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    The mfsk decoder software is a partial 
 *    reimplementation of (parts of) the mfsk part
 *    of the gmfsk0.6 software of Tomi Manninen.
 *    All relevant rights are gratefully acknowledged
 *
 *    This file is part of the swradio
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
#
#include	<QSettings>
#include	"mfsk-decoder.h"

#ifndef	__MINGW32__
#include	"alloca.h"
#endif

#define	BitIsContained(pat, bit) ((pat) & (1 << (mfskNumBits - (bit + 1))))
/*
 *	Viterbi decoding uses a normal 1/2 - 7 encoding/decoding
 *	scheme with standard polynomials (NASA)
 */
#define	mfskK		7
#define	mfskPOLY1	0x6d
#define	mfskPOLY2	0x4f
#define	MFSKIF		800.0

#define	BORDER		10
/*
 */

static	double	x_axis 		[256];
static	double	y_values	[256];
	mfskDecoder::mfskDecoder (int32_t rate,
	                          RingBuffer<std::complex<float> > *b,
	                          QSettings *s):
                                           virtualDecoder (rate, b),
                                           localShifter   (10 * rate),
	                                   mfskbandFilter (2048, 255),
		                           ViterbiDecodera (mfskK,
	                                                    mfskPOLY1,
	                                                    mfskPOLY2) {

	theRate		= rate;
	mfskSettings	= s;

	myFrame		= new QFrame;
	setupUi (myFrame);

	myFrame	-> show ();

	mfskIF		= MFSKIF;
	mfsk_afcon	= false;
	mfsk_reverse	= false;
	mfsk_squelch 	= 0;
	mfskbaudrateEstimate	= 0;
	showCount		= 0;
	mfskDibitpair [0]	= 0;
	mfskDibitpair [1]	= 0;
	ViterbiDecoderb		= NULL;
	mfsk_mode		= MODE_MFSK_16;	/* default		*/
	mfskViewer		= new waterfallScope (mfskScope, 16, 10);
	setupInterfaceforMode (MODE_MFSK_16);

	connect (mfskMode, SIGNAL (activated (const QString &)),
		 this, SLOT (mfsk_setMode (const QString &)));
	connect (mfskReverse, SIGNAL (activated (const QString &)),
		 this, SLOT (mfsk_setReverse (const QString &)));
	connect (mfskAfcon, SIGNAL (activated (const QString &)),
		 this, SLOT (mfsk_setAfcon (const QString &)));
	connect (mfskSquelch, SIGNAL (valueChanged (int)),
	         this, SLOT (mfsk_setSquelch (int)));
}

	mfskDecoder::~mfskDecoder		(void) {
	delete	mfskInterleaver;
	delete	mfskSpectrumCache;
	delete	mfskSlidingfft;
	if ((mfskNumBits & 01) == 1)
	   delete ViterbiDecoderb;
	delete myFrame;
}

bool	mfskDecoder::initforRate	(int32_t workingRate) {
	if (theRate == workingRate)
	   return true;

	theRate	= workingRate;
	delete	mfskInterleaver;
	delete	mfskSpectrumCache;
	delete	mfskSlidingfft;
	if (ViterbiDecoderb != NULL)
	   delete ViterbiDecoderb;
	ViterbiDecoderb	= NULL;
	
	setupInterfaceforMode (mfsk_mode);
	return true;
}

int32_t	mfskDecoder::rateOut	(void) {
	return theRate;
}

int16_t	mfskDecoder::detectorOffset	(void) {
	return mfskIF;
}

void	mfskDecoder::mfsk_setAfcon (const QString &s) {
	mfsk_afcon = s == "afc on";
	if (!mfsk_afcon)
	   mfskIF = MFSKIF;
}

void	mfskDecoder::mfsk_setReverse (const QString &s) {
	mfsk_reverse = s != "normal";
}

void	mfskDecoder::mfsk_setMode (const QString &s) {
uint8_t	newMode;

	if (s == "mfsk 8")
	   newMode = mfskDecoder::MODE_MFSK_8;
	else
	if (s == "mfsk 16")
	   newMode = mfskDecoder::MODE_MFSK_16;
	else
	if (s == "mfsk 4")
	   newMode = mfskDecoder::MODE_MFSK_4;
	else
	if (s == "mfsk 11")
	   newMode = mfskDecoder::MODE_MFSK_11;
	else
	if (s == "mfsk 22")
	   newMode = mfskDecoder::MODE_MFSK_22;
	else
	if (s == "mfsk 31")
	   newMode = mfskDecoder::MODE_MFSK_31;
	else
	if (s == "mfsk 32")
	   newMode = mfskDecoder::MODE_MFSK_32;
	else
	   newMode = mfskDecoder::MODE_MFSK_64;

	if (mfsk_mode == newMode)
	   return;

	mfsk_mode = newMode;
	delete	mfskInterleaver;
	delete	mfskSpectrumCache;
	delete	mfskSlidingfft;
	if (ViterbiDecoderb != NULL)
	   delete ViterbiDecoderb;
	ViterbiDecoderb	= NULL;
	
	setupInterfaceforMode (mfsk_mode);
}

void	mfskDecoder::mfsk_setSquelch (int n) {
	mfsk_squelch = n;
}

void	mfskDecoder::setupInterfaceforMode (uint8_t mode) {

	mfskIF			= MFSKIF;
	mfskMet1		= 0;
	mfskMet2		= 0;
	mfskbitCounter		= 0;

	mfskCacheLineP		= 0;
	mfskPrev2Index	 	= 0;
	mfskPrev2Vector 	= 0;
	mfskPrev1Index		= 0;
	mfskPrev1Vector		= 0;
/*
 *	The determining state elements:
 */
	switch (mode) {
	   case MODE_MFSK_4:
	      mfskNumBits		= 5;
	      mfskNumTones		= 32;
	      mfskBorder		= 16;
	      mfskBaudRate		= (double) 3.90625;
	      break;

	   case MODE_MFSK_8:
	      mfskNumBits		= 5;
	      mfskNumTones		= 32;
	      mfskBorder		= 16;
	      mfskBaudRate		= (double) 7.8125;
	      break;

	   default:
	   case MODE_MFSK_16:
	      mfskNumBits		= 4;
	      mfskNumTones		= 16;
	      mfskBorder		= 8;
	      mfskBaudRate		= (double) 15.625;
	      break;

	   case MODE_MFSK_22:
	      mfskNumBits		= 4;
	      mfskNumTones		= 16;
	      mfskBorder		= 8;
	      mfskBaudRate		= (double) 21.533;
	      break;

	   case MODE_MFSK_11:
	      mfskNumBits		= 4;
	      mfskNumTones		= 16;
	      mfskBorder		= 8;
	      mfskBaudRate		= (double) 10.767;
	      break;

	   case MODE_MFSK_31:
	      mfskNumBits		= 3;
	      mfskNumTones		= 8;
	      mfskBorder		= 4;
	      mfskBaudRate		= (double) 31.250;
	      break;

	   case MODE_MFSK_32:
	      mfskNumBits		= 4;
	      mfskNumTones		= 16;
	      mfskBorder		= 8;
	      mfskBaudRate		= (double) 31.250;
	      break;

	   case MODE_MFSK_64:
	      mfskNumBits		= 4;
	      mfskNumTones		= 16;
	      mfskBorder		= 8;
	      mfskBaudRate		= (double) 62.500;
	      break;
	}
/*
 *	derived fundamental values:
 */
	mfskToneSpacing		= mfskBaudRate;
	mfskSamplesperSymbol	= theRate / mfskBaudRate;
        mfskBaseTone		= (int)(MFSKIF / mfskToneSpacing); 
	mfskBw			= (float)(mfskNumTones - 1) * mfskToneSpacing;

	delete mfskViewer;
	mfskViewer		= new waterfallScope (mfskScope,
	                                     mfskNumTones + mfskBorder, 20);

	mfskViewer	-> set_bitDepth (12);
	for (int i = -mfskBorder / 2;
	      i < mfskNumTones + mfskBorder / 2; i ++)
	   x_axis [i + mfskBorder / 2] = i;
	connect (mfskViewer, SIGNAL (clickedwithLeft (int)),
	         this, SLOT (handleClick (int)));

	mfskSyncCounter		= 0;
	mfskDatashreg		= 0;
	mfskbandFilter. setBand (mfskIF - 5 * mfskToneSpacing,
	                         mfskIF + mfskBw + 5 * mfskToneSpacing,
	                         theRate);
	ViterbiDecodera. viterbi_set_traceback (45);
	ViterbiDecodera. viterbi_set_chunksize (1);
/*
 *	In case we have an odd number of bits, we use
 *	two viterbi decoders.
 */
	if ((mfskNumBits & 01) == 1) {
	   ViterbiDecoderb	= new viterbi (mfskK, mfskPOLY1, mfskPOLY2);
	   ViterbiDecoderb	-> viterbi_set_traceback (45);
	   ViterbiDecoderb	-> viterbi_set_chunksize (1);
	}

	mfskSlidingfft	= new slidingFFT (mfskSamplesperSymbol,
				          mfskBaseTone - mfskBorder / 2,
					  mfskBaseTone + mfskNumTones + mfskBorder / 2 - 1);
//	We keep 2 * mfskSamplesperSymbol lines of spectrum in a cache
	mfskSpectrumCache
			= new Cache (2 * mfskSamplesperSymbol,
	                                   mfskNumTones + mfskBorder);
//	Note that we are receiving here: interleaving is backwards
	mfskInterleaver	= new Interleaver (10, mfskNumBits,
	                                      Interleaver::INTERLEAVE_REV);
	mfsk_clrText	();
	setDetectorMarker	((int)mfskIF);
}
/*
 *	assume a working front end, i.e. separate I and Q's
 */
void	mfskDecoder::process (std::complex<float> z) {
double	phase	= 0.0;
double	max	= -100000.0;
int16_t	i;
int16_t	IndextoLookat;
std::complex<float>	mfskCurrVector;

	audioOut	-> putDataIntoBuffer (&z, 1);
	z		= localShifter. do_shift (z, - 10 * (mfskIF - MFSKIF));
	z		= mfskbandFilter. Pass (z);

	mfskSlidingfft	-> do_FFT (z,
	                           mfskSpectrumCache  ->
	                                CacheLine (mfskCacheLineP));
/*
 *	The detection loop runs once per symbol. When in sync, the
 *	position then is at the strongest value of the spectrum (middle
 *	of the symbol).
 *	We adjust the positioning in the cache to obtain precisely
 *	mfskSamplesperSymbol spectra distance with the previous max.
 */
	if (--mfskSyncCounter <= 0) {
	   std::complex<float> *CacheLinetoLookat =
	               mfskSpectrumCache -> CacheLine (mfskCacheLineP);
	   uint8_t symbits [mfskNumBits];

	   for (int k = 0; k < mfskNumTones + mfskBorder; k ++) {
	      y_values [k] =  abs (CacheLinetoLookat [k]);
	      if (y_values [k] != y_values [k])
	         y_values [k] = 0;
	   }
	   mfskViewer	-> display (x_axis, y_values, 
	                            5 * amplitudeSlider -> value (),
	                            0, mfskBorder / 2);

	   mfskSyncCounter	= mfskSamplesperSymbol;
	   MapTonetoBits	(CacheLinetoLookat, mfskBorder, symbits);
	   mfskInterleaver	-> InterleaveSyms (symbits);
	   for (i = 0; i < mfskNumBits; i ++) 
	      handle_mfsk_bits (symbits [i]);
/*
 *	if possible, we synchronize by moving the pointer in the cache
 */
	   IndextoLookat	= IndextoFullestBin (CacheLinetoLookat);
	   if ((mfskBorder / 2 <= IndextoLookat) &&
	         (IndextoLookat < mfskNumTones + mfskBorder / 2)) {
	      if ((IndextoLookat != mfskPrev1Index) &&
	          (mfskPrev1Index != mfskPrev2Index)) {
	         int syn 	= mfskSamplesperSymbol;
	         for (i = 0; i < 2 * mfskSamplesperSymbol; i ++) {
	            uint16_t Idx = (mfskCacheLineP + i) %
	                                  (2 * mfskSamplesperSymbol);
	            double val =
	                abs ((mfskSpectrumCache -> CacheLine (Idx)) [mfskPrev1Index]);
	            if (val > max) {
	               max = val;
	               syn = i;
	            }
	         }
	         mfskSyncCounter += (int)(floor ((syn - mfskSamplesperSymbol) / mfskNumTones + 0.5));
	         mfskbaudrateEstimate =
	                    decayingAverage (mfskbaudrateEstimate,
                                              (float)theRate / mfskSyncCounter, 32.0);
	         mfsk_showEstimate (mfskbaudrateEstimate);
	      }

	      mfsk_showTuning (0);
	   }
	   else
	   if (IndextoLookat < mfskBorder / 2)
	      mfsk_showTuning (-1);
	   else
	   if (IndextoLookat >= mfskNumTones + mfskBorder / 2)
	      mfsk_showTuning (1);
/*
 *	if the symbol of the current cache filling agrees with the
 *	symbol of the previous cache filling, look at the phase difference
 *	to adjust the IF if requested for.
 */
	   mfskCurrVector	= CacheLinetoLookat [IndextoLookat];
	   if (IndextoLookat == mfskPrev1Index) {
	      phase = arg (mfskCurrVector * conj (mfskPrev1Vector));
/*
 *	phase between - PI .. PI
 *	Take into account a sequence of "adjusts" leading
 *	to an x far beyond what we want
 */
	      if (mfsk_afcon) {
	         double x; 
	         x = phase / mfskSamplesperSymbol / (2 * M_PI) * theRate;
	         if (-mfskToneSpacing / 2.0 < x &&
	                  x <  mfskToneSpacing / 2.0) { 
	            if ((mfskIF + x / 8.0 > MFSKIF - mfskToneSpacing / 2) &&
	                (mfskIF + x / 8.0 < MFSKIF + mfskToneSpacing / 2))
	               mfskIF +=  x / 8.0;
	         }
	      }
	   }

	   mfsk_showCF (mfskIF);
	   setDetectorMarker	((int)mfskIF);

// save relevant data for the next iteration
	   mfskPrev2Index	= mfskPrev1Index;
	   mfskPrev1Index	= IndextoLookat;

	   mfskPrev2Vector	= mfskPrev1Vector;
	   mfskPrev1Vector	= mfskCurrVector;
	}

// and in all cases, increment the cacheline pointer
	mfskCacheLineP	= (mfskCacheLineP + 1) % (2 * mfskSamplesperSymbol);
}
/*
 *	getting the FullestBin (used for synchronizing and frequency tracking)
 *	is straight forward. Just look for the last
 *	fft result and determine the maximum.
 *	purposes.
 */
int16_t	mfskDecoder::IndextoFullestBin (std::complex<float> *bins) {
double	x;
double	max	= 0.0;
int	i;
int	index	= 0;

	for (i = 0; i < mfskNumTones + mfskBorder; i ++) {
	   x = abs (bins [i]);
	   if (x >= max) {
	      max = x;
	      index = i;
	   }
	}
	return index;
}
/*
 *	we go from tones to bits here. each of the tones
 *	represents some (8 tones 3 bits, 16 tones - 4, 32 tones - 5 bits).
 *	Manninen takes here the "soft approach":
 *	for each tone, in the bins, the 'soft' contribution
 *	to the bits, represented by the tone, is calculated.
 */
void	mfskDecoder::MapTonetoBits (std::complex<float> *bins,
	                            int16_t g, uint8_t *bits) {
double	tone;
double	sum;
int16_t	i, j, k;
int16_t	bitPattern;
double	b [mfskNumBits];

	for (i = 0; i < mfskNumBits; i ++)
	   b [i] = 0;

	sum	= 1e-10;	/* virtually 0, better for alu	*/
/*
 * 	tone i (bins [i]) contributes to bits (i) ^ (i >> 1) (gray coding).
 */
	for (i = 0; i < mfskNumTones; i ++) {
	   bitPattern = ((uint8_t)i) ^ (((uint8_t) i) >> 1);
//	   take reversing into account
	   if (!mfsk_reverse)
	      k = i;
	   else
	      k = (mfskNumTones - 1) - i;
// 	bins [k] contributes
// 	a. positively to the bits associated with the bit value represented by
// 	the tone
// 	b. negatively to the other bits.

	   tone = abs (bins [k + g/2]);
	   for (j = 0; j < mfskNumBits; j++) 
	      b [j] += BitIsContained (bitPattern, j) ? tone : - tone;
	   sum += tone;
	}
/*
 * 	squeeze the resulting values in the range 0 .. 255, and store them into
 * 	the bits vector
 */
	for (i = 0; i < mfskNumBits; i ++) { 
	   double t = 128.0 + (b [i] / sum * 128.0);
           bits [i] = t < 0 ? 0 : t > 255.0 ? 255 : (unsigned char)(t);
	}
}

void	mfskDecoder::handle_mfsk_bits (unsigned char bit) {
int16_t	c;
int32_t	 met;
	
	mfskDibitpair [0]	= mfskDibitpair [1];
	mfskDibitpair [1]	= bit;
	mfskbitCounter		= mfskbitCounter ? 0 : 1;

	if ((mfskNumBits & 01) == 0) {	// even number of bits 
	   if (mfskbitCounter) 
	      return;
	
	   c = ViterbiDecodera. viterbi_decode (mfskDibitpair, &met);
	   if (c == -1)
	      return;

	   mfskMet1 = decayingAverage (mfskMet1, met, 32.0);
	   mfskSignalMetric = mfskMet1 / 2.5;
	}
	else 	// odd number of bits, 
	   if (mfskbitCounter) {
	      c = ViterbiDecodera. viterbi_decode (mfskDibitpair, &met);
	      if (c == -1)
	         return;

	      mfskMet1 = decayingAverage (mfskMet1, met, 32.0);
	      if (mfskMet1 < mfskMet2)
	         return;

	      mfskSignalMetric = mfskMet1 / 2.5;
	   }
	   else {
	      c = ViterbiDecoderb -> viterbi_decode (mfskDibitpair, &met);
	      if (c == -1)
	         return;

	      mfskMet2 = decayingAverage (mfskMet2, met, 32.0);
	      if (mfskMet2 < mfskMet1)
	         return;

	      mfskSignalMetric = mfskMet2 / 2.5;
	   }
/*
 *	just as a guard, the metrics should be reasonable
 */
	mfsk_showS2N    ((int)clamp (mfskSignalMetric * 3 - 42, 0.0, 100.0));
	if (mfskSignalMetric > (float)mfsk_squelch * 0.1)
	   shiftBit (c);
}

void	mfskDecoder::shiftBit (uint8_t c) {
int16_t	f;

	mfskDatashreg = (mfskDatashreg << 1)| !!c;
// 	search for '001', but recall that the '1' is
//	part of the next symbol

	if ((mfskDatashreg & 07) == 001) {
	   f = vari_Decode (mfskDatashreg >> 1);
	   mfsk_addText ((char)f);
	   mfskDatashreg = 1;
	}
}

void	mfskDecoder::mfsk_clrText () {
	mfskText = QString ("");
	mfsktextBox		-> setText (mfskText);
}

void	mfskDecoder::mfsk_addText (char c) {
	if (c < ' ') c = ' ';
	mfskText.append (QString (QChar (c)));
	if (mfskText. length () > 100)
	   mfskText. remove (0, 1);
	mfsktextBox		-> setText (mfskText);
}

void	mfskDecoder::mfsk_showTuning	(int v) {
	(void)v;
}

void	mfskDecoder::mfsk_showLocalScope (int v) {
	(void)v;
}

void	mfskDecoder::mfsk_addLocalScope	(int i, double v) {
	(void)i; (void)v;
}

void	mfskDecoder::mfsk_showEstimate	(float v) {
        baudrateEstimate -> display (v);
}

void	mfskDecoder::mfsk_showCF	(float v) {
	cfDisplay	-> display (v);
}

void	mfskDecoder::mfsk_showS2N	(int a) {
	s2nDisplay	-> display	(a);
}

/*
 * The code table in a format suitable for decoding.
 */
static uint16_t varidecode[] = {
	0x75C, 0x760, 0x768, 0x76C, 0x770, 0x774, 0x778, 0x77C, 
	0x0A8, 0x780, 0x7A0, 0x7A8, 0x7AC, 0x0AC, 0x7B0, 0x7B4, 
	0x7B8, 0x7BC, 0x7C0, 0x7D0, 0x7D4, 0x7D8, 0x7DC, 0x7E0, 
	0x7E8, 0x7EC, 0x7F0, 0x7F4, 0x7F8, 0x7FC, 0x800, 0xA00, 
	0x004, 0x1C0, 0x1FC, 0x2D8, 0x2A8, 0x2A0, 0x200, 0x1BC, 
	0x1F4, 0x1F0, 0x2B4, 0x1E0, 0x0A0, 0x1D8, 0x1D4, 0x1E8, 
	0x0E0, 0x0F0, 0x140, 0x154, 0x174, 0x160, 0x16C, 0x1A0, 
	0x180, 0x1AC, 0x1EC, 0x1F8, 0x2C0, 0x1DC, 0x2BC, 0x1D0, 
	0x280, 0x0BC, 0x100, 0x0D4, 0x0DC, 0x0B8, 0x0F8, 0x150, 
	0x158, 0x0C0, 0x1B4, 0x17C, 0x0F4, 0x0E8, 0x0FC, 0x0D0, 
	0x0EC, 0x1B0, 0x0D8, 0x0B4, 0x0B0, 0x15C, 0x1A8, 0x168, 
	0x170, 0x178, 0x1B8, 0x2E8, 0x2D0, 0x2EC, 0x2D4, 0x2B0, 
	0x2AC, 0x014, 0x060, 0x038, 0x034, 0x008, 0x050, 0x058, 
	0x030, 0x018, 0x080, 0x070, 0x02C, 0x040, 0x01C, 0x010, 
	0x054, 0x078, 0x020, 0x028, 0x00C, 0x03C, 0x06C, 0x068, 
	0x074, 0x05C, 0x07C, 0x2DC, 0x2B8, 0x2E0, 0x2F0, 0xA80, 
	0xAA0, 0xAA8, 0xAAC, 0xAB0, 0xAB4, 0xAB8, 0xABC, 0xAC0, 
	0xAD0, 0xAD4, 0xAD8, 0xADC, 0xAE0, 0xAE8, 0xAEC, 0xAF0, 
	0xAF4, 0xAF8, 0xAFC, 0xB00, 0xB40, 0xB50, 0xB54, 0xB58, 
	0xB5C, 0xB60, 0xB68, 0xB6C, 0xB70, 0xB74, 0xB78, 0xB7C, 
	0x2F4, 0x2F8, 0x2FC, 0x300, 0x340, 0x350, 0x354, 0x358, 
	0x35C, 0x360, 0x368, 0x36C, 0x370, 0x374, 0x378, 0x37C, 
	0x380, 0x3A0, 0x3A8, 0x3AC, 0x3B0, 0x3B4, 0x3B8, 0x3BC, 
	0x3C0, 0x3D0, 0x3D4, 0x3D8, 0x3DC, 0x3E0, 0x3E8, 0x3EC, 
	0x3F0, 0x3F4, 0x3F8, 0x3FC, 0x400, 0x500, 0x540, 0x550, 
	0x554, 0x558, 0x55C, 0x560, 0x568, 0x56C, 0x570, 0x574, 
	0x578, 0x57C, 0x580, 0x5A0, 0x5A8, 0x5AC, 0x5B0, 0x5B4, 
	0x5B8, 0x5BC, 0x5C0, 0x5D0, 0x5D4, 0x5D8, 0x5DC, 0x5E0, 
	0x5E8, 0x5EC, 0x5F0, 0x5F4, 0x5F8, 0x5FC, 0x600, 0x680, 
	0x6A0, 0x6A8, 0x6AC, 0x6B0, 0x6B4, 0x6B8, 0x6BC, 0x6C0, 
	0x6D0, 0x6D4, 0x6D8, 0x6DC, 0x6E0, 0x6E8, 0x6EC, 0x6F0, 
	0x6F4, 0x6F8, 0x6FC, 0x700, 0x740, 0x750, 0x754, 0x758
};
//
//	
int16_t mfskDecoder::vari_Decode (uint16_t symbol) {
int i;

	for (i = 0; i < 256; i++)
	   if (symbol == varidecode [i])
	      return i;

	return -1;
}

void	mfskDecoder::handleClick (int a) {
	adjustFrequency (a * mfskToneSpacing);
}

	Cache::Cache (int16_t a, int16_t b) {
int16_t	i;

	rowBound	= a;
	columnBound	= b;
	data		= new std::complex<float> *[a];
	for (i = 0; i < a; i++)
	   data [i] = new std::complex<float> [b];
}

	Cache::~Cache (void) {
int	i;
	for (i = 0; i < rowBound; i ++) 
	   delete[] data [i];

	delete[] data;
}

std::complex<float>	*Cache::CacheLine (int16_t n) {
	return data [n];
}


