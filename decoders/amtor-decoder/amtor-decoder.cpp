#
/*
 *    Copyright (C) 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the swradio-8
 *
 *    swradio-8 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    swradio-8 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with swradio-8; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	"amtor-decoder.h"
#include	"fir-filters.h"
#include	"utilities.h"
#include	<QSettings>

CharMap CCIR_476_ITA4 [34] = {
{'A', '-',  0x71},
{'B', '?',  0x27},
{'C', ':',  0x5C},
{'D', '$',  0x65},
{'E', '3',  0x35},
{'F', '!',  0x6C},
{'G', '&',  0x56},
{'H', '#',  0x4B},
{'I', '8',  0x59},
{'J', '\\', 0x74},
{'K', '(',  0x3C},
{'L', ')',  0x53},
{'M', '.',  0x4E},
{'N', ',',  0x4D},
{'O', '9',  0x47},
{'P', '0',  0x5A},
{'Q', '1',  0x3A},
{'R', '4',  0x55},
{'S', '\'', 0x69},
{'T', '5',  0x17},
{'U', '7',  0x39},
{'V', '=',  0x1E},
{'W', '2',  0x72},
{'X', '/',  0x2E},
{'Y', '6',  0x6A},
{'Z', '+',  0x63},
{015, 015,  0x0F},		
{012, 012,  0x1B},
{040, 040 , 0x2D},		/* letter shift		*/
{040, 040,  0x36},		/* figure shift		*/
{040, 040,  0x1D},		/* space		*/
{040, 040,  0x33},		/* phasing 2 (Beta)	*/
{040, 040,  0x78},		/* phasing 1 (Alpha)	*/
{040, 040,  0x00}
};
/*
 *	The local states of the amtor machine
 */
#define	AMTOR_X0	0100
#define	AMTOR_X1	0101
#define	AMTOR_X2	0102
#define	AMTOR_X3	0103
#define	AMTOR_X4	0104
#define	AMTOR_X5	0105
#define	AMTOR_X6	0106
#define	AMTOR_X7	0107

#define	AMTOR_ALPHA	0x78
#define	AMTOR_BETA	0x33
#define	AMTOR_CR	0x0F
#define	AMTOR_LF	0x1B
#define	AMTOR_DIGIT	0x36
#define	AMTOR_LETTER	0x2D
#define	AMTOR_SCRAPPED	0xFF

#define	AMTOR_IF	000
//
//
	amtorDecoder::amtorDecoder (int32_t rate,
	                            RingBuffer<std::complex<float> > *b,
	                            QSettings *s):
	                                virtualDecoder (rate,  b),
	                                localShifter (rate) {
	amtorSettings		= s;
	theRate			= rate;
	myFrame			= new QFrame;
	setupUi (myFrame);
	myFrame	-> show ();
	setup_amtorDecoder	();
}

	amtorDecoder::~amtorDecoder		(void) {
	amtorSettings	-> beginGroup ("amtorDecoder");
	amtorSettings	-> setValue ("afc_on",
	                              amtorAfconButton -> currentText ());
	amtorSettings	-> setValue ("reverse",
	                              amtorReverseButton -> currentText ()); 
	amtorSettings	-> setValue ("fecError",
	                               amtorFecErrorButton -> currentText ());
	amtorSettings	-> setValue ("message",
	                              amtorMessage -> currentText ());
	amtorSettings	-> endGroup ();
	myFrame		-> hide ();
	delete	LPM_Filter;
	delete	amtorFilter;
	delete	myFrame;
}

