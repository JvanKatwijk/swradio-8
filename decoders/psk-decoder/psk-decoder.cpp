#
/*
 *    Copyright (C) 2010, 2011, 2012, 2013
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
#
#include	<QSettings>
#include	<QFrame>
#include	<QWidget>
#include	<qwt_dial.h>
#include	<qwt_dial_needle.h>
#include	"psk-decoder.h"
#include	"fir-filters.h"
#include	"shifter.h"
#include	"utilities.h"
#include	"viterbi.h"
#include	"slidingfft.h"

#define	pskK			5
#define	pskPOLY1		0x17
#define	pskPOLY2		0x19
#define	PSK31_SPEED		31.25
#define	PSK63_SPEED		62.50
#define	PSK125_SPEED		125.0

#define	PSK_IF			000
#define	PSKRATE			2000

/*
 *	the psk decoder is a rather straightforward implementation
 *	we filter the incoming signal, a signal at rate "rate",
 *	then down decimate to PSKRATE, after which we 
 *	decimate such that we end up with 16 samples per bit
 */
		pskDecoder::pskDecoder (int32_t rate,
	                                RingBuffer<std::complex<float> > *b,
	                                QSettings *s):
	                                   virtualDecoder (rate, b),
	                                   myFrame (nullptr),
	                                   localShifter   (rate) {
	theRate		= rate;
	pskSettings	= s;

	setupUi (&myFrame);
	myFrame. show ();

	setup_pskDecoder (rate);
	screenwidth	= 256;
	screenwidth	= 1000;
	x_axis		= new double [screenwidth];
	for (int i = 0; i < screenwidth; i ++)
	   x_axis [i] = - screenwidth / 2 + i;
	y_values	= new double [screenwidth];

	theFilter	= new decimatingFIR (35, screenwidth / 2, PSKRATE, 2);
	newFilter	= new LowPassFIR (21, screenwidth / 2, theRate);
	pskViewer       = new waterfallScope (pskScope,
	                                      screenwidth, 30);
	the_fft		= new common_fft (screenwidth);
	fftBuffer	= the_fft -> getVector ();
	newFFT		= new slidingFFT (screenwidth, 0, screenwidth - 1);
	connect (pskViewer, SIGNAL (clickedwithLeft (int)),
                 this, SLOT (handleClick (int)));
	psk_setup ();

	restore_GUISettings (pskSettings);
//	setting the initial values
	psk_IF			= PSK_IF;
	psk_setAfcon	 	(pskAfconTrigger	-> currentText ());
	psk_setReverse		(pskReverseTrigger	-> currentText ());
	psk_setSquelchLevel	(pskSquelchLevelTrigger	-> value ());
	psk_setMode		(pskModeTrigger		-> currentText ());
	psk_setFilterDegree	(pskFilterDegreeTrigger	-> value ());
	pskText			= QString ();

	connect	(pskAfconTrigger, SIGNAL (activated (const QString &)),
	         this, SLOT (psk_setAfcon (const QString &)));
	connect (pskReverseTrigger, SIGNAL (activated (const QString &)),
	         this, SLOT (psk_setReverse (const QString &)));
	connect (pskSquelchLevelTrigger, SIGNAL (valueChanged (int)),
	         this, SLOT (psk_setSquelchLevel (int)));
	connect (pskModeTrigger, SIGNAL (activated (const QString &)),
	         this, SLOT (psk_setMode (const QString &)));
	connect (pskFilterDegreeTrigger, SIGNAL (valueChanged (int)),
	         this, SLOT (psk_setFilterDegree (int)));
	psk_clrText ();
	fillP	= 0;
}
//
//
	pskDecoder::~pskDecoder	(void) {
	pskSettings	-> beginGroup ("pskDecoder");
	pskSettings	-> setValue ("pskAfconTrigger",
	                              pskAfconTrigger -> currentText ());
	pskSettings	-> setValue ("pskReverseTrigger",
	                              pskReverseTrigger -> currentText ());
	pskSettings	-> setValue ("pskSquelchLevelTrigger",
	                              pskSquelchLevelTrigger -> value ());
	pskSettings	-> setValue ("pskModeTrigger",
	                              pskModeTrigger -> currentText ());
	pskSettings	-> setValue ("pskFilterDegreeTrigger",
	                              pskFilterDegreeTrigger -> value ());
	pskSettings	-> endGroup ();
	delete		viterbiDecoder;
	delete		BPM_Filter;
	delete[]	pskBuffer;	
	delete[]	x_axis;
	delete[]	y_values;
	delete		newFFT;
}

