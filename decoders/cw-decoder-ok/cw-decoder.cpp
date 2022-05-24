#
/*
 *    Copyright (C) 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of swradio
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
#include	<QFrame>
#include	<QSettings>
#include	"cw-decoder.h"
#include	"utilities.h"
#include	"slidingfft.h"

#define	USECS_PER_SEC		1000000
#define	MODE_IDLE		0100
#define	MODE_IN_TONE		0200
#define	MODE_AFTER_TONE		0300
#define	MODE_END_OF_LETTER	0400

#define	CW_DOT_REPRESENTATION	'.'
#define	CW_DASH_REPRESENTATION	'_'
#define	CW_IF			0
/*
 * 	The standard morse wordlength (i.e.
 * 	PARIS) is 50 bits, then for a wpm of 1,
 * 	one bit takes 1.2 second or 1.2 x 10^6	microseconds
 */
#define	DOT_MAGIC		1200000

extern const char *const codetable [];

	cwDecoder::cwDecoder (int32_t		inRate,
	                      RingBuffer<std::complex<float>> *buffer,
	                      QSettings *s) :
	                           virtualDecoder (inRate, buffer),
	                           inputBuffer (inRate),
	                           inputConverter (inRate, 2000),
	                           localShifter   (10 * 2000) {
	theRate		= inRate;
	(void)s;
	myFrame         = new QFrame;
	setupUi (myFrame);
	myFrame         -> show (); //
	workingRate     = 2000;
	setup_cwDecoder (workingRate);
	screenwidth     = workingRate / 2;
        x_axis          = new double [screenwidth];
        for (int i = 0; i < screenwidth; i ++)
           x_axis [i] = - screenwidth / 2 + i;
        y_values        = new double [screenwidth];

	fillP		= 0;
        theFilter       = new decimatingFIR (35, screenwidth / 2, workingRate, 8);
        cwViewer       = new waterfallScope (cwScope,
                                             screenwidth, 15);

	newFFT		= new slidingFFT (screenwidth, 0, screenwidth - 1);
	connect (cwViewer, SIGNAL (clickedwithLeft (int)),
	         this, SLOT (handleClick (int)));
}

	cwDecoder::~cwDecoder		(void) {
	delete		SmoothenSamples;
	delete		thresholdFilter;
	delete		spaceFilter;
	delete		cwViewer;
	delete[]	x_axis;
	delete[]	y_values;
	delete		myFrame;
	delete	newFFT;
}

void	cwDecoder::setup_cwDecoder (int32_t rate) {

	CycleCount		= rate / 10;
	cw_IF			= CW_IF;

	SmoothenSamples		= new average (4);
	thresholdFilter		= new average (8);
	spaceFilter		= new average (16);
/*
 *	the filter will be set later on
 */
	cw_BandPassFilter	= NULL;
	cwCurrent		= 0;
	agc_peak		= 0;
	noiseLevel		= 0;
	cwState			= MODE_IDLE;
	cwPreviousState		= MODE_IDLE;

	cw_setFilterDegree (FilterDegree -> value ());
	cw_setSquelchValue (SquelchLevel -> value ());
	cw_setWordsperMinute (WPM -> value ());
	cwTracking	= true;		/* default	*/
	CycleCount	= workingRate / 10;

	connect (SquelchLevel, SIGNAL (valueChanged (int)),
	         this, SLOT (cw_setSquelchValue (int)));
	connect (WPM, SIGNAL (valueChanged (int)),
	         this, SLOT (cw_setWordsperMinute (int)));
	connect (FilterDegree, SIGNAL (valueChanged (int)),
	         this, SLOT (cw_setFilterDegree (int)));
	connect (Tracker, SIGNAL (clicked (void)),
	         this, SLOT (cw_setTracking (void)));
	
	CWrange			= cwDefaultSpeed / 2;
	cwSpeed			= cwDefaultSpeed;
	lowerBoundWPM		= cwDefaultSpeed - CWrange;
	upperBoundWPM		= cwDefaultSpeed + CWrange;
	cwSpeed			= cwDefaultSpeed;	/* initial	*/

	cwDefaultDotLength	= DOT_MAGIC / cwDefaultSpeed;
	cwDotLength		= cwDefaultDotLength;
	cwSpaceLength		= cwDefaultDotLength;
	cwDotLengthMax		= DOT_MAGIC / lowerBoundWPM;
	cwDotLengthMin		= DOT_MAGIC / upperBoundWPM;

	cwDashLength		= 3 * cwDotLength;
	cwDashLengthMax		= 3 * cwDotLengthMax;
	cwDashLengthMin		= 3 * cwDotLengthMin;

	cw_adaptive_threshold	= 2 * cwDefaultDotLength;
	cwNoiseSpike		= cwDotLength / 3;

	cwStartTimestamp	= 0;
	cwEndTimestamp		= 0;

	cwCurrent		= 0;
	agc_peak		= 0;
	noiseLevel		= 0;
	
	memset			(dataBuffer, 0, sizeof (dataBuffer));
	thresholdFilter		-> clear (2 * cwDotLength);
	spaceFilter		-> clear (cwDotLength);
	cw_clrText		();
	cwCharbox	-> setText (QString (" "));
}

