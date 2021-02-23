#
/*
 *    Copyright (C) 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
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
#include	"rtty-decoder.h"
#include	<QSettings>
#include	<math.h>
#include	"shifter.h"
#include	"utilities.h"
#include	"fir-filters.h"

#define	RTTY_IF		00

#define	StaysInBand(x)	(((RTTY_IF - rttyShift / 10) < (x)) && ((x) < RTTY_IF + rttyShift / 10))

			rttyDecoder::rttyDecoder (int32_t rate,
	                                          RingBuffer<std::complex<float> > *b,
	                                          QSettings *s):
	                                             virtualDecoder (rate, b),
	                                             localShifter (rate) {
	theRate		= rate;
	rttySettings	= s;

	myFrame		= new QFrame;
	setupUi (myFrame);
	myFrame	-> show ();
	restore_GUISettings	(s);
	setup_rttyDecoder	();
}
//
	rttyDecoder::~rttyDecoder		(void) {
	rttySettings	-> beginGroup ("rttyDecoder");
	rttySettings	-> setValue ("rttyBaudrateSelect",
	                              rttyBaudrateSelect  -> currentText ());
	rttySettings	-> setValue ("rttyWidthSelect",
	                              rttyWidthSelect     -> currentText ());
	rttySettings	-> setValue ("rttyParitySelect",
	                              rttyParitySelect    -> currentText ());
	rttySettings	-> setValue ("rttyMsbSelect",
	                              rttyMsbSelect	  -> currentText ());
	rttySettings	-> setValue ("rttyNbitsTrigger",
	                              rttyNbitsTrigger    -> currentText ());
	rttySettings	-> setValue ("rttyStopbitsTrigger",
	                              rttyStopbitsTrigger -> currentText ());
	rttySettings	-> setValue ("rttyReverseTrigger",
	                              rttyReverseTrigger  -> currentText ());
	rttySettings	-> setValue ("rttyAfconTrigger",
	                              rttyAfconTrigger    -> currentText ());
	rttySettings	-> endGroup ();
	myFrame	-> hide ();
	delete	rttyAverager;
	delete	BPM_Filter;
	delete	myFrame;
}

void	rttyDecoder::setup_rttyDecoder (void) {
QString	temp;

	rttyPrevsample		= 0;
	rttyCycleCount		= 0;
	rttyIF			= RTTY_IF;
	rttyStopbits		= 1;
	rttyBaudrate		= 45;
	rttyShift		= 170;
        rttyParity		= RTTY_PARITY_NONE;
	rttySymbolLength	= (int)(theRate / rttyBaudrate + 0.5);
	rttyReversed		= false;
	rttyNbits		= 5;
	rttyAfcon		= false;
	rttyMsb			= false;
	rttyFilterDegree	= 10;
	BPM_Filter		= new bandpassFIR (rttyFilterDegree,
	                                           RTTY_IF - rttyBaudrate,
	                                           RTTY_IF + rttyBaudrate,
	                                           theRate);

	rttyAverager	= new average (rttySymbolLength / 3);
        rtty_clrText	();
	rtty_setup ();
//
//	Now the actual initial settings
	set_rttyBaudrate	(rttyBaudrateSelect	-> currentText ());
	set_rttyShift		(rttyWidthSelect	-> currentText ());
	set_rttyParity		(rttyParitySelect	-> currentText ());
	set_rttyMsb		(rttyMsbSelect		-> currentText ());
	set_rttyNbits		(rttyNbitsTrigger	-> currentText ());
	set_rttyStopbits	(rttyStopbitsTrigger	-> currentText ());
	set_rttyReverse		(rttyReverseTrigger	-> currentText ());
	set_rttyAfcon		(rttyAfconTrigger	-> currentText ());
	rttyText		= QString ();
//	The connects
//
	connect (rttyBaudrateSelect, SIGNAL (activated (const QString &)),
	         this, SLOT (set_rttyBaudrate (const QString &)));
	connect (rttyWidthSelect, SIGNAL (activated (const QString &)),
	         this, SLOT (set_rttyShift (const QString &)));
	connect (rttyParitySelect, SIGNAL (activated (const QString &)),
		 this, SLOT (set_rttyParity (const QString &)));
	connect (rttyMsbSelect,    SIGNAL (activated (const QString &)),
		 this, SLOT (set_rttyMsb   (const QString &)));
	connect (rttyNbitsTrigger,  SIGNAL (activated (const QString &)),
		 this, SLOT (set_rttyNbits (const QString &)));
	connect (rttyStopbitsTrigger, SIGNAL (activated (const QString &)),
		 this, SLOT (set_rttyStopbits (const QString &)));
	connect (rttyReverseTrigger, SIGNAL (activated (const QString &)),
		 this, SLOT (set_rttyReverse (const QString &)));
	connect (rttyAfconTrigger, SIGNAL (activated (const QString &)),
		 this, SLOT (set_rttyAfcon (const QString &)));
}

void	rttyDecoder::rtty_setup (void) {
	if (rttyStopbits == 1)
	   rttyStoplen	= (int)(theRate / rttyBaudrate + 0.5);
	else
	if (rttyStopbits == 2)
	   rttyStoplen	= (int)(1.5 * theRate / rttyBaudrate + 0.5);
	else
	   rttyStoplen	= (int)(2.0 * theRate / rttyBaudrate + 0.5);

	if (!rttyAfcon) {
	   rttyIF = RTTY_IF;
	}

	if (rttyShift / 2 <= rttyBaudrate) {
	   rttyShift	= 170;
	   rttyBaudrate = 45;
	}
	rttyIF		= RTTY_IF;

	delete		BPM_Filter;
	BPM_Filter	= new  bandpassFIR (rttyFilterDegree,
	                                    RTTY_IF - 3 * rttyBaudrate / 4, 
	                                    RTTY_IF + 3 * rttyBaudrate / 4,
	                                    theRate);
	rttySymbolLength	= (int)(theRate / rttyBaudrate + 0.5);
	delete		rttyAverager;
	rttyAverager		= new average (rttySymbolLength / 3);
	rttyState		= RTTY_RX_STATE_IDLE;
        rttyPhaseacc		= 0;
	rttyPluscnt		= 0;
	rttyMincnt		= 0;
	rttyPrevfragment	= 0;
	rttySignalPower		= 0;
	rttyNoisePower		= 0;

	rttyPoscnt		= 0;
	rttyNegcnt		= 0;
	rttyPosFreq		= 0.0;
	rttyNegFreq		= 0.0;
	rttyFreqError		= 0.0;
}

void	rttyDecoder::rtty_setReverse (bool n) {
	rttyReversed = n;
	rtty_setup ();
}

uint32_t	rttyDecoder::reverseBits (uint32_t in, uint32_t n) {
	if (n <= 1)
	   return in;
	return ((in & 01) << (n - 1)) | reverseBits (in >> 1, n - 1);
}

uint8_t rttyDecoder::pickup_char (void) {
uint8_t	data_mask	= (1 << rttyNbits) - 1;
uint8_t	parbit 		= (rttyRxdata >> rttyNbits) & 01;
uint8_t	data;

        if (rttyParity != RTTY_PARITY_NONE) {
	   if (parbit != check_parity (rttyRxdata, rttyNbits, rttyParity)) {
//	      fprintf (stderr, "Parity error");
	      return 0;
	   }
	}

	data	= (int)(rttyRxdata & data_mask);
	if (rttyMsb)
	   data = reverseBits (data, rttyNbits);

	if (rttyNbits == 5)
	   return BaudottoASCII (data);

	return data;
}
/*
 *	feed the sample here
 */
