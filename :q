#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of sw radio
 *
 *    swradio is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation version 2 of the License.
 *
 *    swradio is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with swradio if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	"sdrplay-handler-v3.h"
#include	<QSettings>
#include	<QThread>
#include	<QTime>
#include	<QDate>
#include	<QLabel>
#include	<QFileDialog>
#include	"sdrplay-commands.h"
#include	"radio.h"
//
//	The Rsp's
#include	"Rsp-device.h"
#include	"RspDx-handler.h"
#include	"Rsp1A-handler.h"
#include	"RspII-handler.h"
#include	"RspDuo-handler.h"

#define	WAITING	0100
#define	FAILED	0101
#define	SUCCESS	0102

#define	SDRPLAY_RSP1_	1
#define	SDRPLAY_RSP1A_	255
#define	SDRPLAY_RSP2_	2
#define	SDRPLAY_RSPduo_	3
#define	SDRPLAY_RSPdx_	4

	sdrplayHandler_v3::sdrplayHandler_v3  (RadioInterface *mr,
	                                       int32_t	outputRate,
	                                       RingBuffer<std::complex<float>> *r,
	                                       QSettings *s):
	                                          deviceHandler (mr),
	                                          myFrame (nullptr) {
	this	-> outputRate		= outputRate;
	sdrplaySettings			= s;
	setupUi (&myFrame);
	myFrame. show	();
	antennaSelector		-> hide	();
	amPortSelector		-> hide	();
	tunerSelector		-> hide	();
	this	-> inputRate	= kHz (2112);
	this	-> _I_Buffer	= r;
	nrBits			= 12;	// default
	denominator		= 2048;	// default
	sampleCnt		= 0;
//	See if there are settings from previous incarnations
//	and config stuff

	sdrplaySettings		-> beginGroup ("sdrplaySettings");
	GRdBSelector 		-> setValue (
	            sdrplaySettings -> value ("sdrplay-ifgrdb", 20). toInt());
	GRdBValue		= GRdBSelector -> value ();

	lnaGainSetting		-> setValue (
	            sdrplaySettings -> value ("sdrplay-lnastate", 4). toInt());
	lnaState		= lnaGainSetting -> value ();

	ppmControl		-> setValue (
	            sdrplaySettings -> value ("sdrplay-ppm", 0). toInt());

	agcMode		=
	       sdrplaySettings -> value ("sdrplay-agcMode", 0). toInt() != 0;
	if (agcMode) {
	   agcControl -> setChecked (true);
	   GRdBSelector         -> hide ();
	   gainsliderLabel      -> hide ();
	}
	biasT		=
	       sdrplaySettings -> value ("biasT", 0). toInt () != 0;
	if (biasT)
	   biasT_selector -> setChecked (true);
	sdrplaySettings	-> endGroup	();

//	and be prepared for future changes in the settings
	connect (GRdBSelector, SIGNAL (valueChanged (int)),
	         this, SLOT (set_ifgainReduction (int)));
	connect (lnaGainSetting, SIGNAL (valueChanged (int)),
	         this, SLOT (set_lnagainReduction (int)));
	connect (agcControl, SIGNAL (stateChanged (int)),
	         this, SLOT (set_agcControl (int)));
	connect (ppmControl, SIGNAL (valueChanged (int)),
	         this, SLOT (set_ppmControl (int)));
	connect (antennaSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (set_selectAntenna (const QString &)));
	connect (amPortSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (set_amPortSelect (const QString &)));
	connect (biasT_selector, SIGNAL (stateChanged (int)),
	         this, SLOT (set_biasT (int)));
//
	filter	= new decimatingFIR (inputRate / outputRate * 5 - 1,
	                                     + outputRate / 2,
	                                     inputRate,
	                                     inputRate / outputRate);

	vfoFrequency	= KHz (14070);
	theGain		= -1;
	debugControl	-> hide ();
//
//	we have to wait for the task to initialize its
	status. store (WAITING);
	start ();
	usleep (2000);
	while (status. load () == WAITING)
	   usleep (1000);
	fprintf (stderr, "status = %o\n", status. load ());
	if (status. load () == FAILED)
	   throw (23);
}

	sdrplayHandler_v3::~sdrplayHandler_v3 () {
	threadRuns. store (false);
	while (isRunning ())
	   usleep (1000);
//	thread should be stopped by now
	sdrplaySettings	-> beginGroup ("sdrplaySettings");
	sdrplaySettings -> setValue ("sdrplay-ppm",
	                                ppmControl -> value ());
	sdrplaySettings -> setValue ("sdrplay-ifgrdb",
	                                GRdBSelector -> value ());
	sdrplaySettings -> setValue ("sdrplay-lnastate",
	                                lnaGainSetting -> value ());
	sdrplaySettings	-> setValue ("sdrplay-agcMode",
	                                agcControl -> isChecked() ? 1 : 0);
	sdrplaySettings -> setValue ("biasT",
	                                biasT_selector -> isChecked () ? 1 : 0);
	sdrplaySettings	-> endGroup ();
	sdrplaySettings	-> sync();

	myFrame. hide ();
	fprintf (stderr, "end of sdrplay-handler-v3\n");
}

