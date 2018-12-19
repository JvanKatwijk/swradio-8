#
/*
 *    Copyright (C) 2014
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
 */

#include	<QThread>
#include	<QSettings>
#include	<QHBoxLayout>
#include	<QLabel>
#include	<QMessageBox>
#include	<QDir>

#include	"elad-s1.h"	// our header
#include	"elad-worker.h"	// the thread, reading in the data
#include	"elad-loader.h"	// function loader

#include	<stdio.h>
DSPCOMPLEX	makeSample_31bits (uint8_t *);
DSPCOMPLEX	makeSample_30bits (uint8_t *);
DSPCOMPLEX	makeSample_15bits (uint8_t *);

typedef union {
	struct __attribute__((__packed__)) {
		float	i;
		float	q;
		} iqf;
	struct __attribute__((__packed__)) {
		int32_t	i;
		int32_t	q;
		} iq;
	struct __attribute__((__packed__)) {
		uint8_t		i1;
		uint8_t		i2;
		uint8_t		i3;
		uint8_t		i4;
		uint8_t		q1;
		uint8_t		q2;
		uint8_t		q3;
		uint8_t		q4;
		};
} iq_sample;

#define	SCALE_FACTOR_30 1073741824.000
#define	SCALE_FACTOR_29 536970912.000
#define	SCALE_FACTOR_14 16384.000
static inline
int32_t validRate	(int32_t r) {

	if ((r == 192000) || (r == 384000) || (r == 768000))
	   return r;
	return 192000;
}

//	Currently, we do not have lots of settings,
//	it just might change suddenly, but not today
bool	elad_s1::createPluginWindow (int32_t r,
	                             QFrame *myFrame, QSettings *s) {
int16_t	success;

	(void)r;
	this	-> myFrame	= myFrame;
	this	-> eladSettings	= s;
	deviceOK		= false;
	setupUi (myFrame);
	_I_Buffer		= NULL;
	theLoader		= NULL;
	theWorker		= NULL;
	conversionNumber	= 2;
	theRate			=
	             eladSettings -> value ("elad-rate", 192000). toInt ();
	vfoOffset			=
	             eladSettings -> value ("elad-offset", 0). toInt ();
	theRate			= validRate (theRate);
	rateDisplay	-> display (theRate);
	conversionNumber	= theRate == 192000 ? 1:
	                          theRate <= 3072000 ? 2 : 3;
	iqSize			= conversionNumber == 3 ? 4 : 8;

//
//	sometimes problems with dynamic linkage of libusb, it is
//	loaded indirectly through the dll
	if (libusb_init (NULL) < 0) {
	   fprintf (stderr, "libusb problem\n");	// should not happen
	   return false;
	}
	libusb_exit (NULL);
	theLoader	= new eladLoader (theRate, &success);
	if (success != 0) {
	   if (success == -1)
	   QMessageBox::warning (myFrame, tr ("sdr"),
	                         tr ("No success in loading libs\n"));
	   else
	   if (success == -2)
	   QMessageBox::warning (myFrame, tr ("sdr"),
	                         tr ("No success in setting up USB\n"));
	   else
	   if (success == -3)
	   QMessageBox::warning (myFrame, tr ("sdr"),
	                         tr ("No success in FPGA init\n"));
	   else
	   if (success == -4)
	   QMessageBox::warning (myFrame, tr ("sdr"),
	                         tr ("No success in hardware init\n"));
	
	   statusLabel -> setText ("not functioning");
	   delete theLoader;
	   theLoader	= NULL;
	   return false;
	}
//
//	Note (10.10.2014: 
//	It turns out that the elad provides for 32 bit samples
//	packed as bytes
	statusLabel	-> setText ("Loaded");
	deviceOK	= true;
	_I_Buffer	= new RingBuffer<uint8_t>(2 * IQ_SIZE * 32768);
	vfoFrequency	= Khz (94700);
//
//	since localFilter and gainReduced are also used as
//	parameter for the API functions, they are int's rather
//	than bool.
	localFilter	= 0;
	filterText	-> setText ("no filter");
	gainReduced	= 0;
	gainLabel	-> setText ("0");
	connect (hzOffset, SIGNAL (valueChanged (int)),
	         this, SLOT (setOffset (int)));
	connect (gainReduction, SIGNAL (clicked (void)),
	         this, SLOT (setGainReduction (void)));
	connect (filter, SIGNAL (clicked (void)),
	         this, SLOT (setFilter (void)));
	return true;
}
//
//	just a dummy
	rigInterface::~rigInterface	(void) {
}
//
//	... that will allow a decent destructor
	elad_s1::~elad_s1	(void) {
	eladSettings -> setValue ("elad-offset", vfoOffset);
	stopReader ();
	if (_I_Buffer != NULL)
	   delete _I_Buffer;
	if (theLoader != NULL)
	   delete theLoader;
	if (theWorker != NULL)
	   delete theWorker;
}

