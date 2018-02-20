#
/*
 *    Copyright (C) 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair programming
 *    
 *    Parts of this fax decoder is derived/copied from
 *    hamfax -- an application for sending and receiving amateur
 *                                                       radio facsimiles
 *    Copyright (C) 2001 Christof Schmitt,
 *    DH1CS <cschmitt@users.sourceforge.net>
 *    All copyrights gratefully acknowledged.
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
#include	<QFile>
#include	<QFileDialog>
#include	<QDir>
#include	<QDebug>
#include	"fax-decoder.h"
#include	"fax-demodulator.h"
#include	"fax-filenames.h"
#include	"fax-image.h"
#include	"shifter.h"

#define		FAX_IF		1000
#define		FILTER_DEFAULT	21

faxParams _faxParams [] = {
	{"Wefax288", 288, 675, 450, false, 120},
	{"Wefax576", 576, 300, 450, false, 120},
	{"HamColor", 204, 200, 450, true,  360},
	{"Ham288b",  240, 675, 450, false, 240},
	{"Color240", 288, 200, 450, true,  240},
	{"FAX480",   204, 500, 450, false, 480},
	{NULL,        -1,  -1,  -1, false,  -1}
};


	faxDecoder::faxDecoder (int32_t rate,
	                        RingBuffer<std::complex<float>> *b,
	                        QSettings *s):
	                          virtualDecoder (rate, b),
	                          localMixer (rate) {
	theRate			= rate;
	audioOut		= b;
	faxSettings		= s;

	myFrame		= new QFrame;
	setupUi			(myFrame);
	myFrame		-> show ();
	faxContainer		= new faxScroller (NULL);
	faxContainer	-> resize (200, 200);
	faxContainer	-> show ();
	setup_faxDecoder	(faxSettings);
	faxCycleCounter		= 0;
}

	faxDecoder::~faxDecoder (void) {
	faxSettings	-> beginGroup ("faxDecoder");
	faxSettings	-> setValue ("fax_iocSetter",
	                          iocSetter		-> currentText	());
	faxSettings	-> setValue ("fax_phaseSetter",
	                          phaseSetter		-> currentText	());
	faxSettings	-> setValue ("fax_deviationSetter",
	                          deviationSetter	-> value	());
	faxSettings	-> endGroup ();
//
	delete	myDemodulator;
	delete	theImage;
	delete	faxLowPass;
	delete	faxContainer;
	delete	nameGenerator;
	delete	myFrame;
}

void	faxDecoder::setup_faxDecoder	(QSettings *s) {
QString h;
int	k;
	faxLowPass		= new LowPassFIR (FILTER_DEFAULT,
	                                          deviation + 50,
	                                          theRate);
	faxAverager		= new average (20);
	myDemodulator		= new faxDemodulator (FAX_FM,
	                                              theRate,
	                                              deviation);
/*
 *	restore some of the settings
 */
	s		-> beginGroup ("faxDecoder");
	h		= s -> value ("fax_iocSetter", "576"). toString ();
	k		= iocSetter	-> findText (h);
	if (k != -1)
	   iocSetter	-> setCurrentIndex (k);

//
//	OK, we know now
faxParams *myfaxParameters 	=  getFaxParams (iocSetter -> currentText ());
	lpmSetter	-> setValue (myfaxParameters -> lpm);
	lpm			= myfaxParameters -> lpm;;
	samplesperLine		= theRate * 60 / lpm;
	fax_IOC			= myfaxParameters -> IOC;
	numberofColums		= M_PI * fax_IOC;
	faxColor		= myfaxParameters -> color ?
	                                    FAX_COLOR: FAX_BLACKWHITE;
	carrier			= FAX_IF;	// default
	phaseInvers		= false;
	lastRow			= 100;	// will change
	theImage		= new faxImage	(numberofColums, lastRow);
	rawData. resize (1024 * 1024);
//
	pointNeeded		= false;
	faxState		= APTSTART;
	aptCount		= 0;
	apt_upCrossings		= 0;
	whiteSide		= false;
	pixelValue		= 0;
	pixelSamples		= 0;
	currentSampleIndex	= 0;
	set_savingRequested  	(false);
	set_autoContinue	(false);
	nameGenerator		= new faxFilenames (0);