void	cwDecoder::cw_setWordsperMinute (int n) {
	cwDefaultSpeed	= n;
	speedAdjust ();
}

void	cwDecoder::cw_setTracking (void) {
	cwTracking = !cwTracking;
	if (cwTracking)
	   Tracker -> setText (QString ("Tracking off"));
	else
	   Tracker -> setText (QString ("Tracking on"));
	speedAdjust ();
}

void	cwDecoder::cw_setSquelchValue (int s) {
	SquelchValue	= s;
}

void	cwDecoder::cw_setFilterDegree (int n) {
	cwFilterDegree	= n;
	if (cw_BandPassFilter != NULL)
	   delete cw_BandPassFilter;
	cw_BandPassFilter	= new bandpassFIR (2 * cwFilterDegree + 1,
	                                           cw_IF - 30,
	                                           cw_IF + 20,
	                                           workingRate);
}

void	cwDecoder::speedAdjust () {
	cwSpeed			= cwDefaultSpeed;	/* init	*/
	CWrange			= cwSpeed / 2;
	lowerBoundWPM		= cwDefaultSpeed - CWrange;
	upperBoundWPM		= cwDefaultSpeed + CWrange;

	cwDefaultDotLength	= DOT_MAGIC / cwDefaultSpeed;
	cwDotLength		= cwDefaultDotLength;
	cw_adaptive_threshold	= 2 * cwDefaultDotLength;

	cwDotLengthMax		= DOT_MAGIC / lowerBoundWPM;
	cwDotLengthMin		= DOT_MAGIC / upperBoundWPM;

	cwDashLength		= 3 * cwDotLength;
	cwDashLengthMax		= 3 * cwDotLengthMax;
	cwDashLengthMin		= 3 * cwDotLengthMin;

	cwNoiseSpike		= cwDotLength / 3;
	cwStartTimestamp	= 0;
	cwEndTimestamp		= 0;

	cwState			= MODE_IDLE;
	cwCurrent		= 0;
	memset			(dataBuffer, 0, sizeof (dataBuffer));
	thresholdFilter		-> clear (2 * cwDotLength);
	cw_clrText	();
	cwCharbox	-> setText (QString (" "));
}

void	cwDecoder::process (std::complex<float> z) {
	audioOut	-> putDataIntoBuffer (&z, 1);
	processBuffer (&z, 1);
	if (++CycleCount > theRate / 10){
	   CycleCount	= 0;
	   cw_showdotLength	(cwDotLength);
	   cw_showspaceLength	(cwSpaceLength);
	   cw_showspeed		(cwSpeed);
	   cw_showagcpeak	(clamp (agc_peak * 1000.0, 0.0, 100.0));
	   cw_shownoiseLevel	(clamp (noiseLevel * 1000.0, 0.0, 100.0));
	   audioAvailable (theRate / 10, theRate);
	   setDetectorMarker	((int)cw_IF);
	}
}

