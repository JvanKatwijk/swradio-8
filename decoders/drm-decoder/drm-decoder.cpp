#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the drm receiver
 *
 *    drm receiver is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm receiver is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with drm receiver; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
//	
#
#include	<QSettings>
#include	<QFrame>
#include	<QWidget>
#include	<QSettings>
#include	<QMessageBox>
#include	<cstring>
#include	"drm-decoder.h"
#include	"radio.h"
#include	"basics.h"
#include	"frame-processor.h"
#include	"iqdisplay.h"
#include	"eqdisplay.h"
/*
 */
		drmDecoder::drmDecoder (int32_t		inRate,
	                                RingBuffer<std::complex<float> > *audioBuffer,
	                                QSettings *s):
	                                   virtualDecoder (inRate,
	                                                   audioBuffer),
	                                   myFrame (nullptr),
	                                   iqBuffer (32768),
	                                   eqBuffer (32768),
	                                   buffer   (12000),
	                                   Filter_10 (21, 5500, inRate) {

QString	temp;
int16_t	symbs;

	this			-> workingRate	= inRate;
	this			-> audioOut	= audioBuffer;
	(void)s;
        setupUi (&myFrame);
	my_iqDisplay		= new IQDisplay (iqPlotter, 512);
	my_eqDisplay		= new EQDisplay (equalizerDisplay);
        myFrame. show ();
	theRate			= inRate;
	decimatorFlag		= false;
	timeSyncLabel	-> setStyleSheet ("QLabel {background-color:red}");
	facSyncLabel	-> setStyleSheet ("QLabel {background-color:red}");
	sdcSyncLabel	-> setStyleSheet ("QLabel {background-color:red}");
	faadSyncLabel	-> setStyleSheet ("QLabel {background-color:red}");

	

	localOscillator. resize (12000);
	for (int i = 0; i < 12000; i ++)
	   localOscillator [i] =
	         std::complex<float> (cos ((float)i * 2 * M_PI / 12000),
	                              sin ((float)i * 2 * M_PI / 12000));

	channel_1	-> hide ();
	channel_2	-> hide ();
	channel_3	-> hide ();
	channel_4	-> hide ();

	symbs			= 45;
	int8_t windowDepth 	= 1;
	int8_t qam64Roulette 	= 10;
	connect (channel_1, SIGNAL (clicked (void)),
	         this, SLOT (selectChannel_1 (void)));
	connect (channel_2, SIGNAL (clicked (void)),
	         this, SLOT (selectChannel_2 (void)));
	connect (channel_3, SIGNAL (clicked (void)),
	         this, SLOT (selectChannel_3 (void)));
	connect (channel_4, SIGNAL (clicked (void)),
	         this, SLOT (selectChannel_4 (void)));
	theFilter	= &Filter_10;
	running			= true;

	my_frameProcessor	= new frameProcessor (this,
	                                              &buffer,
	                                              &iqBuffer,
	                                              &eqBuffer,
	                                              12000,
	                                              symbs,
	                                              windowDepth,
	                                              qam64Roulette);
	my_frameProcessor	-> start	();
	setDetectorMarker	(0);
	currentPhase		= 0;
	phaseOffset		= 0;
}

	drmDecoder::~drmDecoder		(void) {
	running		= false;
	my_frameProcessor -> stop ();
	while (my_frameProcessor -> isRunning ())
	   usleep (10);
	delete	my_frameProcessor;
}

//	Basically a simple approach. The "frameProcessor" does the
//	work, it will read from the buffer that is filled here
void	drmDecoder::processBuffer (std::complex<float>  *dataIn, int amount) {
}

void	drmDecoder::process (std::complex <float> v) {
std::complex<float> x = theFilter -> Pass (v);
	buffer. putDataIntoBuffer (&x, 1);
}

bool	drmDecoder::haltSignal		(void) {
	return !running;
}
//
void	drmDecoder::show_coarseOffset (float offset) {
	show_int_offset	-> display (offset);
}

void	drmDecoder::show_fineOffset (float offset) {
	show_small_offset -> display (offset);
}

void	drmDecoder::show_angle		(float angle) {
	angleDisplay	-> display (angle);
}

void	drmDecoder::show_timeOffset	(float offset) {
	timeOffsetDisplay	-> display (offset);
}

void	drmDecoder::show_timeDelay	(float del) {
	timeDelayDisplay	-> display (del);
}

void	drmDecoder::show_clockOffset	(float o) {
	clockOffsetDisplay	-> display (o);
}

void	drmDecoder::showMessage (QString m) {
	messageLabel -> setText (m);
}

void	drmDecoder::executeTimeSync	(bool f) {
	if (f)
	   timeSyncLabel -> setStyleSheet ("QLabel {background-color:green}");
	else {
	   timeSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	   faadSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	}
}

void	drmDecoder::executeFACSync	(bool f) {
	if (f)
	   facSyncLabel -> setStyleSheet ("QLabel {background-color:green}");
	else {
	   facSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	   faadSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	}
}

void	drmDecoder::executeSDCSync	(bool f) {
	if (f)
	   sdcSyncLabel -> setStyleSheet ("QLabel {background-color:green}");
	else {
	   sdcSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	   faadSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	}
}

void	drmDecoder::show_stationLabel (const QString &s, int stream) {
	switch (stream) {
	   default:
	   case 0:
	      channel_1	-> setText (s);
	      channel_1	-> show ();
	      break;
	   case 1:
	      channel_2	-> setText (s);
	      channel_2	-> show ();
	      break;
	   case 2:
	      channel_3	-> setText (s);
	      channel_3	-> show ();
	      break;
	   case 3:
	      channel_4	-> setText (s);
	      channel_4	-> show ();
	      break;
	}
}