//
	startSetter	-> setValue (myfaxParameters -> aptStart);
	stopSetter	-> setValue (myfaxParameters -> aptStop);

	fax_setstartFreq	(startSetter		-> value ());
	fax_setstopFreq		(stopSetter		-> value ());
	fax_setCarrier		(carrierSetter		-> value ());
	fax_setDeviation	(deviationSetter	-> value ());
	setDetectorMarker	(carrier);
	showState	-> setText (QString ("APTSTART"));
	theImage	-> clear	();
	fax_displayImage	(theImage -> getImage ());
//
//
	connect (colorSetter, SIGNAL (activated (const QString &)),
	         this, SLOT (fax_setColor (const QString &)));
	connect (skipSetter, SIGNAL (clicked (void)),
	         this, SLOT (fax_setSkip (void)));
	connect (iocSetter, SIGNAL (activated (const QString &)),
	         this, SLOT (fax_setIOC (const QString &)));
	connect (modeSetter, SIGNAL (activated (const QString &)),
	         this, SLOT (fax_setMode (const QString &)));
	connect (startSetter, SIGNAL (valueChanged (int)),
	         this, SLOT (fax_setstartFreq (int)));
	connect (stopSetter, SIGNAL (valueChanged (int)),
	         this, SLOT (fax_setstopFreq (int)));
	connect (carrierSetter, SIGNAL (valueChanged (int)),
	         this, SLOT (fax_setCarrier (int)));
	connect (deviationSetter, SIGNAL (valueChanged (int)),
	         this, SLOT (fax_setDeviation (int)));
	connect (lpmSetter, SIGNAL (valueChanged (int)),
	         this, SLOT (fax_setLPM (int)));
	connect (phaseSetter, SIGNAL (activated (const QString &)),
	         this, SLOT (fax_setPhase (const QString &)));
	connect (resetButton, SIGNAL (clicked (void)),
	         this, SLOT (reset (void)));
	connect (saveButton, SIGNAL (clicked (void)),
	         this, SLOT (fax_setsavingonDone (void)));
	connect (faxContainer, SIGNAL (fax_Clicked (int, int)),
	         this, SLOT (fax_setClicked (int, int)));
	connect (autoContinueButton, SIGNAL (clicked (void)),
	         this, SLOT (fax_setautoContinue (void)));
	h		= s -> value ("fax_modeSetter", "FM"). toString ();
	k		= modeSetter	-> findText (h);
	if (k != -1)
	   modeSetter	-> setCurrentIndex (k);

	h		= s -> value ("fax_phaseSetter", "phase"). toString ();
	k		= phaseSetter	-> findText (h);
	if (k != -1)
	   phaseSetter	-> setCurrentIndex (k);

	k		= s -> value ("fax_deviationSetter", 400). toInt ();
	deviationSetter	-> setValue (k);
	s		-> endGroup ();
}

//////////////////////////////////////////////
static inline
bool	realWhite (int16_t x) {
	return x > 229;
}

static inline
bool	realBlack (int16_t x) {
	return x < 25;
}

static inline
bool	isWhite (int16_t x) {
	return x > 128;
}

void	faxDecoder::reset	(void) {
	faxState		= APTSTART;
	showState	-> setText (QString ("APTSTART"));
	aptCount		= 0;
	apt_upCrossings		= 0;
	whiteSide		= false;
	pixelValue		= 0;
	pixelSamples		= 0;
	currentSampleIndex	= 0;
	currPhaseLength		= 0;
	whiteLength		= 0;
	delayCounter		= 0;
	rawData. resize (1024 * 1024);
	theImage	-> clear	();
	fax_displayImage	(theImage	-> getImage ());
}
//
//	as always, we "process" one sample at the time.
//
static
bool	inRange (int16_t x, int16_t y) {
	return y - 2 <= x && x <= y + 2;
}

#define	we_are_Black	(!whiteSide)