/////////////////////////////////////////////////////////////////////////
//	Implementing the interface
/////////////////////////////////////////////////////////////////////////


void	sdrplayHandler_v3::setVFOFrequency	(quint64 f) {
restartRequest r ( (uint32_t)f);
	(void)messageHandler (&r);
	vfoFrequency	= f;
}

void	sdrplayHandler_v3::set_biasT (int c) {
bool b = biasT_selector -> isChecked ();
biasTRequest r (b);
	(void)c;
	(void)messageHandler (&r);
}

quint64	sdrplayHandler_v3::getVFOFrequency() {
	return vfoFrequency;
}

bool	sdrplayHandler_v3::restartReader	() {
restartRequest r ((uint32_t)vfoFrequency);
	if (receiverRuns. load ())
	   return true;
	return messageHandler (&r);
}

void	sdrplayHandler_v3::stopReader	() {
stopRequest r;
	if (!receiverRuns. load ())
	   return;
	messageHandler (&r);
}

void	sdrplayHandler_v3::set_ifgainReduction	(int GRdB) {
GRdBRequest r (GRdB);
	if (!receiverRuns. load ())
	   return;
	messageHandler (&r);
}

void	sdrplayHandler_v3::set_lnagainReduction (int lnaState) {
lnaRequest r (lnaState);
	if (!receiverRuns. load ())
	   return;
	messageHandler (&r);
}

//
void	sdrplayHandler_v3::resetBuffer	() {
	_I_Buffer -> FlushRingBuffer();
}

int16_t	sdrplayHandler_v3::bitDepth	() {
	return nrBits;
}

int32_t	sdrplayHandler_v3::getRate	() {
	return KHz (2112);
}

///////////////////////////////////////////////////////////////////////////
//	Handling the GUI
//////////////////////////////////////////////////////////////////////

void	sdrplayHandler_v3::set_nrBits (int b) {
	nrBits	= b;
	denominator = nrBits == 12 ? 2048 : 4096;
}

void	sdrplayHandler_v3::set_lnabounds(int low, int high) {
	lnaGainSetting	-> setRange (low, high);
}

void	sdrplayHandler_v3::set_deviceName (const QString& s) {
	deviceLabel	-> setText (s);
}

void	sdrplayHandler_v3::set_serial	(const QString& s) {
	serialNumber	-> setText (s);
}

void	sdrplayHandler_v3::set_apiVersion (float version) {
	api_version	-> display (version);
}

void	sdrplayHandler_v3::show_lnaGain (int g) {
	lnaGRdBDisplay	-> display (g);
}

void	sdrplayHandler_v3::set_agcControl (int dummy) {
bool    agcMode = agcControl -> isChecked ();
agcRequest r (agcMode, 30);
	(void)dummy;
	messageHandler (&r);
	if (agcMode) {
	   GRdBSelector         -> hide ();
	   gainsliderLabel      -> hide ();
	}
	else {
	   GRdBSelector		-> show ();
	   gainsliderLabel	-> show ();
	}
}

void	sdrplayHandler_v3::set_ppmControl (int ppm) {
ppmRequest r (ppm);
	messageHandler (&r);
}

void	sdrplayHandler_v3::set_selectAntenna	(const QString &s) {
antennaRequest r (s == "Antenna A" ? 'A' :
	               s == "Antenna B" ? 'B' : 'C');
	messageHandler (&r);
}

void	sdrplayHandler_v3::set_amPortSelect	(const QString &s) {
amPortRequest r (s == "amPort on" ? 1 : 0);
	messageHandler  (&r);
}

void    sdrplayHandler_v3::report_dataAvailable (void) {
	emit dataAvailable (10);
}