void	amtorDecoder::setup_amtorDecoder (void) {
float	H;
QString	temp;
int16_t	k;

	amtorIF		= AMTOR_IF;
	amtorShift	= 170;
	amtorBaudrate	= 100;
	amtorAfcon	= false;
/*
 *	filtering on the zeroed IF is only low pass filtering
 */
	H			= 0.5 * amtorShift + amtorBaudrate;
        LPM_Filter		= new LowPassFIR (35,
	                                          (int)H, theRate);
	amtorBitLen		= (uint16_t)(theRate / amtorBaudrate + 0.5);
	amtorFilter		= new average (amtorBitLen);
//
//	set everything ready
//
	amtorBitclk		= 0.0;
	amtorPhaseacc		= 0;
	fragmentCount		= 0;
	amtorOldz		= std::complex<float> (0, 0);
	amtorState		= AMTOR_X0;
	amtorDecimator		= 0;
	amtorOldFragment	= 0;
	amtorShiftReg		= 0;
	amtorBitCount		= 0;
	amtorLettershift	= 1;
	CycleCount		= 0;
	amtor_clrText ();
	amtor_showCorrection (amtorIF);
	setDetectorMarker    (amtorIF);
	amtorAfcon		= false;
	showAlways		= true;
//
//	settings from previous invocations
	amtorSettings	-> beginGroup ("amtorDecoder");
	temp	= amtorSettings -> value ("afc_con", 
	                         amtorAfconButton -> currentText ()). toString ();
	k = amtorAfconButton	-> findText (temp);
	if (k != -1)
	   amtorAfconButton	-> setCurrentIndex (k);
	set_amtorAfcon		(amtorAfconButton -> currentText ());

	temp	= amtorSettings -> value ("reverse",
	                          amtorReverseButton -> currentText ()). toString ();
	k = amtorReverseButton	-> findText (temp);
	if (k != -1)
	   amtorReverseButton -> setCurrentIndex (k);
	set_amtorReverse	(amtorReverseButton -> currentText ());

	temp	= amtorSettings -> value ("fecError",
	                          amtorFecErrorButton -> currentText ()). toString ();
	k = amtorFecErrorButton	-> findText (temp);
	if (k != -1)
	   amtorFecErrorButton	-> setCurrentIndex (k);
	set_amtorFecError	(amtorFecErrorButton -> currentText ());

	temp	= amtorSettings -> value ("message",
	                          amtorMessage -> currentText ()). toString ();
	k = amtorMessage	-> findText (temp);
	if (k != -1)
	   amtorMessage	-> setCurrentIndex (k);
	set_amtorMessage	(amtorMessage -> currentText ());
	amtorSettings	-> endGroup ();

	connect (amtorAfconButton, SIGNAL (activated (const QString &)),
	         this, SLOT (set_amtorAfcon (const QString &)));
	connect (amtorReverseButton, SIGNAL (activated (const QString &)),
	         this, SLOT (set_amtorReverse (const QString &)));
	connect (amtorFecErrorButton, SIGNAL (activated (const QString &)),
	         this, SLOT (set_amtorFecError (const QString &)));
	connect (amtorMessage, SIGNAL (activated (const QString &)),
	         this, SLOT (set_amtorMessage (const QString &)));
	amtorTextstring		= QString ();
}

void	amtorDecoder::process (std::complex<float> z) {
float	power;
float	f;
float	bf;

	audioOut -> putDataIntoBuffer (&z, 1);
	if (++CycleCount > theRate / 2) {
	   amtor_showStrength   (amtorStrength);
	   amtor_showCorrection (amtorIF);
	   setDetectorMarker	(amtorIF);
	   CycleCount		= 0;
	   audioAvailable (CycleCount, theRate);
	}

//	mix the sample with the LO and filter out unwanted products
	z		= localShifter. do_shift (z, amtorIF);
	z		= LPM_Filter -> Pass (z);

	power	= norm (z);
	amtorStrength = power > 0 ? get_db (power, 2048) : 0;
/*
 *	slice and filter
 */
	f		= arg (z * conj (amtorOldz)) * theRate / (2 * M_PI);
	amtorOldz	= z;
	f		= amtorFilter -> filter (f);
	bf		= amtorReversed? (f < 0.0) : (f > 0.0);

	if (++ amtorDecimator >= amtorBitLen / 10) {
	   addbitfragment (bf ? 1 : 0);
	   if (amtorAfcon) 
	      amtorIF += correctIF (f);
	   amtorDecimator = 0;
	}
}
/*
 *	according to the theory, f should equal rtty_shift / 2 when
 *	amtorIF is in the middle. Check and adjust. The 256 is
 *	still experimental
 */
float	amtorDecoder::correctIF (float f) {
	if (f > 0.0)
	   f = f - amtorShift / 2;
	else
	   f = f + amtorShift / 2;
	if (fabs (f) < amtorShift / 2) 
	   return  f / 256;
	return 0;
}
/*
 *	In this try, we make use of the fact that we
 *	deal with a continuous stream of samples. Each bit
 *	is exactly 10 msec long.
 *	So, as with psk, we try to locate the beginning
 *	of the bits and determine the bit itself by the 
 *	middle of it. We call addbitfragment with a rate
 *	of 10 times per bit, i.e. 100 Hz
 */
void	amtorDecoder::addbitfragment (int8_t bf) {
	if (bf != amtorOldFragment) {
	   fragmentCount = 5;
	   amtorOldFragment = bf;
	   return;
	}

	if (++fragmentCount >= 10) {
	   addfullBit (bf);
	   fragmentCount = 0;
	}
}
/*
 *	once we have a bit (or better, we think we have one)
 *	we add it to the current "word" which is kept in 
 *	"amtorShiftReg", while the number of bits is
 *	kept in "amtorBitCount"
 */	