void	faxDecoder::process	(std::complex<float> z) {
int16_t	sampleValue;

	audioOut	-> putDataIntoBuffer (&z, 1);
	if (++faxCycleCounter >= theRate / 10) {
	   faxCycleCounter = 0;
	   setDetectorMarker (carrier);
	   audioAvailable (theRate / 10, theRate);
	}
//
//	When we are in autoContinue mode, we just wait at least for
//	30 seconds after detecting the FAX_DONE before resetting
	if (faxState == FAX_DONE) { 
	   if (autoContinue && delayCounter ++ > theRate * 30) {
	      reset ();	// will a.o set the state
	      delayCounter = 0;
	   } else
	      return;
	}

	z		= localMixer. do_shift (z, carrier);
	z		= faxLowPass	-> Pass (z);
	sampleValue	= myDemodulator -> demodulate (z);

	if (phaseInvers)
	   sampleValue = 256 - sampleValue;
	if (faxColor == FAX_BLACKWHITE)
	   sampleValue = isWhite (sampleValue) ? 255 : 0;
	fax_oldz	= z;
	currentValue	= sampleValue;

//	We count the number of times the decoded values
//	goes up from black to white. If in a period of half a second
//	(i.e. we had theRate / 2 samples), the number
//	of upCrossings is aptStartFreq / 2, we have a start
//	if the number is aptStopFreq  and we are
//	not DONE or APT_START then we finish the transmission

	if (we_are_Black && realWhite (sampleValue)) {
	   whiteSide = true;
	   apt_upCrossings ++;
	}
	else
	if (whiteSide && realBlack (sampleValue))
	   whiteSide = false;

//	if we have samples for about half a second  of samples,
//	check the frequency
//	to see whether we are ready for phasing or for exit
	if (++aptCount >= theRate / 2) {
	   int16_t freq		= 2 * apt_upCrossings;
	   aptFreq  		-> display (freq);
	   aptCount		= 0;
	   apt_upCrossings	= 0;

	   if ((faxState == APTSTART) &&
	                         inRange (freq, aptStartFreq)) {
	      faxState = FAX_PHASING;
//	      carrier	-= freq - aptStartFreq;
	      initPhasing ();
	      return;
	   }

	   if (((faxState & (FAX_PHASING | FAX_IMAGING)) != 0) &&
	                         inRange (freq, aptStopFreq)) {
	      rawData. resize (currentSampleIndex);
	      faxState		= FAX_DONE;	
	      showState		-> setText (QString ("Done"));
	      fax_displayImage (theImage	-> getImage (),
	                        0,
	                        faxColor == FAX_COLOR ? lastRow / 3 : lastRow);
	      if (savingRequested || autoContinue) {
	         fax_saveFile (theImage -> getImage (), autoContinue);
	         set_savingRequested (false);
	      }
	      return;
	   }
//
//	otherwise: fall through
	}

	if (faxState == FAX_PHASING)
	   addtoPhasing (sampleValue);

	if ((faxState == FAX_PHASING) || (faxState == FAX_IMAGING))
	   if (lpm > 0)		// just a precaution
	      addtoImage (sampleValue);
}

void	faxDecoder::initPhasing (void) {
	faxState	= FAX_PHASING;
	lpm		= 0;
	f_lpm		= 0;
	lpmSum		= 0;
	currPhaseLength	= 0;
	whiteLength	= 0;
	whiteSide	= currentValue >= 128 ? true : false;
	phaseLines	= 0;
	showState	-> setText (QString ("Phasing"));
	theImage	-> clear	();
}

/*
 *	Note that +400 Hz is white and the - 400 Hz is black.
 *	Phasing lines consists of 2.5% white at the beginning,
 *	95 % black and again 2.5 % white at the end (or inverted).
 *	In normal cases we try to count the length between the
 *	white-black transitions. If the line has a reasonable
 *	amount of white (4.8 % - 5.2 %) and the length fits in
 *	the range of 60 -- 360 lpm (plus some tolerance) it is considered
 *	a valid phasing line. Then the start of a line and the
 *	lpm is calculated
 */

bool	faxDecoder::weFoundaValidPhaseline (void) {
double bound1 = 0.048 * currPhaseLength;
double bound2 = 0.052 * currPhaseLength;

//	first check the ratio  of white
	if ((whiteLength < bound1) || (whiteLength > bound2))
	   return false;
//	then, the LPM should be not too large
	if ((double)currPhaseLength < 0.15 * theRate)
	   return false;		// too high LPM
//	... or too small
	if ((double)currPhaseLength > 1.1 * theRate)
	   return false;
//
//	everything OK, so
	return  true;
}