//
////////////////////////////////////////////////////////////////////////
//	showing data
////////////////////////////////////////////////////////////////////////
void	sdrplayHandler_v3::set_antennaSelect (int n) {
	if (n > 0) {
	   antennaSelector	-> addItem ("Antenna B");
	   if (n > 1)  {	// It is a "DX"
	      antennaSelector	-> addItem ("Antenna C");
	      amPortSelector	-> show ();
	   }
	   antennaSelector		-> show	();
	}
	else {
	   antennaSelector		-> hide	();
	   amPortSelector		-> hide	();
	}
}

void	sdrplayHandler_v3::show_tunerSelector	(bool b) {
	if (b)
	   tunerSelector	-> show	();
	else
	   tunerSelector	-> hide	();
}

//
///////////////////////////////////////////////////////////////////////
//	the real controller starts here
///////////////////////////////////////////////////////////////////////

bool    sdrplayHandler_v3::messageHandler (generalCommand *r) {
	server_queue. push (r);
	serverjobs. release (1);
	while (!r -> waiter. tryAcquire (1, 1000))
	   if (!threadRuns. load ())
	      return false;
	return true;
}

static
void    StreamACallback (short *xi, short *xq,
	                 sdrplay_api_StreamCbParamsT *params,
	                 unsigned int numSamples,
	                 unsigned int reset,
	                 void *cbContext) {
sdrplayHandler_v3 *p	= static_cast<sdrplayHandler_v3 *> (cbContext);
std::complex<float> localBuf [numSamples];
int	cnt = 0;

	(void)params;
	if (reset)
	   return;
	if (!p -> receiverRuns. load ())
	   return;

	for (int i = 0; i <  (int)numSamples; i ++) {
	   std::complex<float> tmp =
	                std::complex<float> ((float)xi [i] / p -> denominator,
	                                     (float)xq [i] / p -> denominator);
	   if (p -> filter -> Pass (tmp, &localBuf [cnt]))
	      if (localBuf [cnt] == localBuf [cnt])
	         cnt ++;

	}
	p -> _I_Buffer -> putDataIntoBuffer (localBuf, cnt);
	p -> sampleCnt += cnt;
	if (p -> sampleCnt > p -> outputRate / 8) {
	   p -> report_dataAvailable ();
	   p -> sampleCnt = 0;
	}
}

static
void	StreamBCallback (short *xi, short *xq,
	                 sdrplay_api_StreamCbParamsT *params,
	                 unsigned int numSamples, unsigned int reset,
	                 void *cbContext) {
	(void)xi; (void)xq; (void)params; (void)cbContext;
	if (reset)
	   printf ("sdrplay_api_StreamBCallback: numSamples=%d\n", numSamples);
}

void	EventCallback (sdrplay_api_EventT eventId,
	               sdrplay_api_TunerSelectT tuner,
	               sdrplay_api_EventParamsT *params,
	               void *cbContext) {
sdrplayHandler_v3 *p	= static_cast<sdrplayHandler_v3 *> (cbContext);
	(void)tuner;
	p -> theGain	= params -> gainParams. currGain;
	switch (eventId) {
	   case sdrplay_api_GainChange:
	      break;

	   case sdrplay_api_PowerOverloadChange:
	      p -> update_PowerOverload (params);
	      break;

	   default:
	      fprintf (stderr, "event %d\n", eventId);
	      break;
	}
}

void	sdrplayHandler_v3::
	         update_PowerOverload (sdrplay_api_EventParamsT *params) {
	sdrplay_api_Update (chosenDevice -> dev,
	                    chosenDevice -> tuner,
	                    sdrplay_api_Update_Ctrl_OverloadMsgAck,
	                    sdrplay_api_Update_Ext1_None);
	if (params -> powerOverloadParams.powerOverloadChangeType ==
	                                    sdrplay_api_Overload_Detected) {
//	   fprintf (stderr, "Qt-DAB sdrplay_api_Overload_Detected");
	}
	else {
//	   fprintf (stderr, "Qt-DAB sdrplay_api_Overload Corrected");
	}
}