void	rttyDecoder::process (std::complex<float> z) {
std::complex<float>	local_copy;
double		FreqOffset;
int	res;
//	keep a local copy of the sample
	local_copy	= z;

	z	= BPM_Filter -> Pass (z);
	z	= localShifter. do_shift (z, rttyIF);
// 	then we apply slicing to end up with omega and
//	we compute the offset frequency
	FreqOffset	= arg (conj (rttyPrevsample) * z) *
	                            theRate / (2 * M_PI);
	rttyPrevsample	= z;

	if (FreqOffset > 0) {
	   rttyPoscnt ++;
	   rttyPosFreq += FreqOffset;
	}

	if (FreqOffset < 0) {
	   rttyNegcnt ++;
	   rttyNegFreq += FreqOffset;
	}

	FreqOffset	= clamp (FreqOffset, - rttyShift / 2, + rttyShift / 2);
	FreqOffset	= rttyAverager -> filter (FreqOffset);
/*
 */
	rttyBitFragment	= (rttyReversed) ?
	                     (FreqOffset > 0.0) : (FreqOffset < 0.0);
/*
 *	Note: we were experimenting here with the approach to
 *	compute the "average values" of the mark and space
 *	frequencies
 *	Not very successful so far
 */
	res	= this -> addrttyBitFragment ();
	if (res && rttyAfcon) {
	   float FreqAdjust = this -> adjust_IF (FreqOffset);
	   if (StaysInBand (rttyIF + FreqAdjust)) 
	      rttyIF += FreqAdjust;
	}
	else 
	if (res) {
	   if ((rttyPoscnt > 100) && (rttyNegcnt > 100)) {
	      double ferr = - (rttyPosFreq / rttyPoscnt +
	                             rttyNegFreq / rttyNegcnt) / 2;
	      rttyFreqError = decayingAverage (rttyFreqError, ferr, 10);
	      rtty_showFreqCorrection ((float)rttyFreqError);
	      rttyPoscnt = 0; rttyPosFreq = 0.0;
	      rttyNegcnt = 0; rttyNegFreq = 0.0;
	   }
	}

	if (++rttyCycleCount > theRate / 4) {
	   rttyCycleCount	= 0;
//	   rtty_showStrength   (rttyS2N ());
	   rtty_showStrength   ((float)rttyIF);
//	   rtty_showFreqCorrection ((float)rttyIF);
	   setDetectorMarker	(rttyIF);
	   rtty_showGuess      ((int)(2.0 * FreqOffset));
	   audioAvailable	(theRate / 10, theRate);
	}

	audioOut	-> putDataIntoBuffer (&local_copy, 1);
}
/*
 *	frequency is "equal" to (+ or -) rttyShift / 2 when
 *	rttyIF is precisely in the middle between the mark and the space.
 *	Adjust slowly if needed
 */