void	faxDecoder::addtoPhasing (int16_t x) {
	x	= faxAverager	-> filter (x);
	currPhaseLength ++;

	if (isWhite (x))
	   whiteLength ++;
//
//	Is this a start of segment of white ?
	if (!whiteSide && realWhite (x)) {
	   whiteSide = true;
	   return;
	}

//	are we getting a switch to black, then start working
	if (whiteSide && realBlack (x)) {
	   whiteSide = false;
//
//	We look for - at least - 5 successive phaselines
//
	   if (weFoundaValidPhaseline ()) {
	      lpmSum		+= 60.0 * theRate / currPhaseLength;
	      currPhaseLength	= 0;
	      whiteLength	= 0;
	      phaseLines ++;
	   }
	   else			// we might already be imaging
	   if (phaseLines >= 10) {
	      lpm		= lpmSum / phaseLines;
	      f_lpm		= lpmSum / phaseLines;
	      if (lpm == 0)	// should not happen
	         lpm = 120;	// just a default
	      samplesperLine		= theRate * 60 / lpm;
	      currentSampleIndex	= (int)(1.0 * 60.0 / lpm * theRate);
//	      currentSampleIndex	= (int)(1.025 * 60.0 / lpm * theRate);
	      faxState		= FAX_IMAGING;
	      showState		-> setText (QString ("Image"));
	      double pos	= fmod ((double)currentSampleIndex,
	                                 (double)theRate * 60 / lpm);
//	      pos		/= theRate * 60.0 / lpm;
	      currentColumn		= (int)(pos * numberofColums);
	      pixelValue	= 0;
	      lastRow		= 100;
	      phaseLines	= 0;
	      currPhaseLength	= 0;
	      whiteLength	= 0;
	   }
	   else	{		// check for garbage, and clean up
//	   if (currPhaseLength > 5 * theRate) {
	      currPhaseLength	= 0;
	      whiteLength	= 0;
	      phaseLines	= 0;
	      lpmSum		= 0;
	   }
	}
}
/*
 *	we add the demodulated value (x) to the
 *	current pixel, so first we find out
 *	the position of the current pixel
 *
 *	Number of samples per line =
 *		theRate  * 60.0 / lpm
 *	samplenumber in currentline =
 *		fmod (currentSampleIndex, theRate * 60.0 / lpm)
 *	position of sample in current column =
 *	       samplenumber in current line / number of samples per Line * x
 *
 *
 *	What we really want is to distribute the value of the sample
 *	over the pixels it belongs to!!!
 */
void	faxDecoder::addtoImage (int16_t x) {
int32_t		samplenum	= currentSampleIndex % samplesperLine;
float		temp		= (float)samplenum / samplesperLine;
int32_t		columnforSample	= floor (temp * numberofColums);
int32_t		currentRow	= (int32_t)
	                             (currentSampleIndex * lpm / 60.0 / theRate);
//
//	First check to see whether there is space to store the sample
	if ((int32_t) (rawData. size ()) <= currentSampleIndex) 
	   rawData. resize (rawData. size () + 1024 * 1024);
	rawData [currentSampleIndex ++]	= x;
//
//	A sample may contribute to more than one pixel. E.g., for a samplerate
//	of 8000, and an IOC of 576, there are 4.42 samples contributing.
//	We therefore look for the partial contribution of the first
//	and the last sample for a pixel
	if (columnforSample == currentColumn) { // still dealing with the same pixel
	   if (temp * numberofColums > currentColumn) {	// partial contribution
	      float part_0, part_1;
	      part_0 = temp * numberofColums - currentColumn; // for this pixel
					// and part_1 is for the next one
	      part_1 = (1 - (temp * numberofColums - currentColumn));
	      pixelValue	+= part_0 * x;
	      pixelSamples 	+= part_0;
	      addSampletoImage (pixelValue / pixelSamples,
	                            currentColumn, currentRow);
	      currentColumn ++;
	      pixelValue	= part_1 * x;
	      pixelSamples	= part_1;
	      return;
	   }

	   pixelValue	+= x;
	   pixelSamples	++;
	   return;
	}
//
//	we expect here currentCol > currentColumn
	if (pixelSamples > 0) 	// simple "assertion"
	   addSampletoImage (pixelValue / pixelSamples,
	                         currentColumn, currentRow);

	currentColumn		= columnforSample;
	pixelValue		= x;
	pixelSamples		= 1;
}

