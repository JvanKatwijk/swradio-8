#
/*
 *    Copyright (C) 2010, 2011, 2012, 2013
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
 *
 * 	This particular driver is a very simple wrapper around the
 * 	librtlsdr. The librtlsdr is osmocom software and all rights
 * 	are greatly acknowledged
 *
 *	To keep things simple, we just load the librtlsdr library
 *	and wrap around it
 */

#define READLEN_DEFAULT 8192

#include	<QThread>
#include	"rtlsdr-handler.h"
#include	<stdio.h>
#include	<QSettings>
//
//	The naming of functions for accessing shared libraries
//	differ between Linux and Windows
#ifdef __MINGW32__
#define	GETPROCADDRESS	GetProcAddress
#else
#define	GETPROCADDRESS	dlsym
#endif
//
//	for handling the events in libusb, we need a controlthread
//	whose sole purpose is to process the rtlsdr_read_async function
//	from the lib.

	dll_driver::dll_driver (rtlsdrHandler *d) {
	theStick	= d;
	start ();
}

	dll_driver::~dll_driver (void) {
}


static 
float convTable [] = {
 -128 / 128.0 , -127 / 128.0 , -126 / 128.0 , -125 / 128.0 , -124 / 128.0 , -123 / 128.0 , -122 / 128.0 , -121 / 128.0 , -120 / 128.0 , -119 / 128.0 , -118 / 128.0 , -117 / 128.0 , -116 / 128.0 , -115 / 128.0 , -114 / 128.0 , -113 / 128.0 
, -112 / 128.0 , -111 / 128.0 , -110 / 128.0 , -109 / 128.0 , -108 / 128.0 , -107 / 128.0 , -106 / 128.0 , -105 / 128.0 , -104 / 128.0 , -103 / 128.0 , -102 / 128.0 , -101 / 128.0 , -100 / 128.0 , -99 / 128.0 , -98 / 128.0 , -97 / 128.0 
, -96 / 128.0 , -95 / 128.0 , -94 / 128.0 , -93 / 128.0 , -92 / 128.0 , -91 / 128.0 , -90 / 128.0 , -89 / 128.0 , -88 / 128.0 , -87 / 128.0 , -86 / 128.0 , -85 / 128.0 , -84 / 128.0 , -83 / 128.0 , -82 / 128.0 , -81 / 128.0 
, -80 / 128.0 , -79 / 128.0 , -78 / 128.0 , -77 / 128.0 , -76 / 128.0 , -75 / 128.0 , -74 / 128.0 , -73 / 128.0 , -72 / 128.0 , -71 / 128.0 , -70 / 128.0 , -69 / 128.0 , -68 / 128.0 , -67 / 128.0 , -66 / 128.0 , -65 / 128.0 
, -64 / 128.0 , -63 / 128.0 , -62 / 128.0 , -61 / 128.0 , -60 / 128.0 , -59 / 128.0 , -58 / 128.0 , -57 / 128.0 , -56 / 128.0 , -55 / 128.0 , -54 / 128.0 , -53 / 128.0 , -52 / 128.0 , -51 / 128.0 , -50 / 128.0 , -49 / 128.0 
, -48 / 128.0 , -47 / 128.0 , -46 / 128.0 , -45 / 128.0 , -44 / 128.0 , -43 / 128.0 , -42 / 128.0 , -41 / 128.0 , -40 / 128.0 , -39 / 128.0 , -38 / 128.0 , -37 / 128.0 , -36 / 128.0 , -35 / 128.0 , -34 / 128.0 , -33 / 128.0 
, -32 / 128.0 , -31 / 128.0 , -30 / 128.0 , -29 / 128.0 , -28 / 128.0 , -27 / 128.0 , -26 / 128.0 , -25 / 128.0 , -24 / 128.0 , -23 / 128.0 , -22 / 128.0 , -21 / 128.0 , -20 / 128.0 , -19 / 128.0 , -18 / 128.0 , -17 / 128.0 
, -16 / 128.0 , -15 / 128.0 , -14 / 128.0 , -13 / 128.0 , -12 / 128.0 , -11 / 128.0 , -10 / 128.0 , -9 / 128.0 , -8 / 128.0 , -7 / 128.0 , -6 / 128.0 , -5 / 128.0 , -4 / 128.0 , -3 / 128.0 , -2 / 128.0 , -1 / 128.0 
, 0 / 128.0 , 1 / 128.0 , 2 / 128.0 , 3 / 128.0 , 4 / 128.0 , 5 / 128.0 , 6 / 128.0 , 7 / 128.0 , 8 / 128.0 , 9 / 128.0 , 10 / 128.0 , 11 / 128.0 , 12 / 128.0 , 13 / 128.0 , 14 / 128.0 , 15 / 128.0 
, 16 / 128.0 , 17 / 128.0 , 18 / 128.0 , 19 / 128.0 , 20 / 128.0 , 21 / 128.0 , 22 / 128.0 , 23 / 128.0 , 24 / 128.0 , 25 / 128.0 , 26 / 128.0 , 27 / 128.0 , 28 / 128.0 , 29 / 128.0 , 30 / 128.0 , 31 / 128.0 
, 32 / 128.0 , 33 / 128.0 , 34 / 128.0 , 35 / 128.0 , 36 / 128.0 , 37 / 128.0 , 38 / 128.0 , 39 / 128.0 , 40 / 128.0 , 41 / 128.0 , 42 / 128.0 , 43 / 128.0 , 44 / 128.0 , 45 / 128.0 , 46 / 128.0 , 47 / 128.0 
, 48 / 128.0 , 49 / 128.0 , 50 / 128.0 , 51 / 128.0 , 52 / 128.0 , 53 / 128.0 , 54 / 128.0 , 55 / 128.0 , 56 / 128.0 , 57 / 128.0 , 58 / 128.0 , 59 / 128.0 , 60 / 128.0 , 61 / 128.0 , 62 / 128.0 , 63 / 128.0 
, 64 / 128.0 , 65 / 128.0 , 66 / 128.0 , 67 / 128.0 , 68 / 128.0 , 69 / 128.0 , 70 / 128.0 , 71 / 128.0 , 72 / 128.0 , 73 / 128.0 , 74 / 128.0 , 75 / 128.0 , 76 / 128.0 , 77 / 128.0 , 78 / 128.0 , 79 / 128.0 
, 80 / 128.0 , 81 / 128.0 , 82 / 128.0 , 83 / 128.0 , 84 / 128.0 , 85 / 128.0 , 86 / 128.0 , 87 / 128.0 , 88 / 128.0 , 89 / 128.0 , 90 / 128.0 , 91 / 128.0 , 92 / 128.0 , 93 / 128.0 , 94 / 128.0 , 95 / 128.0 
, 96 / 128.0 , 97 / 128.0 , 98 / 128.0 , 99 / 128.0 , 100 / 128.0 , 101 / 128.0 , 102 / 128.0 , 103 / 128.0 , 104 / 128.0 , 105 / 128.0 , 106 / 128.0 , 107 / 128.0 , 108 / 128.0 , 109 / 128.0 , 110 / 128.0 , 111 / 128.0 
, 112 / 128.0 , 113 / 128.0 , 114 / 128.0 , 115 / 128.0 , 116 / 128.0 , 117 / 128.0 , 118 / 128.0 , 119 / 128.0 , 120 / 128.0 , 121 / 128.0 , 122 / 128.0 , 123 / 128.0 , 124 / 128.0 , 125 / 128.0 , 126 / 128.0 , 127 / 128.0 };