double	rttyDecoder::adjust_IF (double f) {

	f	= f > 0 ? f - rttyShift / 2 : f + rttyShift / 2;

	if (fabs (f) < rttyShift / 2)
	   return f / 256;

	return 0.0;
}

uint8_t	rttyDecoder::addrttyBitFragment (void) {
uint8_t c;
uint8_t	res	= 0;

	switch (rttyState) {
	   case RTTY_RX_STATE_IDLE:
	      if (rttyBitFragment == 0) {
	         rttyState = RTTY_RX_STATE_START;
	         rttyCounter = rttySymbolLength / 2;
	      }
	      break;

	   case RTTY_RX_STATE_START:
	      if (--rttyCounter == 0) {
	         if (rttyBitFragment == 0) {	/* read the data	*/
	            rttyCounter	= rttySymbolLength;
	            rttyBitcntr	= 0;
	            rttyRxdata		= 0;
	            res			= 1;
	            rttyState		= RTTY_RX_STATE_DATA;
	         }
	         else
	            rttyState = RTTY_RX_STATE_IDLE;
	      }
	      break;

	   case RTTY_RX_STATE_DATA:
	      if (--rttyCounter <= 0) {
	         rttyRxdata	|= rttyBitFragment << rttyBitcntr;
	         rttyBitcntr ++;
	         rttyCounter	= rttySymbolLength;
	         res		= 1;
	      }

	      if (rttyBitFragment == 1)
	         rttyPluscnt ++;
	      else
	         rttyMincnt ++;
	      if (rttyBitFragment != rttyPrevfragment) {
	         check_baudrate (rttyMincnt, rttyPluscnt);
	         rttyMincnt		= 0;
	         rttyPluscnt		= 0;
	      }

	      rttyPrevfragment = rttyBitFragment;
	      if (rttyBitcntr >= rttyNbits) {
	         res = 1;
                 if (rttyParity == RTTY_PARITY_NONE)
	            rttyState = RTTY_RX_STATE_STOP;
	         else
	            rttyState = RTTY_RX_STATE_PARITY;
	      }
	      break;

	   case RTTY_RX_STATE_PARITY:
	      if (--rttyCounter == 0) {
	         rttyState = RTTY_RX_STATE_STOP;
	         rttyRxdata |= rttyBitFragment << rttyBitcntr++;
	         rttyCounter = rttyStoplen;
	      }
	      break;

	   case RTTY_RX_STATE_STOP:
	      if (--rttyCounter == 0) {
	         if (rttyBitFragment == 1) {
	            c = pickup_char();
		    rtty_addText (c);
	         }
	         else
	            c = 'F';
	         rttyState = RTTY_RX_STATE_STOP2;
	         rttyCounter = rttySymbolLength / 2;
	      }
	      break;

	   case RTTY_RX_STATE_STOP2:
	      if (--rttyCounter == 0)
	         rttyState = RTTY_RX_STATE_IDLE;
	      break;
	}

	return res;
}