void	faxDecoder::addSampletoImage (float val, int32_t col, int32_t row) {
int32_t realRow = faxColor == FAX_COLOR ? row / 3 : row;

	   theImage -> setPixel (col, realRow, val,
	                         faxColor == FAX_COLOR ? row % 3 : 3);
	   if (lastRow != row) {
	      lineNumber  -> display (row);
	      fax_displayImage (theImage -> getImage (), 0, row);
	      lastRow	= row;
	      currentColumn	= 0;
	   }
}
//
void	faxDecoder::correctWidth	(int16_t w) {
	theImage	-> correctWidth (w);
}
//
//	This function is called from the GUI
void	faxDecoder::fax_setIOC	(const QString &s) {
int	numberofSamples;

faxParams	*myfaxParameters	= getFaxParams (s);
	fax_IOC			= myfaxParameters -> IOC;
	numberofColums		= M_PI * fax_IOC;
	startSetter	-> setValue (myfaxParameters -> aptStart);
	stopSetter	-> setValue (myfaxParameters -> aptStop);
	lpmSetter	-> setValue (myfaxParameters -> lpm);
	lpm			= myfaxParameters -> lpm;
	samplesperLine		= theRate * 60 / lpm;
	numberofColums		= M_PI * fax_IOC;
	lastRow			= 100;	// will change
	lastRow			= 100;
	pixelValue		= 0;
	currentSampleIndex	= 0;
	if (faxState != FAX_DONE) {
	   delete theImage;
	   theImage		= new faxImage	(numberofColums, lastRow);
	   rawData. resize (1024 * 1024);
	   theImage	-> clear	();
	   fax_displayImage	(theImage	-> getImage ());
	   return;
	}
/*
 *	apply the changes to the current image
 */
	numberofSamples	= rawData. size ();
	for (int i = 0; i < numberofSamples; i ++) {
	   addtoImage (rawData [i]);
	}
}

void	faxDecoder::fax_setMode	(const QString &s) {
	faxMode	= s == "am" ? FAX_AM : FAX_FM;
	myDemodulator	-> setMode	(faxMode);
}

void	faxDecoder::fax_setDeviation (int m) {
	deviation	= m;
	delete	faxLowPass;
	faxLowPass		= new LowPassFIR (FILTER_DEFAULT,
	                                          deviation + 50,
	                                          theRate);
	myDemodulator	-> setDeviation	(deviation);
}
/*
 *	fax_setSkip is called through a signal
 *	from the GUI
 */
void	faxDecoder::fax_setSkip	(void) {
double	pos;

	set_autoContinue (false);
	switch (faxState) {
	   case APTSTART:
	      faxState		= FAX_PHASING;
	      lpm		= 120;
	      lpmSum		= 0;
	      currPhaseLength	= 0;
	      phaseLines	= 0;
	      whiteLength	= 0;
	      whiteSide		= currentValue >= 128 ? true : false;
	      showState 	-> setText (QString ("Phasing"));
	      currentSampleIndex	= 0;
	      theImage	-> clear	();
	      break;

	   case FAX_PHASING:
	      faxState		= FAX_IMAGING;
	      lpm		= 120;
	      pos		= fmod ((double)currentSampleIndex,
	                                    (double)theRate * 60 / lpm);
	      pos		/= theRate * 60.0 / lpm;
	      currentColumn		= (int) pos * numberofColums;
	      pixelValue	= 0;
	      pixelSamples	= 0;
	      lastRow		= 100;
	      showState		-> setText (QString ("Imaging"));
	      break;

	   case FAX_IMAGING:
	      faxState		= FAX_DONE;	
	      rawData. resize (currentSampleIndex);
	      showState		-> setText (QString ("Done"));
	      fax_displayImage (theImage -> getImage (),
	                        0,
	                        faxColor == FAX_COLOR ? lastRow / 3 : lastRow);
	      if (savingRequested) {
	         fax_saveFile (theImage -> getImage (), autoContinue);
	         set_savingRequested (false);
	      }
	      break;

	   default:		// cannot happen
	   case FAX_DONE:	// reset everything
	      faxState 			= APTSTART;
	      aptCount			= 0;
	      apt_upCrossings		= 0;
	      whiteSide			= false;
	      pixelValue		= 0;
	      pixelSamples		= 0;
	      currentSampleIndex	= 0;
	      theImage	-> clear	();
	      rawData. resize (1024 * 1024);
	      fax_displayImage (theImage	-> getImage (),
	                        0,
	                        faxColor == FAX_COLOR ? lastRow / 3 : lastRow);
	      showState		-> setText (QString ("APTSTART"));
	      set_savingRequested (false);
	      break;
	}
}