static
std::complex<float> ibuf [READLEN_DEFAULT / 2];

//	For the callback, we do need some environment 
//	This is the user-side call back function
void	dll_driver::RTLSDRCallBack (unsigned char *buf,
	                            uint32_t len, void *ctx) {
rtlsdrHandler	*theStick = (rtlsdrHandler *)ctx;

	if ((len != READLEN_DEFAULT) || (theStick == NULL))
	   return;
//	Note that our "context"  is the stick
	for (int i = 0; i < READLEN_DEFAULT / 2; i ++) {
	   std::complex<float> tmp =
	               std::complex<float> (convTable [buf [2 * i]],
	                                    convTable [buf [2 * i + 1]]);
	   ibuf [i] = tmp;
	}
	int cnt	= 0;
	for (int i = 0; i < READLEN_DEFAULT / 2; i ++)
	   if (theStick -> d_filter -> Pass (ibuf [i], &ibuf [cnt])) 
	      cnt ++;

	theStick -> _I_Buffer -> putDataIntoBuffer (ibuf, cnt);
	if (theStick -> _I_Buffer -> GetRingBufferReadAvailable () >
	   theStick -> outputRate / 10)
	   theStick -> newdataAvailable (theStick -> outputRate / 10);
}

void	dll_driver::run (void) {
	(theStick -> rtlsdr_read_async) (theStick -> device,
	                          (rtlsdr_read_async_cb_t)&RTLSDRCallBack,
	                          (void *)theStick,
	                          0,
	                          READLEN_DEFAULT);
}
//
//	Our wrapper is a simple classs
//	to keep things simple, we just load the osmocom function
//	dynamically
	rtlsdrHandler::rtlsdrHandler (RadioInterface *mr,
                                      int32_t        outputRate,
                                      RingBuffer<DSPCOMPLEX> *r,
                                      QSettings      *s):
                                               deviceHandler (mr) {
int	res;
int16_t	deviceIndex;
int16_t	i;
	this	-> myFrame	= new QFrame (NULL);
	setupUi (this -> myFrame);
        this    -> myFrame -> show ();
        this    -> _I_Buffer   = r;
        this    -> dabSettings        = s;

	inputRate		= getInputRate (outputRate);
	gains			= NULL;
	libraryLoaded	= false;
	workerHandle	= NULL;
	d_filter	= NULL;
	open		= false;

	statusLabel	-> setText ("setting up");
#ifdef	__MINGW32__
	Handle		= LoadLibrary ((wchar_t *)L"rtlsdr.dll");
#else
	Handle		= dlopen ("librtlsdr.so", RTLD_NOW);
#endif
	if (Handle == NULL) {
	   fprintf (stderr, "Failed to open rtlsdr.dll\n");
	   statusLabel	-> setText ("no rtlsdr lib");
	   delete myFrame;
	   throw (21);
	}
//
//	library is loaded
//	and widget is visible
	libraryLoaded	= true;
	fprintf (stderr, "rtlsdr is loaded\n");

	if (!load_rtlFunctions	()) {
	   statusLabel	-> setText ("load failed");
	   goto err;
	}

//	vfoFrequency is the VFO frequency
	vfoFrequency		= KHz (22000);	// just a dummy
	deviceCount 		= (this -> rtlsdr_get_device_count) ();
	if (deviceCount == 0) {
	   statusLabel	-> setText ("no device");
	   fprintf (stderr, "No devices found\n");
	   goto err;
	}

	deviceIndex = 0;	// default
	if (deviceCount > 1) {
	   dongleSelector	= new dongleSelect ();
	   for (deviceIndex = 0; deviceIndex < deviceCount; deviceIndex ++) {
	      dongleSelector ->
	           addtoDongleList (rtlsdr_get_device_name (deviceIndex));
	   }
	   deviceIndex = dongleSelector -> QDialog::exec ();
	   delete dongleSelector;
	}
//
//	OK, now open the hardware
	res	= this -> rtlsdr_open (&device, deviceIndex);
	if (res < 0) {
	   fprintf (stderr, "Opening dabstick failed\n");
	   statusLabel	-> setText ("opening failed");
	   goto err;
	}

	open	= true;
	res	= (this -> rtlsdr_set_sample_rate) (device, inputRate);
	if (res < 0) {
	   fprintf (stderr, "Setting samplerate failed\n");
	   statusLabel	-> setText ("samplerate");
	   goto err;
	}

	gainsCount = rtlsdr_get_tuner_gains (device, NULL);
	fprintf(stderr, "Supported gain values (%d): ", gainsCount);
	gains		= new int [gainsCount];
	gainsCount = rtlsdr_get_tuner_gains (device, gains);
	for (i = 0; i < gainsCount; i++)
	   fprintf(stderr, "%.1f ", gains [i] / 10.0);
	rtlsdr_set_tuner_gain_mode (device, 1);
	rtlsdr_set_tuner_gain (device, gains [gainsCount / 2]);
	gainSlider	-> setMaximum (gainsCount);
	gainSlider	-> setValue (gainsCount / 2);
	workerHandle	= NULL;

	d_filter	= new decimatingFIR (inputRate / outputRate * 5 - 1,
	                                     - outputRate / 2,
	                                     outputRate / 2,
	                                     inputRate,
	                                     inputRate / outputRate);
	connect (gainSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (setExternalGain (int)));
	connect (f_correction, SIGNAL (valueChanged (int)),
	         this, SLOT (setCorrection (int)));
	connect (checkAgc, SIGNAL (stateChanged (int)),
	         this, SLOT (setAgc (int)));
	if (dabSettings != NULL) {
	   dabSettings	-> beginGroup ("Stick");
	   int	k	= dabSettings	-> value ("dab_externalGain", 24). toInt ();
	   gainSlider	-> setValue (k);
	   k		= dabSettings	-> value ("dab_correction", 0). toInt ();
	   dabSettings	-> endGroup ();
	}
	fprintf (stderr, "Loaded, d_filter = %X\n", (uint64_t)d_filter);
	statusLabel	-> setText ("Loaded");
	return;
err:
	if (open)
	   rtlsdr_close (device);
#ifdef __MINGW32__
	FreeLibrary (Handle);
#else
	dlclose (Handle);
#endif
	libraryLoaded	= false;
	open		= false;
	throw (22);
}

	rtlsdrHandler::~rtlsdrHandler	(void) {
	if (open && libraryLoaded && dabSettings != NULL) {
	   dabSettings	-> beginGroup ("Stick");
	   dabSettings	-> setValue ("rtlsdr_externalGain",
	                                             gainSlider -> value ());
	   dabSettings	-> setValue ("rtlsdr_correction",
	                                             f_correction -> value ());
	   dabSettings	-> endGroup ();
	}

	if (open && libraryLoaded) 
	   stopReader ();
	if (open) 
	   rtlsdr_close (device);

	if (libraryLoaded)
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
	if (d_filter != NULL)
	   delete d_filter;
	d_filter	= NULL;
	if (gains != NULL)
	   delete gains;
}