void	sdrplayHandler_v3::run		() {
sdrplay_api_ErrT        err;
sdrplay_api_DeviceT     devs [6];
uint32_t                ndev;

	threadRuns. store (false);
	receiverRuns. store (false);

	chosenDevice		= nullptr;

	connect (this, SIGNAL (set_serial_signal (const QString &)),
	         this, SLOT (set_serial (const QString &)));
	connect (this, SIGNAL (set_apiVersion_signal (float)),
	         this, SLOT (set_apiVersion (float)));

	denominator		= 2048;		// default
	nrBits			= 12;		// default

	Handle			= fetchLibrary ();
	if (Handle == nullptr) {
	   status. store (FAILED);
	   return;
	}

//	load the functions
	bool success	= loadFunctions ();
	if (!success) {
	   releaseLibrary ();
	   status. store (FAILED);
	   return;
	}

	fprintf (stderr, "functions loaded\n");

//	try to open the API
	err	= sdrplay_api_Open ();
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "sdrplay_api_Open failed %s\n",
	                          sdrplay_api_GetErrorString (err));
	   releaseLibrary ();
	   status. store (FAILED);
	   return;
	}

	fprintf (stderr, "api opened\n");

//	Check API versions match
	err = sdrplay_api_ApiVersion (&apiVersion);
	if (err  != sdrplay_api_Success) {
	   fprintf (stderr, "sdrplay_api_ApiVersion failed %s\n",
	                             sdrplay_api_GetErrorString (err));
	   goto closeAPI;
	}

	if (apiVersion < (SDRPLAY_API_VERSION - 0.01)) {
	   fprintf (stderr, "API versions don't match (local=%.2f dll=%.2f)\n",
	                                      SDRPLAY_API_VERSION, apiVersion);
	   goto closeAPI;
	}
	
	fprintf (stderr, "api version %f detected\n", apiVersion);
//
//	lock API while device selection is performed
	sdrplay_api_LockDeviceApi ();
	{  int s	= sizeof (devs) / sizeof (sdrplay_api_DeviceT);
	   err	= sdrplay_api_GetDevices (devs, &ndev, s);
	   if (err != sdrplay_api_Success) {
	      fprintf (stderr, "sdrplay_api_GetDevices failed %s\n",
	                      sdrplay_api_GetErrorString (err));
	      goto unlockDevice_closeAPI;
	   }
	}

	if (ndev == 0) {
	   fprintf (stderr, "no valid device found\n");
	   goto unlockDevice_closeAPI;
	}

	fprintf (stderr, "%d devices detected\n", ndev);
	chosenDevice	= &devs [0];
	err	= sdrplay_api_SelectDevice (chosenDevice);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "sdrplay_api_SelectDevice failed %s\n",
	                         sdrplay_api_GetErrorString (err));
	   goto unlockDevice_closeAPI;
	}

//	assign callback functions
//	will be effectuated when initing the device
	cbFns. StreamACbFn   = StreamACallback;
	cbFns. StreamBCbFn   = StreamBCallback;
	cbFns. EventCbFn     = EventCallback;

//	we have a device, unlock and go on with the specified device
	sdrplay_api_UnlockDeviceApi ();