static
char letters [32] = {
	'\0',	'E',	'\n',	'A',	' ',	'S',	'I',	'U',
	'\r',	'D',	'R',	'J',	'N',	'F',	'C',	'K',
	'T',	'Z',	'L',	'W',	'H',	'Y',	'P',	'Q',
	'O',	'B',	'G',	'\·',	'M',	'X',	'V',	'·'
};

///*
// * ITA-2 version of the figures case.
// */
//static unsigned char figures[32] = {
//	'\0',	'3',	'\n',	'-',	' ',	'\'',	'8',	'7',
//	'\r',	'·',	'4',	'\a',	',',	'·',	':',	'(',
//	'5',	'+',	')',	'2',	'·',	'6',	'0',	'1',
//	'9',	'?',	'·',	'·',	'.',	'/',	'=',	'·'
//};
///*
// * U.S. version of the figures case.
// */
//static unsigned char figures[32] = {
//	'\0',	'3',	'\n',	'-',	' ',	'\a',	'8',	'7',
//	'\r',	'$',	'4',	'\'',	',',	'!',	':',	'(',
//	'5',	'"',	')',	'2',	'#',	'6',	'0',	'1',
//	'9',	'?',	'&',	'·',	'.',	'/',	';',	'·'
//};
/*
 * A mix of the two. This is what seems to be what people actually use.
 */
static
char figures [32] = {
	'\0',	'3',	'\n',	'-',	' ',	'\'',	'8',	'7',
	'\r',	'$',	'4',	'\a',	',',	'!',	':',	'(',
	'5',	'+',	')',	'2',	'H',	'6',	'0',	'1',
	'9',	'?',	'&',	'·',	'.',	'/',	'=',	'·'
};