int32_t	pskDecoder::rateOut		(void) {
	return theRate;
}

int16_t	pskDecoder::detectorOffset	(void) {
	return psk_IF;
}

void	pskDecoder::setup_pskDecoder	(int32_t rate) {
	theRate		= rate;
	mapTable	(theRate, PSKRATE);
	pskFilterDegree	= 12;		/* default	*/
	pskAfcon	= false;
	pskMode		= MODE_PSK31;
	pskSquelchLevel	= 5;
	pskReverse	= false;

	pskBuffer	= new std::complex<float> [16];
	for (int i = 0; i < 16; i ++)
	   pskBuffer [i] = std::complex<float> (0, 0);

	pskBitclk	= 0;
	pskDecimatorCount	= 0;
	BPM_Filter	= new bandpassFIR (2 * pskFilterDegree + 1,
	                                  psk_IF - PSK31_SPEED,
	                                  psk_IF + PSK31_SPEED,
	                                  theRate);

	viterbiDecoder	= new viterbi (pskK, pskPOLY1, pskPOLY2);
	QwtDialSimpleNeedle *needle;
	pskPhaseDisplay	-> setWrapping	(false);
	pskPhaseDisplay	-> setOrigin	(0.0);
	pskPhaseDisplay	-> setScale	(2, 360);
	pskPhaseDisplay	-> setScaleArc	(-180.0, 180.0);
	needle		= new QwtDialSimpleNeedle (
	                          QwtDialSimpleNeedle::Arrow, true, Qt::red,
	                          QColor (Qt::yellow). light(130));
	pskPhaseDisplay	-> setNeedle (needle);
}

void	pskDecoder::psk_setup (void) {

	if (!pskAfcon) {
	   psk_IF	= PSK_IF;
	}

//	pskReverse
//	pskSquelchLevel
//	pskMode
//	pskFilterDegree

	pskCycleCount		= 0;
        pskPhaseAcc		= 0;
	pskOldz			= 0;
	psk_phasedifference	= 0.0;
	pskShiftReg		= 0;
	pskDecodeShifter	= 0;
	pskSyncCounter		= 0;
	pskAfcmetrics		= 0;
	pskShiftReg		= 0;
	pskQuality		= 0;
	pskBitclk		= 0;
	pskDecimatorCount	= 0;

	delete	BPM_Filter;
	BPM_Filter		= new bandpassFIR (2 * pskFilterDegree + 1,
	                                           PSK_IF - speedofPskMode (),
	                                           PSK_IF + speedofPskMode (),
	                                           theRate);
}

bool	pskDecoder::isBinarypskMode () {
	if ((pskMode == MODE_PSK31) ||
	    (pskMode == MODE_PSK63) ||
	    (pskMode == MODE_PSK125)) return true;
	return false;
}

bool	pskDecoder::isQuadpskMode () {

	if ((pskMode == MODE_QPSK31) ||
	    (pskMode == MODE_QPSK63) ||
	    (pskMode == MODE_QPSK125)) return true;
	return false;
}

float	pskDecoder::speedofPskMode (void) {
	switch (pskMode) {
	   default:
	   case MODE_PSK31:
	   case MODE_QPSK31:
	      return PSK31_SPEED;

	   case MODE_PSK63:
	   case MODE_QPSK63:
	      return PSK63_SPEED;

	   case MODE_PSK125:
	   case MODE_QPSK125:
	      return PSK125_SPEED;
	}
}

void	pskDecoder::psk_setAfcon (const QString &s) {
	pskAfcon = s == "Afc on";
	psk_setup ();
}

void	pskDecoder::psk_setReverse (const QString &s) {
	pskReverse = s != "normal";
	psk_setup ();
}

void	pskDecoder::psk_setSquelchLevel (int n) {
	pskSquelchLevel = n;
}

void	pskDecoder::psk_setMode (const QString &s) {
	if (s == "psk31")
	   pskMode	= MODE_PSK31;
	else
	if (s == "qpsk31")
	   pskMode	= MODE_QPSK31;
	else
	if (s == "psk63")
	   pskMode	= MODE_PSK63;
	else
	if (s == "qpsk63")
	   pskMode	= MODE_QPSK63;
	else
	if (s == "psk125")
	   pskMode	= MODE_PSK125;
	else
	   pskMode	= MODE_QPSK125;
	psk_setup ();
}