int32_t	elad_s1::getRate	(void) {
	return theRate;
}

bool	elad_s1::legalFrequency (int32_t f) {
	return Khz (100) < f && f < Mhz (30);
}

int32_t	elad_s1::defaultFrequency	(void) {
	return Khz (14070);
}

void	elad_s1::setVFOFrequency	(int32_t newFrequency) {
int32_t	realFreq = newFrequency - vfoOffset;

	if (theWorker == NULL) {
	   vfoFrequency = newFrequency - vfoOffset;
	   return;
	}

	theWorker -> setVFOFrequency (realFreq);
	vfoFrequency = theWorker -> getVFOFrequency ();
}

int32_t	elad_s1::getVFOFrequency	(void) {
	return vfoFrequency + vfoOffset;
}

bool	elad_s1::restartReader	(void) {
bool	success;

	if ((theWorker != NULL) || !deviceOK)
	   return true;

	_I_Buffer	-> FlushRingBuffer ();
	theWorker	= new eladWorker (theRate,
	                                  vfoFrequency,
	                                  theLoader,
	                                  _I_Buffer,
	                                  &success);
	connect (theWorker, SIGNAL (samplesAvailable (int)),
	         this, SIGNAL (samplesAvailable (int)));
	fprintf (stderr,
	         "restarted eladWorker with %s\n",
	                  success ? "success" : "no success");
	return success;
}

void	elad_s1::stopReader	(void) {
	if ((theWorker == NULL) || !deviceOK)
	   return;

	theWorker	-> stop ();
	while (theWorker -> isRunning ())
	   usleep (100);
	delete theWorker;
	theWorker = NULL;
}

void	elad_s1::exit		(void) {
	stopReader ();
}

bool	elad_s1::isOK		(void) {
	return true;
}

//
//	The brave old getSamples. For the elad
//	reading of the "raw" data (i.e. the bytes!!!), we use
//	the "worker". Here we unpack and make the samples into I/Q samples

DSPCOMPLEX	makeSample_31bits (uint8_t *buf) {
int ii = 0; int qq = 0;
int16_t	i = 0;

	uint8_t q0 = buf [i++];
	uint8_t q1 = buf [i++];
	uint8_t q2 = buf [i++];
	uint8_t q3 = buf [i++];

	uint8_t i0 = buf [i++];
	uint8_t i1 = buf [i++];
	uint8_t i2 = buf [i++];
	uint8_t i3 = buf [i++];

	ii = (i3 << 24) | (i2 << 16) | (i1 << 8) | i0;
	qq = (q3 << 24) | (q2 << 16) | (q1 << 8) | q0;
	return DSPCOMPLEX ((float)qq / SCALE_FACTOR_30,
	                   (float)ii / SCALE_FACTOR_30);
	return DSPCOMPLEX ((float)ii / SCALE_FACTOR_30,
	                   (float)qq / SCALE_FACTOR_30);
}