int32_t	rtlsdrHandler::getRate	(void) {
	return outputRate;
}
//
//
quint64	rtlsdrHandler::getVFOFrequency	(void) {
	if (!open || !libraryLoaded)
	   return KHz (14070);
	return (int32_t)rtlsdr_get_center_freq (device) - vfoOffset;
}
//
//	The external world sets a virtual VFO frequency
//	The real VFO frequency can be influences by an externally
//	set vfoOffset
void	rtlsdrHandler::setVFOFrequency	(quint64 f) {
	if (!open || !libraryLoaded)
	   return;

	vfoFrequency	= f;
	(void)rtlsdr_set_center_freq (device, f + vfoOffset);
}

bool	rtlsdrHandler::legalFrequency	(int32_t f) {
	(void)f;
	return true;
}

//
//
bool	rtlsdrHandler::restartReader	(void) {
int32_t	r;

	if (!open ||!libraryLoaded)
	   return false;

	if (workerHandle != NULL)	// running already
	   return true;

	r = rtlsdr_reset_buffer (device);
	if (r < 0)
	   return false;

	rtlsdr_set_center_freq (device, vfoFrequency + vfoOffset);
	workerHandle	= new dll_driver (this);
	return true;
}

void	rtlsdrHandler::stopReader	(void) {
	if (workerHandle == NULL)
	   return;

	rtlsdr_cancel_async (device);
	if (workerHandle != NULL) {
	   while (!workerHandle -> isFinished ()) 
	      usleep (10);

	   delete	workerHandle;
	}
	workerHandle	= NULL;
}
//
//
void	rtlsdrHandler::setExternalGain	(int gain) {
static int	oldGain	= 0;

	if (gain == oldGain)
	   return;

	if ((gain < 0) || (gain >= gainsCount))
	   return;	

	oldGain	= gain;
	rtlsdr_set_tuner_gain (device, gains [gain]);
	(void)rtlsdr_get_tuner_gain (device);
	gainDisplay	-> display (gain);
}
//