void	pskDecoder::psk_setFilterDegree (int n) {
	pskFilterDegree = n;
	psk_setup ();
}

uint8_t	pskDecoder::getIntPhase (DSPCOMPLEX z) {
double phase = arg (z);

	if (phase < 0)
	   phase += 2 * M_PI;
	if (this -> isQuadpskMode ())	/* phases 0 .. 3	*/
	   return ((uint8_t)(phase / (M_PI / 2) + 0.5)) & 03;
	else				/* just 0 or 1		*/
	   return (((uint8_t)(phase / M_PI + 0.5)) & 1);
}

int16_t	pskDecoder::DecimatingCountforpskMode (void) {
	switch (pskMode) {
	   default:
	   case MODE_PSK31:
	   case MODE_QPSK31:
	      return 4;
	   case MODE_PSK63:
	   case MODE_QPSK63:
	      return 2;
	   case MODE_PSK125:
	   case MODE_QPSK125:
	      return 1;
	}
}

void	pskDecoder::process (std::complex<float> z) {
std::complex<float>	old_z;
std::complex<float>	out [256];
uint16_t	i;
int16_t		cnt;
std::complex<float> v [PSKRATE / 8];

	if (++pskCycleCount > theRate / 10) {
	   pskCycleCount	= 0;
	   psk_showMetrics	(pskAfcmetrics);
	   psk_showIF		(psk_IF);
	   setDetectorMarker	(psk_IF);
	   psk_showDial		(psk_phasedifference);
	   audioAvailable	(theRate / 10, theRate);
	}
	
	audioOut -> putDataIntoBuffer (&z, 1);
//	we down-mix the signal to a zero-centered one,
	old_z	= z;
	z	= BPM_Filter -> Pass (z);
	z	= localShifter. do_shift  (z, psk_IF);
	z	= newFilter -> Pass (z);
	
//
//	and resample to PSKRATE
	cnt	= resample (z, out);
	if (cnt < 0) 
	   return;

#define NO_OFFSET_FOUND	-500
std::complex<float> outV [2000];
static int fftTeller = 0;
//	Now we are on PSKRATE 
	for (i = 0; i < cnt; i ++) {
	   int j;
	   std::complex<float> res;
	   if (theFilter -> Pass (out [i], &res)) {
	      fftBuffer [fillP ++] = res;
	      newFFT	-> do_FFT (res, outV);
	      if (fillP < screenwidth / 2) 
	         continue;
	      for (int j = fillP; j < screenwidth; j ++)
                 fftBuffer [j] = std::complex< float> (0, 0);
//	      the_fft -> do_FFT ();
	      for (int j = 0; j < screenwidth; j ++) 
	            y_values [j] = 
	                   abs (outV [(screenwidth / 2 + j) % screenwidth]);
	      pskViewer -> display (x_axis, y_values,
                                        amplitudeSlider -> value (), 0, 0);
	      fillP     = 0;
	      fftTeller ++;
	      if (fftTeller >= 4) {
	         int offs = offset (outV);
	         fprintf (stderr, "offset = %d\n", offs);
	         if ((offs != NO_OFFSET_FOUND) && (abs (offs) >= 3))
	            psk_IF += offs / 2;
	         fftTeller = 0;
	         newFFT -> reset ();
	      }
	   }
	}

	for (i = 0; i < cnt; i ++) {
	   if (++pskDecimatorCount < this -> DecimatingCountforpskMode ()) 
	      continue;
	   doDecode (out [i]);
	}
}

/*
 *	now we are finally back on 16 samples per symbol
 *	to apply the "standard" algorithm
 */
