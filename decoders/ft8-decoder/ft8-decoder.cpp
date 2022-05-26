#
/*
 *    Copyright (C) 2022
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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
#include	<QFileDialog>
#include	"ft8-decoder.h"
#include	"radio.h"
#include	"pack-handler.h"
#include	<QStandardItemModel>

#include	"radio.h"

#define	LOG_BASE	240

	ft8_Decoder::ft8_Decoder (RadioInterface *mr,
	                          int32_t	inRate,
	                          RingBuffer<std::complex<float>> *buffer,
	                          QSettings		*settings) :
	                                   virtualDecoder (inRate, buffer),
	                                   myFrame (nullptr),
	                                   theCache (30),
	                                   theProcessor (this, 20) {
	ft8Settings	= settings;
	this	-> mr	= mr;
	setupUi (&myFrame);
	myFrame. show ();
	this	-> inputRate	= 12000;
	this	-> outputRate	= 12000;
	this	-> toneLength	= inputRate / 6.25;
	for (int i = 0; i < nrBUFFERS; i ++)
	   theBuffer [i] = new float [2 * toneLength];
	window			= new float [toneLength];
	inputBuffer		= new std::complex<float> [toneLength];
	inBuffer		= new std::complex<float> [toneLength / FRAMES_PER_TONE];
	fftVector_in		= new kiss_fft_cpx [2 * toneLength];
	fftVector_out		= new kiss_fft_cpx [2 * toneLength];
	plan			= kiss_fft_alloc (2 * toneLength, false, 0, 0);

	for (int i = 0; i < toneLength; i ++)  {
	   window [i] =  (0.42 -
	            0.5 * cos (2 * M_PI * (float)i / toneLength) +
	            0.08 * cos (4 * M_PI * (float)i / toneLength));
	}

	memset (is_syncTable, 0, 79);
	for (int i = 0; i < 7; i ++) {
	   is_syncTable [i] = true;
	   is_syncTable [36 + i] = true;
	   is_syncTable [72 + i] = true;
	}
	fillIndex	= 0;
	readIndex	= 0;
	inPointer	= 0;
	lineCounter	= 0;

	filePointer. store (nullptr);
	ft8Settings	-> beginGroup ("ft8Settings");
	int val		= ft8Settings -> value ("maxIterations", 20). toInt ();
	maxIterations. store (val);
	theProcessor. set_maxIterations (val);
	iterationSelector -> setValue (val);
	val		= ft8Settings -> value ("width", 4000). toInt ();
	spectrumWidth. store (val);
	spectrumwidthSelector -> setValue (val);
	ft8Settings	-> endGroup ();
	
	connect (iterationSelector, SIGNAL (valueChanged (int)),
	         this, SLOT (set_maxIterations (int)));
	connect (spectrumwidthSelector, SIGNAL (valueChanged (int)),
	         this, SLOT (set_spectrumWidth (int)));
	connect (filesaveButton, SIGNAL (clicked ()),
	         this, SLOT (handle_filesaveButton ()));
}

	ft8_Decoder::~ft8_Decoder	() {
	for (int i = 0; i < nrBUFFERS; i ++)
	   delete [] theBuffer [i];
	delete [] window;
	delete [] fftVector_in;
	delete [] fftVector_out;
	ft8Settings	-> beginGroup ("ft8Settings");
	ft8Settings	-> setValue ("maxIterations", maxIterations. load ());
	ft8Settings	-> setValue ("width", spectrumWidth. load ());
	ft8Settings	-> endGroup ();
}

void	ft8_Decoder::processBuffer	(std::complex<float> *buffer,
	                                                 int32_t amount) {
	for (int i = 0; i < amount; i ++) 
	   process (buffer [i]);
}

static inline
float abs (kiss_fft_cpx x) {
	return x.r * x.r + x.i * x.i;
}

//
void	ft8_Decoder::process		(std::complex<float> z) {
	inBuffer [inPointer ++] = z;
	if (inPointer < toneLength / FRAMES_PER_TONE)
	   return;

	int content = (FRAMES_PER_TONE - 1) * toneLength / FRAMES_PER_TONE;
	int newAmount = toneLength / FRAMES_PER_TONE;

	inPointer = 0;
//
//	shift the inputBuffer to left
	memmove (inputBuffer, &inputBuffer [newAmount],
	                   content * sizeof (std::complex<float>));
//
//	copy the data that is read, into the buffer
	memcpy (&inputBuffer [content], inBuffer, 
	                    newAmount * sizeof (std::complex<float>));
//
//	prepare for fft
	for (int i = 0; i < toneLength; i ++) {
	   fftVector_in [i]. r = real (inputBuffer [i]) * window [i];
	   fftVector_in [i]. i = imag (inputBuffer [i]) * window [i];
	   fftVector_in [toneLength + i]. r = 0;
	   fftVector_in [toneLength + i]. i = 0;
	}

	kiss_fft (plan, fftVector_in, fftVector_out);
//
//	compute avg strength
	float sum	= 0;
	for (int i = 0; i < 2 * toneLength; i ++)
	   sum += abs (fftVector_out [i]);
	sum /=  (2 * toneLength);

//
//	copy the spectrum into the buffer, negative frequencies first
	for (int i = 0; i < toneLength; i ++) {
	   float x = abs (fftVector_out [i]);
	   if ((x < 0) || (x > 10000000)) {
	      fprintf (stderr, "ellende\n");
	      x = 0;
	   }
	   float x_p = 10 * log10 (0.0001 + x);

	   float y = abs (fftVector_out [toneLength + i]);
	   if ((y < 0) || (y > 10000000)) {
	      fprintf (stderr, "ellende\n");
	      y = 0;
	   }
	   float x_n = 10 * log10 (0.0001 + y);

	   if (x_p < 0) x_p = 0;
	   if (x_n < 0) x_n = 0;
	   theBuffer [fillIndex][toneLength + i] = x_p + LOG_BASE;
	   theBuffer [fillIndex][i] = x_n + LOG_BASE;
	}
	theBuffer [fillIndex] [0] = 10 * log10 (0.0001 + sum);

	fillIndex = (fillIndex + 1) % nrBUFFERS;
	lineCounter ++;
	if (lineCounter < FRAMES_PER_TONE * nrTONES) {
	   return;
	}
//
//	there is a constant relationship between the fill- and read-index
	readIndex	= fillIndex - FRAMES_PER_TONE * nrTONES;
	if (readIndex < 0)
	   readIndex += nrBUFFERS;
	processLine (readIndex);
}
//
//	we apply a costas test on selected elements in the line
//	and if we are (more or less) convinced the data is recorded
//	in the cache
void	ft8_Decoder::processLine (int lineno) {
std::vector<costasValue> cache;

	int	lowBin	= - spectrumWidth. load () / 2 / BINWIDTH + toneLength;
	int	highBin	=   spectrumWidth. load () / 2 / BINWIDTH + toneLength;
	float xxx [highBin - lowBin + 1];
	for (int bin = lowBin + 10; bin < highBin- 10; bin ++) {
	   float tmp = testCostas (lineno, bin);
	   xxx [bin - lowBin] = tmp;
	}

	peakFinder (xxx, lowBin, highBin, cache);

	float log174 [FT8_LDPC_BITS];
	for (auto it: cache) {
//	   int errors	= 0;
	   it. value = decodeTones (lineno, it. index, log174);
	   theProcessor . PassOn (lineCounter, it.value,
	                 (int)(it. index - toneLength) * BINWIDTH, log174);
	}
}

//      with an FFT of 3840 over een band of 12 K, the binwidth = 3.125,
//      i.e. one tone fits takes 2 bins
static
int costasPattern [] = {3, 1, 4, 0, 6, 5, 2};

static
float	getScore	(float *p8, int bin, int tone) {
	int index = bin + 2 * costasPattern [tone];
	float res =  8 * (p8 [index] + p8 [index + 1]);
	for (int i = -2; i < +3; i ++)
	   res -= p8 [index + 2 * i] + p8 [index + 2 * i];;
	return res < 0 ? 0 : res;
}

float	ft8_Decoder::testCostas (int row, int bin) {
float	*p8;
float	score = 0;

	for (int tone = 0; tone < 7; tone ++) {
	   p8	= theBuffer [(row  +  FRAMES_PER_TONE * tone) % nrBUFFERS];
	   score += getScore (p8, bin, tone);
	}

	for (int tone = 0; tone < 7; tone ++) {
	   p8	= theBuffer [(row + FRAMES_PER_TONE * (36 +  tone)) % nrBUFFERS];
	   score += getScore (p8, bin, tone);
	}

	for (int tone = 0; tone < 7; tone ++) {
	   p8	= theBuffer [(row + FRAMES_PER_TONE * (72 + tone)) % nrBUFFERS];
	   score += getScore (p8, bin, tone);
	}

	return score / 21;
}
//
//	we compute the "strength" of the tone bins as relation beetween
//	the bin with the most energy compared to the average of the
//	bins for each tone, and take the average over the tones.
float	ft8_Decoder::decodeTones (int row, int bin, float *log174) {
int	filler	= 0;
float	strength = 0;

	for (int i = 0; i < nrTONES; i ++) {
	   if (is_syncTable [i])
	      continue;

	   int theRow = (row + FRAMES_PER_TONE * i) % nrBUFFERS;
	   float f =  decodeTone (theRow, bin, &log174 [filler]);
//	   strength += f - 10 * log10 (8192);
	   strength += f -  theBuffer [theRow][0];
	   filler += 3;
	}
	normalize_logl (log174);
	return strength / (79 - 21);
}

static inline
float max2 (float a, float b) {
	return (a >= b) ? a : b;
}

static inline
float max4 (float a, float b, float c, float d) {
	return max2 (max2 (a, b), max2 (c, d));
}

static
const uint8_t FT8_Gray_map [8] = {0, 1, 3, 2, 5, 6, 4, 7};
float	ft8_Decoder::decodeTone (int row, int bin, float *logl) {
float s1 [8];
float s2 [8];
float strength	= 0;

	for (int i = 0; i < 8; i ++) {
	   float a1 =        theBuffer [row][bin + 2 * i];
	   float a2 =        theBuffer [row][bin + 2 * i + 1];
	   if ((a1 + a2) / 2 > strength)
	      strength = (a1 + a2) / 2;
	   s1 [i] =  (a1 + a2) / 2;
	}

	for (int i = 0; i < 8; i ++)
	   s2 [i] = s1 [FT8_Gray_map [i]];

	logl [0] = max4 (s2 [4], s2 [5], s2 [6], s2 [7]) -
	                         max4 (s2 [0], s2 [1], s2 [2], s2 [3]);
	logl [1] = max4 (s2 [2], s2 [3], s2 [6], s2 [7]) -
	                         max4 (s2 [0], s2 [1], s2 [4], s2 [5]);
	logl [2] = max4 (s2 [1], s2 [3], s2 [5], s2 [7]) -
	                        max4 (s2 [0], s2 [2], s2 [4], s2 [6]);

	return strength - LOG_BASE;
}

void	ft8_Decoder::normalize_logl (float *log174) {
float sum = 0;
float sum2 = 0;
//	Compute the variance of log174

	for (int i = 0; i < 174; ++i) {
	   sum  += log174 [i];
	   sum2 += log174 [i] * log174 [i];
	}

	float inv_n = 1.0f / FT8_LDPC_BITS;
	float variance = (sum2 - (sum * sum * inv_n)) * inv_n;

//	Normalize log174 distribution and scale it with
//	experimentally found coefficient
	float norm_factor = sqrtf (48.0f / variance);
//	float norm_factor = sqrtf(24.0f / variance);
	for (int i = 0; i < FT8_LDPC_BITS; ++i) {
	   log174 [i] *= norm_factor;
	}
}

//
void	ft8_Decoder::set_maxIterations (int n) {
	maxIterations. store (n);
	theProcessor. set_maxIterations (n);
}

void	ft8_Decoder::showText (const QStringList &resList) {
	model. clear ();
	for (auto res : resList) 
	   model. appendRow (new QStandardItem (res));
	display -> setModel (&model);
}

void	ft8_Decoder::set_spectrumWidth (int n) {
	spectrumWidth. store (n);
}

void	ft8_Decoder::showLine (const QString &s, float strength, int freq,
	                                        const QString &res) {
std::string a = res. toUtf8 (). data ();
	if (!theCache. update (strength, freq, a)) {
	   int32_t f = mr -> tunedFrequency () + freq;
	   QString temp = s + " \t"
	                + QString::number (strength) + "\t"
	                + QString::number (f) + "\t"
	                + res;

	   if (theResults. size () >= 50)
	      theResults. pop_front ();
	   theResults += temp;
	   if (filePointer. load () != nullptr)
	      fprintf (filePointer, "%s\n", temp. toUtf8 (). data ());
	   fprintf (stderr, "%s\n", res. toLatin1 (). data ());
	   showText (theResults);
	}
}

static inline
float	sum (float *V, int index) {
	return V [index - 1] + V [index] + V [index + 1];
}

void	ft8_Decoder::peakFinder (float *V, int begin, int end,
	                                    std::vector<costasValue> &cache) {
costasValue	E;
float workVector [end - begin + 1];

	for (int i = 3; i < end - begin - 4; i ++)
	   workVector [i] = sum (V, i);

	for (int index = 10; index < end - begin - 10; index ++) {
	   if ((1.1 * workVector [index - 5] < workVector [index]) &&
	       (1.1 * workVector [index + 5] < workVector [index]) )
	  {
//	      E. index = index + begin - 1;
//	      E. value = theBuffer [readIndex] [begin + index - 1];
//	      cache. push_back (E);
	      E. index = index + begin;
	      E. value = theBuffer [readIndex] [begin + index];
	      cache. push_back (E);
	      E. index = index + begin + 1;
	      E. value = theBuffer [readIndex] [begin + index + 1];
	      cache. push_back (E);
	      index += 10;
	   }
	}
}

void	ft8_Decoder::handle_filesaveButton	() {
	if (filePointer. load () != nullptr) {
	   fclose (filePointer);
	   filesaveButton -> setText ("save file");
	   filePointer. store (nullptr);
	}

	QString saveName = 
		 QFileDialog::getSaveFileName (nullptr,
                                                 tr ("save file as .."),
                                                 QDir::homePath (),
                                                 tr ("Images (*.txt)"));
	if (saveName == "")
	   return;

	filePointer. store (fopen (saveName. toUtf8 (). data (), "w"));
	if (filePointer. load () == nullptr)
	   return;

	filesaveButton -> setText ("saving");
}