void	rtlsdrHandler::setCorrection	(int ppm) {
	if (!open || !libraryLoaded)
	   return;

	rtlsdr_set_freq_correction (device, ppm);
}
//
bool	rtlsdrHandler::load_rtlFunctions (void) {
//	link the required procedures
	rtlsdr_open	= (pfnrtlsdr_open)
	                       GETPROCADDRESS (Handle, "rtlsdr_open");
	if (rtlsdr_open == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_open\n");
	   return false;
	}
	rtlsdr_close	= (pfnrtlsdr_close)
	                     GETPROCADDRESS (Handle, "rtlsdr_close");
	if (rtlsdr_close == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_close\n");
	   return false;
	}

	rtlsdr_set_sample_rate =
	    (pfnrtlsdr_set_sample_rate)GETPROCADDRESS (Handle, "rtlsdr_set_sample_rate");
	if (rtlsdr_set_sample_rate == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_sample_rate\n");
	   return false;
	}

	rtlsdr_get_sample_rate	=
	    (pfnrtlsdr_get_sample_rate)
	               GETPROCADDRESS (Handle, "rtlsdr_get_sample_rate");
	if (rtlsdr_get_sample_rate == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_sample_rate\n");
	   return false;
	}

	rtlsdr_set_agc_mode	=
	    (pfnrtlsdr_set_agc_mode)
	               GETPROCADDRESS (Handle, "rtlsdr_set_agc_mode");
	if (rtlsdr_set_agc_mode == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_agc_mode\n");
	   return false;
	}

	rtlsdr_get_tuner_gains		= (pfnrtlsdr_get_tuner_gains)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_tuner_gains");
	if (rtlsdr_get_tuner_gains == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_tuner_gains\n");
	   return false;
	}

	rtlsdr_set_tuner_gain_mode	= (pfnrtlsdr_set_tuner_gain_mode)
	                     GETPROCADDRESS (Handle, "rtlsdr_set_tuner_gain_mode");
	if (rtlsdr_set_tuner_gain_mode == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_tuner_gain_mode\n");
	   return false;
	}

	rtlsdr_set_tuner_gain	= (pfnrtlsdr_set_tuner_gain)
	                     GETPROCADDRESS (Handle, "rtlsdr_set_tuner_gain");
	if (rtlsdr_set_tuner_gain == NULL) {
	   fprintf (stderr, "Cound not find rtlsdr_set_tuner_gain\n");
	   return false;
	}

	rtlsdr_get_tuner_gain	= (pfnrtlsdr_get_tuner_gain)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_tuner_gain");
	if (rtlsdr_get_tuner_gain == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_tuner_gain\n");
	   return false;
	}
	rtlsdr_set_center_freq	= (pfnrtlsdr_set_center_freq)
	                     GETPROCADDRESS (Handle, "rtlsdr_set_center_freq");
	if (rtlsdr_set_center_freq == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_center_freq\n");
	   return false;
	}

	rtlsdr_get_center_freq	= (pfnrtlsdr_get_center_freq)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_center_freq");
	if (rtlsdr_get_center_freq == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_center_freq\n");
	   return false;
	}

	rtlsdr_reset_buffer	= (pfnrtlsdr_reset_buffer)
	                     GETPROCADDRESS (Handle, "rtlsdr_reset_buffer");
	if (rtlsdr_reset_buffer == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_reset_buffer\n");
	   return false;
	}

	rtlsdr_read_async	= (pfnrtlsdr_read_async)
	                     GETPROCADDRESS (Handle, "rtlsdr_read_async");
	if (rtlsdr_read_async == NULL) {
	   fprintf (stderr, "Cound not find rtlsdr_read_async\n");
	   return false;
	}

	rtlsdr_get_device_count	= (pfnrtlsdr_get_device_count)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_device_count");
	if (rtlsdr_get_device_count == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_device_count\n");
	   return false;
	}

	rtlsdr_cancel_async	= (pfnrtlsdr_cancel_async)
	                     GETPROCADDRESS (Handle, "rtlsdr_cancel_async");
	if (rtlsdr_cancel_async == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_cancel_async\n");
	   return false;
	}

	rtlsdr_set_direct_sampling = (pfnrtlsdr_set_direct_sampling)
	                  GETPROCADDRESS (Handle, "rtlsdr_set_direct_sampling");
	if (rtlsdr_set_direct_sampling == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_direct_sampling\n");
	   return false;
	}

	rtlsdr_set_freq_correction = (pfnrtlsdr_set_freq_correction)
	                  GETPROCADDRESS (Handle, "rtlsdr_set_freq_correction");
	if (rtlsdr_set_freq_correction == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_freq_correction\n");
	   return false;
	}

	rtlsdr_get_device_name = (pfnrtlsdr_get_device_name)
	                   GETPROCADDRESS (Handle, "rtlsdr_get_device_name");
	if (rtlsdr_get_device_name == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_device_name\n");
	   return false;
	}

	fprintf (stderr, "OK, functions seem to be loaded\n");
	return true;
}

int32_t	rtlsdrHandler::getInputRate	(int32_t rate) {
int32_t temp 	= rate;

	while (temp < 960000)
	   temp += rate;

	return temp;
}

int16_t	rtlsdrHandler::bitDepth	(void) {
	return 8;
}

void	rtlsdrHandler::newdataAvailable (int n) {
	dataAvailable (n);
}

void	rtlsdrHandler::setAgc	(int s) {
	(void) s;
	if (checkAgc -> isChecked ())
	   (void)rtlsdr_set_agc_mode (device, 1);
	else
	   (void)rtlsdr_set_agc_mode (device, 0);
}