uint8_t	rttyDecoder::BaudottoASCII (uint8_t data) {
int16_t out = 0;

	switch (data) {
	   case 0x1F:		/* letters */
	      rttyRxmode = BAUDOT_LETS;
	      break;

	   case 0x1B:		/* figures */
	      rttyRxmode = BAUDOT_FIGS;
	      break;

	   case 0x04:		/* unshift-on-space */
	      rttyRxmode = BAUDOT_LETS;
	      return ' ';
	      break;

	   default:
	      if (rttyRxmode == BAUDOT_LETS)
	         out = letters [data];
	      else
	         out = figures [data];
	      break;
	}

	return out;
}
/*
 * Parity function. Return one if `w' has odd number of ones, zero otherwise.
 */
uint8_t rttyDecoder::odd (uint32_t w) {
int16_t	i;
int16_t	b = 0;
	for (i = 0; w != 0; i ++) {
	   b += (w & 01);
	   w >>= +1;
	}
	return b & 01;
}

uint8_t	rttyDecoder::check_parity (uint32_t c,
				   uint32_t nbits, uint8_t par) {
	c &= (1 << nbits) - 1;

	switch (par) {
	default:
        case RTTY_PARITY_NONE:
		return 1;

        case RTTY_PARITY_ODD:
		return odd (c);

        case RTTY_PARITY_EVEN:
		return odd (c) == 0;

        case RTTY_PARITY_ZERO:
		return 0;

        case RTTY_PARITY_ONE:
		return 1;
	}
}
/*
 *	very experimental. While composing the bits of the character
 *	we count the number of samples with a given sign
 *	Obviously, this stands (if anything) for one or more
 *	bits, we'll count a while.
 */

int32_t	inrange (int32_t t, int32_t l, int32_t h) {
	return (l <= t && t <= h);
}

void	rttyDecoder::check_baudrate (int16_t mincnt, int16_t pluscnt) {
int16_t	baudrate;

	mincnt = mincnt > pluscnt ? mincnt : pluscnt;
	if (mincnt < theRate / 1000) /* arbitrary value */
	   return;

	baudrate = theRate / mincnt;

	if (inrange (baudrate, 40, 47))
	   baudrate = 45;
	else
	if (inrange (baudrate, 47, 55))
	   baudrate = 50;
	else
	if (inrange (baudrate, 65, 85))
	   baudrate = 75;
	else
	if (inrange (baudrate, 90, 110))
	   baudrate = 100;
	else
	   baudrate = 0;

	if (baudrate != 0)
	   rtty_showBaudRate (baudrate);
}

double	rttyDecoder::rttyS2N (void) {
	return 0;
}
/*
 *	Now for rtty
 */
void	rttyDecoder::rtty_showStrength (float f) {
	rttyStrengthMeter	-> display (f);
}

void	rttyDecoder::rtty_showFreqCorrection (float f) {
	rttyFreqCorrection	-> display (f);
}

void	rttyDecoder::rtty_showGuess (int f) {
	rttyGuess		-> display (f);
}

void	rttyDecoder::rtty_showBaudRate (int f) {
	rttyBaudRate		-> display (f);
}

void	rttyDecoder::rtty_clrText (void) {
	rttyText = QString ("");
	rttytextBox		-> setText (rttyText);
}

void	rttyDecoder::rtty_addText (char c) {
	if (c < ' ') c = ' ';
	rttyText.append (QString (QChar (c)));
	if (rttyText. length () > 86)
	   rttyText. remove (0, 1);
	rttytextBox		-> setText (rttyText);
}

void	rttyDecoder::set_rttyAfcon (const QString &s) {
	rttyAfcon = s == "afc on";
	rtty_setup ();
}

void	rttyDecoder::set_rttyStopbits (const QString &s) {
int16_t	stopbits;

	stopbits = (s == "2 stop") ? 3 : (s == "1.5 stop") ? 2 : 1;
	rttyStopbits = stopbits;
	rtty_setup ();
}

void	rttyDecoder::set_rttyBaudrate (const QString &s) {
int16_t	n = s. toInt ();
	rttyBaudrate = n;
	rtty_setup ();
}