void	cwDecoder::processBuffer (std::complex<float> *buffer,
	                                          int32_t amount) {
int32_t inSize	= inputConverter. bufferSize_in ();
int32_t	outSize	= inputConverter. bufferSize_out () + 10;
std::complex<float>	inBuffer	[inSize];
std::complex<float>	outBuffer	[outSize];
int32_t	i;

	inputBuffer. putDataIntoBuffer (buffer, amount);
	while ((int32_t)(inputBuffer. GetRingBufferReadAvailable ()) > inSize) {
	   inputBuffer.    getDataFromBuffer (inBuffer, inSize);
	   inputConverter. convert_in (inBuffer);
	   amount = inputConverter. hasData ();
	   inputConverter. dataOut (outBuffer, amount);
	   for (i = 0; i < amount; i ++) 
	      processSample (outBuffer [i]);
	}
}

void	cwDecoder::processSample (std::complex<float> z) {
DSPCOMPLEX	ret;
int32_t	lengthOfTone;
int32_t	lengthOfSilence;
int32_t	t;
int i;
char	buffer [4];
std::complex<float>	s;
std::complex<float> res;
std::complex<float>v [2000];
static
std::complex<float> outV [2000];

	s	= cw_BandPassFilter	-> Pass (s);
	s	= localShifter. do_shift (z, cw_IF * 10);
	value	=  abs (s);

	if (theFilter -> Pass (s, &res)) {
	   newFFT -> do_FFT (res, outV);
	   int offs = offset (outV);
static int ddd = 0;
	   ddd ++;
	   if ((ddd > workingRate / 4) && (offs != -100)) {
	      fprintf (stderr, "offset %d (%d)\n", offs, 4 * offs);
	      cw_IF += offs / 2;
	      ddd = 0;
	   }
	   fillP ++;
	   if (fillP >= workingRate / 4) {
              for (i = 0; i < screenwidth; i ++)
                 y_values [i] =
	                abs (outV [(screenwidth / 2 + i) % screenwidth]);
              cwViewer -> display (x_axis, y_values,
                                 amplitudeSlider -> value (), 0, 0);
              fillP     = 0;
	   }
	}
	if (value > agc_peak)
	   agc_peak = decayingAverage (agc_peak, value, 50.0);
	else
	   agc_peak = decayingAverage (agc_peak, value, 500.0);

	currentTime += USECS_PER_SEC / workingRate;
	switch (cwState) {
	   case MODE_IDLE:
	      if ((value > 0.67 * agc_peak) &&
	          (value > SquelchValue * noiseLevel)) { 
/*
 *	we seem to start a new tone
 */
	         cwState		= MODE_IN_TONE;
	         currentTime		= 0;
	         cwCurrent		= 0;
	         cwStartTimestamp	= currentTime;
	         cwPreviousState	= cwState;
	      }
	      else 
	         noiseLevel		= decayingAverage (noiseLevel,
	                                                   value, 500.0);
	      break;				/* just wait	*/

	   case MODE_IN_TONE:
/*
 *	we are/were receiving a tone, either continue
 *	or stop it, depending on some threshold value.
 */
	      if (value > 0.33 * agc_peak)
	         break;			/* just go on	*/
/*
 *	if we are here, the tone has ended
 */
	      cwEndTimestamp	= currentTime;
	      lengthOfTone	= currentTime - cwStartTimestamp;

	      if (lengthOfTone < cwNoiseSpike) {
	         cwState = cwPreviousState;
	         break;
	      }

	      noiseLevel	= decayingAverage (noiseLevel, value, 500.0);

	      if (lengthOfTone <= cw_adaptive_threshold)
	         dataBuffer [cwCurrent ++] = CW_DOT_REPRESENTATION;
	      else
	         dataBuffer [cwCurrent ++] = CW_DASH_REPRESENTATION;

/*
 *	if we gathered too many dots and dashes, we end up
 *	with garbage.
 */
	      if (cwCurrent >= CW_RECEIVE_CAPACITY) {
	         cwCurrent = 0;
	         cwState = MODE_IDLE;
	         break;
	      }

	      dataBuffer [cwCurrent] = 0;
	      cwCharbox	-> setText (QString (dataBuffer));
	      cwState = MODE_AFTER_TONE;

	      if (cwTracking) {
	         t = newThreshold (lengthofPreviousTone, lengthOfTone);
	         if (t > 0) {
	            cw_adaptive_threshold = thresholdFilter -> filter (t);
	            cwDotLength		= cw_adaptive_threshold / 2;
	            cwDashLength	= 3 * cwDotLength;
	            cwSpeed		= DOT_MAGIC / cwDotLength;
	            cwNoiseSpike	= cwDotLength / 3;
	         }
	      }

	      lengthofPreviousTone = lengthOfTone;
	      break;

	   case MODE_AFTER_TONE:
/*
 *	following the end of the tone, we might go back for the
 *	next dash or dot, or we might decide that we reached
 *	the end of the letter
 */
	      if ((value > 0.67 * agc_peak) &&
	          (value > SquelchValue * noiseLevel)) {
	         int t = currentTime - cwEndTimestamp;
	         cwSpaceLength		= spaceFilter -> filter (t);
	         cwState		= MODE_IN_TONE;
	         cwStartTimestamp	= currentTime;
	         cwPreviousState	= cwState;
	         break;
	      }	
// 	no tone, adjust noiselevel and handle silence
	      noiseLevel	= decayingAverage (noiseLevel, value, 500.0);
	      lengthOfSilence	= currentTime - cwEndTimestamp;

	      if ((lengthOfSilence >= 2 * (cwDotLength + cwSpaceLength) / 2)) {
	         lookupToken (dataBuffer, buffer);
	         cwCurrent = 0;
	         cwState = MODE_END_OF_LETTER;
	         cw_addText (buffer [0]);
	      }
//	otherwise, silence too short, do nothing as yet
	      break;
/*
 * 	end_of_letter may be followed by another letter or an end
 * 	of word indication
 */
	   case MODE_END_OF_LETTER:	/* almost MODE_IDLE	*/
	      if ((value > 0.67 * agc_peak) &&
	          (value > SquelchValue * noiseLevel)) {
	         cwState		= MODE_IN_TONE;
	         cwStartTimestamp	= currentTime;
	         cwPreviousState	= cwState;
	      }
	      else {
/*
 *	still silence, look what to do
 */
	         noiseLevel		= decayingAverage (noiseLevel,
	                                                   value, 500.0);
	         lengthOfSilence = currentTime - cwEndTimestamp;
	         if (lengthOfSilence > 4.0 * (cwSpaceLength + cwDotLength) / 2) {
	            cw_addText (' ');	/* word space	*/
	            cwState = MODE_IDLE;
	         }
	      }
	      break;

	   default:
	      break;
	}
}
/*
 * 	check for dot/dash or dash/dot sequence to adapt the
 * 	threshold.
 */