//
	serial		= devs [0]. SerNo;
	hwVersion	= devs [0]. hwVer;
	try {
	   switch (hwVersion) {
	      case SDRPLAY_RSPdx_ :
	         theRsp	= new RspDx_handler (this,
	                                     chosenDevice,
	                                     inputRate,
	                                     KHz (14070),
	                                     agcMode,
	                                     lnaState,
	                                     GRdBValue,
	                                     biasT);
	         break;

	      case SDRPLAY_RSP1A_ :
	         theRsp	= new Rsp1A_handler (this,
	                                     chosenDevice,
	                                     inputRate,
	                                     KHz (14070),
	                                     agcMode,
	                                     lnaState,
	                                     GRdBValue,
	                                     biasT);
	         break;

	      case SDRPLAY_RSP2_ :
	         theRsp	= new RspII_handler (this,
	                                     chosenDevice,
	                                     inputRate,
	                                     KHz (14070),
	                                     agcMode,
	                                     lnaState,
	                                     GRdBValue,
	                                     biasT);
	         break;

	      case SDRPLAY_RSPduo_ :
	         theRsp	= new RspDuo_handler (this,
	                                     chosenDevice,
	                                     inputRate,
	                                     KHz (14070),
	                                     agcMode,
	                                     lnaState,
	                                     GRdBValue,
	                                     biasT);
	         break;

	      default:
	         theRsp	= new Rsp_device (this,
	                                  chosenDevice,
	                                  2112000,
	                                  KHz (14070),
	                                  agcMode,
	                                  lnaState,
	                                  GRdBValue,
	                                  biasT);
	         break;
	   }
	} catch (int e) {
	   goto closeAPI;
	}

	set_serial_signal	(serial);
	set_apiVersion_signal	(apiVersion);

	threadRuns. store (true);	// it seems we can do some work
	status. store (SUCCESS);
	while (threadRuns. load ()) {
	   while (!serverjobs. tryAcquire (1, 1000))
	   if (!threadRuns. load ())
	      goto normal_exit;
//
//	here we could assert that the server_queue is not empty
	   switch (server_queue. front () -> cmd) {
	      case RESTART_REQUEST: {
	         restartRequest *p = (restartRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = theRsp	-> restart (p -> freq);
	         receiverRuns. store (true);
	         p -> waiter. release (1);
	         break;
	      }
	       
	      case STOP_REQUEST: {
	         stopRequest *p = (stopRequest *)(server_queue. front ());
	         server_queue. pop ();
	         receiverRuns. store (false);
	         p -> waiter. release (1);
	         break;
	      }
	       
	      case AGC_REQUEST: {
	         agcRequest *p = (agcRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = theRsp -> set_agc (-p -> setPoint, p -> agcMode);
	         p -> waiter. release (1);
	         break;
	      }

	      case GRDB_REQUEST: {
	         GRdBRequest *p =  (GRdBRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = theRsp -> set_GRdB (p -> GRdBValue);
	         p -> waiter. release (1);
	         break;
	      }

	      case PPM_REQUEST: {
	         ppmRequest *p = (ppmRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = theRsp -> set_ppm (p -> ppmValue);
	         p -> waiter. release (1);
	         break;
	      }

	      case LNA_REQUEST: {
	         lnaRequest *p = (lnaRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = theRsp -> set_lna (p -> lnaState);
	         p -> waiter. release (1);
	         break;
	      }

	      case ANTENNASELECT_REQUEST: {
	         antennaRequest *p = (antennaRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = theRsp -> set_antenna (p -> antenna);
	         p -> waiter. release (1);
	         break;
	      }
	
	      case AMPORT_REQUEST: {
	         amPortRequest *p = (amPortRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = theRsp -> set_amPort (p -> amPort);
	         p -> waiter. release (1);
	         break;
	      }

	      case BIAST_REQUEST: {
	         biasTRequest *p = (biasTRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = theRsp -> set_biasT (p -> checked);
	         p -> waiter. release (1);
	         break;
	      }
	
	      default:		// cannot happen
	         break;
	   }
	}

normal_exit:
	err = sdrplay_api_Uninit	(chosenDevice -> dev);
	if (err != sdrplay_api_Success) 
	   fprintf (stderr, "sdrplay_api_Uninit failed %s\n",
	                          sdrplay_api_GetErrorString (err));

	err = sdrplay_api_ReleaseDevice	(chosenDevice);
	if (err != sdrplay_api_Success) 
	   fprintf (stderr, "sdrplay_api_ReleaseDevice failed %s\n",
	                          sdrplay_api_GetErrorString (err));

//	sdrplay_api_UnlockDeviceApi	(); ??
	sdrplay_api_Close               ();
	if (err != sdrplay_api_Success) 
	   fprintf (stderr, "sdrplay_api_Close failed %s\n",
	                          sdrplay_api_GetErrorString (err));

	releaseLibrary			();
	fprintf (stderr, "library released, ready to stop thread\n");
	msleep (200);
	return;

unlockDevice_closeAPI:
	sdrplay_api_UnlockDeviceApi	();
closeAPI:	
	sdrplay_api_ReleaseDevice       (chosenDevice);
	sdrplay_api_Close               ();
	releaseLibrary	();
	status. store (FAILED);
	fprintf (stderr, "De taak is gestopt\n");
}

/////////////////////////////////////////////////////////////////////////////
//	handling the library
/////////////////////////////////////////////////////////////////////////////

HINSTANCE	sdrplayHandler_v3::fetchLibrary () {
HINSTANCE	Handle	= nullptr;
#ifdef	__MINGW32__
HKEY APIkey;
wchar_t APIkeyValue [256];
ULONG APIkeyValue_length = 255;

	wchar_t *libname = (wchar_t *)L"sdrplay_api.dll";
	Handle	= LoadLibrary (libname);
	if (Handle == nullptr) {
	   if (RegOpenKey (HKEY_LOCAL_MACHINE,
	                   TEXT("Software\\MiricsSDR\\API"),
	                   &APIkey) != ERROR_SUCCESS) {
	      fprintf (stderr,
	           "failed to locate API registry entry, error = %d\n",
	           (int)GetLastError());
	      return nullptr;
	   }

	   RegQueryValueEx (APIkey,
	                    (wchar_t *)L"Install_Dir",
	                    nullptr,
	                    nullptr,
	                    (LPBYTE)&APIkeyValue,
	                    (LPDWORD)&APIkeyValue_length);
//	Ok, make explicit it is in the 32/64 bits section
	   wchar_t *x =
	        wcscat (APIkeyValue, (wchar_t *)L"\\x86\\sdrplay_api.dll");
//	        wcscat (APIkeyValue, (wchar_t *)L"\\x64\\sdrplay_api.dll");
	   RegCloseKey(APIkey);

	   Handle	= LoadLibrary (x);
	   if (Handle == nullptr) {
	      fprintf (stderr, "Failed to open sdrplay_api.dll\n");
	      return nullptr;
	   }
	}
#else
	Handle		= dlopen ("libusb-1.0.so", RTLD_NOW | RTLD_GLOBAL);
	Handle		= dlopen ("libsdrplay_api.so", RTLD_NOW);
	if (Handle == nullptr) {
	   fprintf (stderr, "error report %s\n", dlerror());
	   return nullptr;
	}
#endif
	return Handle;
}

void	sdrplayHandler_v3::releaseLibrary () {
#ifdef __MINGW32__
	FreeLibrary (Handle);
#else
	dlclose (Handle);
#endif
}

bool	sdrplayHandler_v3::loadFunctions () {
	sdrplay_api_Open	= (sdrplay_api_Open_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_Open");
	if ((void *)sdrplay_api_Open == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_Open\n");
	   return false;
	}

	sdrplay_api_Close	= (sdrplay_api_Close_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_Close");
	if (sdrplay_api_Close == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_Close\n");
	   return false;
	}

	sdrplay_api_ApiVersion	= (sdrplay_api_ApiVersion_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_ApiVersion");
	if (sdrplay_api_ApiVersion == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_ApiVersion\n");
	   return false;
	}

	sdrplay_api_LockDeviceApi	= (sdrplay_api_LockDeviceApi_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_LockDeviceApi");
	if (sdrplay_api_LockDeviceApi == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_LockdeviceApi\n");
	   return false;
	}

	sdrplay_api_UnlockDeviceApi	= (sdrplay_api_UnlockDeviceApi_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_UnlockDeviceApi");
	if (sdrplay_api_UnlockDeviceApi == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_UnlockdeviceApi\n");
	   return false;
	}

	sdrplay_api_GetDevices		= (sdrplay_api_GetDevices_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_GetDevices");
	if (sdrplay_api_GetDevices == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_GetDevices\n");
	   return false;
	}

	sdrplay_api_SelectDevice	= (sdrplay_api_SelectDevice_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_SelectDevice");
	if (sdrplay_api_SelectDevice == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_SelectDevice\n");
	   return false;
	}

	sdrplay_api_ReleaseDevice	= (sdrplay_api_ReleaseDevice_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_ReleaseDevice");
	if (sdrplay_api_ReleaseDevice == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_ReleaseDevice\n");
	   return false;
	}

	sdrplay_api_GetErrorString	= (sdrplay_api_GetErrorString_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_GetErrorString");
	if (sdrplay_api_GetErrorString == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_GetErrorString\n");
	   return false;
	}

	sdrplay_api_GetLastError	= (sdrplay_api_GetLastError_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_GetLastError");
	if (sdrplay_api_GetLastError == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_GetLastError\n");
	   return false;
	}

	sdrplay_api_DebugEnable		= (sdrplay_api_DebugEnable_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_DebugEnable");
	if (sdrplay_api_DebugEnable == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_DebugEnable\n");
	   return false;
	}

	sdrplay_api_GetDeviceParams	= (sdrplay_api_GetDeviceParams_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_GetDeviceParams");
	if (sdrplay_api_GetDeviceParams == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_GetDeviceParams\n");
	   return false;
	}

	sdrplay_api_Init		= (sdrplay_api_Init_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_Init");
	if (sdrplay_api_Init == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_Init\n");
	   return false;
	}

	sdrplay_api_Uninit		= (sdrplay_api_Uninit_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_Uninit");
	if (sdrplay_api_Uninit == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_Uninit\n");
	   return false;
	}

	sdrplay_api_Update		= (sdrplay_api_Update_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_Update");
	if (sdrplay_api_Update == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_Update\n");
	   return false;
	}

	return true;
}