void	rttyDecoder::set_rttyShift (const QString &s) {
int16_t	n	= s. toInt ();
	rttyShift = n;
	rtty_setup ();
}

void	rttyDecoder::set_rttyParity (const QString &s) {

	if (s == "par none")
           rttyParity = rttyDecoder::RTTY_PARITY_NONE;
	else
	if (s == "par one")
           rttyParity = rttyDecoder::RTTY_PARITY_ONE;
	else
	if (s == "par odd")
           rttyParity = rttyDecoder::RTTY_PARITY_ODD;
	else
	if (s == "par even")
           rttyParity = rttyDecoder::RTTY_PARITY_EVEN;
	else
	if (s == "par zero")
           rttyParity = rttyDecoder::RTTY_PARITY_ZERO;
	else
           rttyParity = rttyDecoder::RTTY_PARITY_NONE;

	rtty_setup ();
}


void	rttyDecoder::set_rttyReverse (const QString &s) {
	rtty_setReverse (s == "normal");
}

void	rttyDecoder::set_rttyNbits (const QString &s) {
int	nbits;

	nbits = (s == "8 bits") ? 8 : (s == "7 bits") ? 7 : 5;
	rttyNbits = nbits;
	rtty_setup ();
}

void	rttyDecoder::set_rttyMsb (const QString &s) {
	rttyMsb = s == "msb true";
}

void	rttyDecoder::restore_GUISettings (QSettings *s) {
QString temp;
int	k;

//	Initial settings from previous incarnations or just the GUI
	s	-> beginGroup ("rttyDecoder");
	temp	= s -> value ("rttyBaudrateSelect",
	                              rttyBaudrateSelect -> currentText ()).
	                                                       toString ();
	k	= rttyBaudrateSelect -> findText (temp);
	if (k != -1)
	   rttyBaudrateSelect	-> setCurrentIndex (k);
	temp	= s -> value ("rttyWidthSelect",
	                               rttyWidthSelect -> currentText ()).
	                                                       toString ();
	k	= rttyWidthSelect -> findText (temp);
	if (k != -1)
	   rttyWidthSelect	-> setCurrentIndex (k);
	temp	= s -> value ("rttyParitySelect",
	                               rttyParitySelect -> currentText ()).
	                                                       toString ();
	k	= rttyParitySelect -> findText (temp);
	if (k != -1)
	   rttyParitySelect	-> setCurrentIndex (k);
	temp	= s -> value ("rttyMsbSelect",
	                               rttyMsbSelect	-> currentText ()).
	                                                       toString ();
	k	= rttyMsbSelect -> findText (temp);
	if (k != -1)
	   rttyMsbSelect	-> setCurrentIndex (k);
	temp	= rttySettings -> value ("rttyNbitsTrigger",
	                               rttyNbitsTrigger -> currentText ()).
	                                                       toString ();
	k	= rttyNbitsTrigger -> findText (temp);
	if (k != -1)
	   rttyNbitsTrigger	-> setCurrentIndex (k);
	temp	= s -> value ("rttyStopbitsTrigger",
	                               rttyStopbitsTrigger -> currentText ()).
	                                                       toString ();
	k	= rttyStopbitsTrigger -> findText (temp);
	if (k != -1)
	   rttyStopbitsTrigger	-> setCurrentIndex (k);
	temp	= s -> value ("rttyReverseTrigger",
	                               rttyReverseTrigger -> currentText ()).
	                                                       toString ();
	k	= rttyReverseTrigger -> findText (temp);
	if (k != -1)
	   rttyReverseTrigger	-> setCurrentIndex (k);
	temp	= s -> value ("rttyAfconTrigger",
	                               rttyAfconTrigger -> currentText ()).
	                                                       toString ();
	k	= rttyAfconTrigger -> findText (temp);
	if (k != -1)
	   rttyAfconTrigger	-> setCurrentIndex (k);
	s	-> endGroup ();
}