void	pskDecoder::doDecode (std::complex<float> v) {
uint16_t	pskBufferp;
float		sum	= 0;
float		ampsum	= 0;
int16_t	i;
uint8_t	bits;

	pskDecimatorCount			= 0;
	pskBufferp				= (int) pskBitclk;
	pskBuffer [pskBufferp % 16]		= v;

	for (i = 0; i < 16 / 2; i ++) {
	   sum    += abs (pskBuffer [i]) - abs (pskBuffer [16 / 2 + i]);
	   ampsum += abs (pskBuffer [i]) + abs (pskBuffer [16 / 2 + i]);
	}

	sum = (ampsum < 1.0 / 16 ? 0 : sum / ampsum);
	pskBitclk -= sum / 5.0;
	pskBitclk += 1;
	if (pskBitclk < 0)
	   pskBitclk += 16;
	if (pskBitclk < 16) 
	   return;

	pskBitclk -= 16;
	psk_phasedifference = arg (pskBuffer [4] * conj (pskBuffer [12]));
	if (psk_phasedifference < 0)
	   psk_phasedifference += 2 * M_PI;

	if (this -> isQuadpskMode ()) {
	   bits = (uint8_t(psk_phasedifference / (M_PI / 2) + 0.5)) & 03;
	   if (decodingIsOn (4, psk_phasedifference, bits, &pskQuality))
	      qpsk_bit (bits);
	}
	else {  //if (isBinarypskMode (pskMode)) 
	   bits = (((uint8_t)(psk_phasedifference / M_PI + 0.5)) & 1) << 1;
	   if (decodingIsOn (2, psk_phasedifference, bits, &pskQuality))
	      bpsk_bit  (!bits);
	}
	
	pskAfcmetrics	= 0.99 * pskAfcmetrics +
	                               0.01 * (100 * norm (pskQuality));

	if (pskAfcon)
	   psk_IF += psk_IFadjust (psk_phasedifference, bits);
}


uint8_t	pskDecoder::decodingIsOn (uint8_t n,
	                          double phase,
	                          uint8_t bits, DSPCOMPLEX *pskQuality) {
	pskDecodeShifter	= (pskDecodeShifter << 2) | bits;

	switch (pskDecodeShifter) {
	   case 0xAAAAAAAA:	/* "1010101010..." preAmble	*/
	      *pskQuality	= DSPCOMPLEX (1.0, 0.0);
	      return true;

	   case 0x0:		/* postamble or garbage		*/
	      *pskQuality	= 0;
	      return false;

	   default:
	      *pskQuality	 =  DSPCOMPLEX (0.05 * cos (n * phase) +
		                                0.95 * real (*pskQuality),
		                                0.05 * sin (n * phase) +
		                                0.95 * imag (*pskQuality));
	      return
		 ((100.0 * norm (*pskQuality)) > (pskSquelchLevel));
	}
}
/*
 *	in case of complete sync, phase = bits * PI,
 *	otherwise, find a basis for correcting the IF
 *	phase and bits are related
 */
double	pskDecoder::psk_IFadjust (double phasedifference, uint16_t bits) {
double	error;

	if (pskAfcmetrics < 0.1)
	   return 0.0;

	error = phasedifference - (double)bits * (M_PI / 2);
	if (error < - M_PI / 2)
	   error += 2 * M_PI;
	else
	if (error >= M_PI /2)
	   error -= 2 * M_PI;

	error /= 2 * M_PI;
	error *= (double)(PSKRATE / DecimatingCountforpskMode ()) / 8;
	return  - error / 256;
}

void	pskDecoder::bpsk_bit (uint16_t bit) {
int16_t	c;

	pskShiftReg = (pskShiftReg << 1) | !!bit;
	if ((pskShiftReg & 03) == 0) {
	   c = pskVaridecode (pskShiftReg >> 2);
	   if (c != -1) 
	      psk_addText ((char)c);
	   pskShiftReg = 0;
	}
}

void	pskDecoder::qpsk_bit (uint16_t bits) {
uint8_t sym [2];
int16_t	c;

	if (pskReverse)
	   bits = (4 - bits) & 03;

	sym [0] = (bits & 01) ? 255 : 0;
	sym [1] = (bits & 02) ? 0 : 255;	// top bit is flipped

	c = viterbiDecoder -> viterbi_decode (sym, NULL);

	if (c != -1) {
	   bpsk_bit (c & 0x80);
	   bpsk_bit (c & 0x40);
	   bpsk_bit (c & 0x20);
	   bpsk_bit (c & 0x10);
	   bpsk_bit (c & 0x08);
	   bpsk_bit (c & 0x04);
	   bpsk_bit (c & 0x02);
	   bpsk_bit (c & 0x01);
	}
}
/*
 * The code table in a format suitable for decoding.
 */
