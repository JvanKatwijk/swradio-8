#
/*
 *    Copyright (C) 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *    
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
#include	<QDateTime>
#include	<QDebug>
#include	<vector>
#include	"fax-decoder.h"
#include	"fax-demodulator.h"
#include	"fax-filenames.h"
#include	"fax-image.h"
#include	"shifter.h"

#define		FAX_IF		0
#define		FILTER_DEFAULT	21

static inline
float	square (float f) {
	return f * f;
}

faxParams _faxParams [] = {
	{"Wefax288", 288, 675, 450, false, 120, 600},
	{"Wefax576", 576, 300, 450, false, 120, 1200},
	{"HamColor", 204, 200, 450, true,  360, 0},
	{"Ham288b",  240, 675, 450, false, 240, 0},
	{"Color240", 288, 200, 450, true,  240, 0},
	{"FAX480",   204, 500, 450, false, 480, 0},
	{NULL,        -1,  -1,  -1, false,  -1, 0}
};

	faxDecoder::faxDecoder (int32_t rate,
	                        RingBuffer<std::complex<float>> *b,
	                        QSettings *s):
	                          virtualDecoder (rate, b),
	                          myFrame (nullptr),
	                          faxContainer (nullptr),
	                          localMixer (rate),
	                          faxLineBuffer (600) {
	theRate			= rate;
	audioOut		= b;
	faxSettings		= s;

	setupUi	(&myFrame);
	myFrame. show ();
	faxContainer. resize (200, 200);
	faxContainer. show ();
	deviation		= 400;
	setup_faxDecoder	(faxSettings);
}

	faxDecoder::~faxDecoder (void) {
	faxSettings	-> beginGroup ("faxDecoder");
	faxSettings	-> setValue ("fax_iocSetter",
	                          iocSetter		-> currentText	());
	faxSettings	-> setValue ("fax_phaseSetter",
	                          phaseSetter		-> currentText	());
	faxSettings	-> endGroup ();
//
	delete	myDemodulator;
	delete	theImage;
	delete	faxLowPass;
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
	faxParams *myfaxParameters 	=
	                       getFaxParams (iocSetter -> currentText ());
	lpm			= myfaxParameters -> lpm;;
	samplesperLine		= theRate * 60 / lpm;
	faxLineBuffer. resize (5 * samplesperLine);
	checkBuffer. resize (samplesperLine);
	fax_IOC			= myfaxParameters -> IOC;
	numberofColums		= M_PI * fax_IOC;
	numberofColums		= 800;
	faxColor		= myfaxParameters -> color ?
	                                    FAX_COLOR: FAX_BLACKWHITE;
	carrier			= FAX_IF;	// default
	phaseInvers		= false;
	lastRow			= 100;	// will change
	theImage		= new faxImage	(numberofColums, lastRow);
	rawData. resize (1024 * 1024);
//
	faxState		= APTSTART;
	savingRequested		= false;
	apt_upCrossings		= 0;
	whiteSide		= false;
	pixelValue		= 0;
	pixelSamples		= 0;
	currentSampleIndex	= 0;
	aptStartFreq		= myfaxParameters -> aptStart;
	aptStopFreq		= myfaxParameters -> aptStop;
	nrLines			= myfaxParameters -> nrLines;
	showState	-> setText (QString ("APTSTART"));
	theImage	-> clear	();
	fax_displayImage	(theImage -> getImage ());
//
//
	connect (colorSetter, SIGNAL (activated (const QString &)),
	         this, SLOT (fax_setColor (const QString &)));
	connect (iocSetter, SIGNAL (activated (const QString &)),
	         this, SLOT (fax_setIOC (const QString &)));
	connect (modeSetter, SIGNAL (activated (const QString &)),
	         this, SLOT (fax_setMode (const QString &)));
	connect (phaseSetter, SIGNAL (activated (const QString &)),
	         this, SLOT (fax_setPhase (const QString &)));
	connect (resetButton, SIGNAL (clicked (void)),
	         this, SLOT (reset (void)));
	connect (saveButton, SIGNAL (clicked (void)),
	         this, SLOT (fax_setsavingonDone (void)));
	connect (cheatButton, SIGNAL (clicked (void)),
	         this, SLOT (fax_setCheat ()));
	h		= s -> value ("fax_modeSetter", "FM"). toString ();
	k		= modeSetter	-> findText (h);
	if (k != -1)
	   modeSetter	-> setCurrentIndex (k);

	h		= s -> value ("fax_phaseSetter", "phase"). toString ();
	k		= phaseSetter	-> findText (h);
	if (k != -1)
	   phaseSetter	-> setCurrentIndex (k);

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

static inline
bool	isBlack (int16_t x) {
	return x < 128;
}

void	faxDecoder::reset	(void) {
	faxState		= APTSTART;
	showState	-> setText (QString ("APTSTART"));
	aptCount		= 0;
	apt_upCrossings		= 0;
	whiteSide		= false;
	savingRequested		= false;
	pixelValue		= 0;
	pixelSamples		= 0;
	currentSampleIndex	= 0;
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
int	baseP;
static	std::complex<float> prevSample = std::complex<float> (0, 0);
static float avgOffset = 0;
static	int cnt	= 0;

	locker. lock ();
	z	= localMixer. do_shift (z, carrier);
        z	= faxLowPass    -> Pass (z);
	float freqOffset = arg (conj (prevSample) * z) * theRate / (2 * M_PI);
	prevSample	= z;
	if (freqOffset > 0)
	   avgOffset 	= 0.001 * freqOffset + 0.999 * avgOffset;
        sampleValue     = myDemodulator -> demodulate (z);
	locker. unlock ();

	cnt ++;
	if (cnt >= samplesperLine) {
	   setDetectorMarker	(carrier);
//	   fprintf (stderr, "offset %f\n", avgOffset);
	   cnt = 0;
	}

        if (phaseInvers)
           sampleValue = 256 - sampleValue;
        if (faxColor == FAX_BLACKWHITE)
           sampleValue = isWhite (sampleValue) ? 255 : 0;

	switch (faxState) {
	   case	APTSTART:
	      saveName		= "";
	      saveButton	-> setText ("Save");
	      bufferP	= 0;
	      showState	-> setText (QString ("APTSTART"));
	      faxLineBuffer [bufferP] = sampleValue;
	      toRead --;
	      bufferP ++;
	      faxState	= WAITING_FOR_START;
	      break;

	   case WAITING_FOR_START:
	      faxLineBuffer [bufferP] = sampleValue;
	      bufferP ++;
	      if (bufferP >= samplesperLine) {
	         if (checkStart (faxLineBuffer, samplesperLine)) {
	            faxState = START_RECOGNIZED;
	            toRead = 6 * samplesperLine;
	            bufferP = 0;
	         }
	         else {
	            for (int i = samplesperLine / 10; i < samplesperLine; i ++)
	               faxLineBuffer [i - samplesperLine / 10] = 
	                                       faxLineBuffer [i];
	            bufferP -= samplesperLine / 10;
//	            fprintf (stderr, "shifted to %d\n", bufferP);
	         }
	      }
	      break;

	   case START_RECOGNIZED:
	      toRead --;
	      if (toRead <= 0) {
	         faxState = WAITING_FOR_PHASE;
	         showState -> setText ("PHASING");
	         alarmCount	= 0;
	         bufferP = 0;
	      }
	      break;

	   case WAITING_FOR_PHASE:
	      faxLineBuffer [bufferP] = sampleValue;
	      bufferP ++;
	      if (bufferP == faxLineBuffer. size () - 2) {
	         faxState = READ_PHASE;
	      }
	      break;

	   case READ_PHASE:
	      faxLineBuffer [bufferP] = sampleValue;
	      baseP = checkPhase (faxLineBuffer, 0, 0.90);
//	      fprintf (stderr, "baseP = %d\n", baseP);
	      if (baseP >= 0) {
	         for (int i = baseP; i < samplesperLine; i ++)
	            faxLineBuffer [i - baseP] = faxLineBuffer [i];
	         bufferP	= samplesperLine - baseP;
	         checkP		= 0;
	         faxState	= SYNCED;
	         showState -> setText ("ON SYNC");
	         stoppers	= 0;
	         linesRecognized = 0;
	      }
	      else {
	         for (int i = samplesperLine; i < faxLineBuffer. size (); i ++)
	            faxLineBuffer [i - samplesperLine] = 
	                                faxLineBuffer [i];
	         bufferP = faxLineBuffer. size () - samplesperLine - 1;
	         alarmCount ++;
//	         fprintf (stderr, "alarmcount %d\n", alarmCount);
	         if (alarmCount >= 20)
	            faxState = APTSTART;
	         else
	            faxState = WAITING_FOR_PHASE;
	      }
	      break;

	   case SYNCED:
	      faxLineBuffer [bufferP] = sampleValue;
	      bufferP = (bufferP + 1) % samplesperLine;
	      if (bufferP == 0) {
	         processBuffer (faxLineBuffer);
	         bufferP = 0;
	         linesRecognized ++;
	         lineNumber	-> display (linesRecognized);
	         if (linesRecognized >= nrLines) {
	            checkBuffer [checkP ++] = sampleValue;
	            if (checkP >= samplesperLine) {
	               if (checkStop (checkBuffer, samplesperLine)) {
	                  stoppers ++;
	                  for (int i = samplesperLine / 10;
	                              i < samplesperLine; i ++)
	                     checkBuffer [i - samplesperLine / 10] =
	                                       checkBuffer [i];
	                  checkP -= samplesperLine / 10;
	               }
	               else {
	                  checkP = 0;
	                  stoppers = 0;
	               }
	               if (stoppers >= 2)
	                  faxState = FAX_DONE;
	            }
	         }
	         if (linesRecognized > nrLines + 50)
	            faxState = FAX_DONE;
	      }
	      break;

	   case FAX_DONE:
	      showState         -> setText (QString ("FAX_DONE"));
              fax_displayImage (theImage        -> getImage (),
                                0,
                                faxColor == FAX_COLOR ? lastRow / 3 : lastRow);

              if (savingRequested) {
	         QFile outFile;
	         saveName	= getSaveName ();
	         outFile. setFileName (saveName);
	         if (!outFile. open (QIODevice::WriteOnly)) {
	            qDebug () << "Cannot open" << saveName;
	            fprintf (stderr, "Could not open %s for writing\n",
	                                     saveName. toLatin1 (). data ());
	         }
	         else {
	            QImage localImage	= theImage -> getImage ();
	            fprintf (stderr, "height = %d\n",
	                                      localImage. height ());
	            localImage. save (&outFile, "PNG");
	            fprintf (stderr, "file %s geschreven\n",
	                                  saveName. toLatin1 (). data ());
	            outFile. close ();
	         }
	      }
	      faxState	= WAITER;
	      toRead	= 10 * samplesperLine;
	      break;

	   case WAITER:
	      toRead --;
	      if (toRead <= 0)
	         faxState = APTSTART;
	      break;

	   default:		// cannot happen
	      faxState = APTSTART;
	}
}

static inline
bool    isValid (QChar c) {
        return c. isLetter () || c. isDigit () || (c == '-');
}

QString	faxDecoder::getSaveName	() {
QString theTime = QDateTime::currentDateTime (). toString ();
QString saveDir	= QDir::homePath ();

	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	QString fileName = "weatherFax-" + saveDir + "-" + theTime + ".png";
	for (int i = 0; i < fileName. length (); i ++)
	   if (!isValid (fileName. at (i)))
	      fileName. replace (i,1, '-');

	return fileName;
}
	
//
//	
bool	faxDecoder::checkStart		(std::vector<int> &buffer,
	                                 int length) {
int	upCrossings = 0;
std::vector<int> crossings (0);

	for (int i = 1; i < length; i ++) {
	   if (realBlack (buffer [i - 1]) &&
	       realWhite (buffer [i])) {
	       crossings . push_back (i);
	       i ++;
	       upCrossings ++;
	   }
	}

//	fprintf (stderr, "crossings %d at %d\n", upCrossings, length);
	if (!inRange (upCrossings, aptStartFreq * length / theRate))
	   return false;

	float 	error	= 0;
	for (int i = 2; i < upCrossings; i ++) {
	   int ff = crossings [i] - crossings [i - 1];
	   error += square (length / upCrossings - ff);
	}

	error	= sqrt (error);
	fprintf (stderr, " %d %d %d %f\n",
	                              aptStartFreq, length, upCrossings,
	                                  error / upCrossings);
	return error / upCrossings < 2.0;
}

int	faxDecoder::checkPhase		(std::vector<int> &buffer,
	                                 int index, float threshold) {
int	baseP	= findPhaseLine (buffer, 0, samplesperLine, threshold);
	(void)index;
	if (baseP < 0)
	   return -1;
//	fprintf (stderr, "initial Phase %d\n", baseP);
	if (!checkPhaseLine (buffer, baseP + samplesperLine, threshold))
	   return -1;
//	fprintf (stderr, "phaseline checked %d\n", baseP);
	if (!checkPhaseLine (buffer, baseP + 2 * samplesperLine, threshold))
	   return -1;
	return baseP;
}

bool	faxDecoder::checkPhaseLine	(std::vector<int> &buffer,
	                                   int index, float threshold) {
int	L1	= 2.5 * samplesperLine / 100;
int	nrWhites	= 0;
int	nrBlacks	= 0;

	for (int i = 0; i < L1; i ++)
	   if (realWhite (buffer [index + i]) &&
	       realWhite (buffer [index + samplesperLine - i - 1]))
	      nrWhites ++;
	for (int i = L1; i < samplesperLine - L1; i ++)
	   if (realBlack (buffer [index + i]))
	      nrBlacks ++;
	if (nrWhites < threshold * L1)
	   return false;
	return nrBlacks > threshold * (0.95 * samplesperLine);
}

int	faxDecoder::findPhaseLine	(std::vector<int> &buffer,
	                                   int ind, int end, float threshold) {
	for (int i = ind; i < end; i ++)
	   if (checkPhaseLine (buffer, i, threshold))
	      return i;
	return -1;
}

int	faxDecoder::nrBlanks		() {}

int	faxDecoder::shiftBuffer		(std::vector<int> &buffer,
	                                              int start, int end) {
	for (int i = start; i < end; i ++)
	   buffer [i - start] = buffer [i];
	return end - start;
}

void	faxDecoder::processBuffer	(std::vector<int> &buffer) {
	for (int i = 0; i < samplesperLine; i ++)
	   addtoImage (buffer [i]);
}

bool	faxDecoder::checkStop		(std::vector<int> &buffer,
	                                               int length) {
int	upCrossings = 0;
std::vector<int> crossings (0);

	for (int i = 1; i < length; i ++) {
	   if (realBlack (buffer [i - 1]) &&
	       realWhite (buffer [i])) {
	       crossings . push_back (i);
	       upCrossings ++;
	   }
	}

	if (!inRange (upCrossings, aptStopFreq * length / theRate))
	   return false;
	float 	error	= 0;
	for (int i = 2; i < upCrossings; i ++) {
	   int ff = crossings [i] - crossings [i - 1];
	   error += square (length / aptStopFreq - ff);
	}

	error	= sqrt (error);
	fprintf (stderr, "%d %f (%f)\n", upCrossings, error, error / upCrossings);
	return error / upCrossings < 2.0;
}

//
//	This function is called from the GUI
void	faxDecoder::fax_setIOC	(const QString &s) {
faxParams	*myfaxParameters	= getFaxParams (s);
	locker. lock ();
	fax_IOC			= myfaxParameters -> IOC;
	numberofColums		= M_PI * fax_IOC;
	numberofColums		= 800;
	aptStartFreq		= myfaxParameters -> aptStart;
	aptStopFreq		= myfaxParameters -> aptStop;
	lpm			= myfaxParameters -> lpm;
	samplesperLine		= theRate * 60 / lpm;
	lastRow			= 100;	// will change
	lastRow			= 100;
	pixelValue		= 0;
	currentSampleIndex	= 0;
	faxState		= APTSTART;
	delete theImage;
	delete theImage;
	theImage             = new faxImage  (numberofColums, lastRow);
	rawData. resize (1024 * 1024);
	theImage     -> clear        ();
	fax_displayImage     (theImage       -> getImage ());
	locker. unlock ();
}

void	faxDecoder::fax_setMode	(const QString &s) {
	faxMode	= s == "am" ? FAX_AM : FAX_FM;
	myDemodulator	-> setMode	(faxMode);
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

void	faxDecoder::fax_displayImage (QImage image) {
QLabel	*imageLabel = new QLabel;

	imageLabel	-> setPixmap (QPixmap::fromImage (image));
	faxContainer. setWidget (imageLabel);
	imageLabel	-> show ();
}

void	faxDecoder::fax_displayImage (QImage image, int x, int y) {
QLabel	*imageLabel = new QLabel;

	imageLabel	-> setPixmap (QPixmap::fromImage (image));
	faxContainer.  setWidget (imageLabel);
	imageLabel	-> show ();
	faxContainer. ensureVisible (x, y);
}

faxParams *faxDecoder::getFaxParams	(QString s) {
int16_t	i;

	for (i = 0; _faxParams [i].Name != NULL; i ++)
	   if (s == _faxParams [i]. Name)
	      return &_faxParams [i];
	return NULL;
}

void    faxDecoder::fax_setsavingonDone (void) {
        savingRequested  = !savingRequested;
	   savingRequested = false;
	if (savingRequested)
	   saveButton -> setText ("saving");
	else
	   saveButton -> setText ("Save");
}

void	faxDecoder::fax_setDeviation	(const QString &s) {
int	newIF;
int	newDeviation;

	if (s == "1900-400") {
	   newIF	= 1900;
	   newDeviation	= 400;
	}
	else {
	   newIF	= 1950;
	   newDeviation	= 450;
	}
	if (deviation == newDeviation)
	   return;

	locker. lock ();
	deviation		= newDeviation;
	carrier			= newIF;
	delete faxLowPass;
	faxLowPass              = new LowPassFIR (FILTER_DEFAULT,
                                                  deviation + 50,
                                                  theRate);
	delete myDemodulator;
        myDemodulator           = new faxDemodulator (FAX_FM,
                                                      theRate,
                                                      deviation);
	locker. unlock ();
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

void	faxDecoder::fax_setCheat () {
	if (faxState == APTSTART) {
	   showState -> setText ("ON SYNC");
	   theImage	-> clear ();
	   bufferP	= 0;
	   lastRow	= 0;
	   linesRecognized	= 0;
	   checkP	= 0;
	   faxState	= SYNCED;
	   stoppers	= 0;
	}
	else {
	   showState	-> setText (QString ("FAX_DONE"));
	   faxState	= FAX_DONE;
	}
}