void	amtorDecoder::addfullBit (int8_t bit) {
	amtorShiftReg <<= 1;
	amtorShiftReg |= (bit & 01);

	if (++amtorBitCount >= 7) {
	   if (validate (amtorShiftReg, 4)) {
	      decoder (amtorShiftReg);
	      amtorBitCount	= 0;
	      amtorShiftReg	= 0;
	   }
	   else {
	      amtorBitCount --;
	      amtorShiftReg &= 077;
	   }
	}
}
/*
 *	whether or not the bits form a word in the
 *	characterset, we don't know
 */
bool	amtorDecoder::validate (uint16_t bits, int8_t cnt) {
int	i;
int	No1	= 0;

	for (i = 0; i < 7; i ++) {
	   if (bits & 01) No1 ++;
	   bits >>= 01;
	}

	return (No1 == cnt);
}
/*
 *	In decoder we process the input such that the raw
 *	characterstream that forms the messages remains
 *	and is sent to the character handler
 *	I.e., all synch data is processed and most
 *	of it is removed
 */
void	amtorDecoder::decoder (int val) {

	switch (amtorState) {
	   case AMTOR_X0:		/* waiting for ALPHA	*/
	      if (val == AMTOR_ALPHA)
	         amtorState = AMTOR_X1;
	      if (val == AMTOR_BETA)
	         amtorState = AMTOR_X2;
	      break;

	   case AMTOR_X1:		/* waiting for Beta or data	*/
	      if (val == AMTOR_BETA)
	         amtorState = AMTOR_X2;
	      else
	      {  initMessage ();
	         addBytetoMessage (val);
	         amtorState = AMTOR_X3;
	      }
	      break;

	   case AMTOR_X2:		/* waiting for Alfa	*/
	      if (val == AMTOR_ALPHA)
	         amtorState = AMTOR_X1;
	      break;

	   case AMTOR_X3:		/* the message		*/
	      if (val == AMTOR_ALPHA) 	/* might be the end	*/
	         amtorState = AMTOR_X4;
	      else
	      if (val == AMTOR_BETA)
	         amtorState = AMTOR_X5;
	      addBytetoMessage (val);
	      break;

	   case AMTOR_X4:		/* we got a possible end */
	      if (val == AMTOR_BETA)
	         amtorState = AMTOR_X6;
	      else
	         amtorState = AMTOR_X3;
	      addBytetoMessage (val);
	      break;

	   case AMTOR_X5:		/* we got a possible end */
	      if (val == AMTOR_ALPHA)
	         amtorState = AMTOR_X6;
	      else
	         amtorState = AMTOR_X3;
	      addBytetoMessage (val);
	      break;

	   case AMTOR_X6:
	      flushBuffer ();
	      amtorState = AMTOR_X0;
	      break;

	   default:;			/* does not happen	*/
	}
}
/*
 *	The raw data forming the characters are handled
 *	by the addBytetoMessage and byteinfo will be
 *	passed on to HandleChar
 */
void	amtorDecoder::addBytetoMessage (int val) {

	if ((amtorQueue [amtorQP] == AMTOR_ALPHA) ||
	    (amtorQueue [amtorQP] == AMTOR_BETA)) {	/* just skip	*/
	   amtorQP = (amtorQP + 1) % AMTOR_DECODERQUEUE;
	   amtorQueue [(amtorQP + 5) % AMTOR_DECODERQUEUE] = val;
	   return;
	}

	if (amtorQueue [amtorQP] == AMTOR_SCRAPPED) {	/* just skip	*/
	   amtorQP = (amtorQP + 1) % AMTOR_DECODERQUEUE;
	   amtorQueue [(amtorQP + 5) % AMTOR_DECODERQUEUE] = val;
	   return;
	}

	if (amtorQueue [amtorQP] ==
	                amtorQueue [(amtorQP + 5) % AMTOR_DECODERQUEUE]) {
	   HandleChar (amtorQueue [amtorQP]);
	   amtorQueue [(amtorQP + 5) % AMTOR_DECODERQUEUE] = AMTOR_SCRAPPED;
	   amtorQP = (amtorQP + 1) % AMTOR_DECODERQUEUE;
	   amtorQueue [(amtorQP + 5) % AMTOR_DECODERQUEUE] = val;
	   return;
	}
	else {
	   if (amtorFecError)
	      HandleChar ('x');
	   else
	      HandleChar (amtorQueue [amtorQP]);
	   amtorQueue [(amtorQP + 5) % AMTOR_DECODERQUEUE] = AMTOR_SCRAPPED;
	   amtorQP   = (amtorQP + 1) % AMTOR_DECODERQUEUE;
	   amtorQueue [(amtorQP + 5) % AMTOR_DECODERQUEUE] = val;
	   return;
	}
}