void	drmDecoder::show_timeLabel	(const QString &s) {
//	timedisplayLabel	-> setText (s);
}

void	drmDecoder::execute_showMode		(int l) {
	if (1 <= l && l <= 4)
	   modeIndicator	-> setText (QString (char ('A' + (l - 1))));
}

void	drmDecoder::execute_showSpectrum	(int l) {
	if (0 <= l && l < 6)
	   spectrumIndicator	-> setText (QString (char ('0' + l)));
}

void	drmDecoder::show_channels	(int audio, int data) {
	if (audio > 0)
	   channel_1	-> show ();
	if (audio > 1)
	   channel_2	-> show ();
	else
	   channel_2	-> hide ();
	if (audio == 0)
	   channel_1	-> hide ();

	if (data > 0)
	   channel_3	-> show ();
	if (data > 1)
	   channel_4	-> show ();
	else
	   channel_4	-> hide ();
	if (data == 0)
	   channel_3	-> hide ();
}

void	drmDecoder::show_audioMode	(QString s) {
	audioModelabel	-> setText (s);
}

static std::complex<float> lbuf [4800];
static int fillP	= 0;
void	drmDecoder::sampleOut		(float re, float im) {
std::complex<float> z	= std::complex<float> (re, im);
	lbuf [fillP] = z;
	fillP ++;
	if (fillP >= 4800) {
	   audioOut	-> putDataIntoBuffer (lbuf, 4800);
	   audioAvailable (audioOut -> GetRingBufferReadAvailable (), 48000);
	   fillP = 0;
	}
}

void	drmDecoder::showSNR		(float snr) {
	snrDisplay	-> display (snr);
}

static	bool audio_channel_1	= true;
static	bool audio_channel_2	= false;
static	bool data_channel_1	= true;
static	bool data_channel_2	= false;

void	drmDecoder::selectChannel_1	(void) {
	audio_channel_1 = true;
	audio_channel_2 = false;
}

void	drmDecoder::selectChannel_2	(void) {
	audio_channel_1 = false;
	audio_channel_2 = true;
}

void	drmDecoder::selectChannel_3	(void) {
	data_channel_1	= true;
	data_channel_2	= false;
}

void	drmDecoder::selectChannel_4	(void) {
	data_channel_1	= false;
	data_channel_2	= true;
}

int16_t	drmDecoder::getAudioChannel	(void) {
	return audio_channel_1 ? 0 : 1;
}

int16_t	drmDecoder::getDataChannel	(void) {
int16_t	c = 0;
	if (channel_1 -> isHidden ()) c ++;
	if (channel_2 -> isHidden ()) c ++;
	return data_channel_1 ? 2 - c : 
	          data_channel_2 ? 3 - c: -1;
}

static	int	faadCounter	= 0;
static	int	goodfaad	= 0;
void	drmDecoder::faadSuccess		(bool b) {
	faadCounter ++;
	if (b)
	   faadSyncLabel -> setStyleSheet ("QLabel {background-color:green}");
	else
	   faadSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	if (b)
	   goodfaad ++;
	if (faadCounter > 500) {
	   fprintf (stderr, "faad ratio is %f\n", (float)goodfaad / faadCounter);
	   goodfaad	= 0;
	   faadCounter	= 0;
	}
}

void	drmDecoder::aacData (QString text) {
	aacDataLabel -> setText (text);
}

//	showMOT is triggered by the MOT handler,
//	the GUI may decide to ignore the data sent
//	since data is only sent whenever a data channel is selected
void	drmDecoder::showMOT		(QByteArray data, int subtype) {
	if (!running)
	   return;

	QPixmap p;
	p. loadFromData (data, subtype == 0 ? "GIF" :
	                       subtype == 1 ? "JPEG" :
	                       subtype == 2 ? "BMP" : "PNG");
//	pictureLabel ->  setPixmap (p);
//	pictureLabel ->  show ();
}

void    drmDecoder::set_phaseOffset (int f) {
        phaseOffset += f;
	phaseOffsetDisplay	-> display (phaseOffset);
}


void	drmDecoder::show_country (QString s) {
	countryLabel	-> setText (s);
}

void	drmDecoder::show_programType (QString s) {
	programTypeLabel	-> setText (s);
}

void	drmDecoder::show_time	(QString s) {
	timeLabel		-> setText (s);
}

void    drmDecoder::showIQ  (int amount) {
std::complex<float> Values [amount];
int16_t i;
int16_t t;
double  avg     = 0;
int     scopeWidth      = scopeSlider -> value();

	if (iqBuffer. GetRingBufferReadAvailable () < amount)
	   return;
        t = iqBuffer. getDataFromBuffer (Values, amount);
	for (i = 0; i < t; i ++) {
	   float x = abs (Values [i]);
	   if (!std::isnan (x) && !std::isinf (x))
	   avg += abs (Values [i]);
	}
	avg     /= t;
	my_iqDisplay -> DisplayIQ (Values, scopeWidth / avg);
}

void	drmDecoder::show_eqsymbol	(int amount) {
std::complex<float> line [amount];

	eqBuffer. getDataFromBuffer (line, amount);
	my_eqDisplay	-> show (line, amount);
}

void	drmDecoder::show_datacoding	(QString s) {
	datacoding -> setText (s);
}

void	drmDecoder::show_mer_sdc	(float f) {
	mer_sdc -> display (f);
}

void	drmDecoder::show_mer_msc	(float f) {
	mer_msc	-> display (f);
}