static unsigned int varicodetab2[] = {
	0x2AB, 0x2DB, 0x2ED, 0x377, 0x2EB, 0x35F, 0x2EF, 0x2FD, 
	0x2FF, 0x0EF, 0x01D, 0x36F, 0x2DD, 0x01F, 0x375, 0x3AB, 
	0x2F7, 0x2F5, 0x3AD, 0x3AF, 0x35B, 0x36B, 0x36D, 0x357, 
	0x37B, 0x37D, 0x3B7, 0x355, 0x35D, 0x3BB, 0x2FB, 0x37F, 
	0x001, 0x1FF, 0x15F, 0x1F5, 0x1DB, 0x2D5, 0x2BB, 0x17F, 
	0x0FB, 0x0F7, 0x16F, 0x1DF, 0x075, 0x035, 0x057, 0x1AF, 
	0x0B7, 0x0BD, 0x0ED, 0x0FF, 0x177, 0x15B, 0x16B, 0x1AD, 
	0x1AB, 0x1B7, 0x0F5, 0x1BD, 0x1ED, 0x055, 0x1D7, 0x2AF, 
	0x2BD, 0x07D, 0x0EB, 0x0AD, 0x0B5, 0x077, 0x0DB, 0x0FD, 
	0x155, 0x07F, 0x1FD, 0x17D, 0x0D7, 0x0BB, 0x0DD, 0x0AB, 
	0x0D5, 0x1DD, 0x0AF, 0x06F, 0x06D, 0x157, 0x1B5, 0x15D, 
	0x175, 0x17B, 0x2AD, 0x1F7, 0x1EF, 0x1FB, 0x2BF, 0x16D, 
	0x2DF, 0x00B, 0x05F, 0x02F, 0x02D, 0x003, 0x03D, 0x05B, 
	0x02B, 0x00D, 0x1EB, 0x0BF, 0x01B, 0x03B, 0x00F, 0x007, 
	0x03F, 0x1BF, 0x015, 0x017, 0x005, 0x037, 0x07B, 0x06B, 
	0x0DF, 0x05D, 0x1D5, 0x2B7, 0x1BB, 0x2B5, 0x2D7, 0x3B5, 
	0x3BD, 0x3BF, 0x3D5, 0x3D7, 0x3DB, 0x3DD, 0x3DF, 0x3EB, 
	0x3ED, 0x3EF, 0x3F5, 0x3F7, 0x3FB, 0x3FD, 0x3FF, 0x555, 
	0x557, 0x55B, 0x55D, 0x55F, 0x56B, 0x56D, 0x56F, 0x575, 
	0x577, 0x57B, 0x57D, 0x57F, 0x5AB, 0x5AD, 0x5AF, 0x5B5, 
	0x5B7, 0x5BB, 0x5BD, 0x5BF, 0x5D5, 0x5D7, 0x5DB, 0x5DD, 
	0x5DF, 0x5EB, 0x5ED, 0x5EF, 0x5F5, 0x5F7, 0x5FB, 0x5FD, 
	0x5FF, 0x6AB, 0x6AD, 0x6AF, 0x6B5, 0x6B7, 0x6BB, 0x6BD, 
	0x6BF, 0x6D5, 0x6D7, 0x6DB, 0x6DD, 0x6DF, 0x6EB, 0x6ED, 
	0x6EF, 0x6F5, 0x6F7, 0x6FB, 0x6FD, 0x6FF, 0x755, 0x757, 
	0x75B, 0x75D, 0x75F, 0x76B, 0x76D, 0x76F, 0x775, 0x777, 
	0x77B, 0x77D, 0x77F, 0x7AB, 0x7AD, 0x7AF, 0x7B5, 0x7B7, 
	0x7BB, 0x7BD, 0x7BF, 0x7D5, 0x7D7, 0x7DB, 0x7DD, 0x7DF, 
	0x7EB, 0x7ED, 0x7EF, 0x7F5, 0x7F7, 0x7FB, 0x7FD, 0x7FF, 
	0xAAB, 0xAAD, 0xAAF, 0xAB5, 0xAB7, 0xABB, 0xABD, 0xABF, 
	0xAD5, 0xAD7, 0xADB, 0xADD, 0xADF, 0xAEB, 0xAED, 0xAEF, 
	0xAF5, 0xAF7, 0xAFB, 0xAFD, 0xAFF, 0xB55, 0xB57, 0xB5B
};


int16_t	pskDecoder::pskVaridecode (uint16_t symbol) {
int i;

	for (i = 0; i < 256; i++)
	   if (symbol == varicodetab2[i])
	      return i;

	return -1;
}

void	pskDecoder::psk_showDial	(float f) {
	pskPhaseDisplay		-> setValue (f * 360.0 / (2 * M_PI));
}

void	pskDecoder::psk_showMetrics	(float f) {
	pskQualitydisplay	-> display (f);
}

void	pskDecoder::psk_showIF		(float f) {
	pskIFdisplay		-> display (f);
}