DSPCOMPLEX	makeSample_30bits (uint8_t *buf) {
int ii = 0; int qq = 0;
int16_t	i = 0;
	uint8_t q0 = buf [i++];
	uint8_t q1 = buf [i++];
	uint8_t q2 = buf [i++];
	uint8_t q3 = buf [i++];

	uint8_t i0 = buf [i++];
	uint8_t i1 = buf [i++];
	uint8_t i2 = buf [i++];
	uint8_t i3 = buf [i++];

	ii = (i3 << 24) | (i2 << 16) | (i1 << 8) | i0;
	qq = (q3 << 24) | (q2 << 16) | (q1 << 8) | q0;
	return DSPCOMPLEX ((float)ii / SCALE_FACTOR_29,
	                   (float)qq / SCALE_FACTOR_29);
	return DSPCOMPLEX ((float)ii / SCALE_FACTOR_29,
	                   (float)qq / SCALE_FACTOR_29);
}
//
DSPCOMPLEX	makeSample_15bits (uint8_t *buf) {
int ii	= 0; int qq = 0;
int16_t	i = 0;

              ii   = (int)((unsigned char)(buf[i++]));
              ii  += (int)((unsigned char)(buf[i++])) << 8;
              qq   = (int)((unsigned char)(buf[i++]));
              qq  += (int)((unsigned char)(buf[i++])) << 8;
              return DSPCOMPLEX ((float)ii / SCALE_FACTOR_14,
	                         (float)ii / SCALE_FACTOR_14);
              return DSPCOMPLEX ((float)qq / SCALE_FACTOR_14,
	                         (float)ii / SCALE_FACTOR_14);
}
 
int32_t	elad_s1::getSamples (DSPCOMPLEX *V, int32_t size, uint8_t Mode) { 
int32_t		amount, i;
uint8_t		buf [iqSize * size];

	if (!deviceOK) 
	   return 0;


	amount = _I_Buffer	-> getDataFromBuffer (buf, iqSize * size);

	for (i = 0; i < amount / iqSize; i ++)  {
	   switch (conversionNumber) {
	      case 1: default:
	         V [i] = makeSample_31bits (&buf [iqSize * i]);
	         break;
	      case 2:
	         V [i] = makeSample_30bits (&buf [iqSize * i]);
	         break;
	      case 3:
	         V [i] = makeSample_15bits (&buf [iqSize * i]);
	         break;
	   }
	   switch (Mode) {
	      default:
	      case IandQ:
	         break;
	      case QandI:
	         V [i] = DSPCOMPLEX (imag (V [i]), real (V [i]));
	         break;
	   }
	}
	return amount / iqSize;
}

int32_t	elad_s1::Samples	(void) {
	if (!deviceOK)
	   return 0;
	return _I_Buffer	-> GetRingBufferReadAvailable () / iqSize;
}
//
//	Although we are getting 30-more bits in, the adc in the
//	elad gives us 14 bits. That + 20 db gain results in app 105 db
//	plus a marge it is app 120 a 130 db, so the bit depth for the scope
//	is 21
int16_t	elad_s1::bitDepth	(void) {
	return theRate <= 3072000 ? 21 : 15;
	return theRate == 192000 ? 31 :
	       theRate <= 3072000 ? 30 : 15;
}

//
void	elad_s1::setOffset	(int k) {
	vfoOffset	= k;
}

void	elad_s1::setGainReduction	(void) {
	gainReduced = gainReduced == 1 ? 0 : 1;
	theLoader -> set_en_ext_io_ATT20 (theLoader -> getHandle (),
	                                     &gainReduced);
	gainLabel -> setText (gainReduced == 1 ? "-20" : "0");
}

void	elad_s1::setFilter	(void) {
	localFilter = localFilter == 1 ? 0 : 1;
	theLoader -> set_en_ext_io_LP30 (theLoader -> getHandle (),
	                                     &localFilter);
	filterText	-> setText (localFilter == 1 ? "30 Mhz" : "no filter");
}

#if QT_VERSION < 0x050000
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(device-elad_s1, elad_s1)
QT_END_NAMESPACE
#endif