/*
 *	Called from both the GUI as well as from elsewhere in
 *	the program.
 */
void	faxDecoder::fax_setLPM	(int f) {
	fax_setLPM ((float)f);
}

void	faxDecoder::fax_setLPM (float f) {
int64_t	numberofSamples;

	if (faxState != FAX_DONE) 	// just ignore
	   return;

	lpm			= f;
	pixelValue		= 0;
	pixelSamples		= 0;
	currentSampleIndex	= 0;
	lastRow			= 100;
	currentColumn		= 0;
	numberofSamples		= rawData. size ();
	for (int i = 0; i < numberofSamples; i ++) {
	   addtoImage (rawData [i]);
	}
}

void	faxDecoder::fax_setPhase (const QString &s) {
	phaseInvers = s == "inverse";
}

void	faxDecoder::fax_setColor (const QString &s) {
	if (s == "BW")
	   faxColor = FAX_BLACKWHITE;
	else
	if (s == "COLOR")
	   faxColor = FAX_COLOR;
	else
	   faxColor = FAX_GRAY;
}

void	faxDecoder::fax_setstartFreq (int m) {
	aptStartFreq	= m;
}

void	faxDecoder::fax_setstopFreq (int m) {
	aptStopFreq	= m;
}

void	faxDecoder::fax_setCarrier (int m) {
	carrier			= m;
	setDetectorMarker	(carrier);
}

void	faxDecoder::fax_setClicked (int x_coord, int y_coord) {
float	lpma;
float	adjustment;
QPoint x (x_coord, y_coord);

	if (pointNeeded) {
	   pointNeeded = false;
	   slant2	= x;
	   lpma = ((float)(slant2. x() - slant1. x())) / (slant1. y () - slant2. y ());
	   lpma	= lpma	/ theImage -> getCols ();
	   adjustment = 1.0 + (faxColor != FAX_BLACKWHITE ? lpma / 3 : lpma);
	   fprintf (stderr, "adjusting lpm with %f\n", adjustment); 
	   fax_setLPM  (adjustment);
	   slantText -> setText(QString (""));
	   return;
	}

	pointNeeded	= true;
	slant1		= x;
	slantText -> setText (QString ("mark a second point"));
}

void	faxDecoder::fax_displayImage (QImage image) {
QLabel	*imageLabel = new QLabel;

	imageLabel	-> setPixmap (QPixmap::fromImage (image));
	faxContainer -> setWidget (imageLabel);
	imageLabel	-> show ();
}

void	faxDecoder::fax_displayImage (QImage image, int x, int y) {
QLabel	*imageLabel = new QLabel;

	imageLabel	-> setPixmap (QPixmap::fromImage (image));
	faxContainer	-> setWidget (imageLabel);
	imageLabel	-> show ();
	faxContainer	-> ensureVisible (x, y);
}

void	faxDecoder::fax_saveFile (QImage image, bool getName) {
QFile	outFile;
QString	file;

	if (getName)
	   file	= nameGenerator	-> newFileName ();
	else
	   file	= QFileDialog::getSaveFileName (myFrame,
	                                        tr ("save file as .."),
	                                        QDir::homePath (),
	                                        tr ("Images (*.jpg)"));
	                               
	outFile. setFileName (file);
	if (!outFile. open (QIODevice::WriteOnly)) {
	   qDebug () << "Cannot open" << file;
	   return;
	}

	image. save (&outFile);
	outFile. close ();
}

void	faxDecoder::fax_setsavingonDone (void) {
	set_savingRequested (!savingRequested);
}

void	faxDecoder::set_savingRequested (bool b) {
	savingRequested = b;
	if (b)
	   saveButton -> setText ("will save");
	else
	   saveButton -> setText ("press for save");
}

void	faxDecoder::fax_setautoContinue (void) {
	set_autoContinue (!autoContinue);
}

void	faxDecoder::set_autoContinue (bool b) {
	autoContinue	= b;
	if (b)
	   autoContinueButton -> setText ("will continue");
	else
	   autoContinueButton -> setText ("press for autoContinue");
}

faxParams *faxDecoder::getFaxParams	(QString s) {
int16_t	i;

	for (i = 0; _faxParams [i].Name != NULL; i ++)
	   if (s == _faxParams [i]. Name)
	      return &_faxParams [i];
	return NULL;
}