void	pskDecoder::psk_addText		(char c) {
	if (c < ' ') c = ' ';
	pskText. append (QString (QChar (c)));
	if (pskText. length () > 86)
	   pskText. remove (0, 1);
	psktextBox	-> setText (pskText);
}

void	pskDecoder::psk_clrText		(void) {
	pskText		= QString ("");
	psktextBox	-> setText (pskText);
}
//
//	simple "resampler", a simple table addressing N output samples
//	in M inputsamples
void	pskDecoder::mapTable		(int32_t inRate, int32_t outRate) {
int16_t	i;

	theTable. resize (outRate / 100);
	inTable.  resize (inRate / 100 + 1);
	for (i = 0; i < (int16_t)(theTable. size ()); i ++)
	   theTable [i] = i * (float)inRate / outRate;
	tablePointer	= 0;
}

int	pskDecoder::resample		(std::complex<float> in,
	                                 std::complex<float> *out) {
int16_t i;

	inTable [tablePointer ++] = in;
	if (tablePointer < (int16_t)(inTable. size ()))
	   return -1;

	for (i = 0; i < (int16_t)(theTable. size ()); i ++) {
	   int16_t p = floor (theTable [i]);
	   float   q = theTable [i];
	   out [i] = cmul (inTable [p + 1], q - p) +
	                        cmul (inTable [p], (1 - (q - p)));
	}
	inTable [0]	= inTable [inTable. size () - 1];
	tablePointer	= 1;
	return theTable. size ();
}
	
void	pskDecoder::restore_GUISettings (QSettings *p) {
QString	temp;
int	k;

	p	-> beginGroup ("pskDecoder");
	temp	= pskSettings -> value ("pskAfconTrigger",
	                               pskAfconTrigger -> currentText ()).
	                                                          toString ();
	k	= pskAfconTrigger -> findText (temp);
	if (k != -1)
	   pskAfconTrigger	-> setCurrentIndex (k);
	temp	= pskSettings -> value ("pskReverseTrigger",
	                               pskReverseTrigger -> currentText ()).
	                                                          toString ();
	k	= pskReverseTrigger -> findText (temp);
	if (k != -1)
	   pskReverseTrigger	-> setCurrentIndex (k);
	k	= pskSettings -> value ("pskSquelchLevelTrigger",
	                               pskSquelchLevelTrigger -> value ()).
	                                                          toInt ();
	pskSquelchLevelTrigger -> setValue (k);
	temp	= pskSettings -> value ("pskModeTrigger",
	                               pskModeTrigger -> currentText ()).
	                                                          toString ();
	k	= pskModeTrigger -> findText (temp);
	if (k != -1)
	   pskModeTrigger	-> setCurrentIndex (k);
	k	= pskSettings -> value ("pskFilterDegreeTrigger",
	                               pskFilterDegreeTrigger -> value ()).
	                                                          toInt ();
	pskFilterDegreeTrigger	-> setValue (k);
	pskSettings		-> endGroup ();
}

void	pskDecoder::handleClick	(int a) {
	fprintf (stderr, "adjust %d\n", a);
	adjustFrequency (a);
}

#define	MAX_SIZE 50
int	pskDecoder::offset (std::complex<float> *v) {
float avg	= 0;
float max	= 0;
float	supermax	= 0;
int	superIndex	= 0;

//	for (int i = 0; i < screenwidth; i ++)
//	   v [(screenwidth / 2 + i) % screenwidth] = 
//	             std::complex<float> (2 * sin ((float)i / (M_PI)), 0);
	for (int i = 0; i < screenwidth; i ++)
	   avg += abs (v [i]);
	avg /= screenwidth;
	int	index	= screenwidth - 200;
	for (int i = 0; i < MAX_SIZE; i ++)
	   max +=  abs (v [index + i]);

	supermax	= max;
	for (int i = MAX_SIZE; i < 400; i ++) {
	   max -=  abs (v [(index + i - MAX_SIZE) % screenwidth]);
	   max +=  abs (v [(index + i) % screenwidth]);
	   if (max > supermax) {
	      superIndex = (index + i - MAX_SIZE / 2) % screenwidth;
	      supermax = max;
	   }
	}
	fprintf (stderr, "%f (%f) %d\n", supermax / MAX_SIZE, avg, superIndex);
	if (supermax / MAX_SIZE > 3 * avg)
	   return superIndex > screenwidth / 2 ?
	                            superIndex - screenwidth :
	                                     superIndex;
	else
	   return NO_OFFSET_FOUND;
}