int32_t	cwDecoder::newThreshold (int32_t prev_element, int32_t curr_element) {

	if ((prev_element > 0) && (curr_element > 0)) {
	   if ((curr_element > 2 * prev_element) &&
	       (curr_element < 4 * prev_element))
	      return  getMeanofDotDash (prev_element, curr_element);
/*
 * 	check for dash dot sequence
 */
	   if ((prev_element > 2 * curr_element) &&
	       (prev_element < 4 * curr_element))
	      return getMeanofDotDash (curr_element, prev_element);
	}
	return -1;
}
/*
 * 	if we have a dot/dash sequence, we might adapt
 * 	the adaptive threshold if both dot and dash length
 * 	are reasonable.
 */
int32_t	cwDecoder::getMeanofDotDash (int32_t idot, int32_t idash) {
int32_t	dot, dash;

	if (idot > cwDotLengthMin &&  idot < cwDotLengthMax)
	   dot = idot;
	else
	   dot = cwDotLength;

	if (idash > cwDashLengthMin && idash < cwDashLengthMax)
	   dash = idash;
	else
	   dash = cwDashLength;

	return (dash + dot) / 2;
}

void 	cwDecoder::printChar (char a, char er) {
	if ((a & 0x7f) < 32) {
	   switch (a) {
	      case '\n':		break;
	      case '\r':		return;
	      case 8:			break;
	      case 9:			break;
	      default:			a = ' ';
	   }
	}

	switch (er) {
	   case 0:	printf("%c",a);break;
	   case 1:	printf("\033[01;42m%c\033[m",a); break;
	   case 2:	printf("\033[01;41m%c\033[m",a); break;
	   case 3:	printf("\033[01;43m%c\033[m",a); break;
	   case 4:
	   case 5:
	   case 6:
	   case 7:	printf("\033[2J\033[H<BRK>\n"); break;
	   default:
			break;
	}
}