void	amtorDecoder::HandleChar (int16_t val) {
int16_t	i;
CharMap	*p = CCIR_476_ITA4;

	if (val == -1) {	/* error, show		*/
	   amtorText ((int)'x');
	   return;
	}

	if (val == AMTOR_LETTER) {
	   amtorLettershift = 1;
	   return;
	}

	if (val == AMTOR_DIGIT) {
	   amtorLettershift = 0;
	   return;
	}

	for (i = 0; p [i]. key != 0; i ++) {
	   if (p [i]. key == val) {
	      if (amtorLettershift)
	         amtorText (p [i]. letter);
	      else
	         amtorText (p [i]. digit);
	      return;
	   }
	}
/*
 *	we shouldn't be here
 */
	amtorText ('?');
}

void	amtorDecoder::amtorText (uint8_t c) {
	if (showAlways) {
	   amtor_addText ((char)c);
	   return;
	}
//	in case we only want to see the official messages, we
//	just wait until we see a "ZCZC" sequence of characters	
	switch (messageState) {
	   case NOTHING:
	      if ((char)c == 'Z') 
	         messageState = STATE_Z;
	      return;

	   case STATE_Z:
	      if ((char)c == 'C')
	         messageState = STATE_ZC;
	      else
	         messageState = NOTHING;
	      return;

	   case STATE_ZC:
	      if ((char)c == 'Z')
	         messageState = STATE_ZCZ;
	      else
	         messageState = NOTHING;
	      return;

	   case STATE_ZCZ:
	      if ((char)c == 'C') {
	         messageState = STATE_ZCZC;
	         amtor_addText ((char)'Z');
	         amtor_addText ((char)'C');
	         amtor_addText ((char)'Z');
	         amtor_addText ((char)'C');
	      }
	      else
	         messageState = NOTHING;
	      return;

	   case STATE_ZCZC:
	      if ((char)c == 'N')
	         messageState = STATE_N;
	      amtor_addText ((char)c);
	      return;

	   case STATE_N:
	      if ((char)c == 'N')
	         messageState = STATE_NN;
	      else
	         messageState = STATE_ZCZC;
	      amtor_addText ((char)c);
	      return;

	   case STATE_NN:
	      if ((char)c == 'N')
	         messageState = STATE_NNN;
	      else
	         messageState = STATE_ZCZC;
	      amtor_addText ((char)c);
	      return;

	   case STATE_NNN:
	      if ((char)c == 'N')
	         messageState = STATE_NNNN;
	      else
	         messageState = STATE_ZCZC;
	      amtor_addText ((char)c);
	      return;

	   case STATE_NNNN:
	      messageState = NOTHING;
	      amtor_addText ((char)c);
	      return;
	}
}

void	amtorDecoder::initMessage () {
int	i;

	for (i = 0; i < AMTOR_DECODERQUEUE; i ++)
	   amtorQueue [i] = AMTOR_BETA;
	amtorQP = 0;
	messageState	= NOTHING;
}

void	amtorDecoder::flushBuffer () {
	amtorText ((int)' ');
	amtorText ((int)' ');
	amtorText ((int)' ');
	amtorText ((int)' ');
}

/*
 *	functions for the amtor decoder
 */
void	amtorDecoder::set_amtorAfcon (const QString &s) {
	amtorAfcon = s == "afc on";
	if (!amtorAfcon) {
	   amtorIF	= AMTOR_IF;
	   amtor_showCorrection (amtorIF);
	   setDetectorMarker	(amtorIF);
	}
}

void	amtorDecoder::set_amtorReverse (const QString &s) {
	amtorReversed = s != "normal";
}

void	amtorDecoder::set_amtorFecError (const QString &s) {
	amtorFecError = s == "strict fec";
}

void	amtorDecoder::set_amtorMessage (const QString &s) {
	showAlways = !(s == "message");
	messageState = NOTHING;
}

void	amtorDecoder::amtor_showStrength (float f) {
	amtorStrengthMeter	-> display (f);
}

void	amtorDecoder::amtor_showCorrection (float f) {
	amtorFreqCorrection	-> display (f);
}

void	amtorDecoder::amtor_clrText () {
	amtorTextstring = QString ("");
	amtortextBox	-> setText (amtorTextstring);
}

void	amtorDecoder::amtor_addText (char c) {
	if (c < ' ') c = ' ';
	amtorTextstring.append (QString (QChar (c)));
	if (amtorTextstring. length () > 70)
	   amtorTextstring. remove (0, 1);
	amtortextBox	-> setText (amtorTextstring);
}