const char * const codetable[] = {
	"A._",
	"B_...",
	"C_._.",
	"D_..",
	"E.",
	"F.._.",
	"G__.",
	"H....",
	"I..",
	"J.___",
	"K_._",
	"L._..",
	"M__",
	"N_.",
	"O___",
	"P.__.",
	"Q__._",
	"R._.",
	"S...",
	"T_",
	"U.._",
	"V..._",
	"W.__",
	"X_.._",
	"Y_.__",
	"Z__..",
	"0_____",
	"1.____",
	"2..___",
	"3...__",
	"4...._",
	"5.....",
	"6_....",
	"7__...",
	"8___..",
	"9____.",
	".._._._",
	",__..__",
	"?..__..",
	"~"
	};

void	cwDecoder::lookupToken (char *in, char *out) {
int	i;

	for (i = 0; codetable [i][0] != '~'; i ++) {
	   if (strcmp (&codetable [i][1], in) == 0) {
	      out [0] = codetable [i][0];
	      return;
	   }
	}
	out [0] = '*';
}

void	cwDecoder::cw_clrText () {
	cwText = QString ("");
	cwTextbox	-> setText (cwText);
}

void	cwDecoder::cw_addText (char c) {
	if (c < ' ') c = ' ';
	cwText.append (QString (QChar (c)));
	if (cwText. length () > 75)
	   cwText. remove (0, 1);
	cwTextbox	-> setText (cwText);
}

void	cwDecoder::cw_showdotLength (int l) {
	dotLengthdisplay	-> display (l);
}

void	cwDecoder::cw_showspaceLength (int l) {
	spaceLengthdisplay	-> display (l);
}

void	cwDecoder::cw_showagcpeak	(int l) {
	agcPeakdisplay		-> display (l);
}

void	cwDecoder::cw_shownoiseLevel (int l) {
	noiseLeveldisplay	-> display (l);
}

void	cwDecoder::cw_showspeed (int l) {
	actualWPM	-> display (l);
}

void    cwDecoder::cw_adjustFrequency (int f) {
	cw_IF  += f;
	if (cw_IF > workingRate / 2 - workingRate / 20)
	   cw_IF = workingRate / 2 - workingRate / 20;
	if (cw_IF < - workingRate / 2 + workingRate / 20)
	   cw_IF = -workingRate / 2 + workingRate / 20;
	setDetectorMarker (cw_IF);
	
}

void    cwDecoder::handleClick (int a) {
	fprintf (stderr, "adjusting with %d\n", a);
        adjustFrequency (a / 2);
}

#define	MAX_SIZE 20
int	cwDecoder::offset (std::complex<float> *v) {
float avg	= 0;
float max	= 0;
float	supermax	= 0;
int	superIndex	= 0;
	for (int i = 0; i < screenwidth; i ++)
	   avg += abs (v [i]);
	avg /= screenwidth;
	int	index	= screenwidth - 40;
	for (int i = 0; i < MAX_SIZE; i ++)
	   max +=  abs (v [index + i]);

	supermax	= max;
	for (int i = MAX_SIZE; i < 80; i ++) {
	   max -=  abs (v [(index + i - MAX_SIZE) % screenwidth]);
	   max +=  abs (v [(index + i) % screenwidth]);
	   if (max > supermax) {
	      superIndex = (index + i + MAX_SIZE / 2) % screenwidth;
	      supermax = max;
	   }
	}

	if (supermax / MAX_SIZE > 3 * avg)
	   return superIndex > screenwidth - 100 ?
	                            superIndex - screenwidth :
	                                     superIndex;
	else
	   return -100;
}
	      
