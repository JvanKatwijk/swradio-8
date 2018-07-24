#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of swradio-8
 *
 *    swradio-8 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation version 2 of the License.
 *
 *    swradio-8 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with swradio-8 if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	<QThread>
#include	<QSettings>
#include	<QHBoxLayout>
#include	<QLabel>
#include	"sdrplay-handler.h"
#include	"sdrplayselect.h"
#include	"radio.h"

#define	DEFAULT_GRED	40
static
int     RSP1_Table [] = {0, 24, 19, 43};

static
int     RSP1A_Table [] = {0, 6, 12, 18, 20, 26, 32, 38, 57, 62};

static
int     RSP2_Table [] = {0, 10, 15, 21, 24, 34, 39, 45, 64};

static
int     RSPduo_Table [] = {0, 6, 12, 18, 20, 26, 32, 38, 57, 62};

	sdrplayHandler::sdrplayHandler  (RadioInterface *mr,
	                                 int32_t	outputRate,
	                                 RingBuffer<DSPCOMPLEX> *r,
	                                 QSettings	*s):
	                                              deviceInput (mr) {
mir_sdr_ErrT err;
float	ver;
mir_sdr_DeviceT devDesc [4];
mir_sdr_GainValuesT gainDesc;
sdrplaySelect	*sdrplaySelector;

	this	-> outputRate	= outputRate;
	this	-> sdrplaySettings	= s;
	this	-> myFrame	= new QFrame (NULL);
	setupUi (this -> myFrame);
	this	-> myFrame	-> show ();
	antennaSelector		-> hide ();
	tunerSelector		-> hide ();
	this	-> inputRate	= kHz (2112);
	_I_Buffer		= r;
	libraryLoaded		= false;

#ifdef	__MINGW32__
HKEY APIkey;
wchar_t APIkeyValue [256];
ULONG APIkeyValue_length = 255;

	if (RegOpenKey (HKEY_LOCAL_MACHINE,
	                TEXT("Software\\MiricsSDR\\API"),
	                &APIkey) != ERROR_SUCCESS) {
          fprintf (stderr,
	           "failed to locate API registry entry, error = %d\n",
	           (int)GetLastError());
	   delete myFrame;
	   throw (21);
	}

	RegQueryValueEx (APIkey,
	                 (wchar_t *)L"Install_Dir",
	                 NULL,
	                 NULL,
	                 (LPBYTE)&APIkeyValue,
	                 (LPDWORD)&APIkeyValue_length);
//	Ok, make explicit it is in the 64 bits section
	wchar_t *x = wcscat (APIkeyValue, (wchar_t *)L"\\x86\\mir_sdr_api.dll");
//	wchar_t *x = wcscat (APIkeyValue, (wchar_t *)L"\\x64\\mir_sdr_api.dll");
//	fprintf (stderr, "Length of APIkeyValue = %d\n", APIkeyValue_length);
//	wprintf (L"API registry entry: %s\n", APIkeyValue);
	RegCloseKey(APIkey);

	Handle	= LoadLibrary (x);
	if (Handle == NULL) {
	  fprintf (stderr, "Failed to open mir_sdr_api.dll\n");
	  delete myFrame;
	  throw (22);
	}
#else
	Handle		= dlopen ("libusb-1.0.so", RTLD_NOW | RTLD_GLOBAL);
	Handle		= dlopen ("libmirsdrapi-rsp.so", RTLD_NOW);
	if (Handle == NULL)
	   Handle	= dlopen ("libmir_sdr.so", RTLD_NOW);

	fprintf (stderr, "library loaded\n");
	if (Handle == NULL) {
	   fprintf (stderr, "error report %s\n", dlerror ());
	   delete myFrame;
	   throw (23);
	}
#endif
	libraryLoaded	= true;

	bool success = loadFunctions ();
	if (!success) {
#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
           dlclose (Handle);
#endif
	   delete myFrame;
	   throw (23);
	}

	err	= my_mir_sdr_ApiVersion (&ver);
	if (err != mir_sdr_Success) {
           fprintf (stderr, "error at ApiVersion %s\n",
                         errorCodes (err). toLatin1 (). data ());
#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
           dlclose (Handle);
#endif
           delete myFrame;
           throw (24);
        }

	if (ver < 2.13) {
	   fprintf (stderr, "Library incompatible, upgrade to mirsdr-2.13\n");
#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
           dlclose (Handle);
#endif
	   delete myFrame;
	   throw (24);
	}

	sdrplaySettings -> beginGroup ("sdrplaySettings");
	ifgainSlider      -> setValue (
                    sdrplaySettings -> value ("GRdB", 40). toInt ());
        ppmControl      -> setValue (
                    sdrplaySettings -> value ("ppmOffset", 0). toInt ());
        sdrplaySettings -> endGroup ();

	oscillatorTable	= new std::complex<float> [inputRate];
	for (int32_t i = 0; i < inputRate; i ++)
	   oscillatorTable [i] = std::complex<float> (
	                           cos ((float) i * 2 * M_PI / inputRate),
	                           sin ((float) i * 2 * M_PI / inputRate));
	localShift	= 0;
	oscillatorPhase	= 0;
	filter		= new decimatingFIR (inputRate / outputRate * 8 + 1,
	                                     + outputRate / 2,
	                                     inputRate,
	                                     inputRate / outputRate);
	api_version	-> display (ver);
	lastFrequency	= Khz (2200);
//
	err = my_mir_sdr_GetDevices (devDesc, &numofDevs, uint32_t (4));
	if (err != mir_sdr_Success) {
           fprintf (stderr, "error at GetDevices %s \n",
                           errorCodes (err). toLatin1 (). data ());

#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
           dlclose (Handle);
#endif
           delete myFrame;
           throw (25);
        }

	if (numofDevs == 0) {
	   fprintf (stderr, "Sorry, no device found\n");
#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
           dlclose (Handle);
#endif
	   delete myFrame;
	   throw (25);
	}

	if (numofDevs > 1) {
           sdrplaySelector       = new sdrplaySelect ();
           for (deviceIndex = 0; deviceIndex < numofDevs; deviceIndex ++) {
#ifndef	__MINGW32__
              sdrplaySelector ->
                   addtoList (devDesc [deviceIndex]. DevNm);
#else
              sdrplaySelector ->
                   addtoList (devDesc [deviceIndex]. SerNo);
#endif
           }
           deviceIndex = sdrplaySelector -> QDialog::exec ();
           delete sdrplaySelector;
        }
	else
	   deviceIndex = 0;

	serialNumber -> setText (devDesc [deviceIndex]. SerNo);
	hwVersion = devDesc [deviceIndex]. hwVer;
        fprintf (stderr, "hwVer = %d\n", hwVersion);
	err	= my_mir_sdr_SetDeviceIdx (deviceIndex);
	if (err != mir_sdr_Success) {
           fprintf (stderr, "error at SetDeviceIdx %s \n",
                           errorCodes (err). toLatin1 (). data ());

#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
           dlclose (Handle);
#endif
           delete myFrame;
           throw (25);
        }

        switch (hwVersion) {
           case 1:              // old RSP
              lnaGainSetting    -> setRange (0, 3);
              deviceLabel       -> setText ("RSP-I");
              break;
           case 2:
              lnaGainSetting    -> setRange (0, 8);
              deviceLabel       -> setText ("RSP-II");
              break;
           case 3:
              lnaGainSetting    -> setRange (0, 8);
              deviceLabel       -> setText ("RSP-DUO");
              break;
           default:
              lnaGainSetting    -> setRange (0, 9);
              deviceLabel       -> setText ("RSP-1A");
              break;
        }

	if ((hwVersion == 255) || (hwVersion == 3)) {
           nrBits       = 14;
           denominator  = 8192;
        }
        else {
           nrBits       = 12;
           denominator  = 2048;
        }

        if (hwVersion == 2) {
           mir_sdr_ErrT err;
           antennaSelector -> show ();
           err = my_mir_sdr_RSPII_AntennaControl (mir_sdr_RSPII_ANTENNA_A);
           if (err != mir_sdr_Success)
              fprintf (stderr, "error %d in setting antenna\n", err);
           connect (antennaSelector, SIGNAL (activated (const QString &)),
                    this, SLOT (set_antennaSelect (const QString &)));
        }

       if (hwVersion == 3) {   // duo
           tunerSelector        -> show ();
           err  = my_mir_sdr_rspDuo_TunerSel (mir_sdr_rspDuo_Tuner_1);
           if (err != mir_sdr_Success)
              fprintf (stderr, "error %d in setting of rspDuo\n", err);
           connect (tunerSelector, SIGNAL (activated (const QString &)),
                    this, SLOT (set_tunerControl (const QString &)));
        }

//      and be prepared for future changes in the settings
	connect (ifgainSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (setExternalGain (int)));
	connect (lnaGainSetting, SIGNAL (valueChanged (int)),
	         this, SLOT (setExternalGain (int)));
	connect (agcControl, SIGNAL (stateChanged (int)),
	         this, SLOT (agcControl_toggled (int)));
	connect (ppmControl, SIGNAL (valueChanged (int)),
	         this, SLOT (set_ppmControl (int)));

	running. store (false);	
	agcMode		= false;
	sampleCnt	= 0;
}

	sdrplayHandler::~sdrplayHandler	(void) {
	stopReader ();
	sdrplaySettings -> beginGroup ("sdrplaySettings");
	sdrplaySettings -> setValue   ("GRdBGain",
	                                ifgainSlider -> value ());
	sdrplaySettings -> setValue   ("ppmOffset",
	                                ppmControl -> value ());
	sdrplaySettings -> endGroup ();

	if (!libraryLoaded)
	   return;
	delete	myFrame;

	if (numofDevs > 0)
	   my_mir_sdr_ReleaseDeviceIdx (deviceIndex);

#ifdef __MINGW32__
        FreeLibrary (Handle);
#else
        dlclose (Handle);
#endif
}

int32_t	sdrplayHandler::getRate	(void) {
	return kHz (2112);
}

static inline
int16_t	bankFor_rsp (int32_t freq) {
	if (freq < 60 * MHz (1))
	   return 1;
	if (freq < 120 * MHz (1))
	   return 2;
	if (freq < 250 * MHz (1))
	   return 3;
	if (freq < 420 * MHz (1))
	   return 4;
	if (freq < 1000 * MHz (1))
	   return 5;
	if (freq < 2000 * MHz (1))
	   return 6;
	return -1;
}

static
int	rsp1Tables []	= {4, 4, 4, 4, 4, 4};

static
int	rsp1ATables []	= {7, 10, 10, 10, 10, 9};

static
int	rsp2Tables []	= {9, 9, 9, 9, 6, 6};

static
int	rspduoTables []	= {7, 10, 10, 10, 9};

static
int	lnaStates (int hwVersion, int band) {
	switch (hwVersion) {
	   case 1:
	      return rsp1Tables [band - 1];
	   case 2:
	      return rsp2Tables [band - 1];
	   case 3:
	      return rspduoTables [band - 1];
	   default:
	      return rsp1ATables [band - 1];
	}
}

void	sdrplayHandler::setVFOFrequency		(quint64 newFrequency) {
int	gRdBSystem;
int	samplesPerPacket;
mir_sdr_ErrT	err;
int     GRdB            = ifgainSlider          -> value ();
int     lnaState        = lnaGainSetting        -> value ();

	if (bankFor_rsp ((uint32_t)newFrequency) == -1)
	   return;
	
	if ((uint32_t)newFrequency < inputRate / 2) {
	   localShift	= inputRate / 2 -  (uint32_t)newFrequency;
	   newFrequency = inputRate / 2;
	}
	else
	   localShift = 0;

//	fprintf (stderr, "localShift = %d\n", localShift);
	if (!running. load ()) {
	   lastFrequency = newFrequency;
	   return;
	}

	if (bankFor_rsp ((uint32_t)newFrequency) ==
	                        bankFor_rsp ((uint32_t)lastFrequency)) {
	   err = my_mir_sdr_SetRf (double ((uint32_t)newFrequency), 1, 0);
	   if (err != mir_sdr_Success) 
	      fprintf (stderr, "Error in freq select %s\n", 
	                           errorCodes (err). toLatin1 (). data ());
	   else
	      lastFrequency	= newFrequency;
	   return;
	}

	lnaGainSetting	-> setRange (0, lnaStates (hwVersion,
	                              bankFor_rsp ((uint32_t)lastFrequency)));
	if (lnaState >= lnaStates (hwVersion,
	                              bankFor_rsp ((uint32_t)lastFrequency)))
	   lnaState = 0;

	err	= my_mir_sdr_Reinit (&GRdB,
	                             double (inputRate) / Mhz (1),
	                             double ((uint32_t)newFrequency) / Mhz (1),
	                             mir_sdr_BW_1_536,
	                             mir_sdr_IF_Zero,
	                             mir_sdr_LO_Undefined,	// LOMode
	                             lnaState,
	                             &gRdBSystem,
	                             agcMode,	
	                             &samplesPerPacket,
	                             mir_sdr_CHANGE_RF_FREQ);
	if (err != mir_sdr_Success) 
	   fprintf (stderr, "Error with Reinit %s\n", 
	                               errorCodes (err). toLatin1 (). data ());
	else
	   lastFrequency = newFrequency;
}

quint64	sdrplayHandler::getVFOFrequency	(void) {
	return lastFrequency - localShift;
}

void	sdrplayHandler::setExternalGain	(int newGain) {
	(void)newGain;
int	GRdB		= ifgainSlider	-> value ();
int	lnaState	= lnaGainSetting -> value ();
int	lnaGRdB;
mir_sdr_ErrT err;

	switch (hwVersion) {
	   case 1:
	      lnaGRdB = RSP1_Table [lnaState];
	      break;
	   case 2:
	      lnaGRdB = RSP2_Table [lnaState];
	      break;
	   default:
	      lnaGRdB = RSP1A_Table [lnaState];
	      break;
	}

	err = my_mir_sdr_RSP_SetGr (GRdB, lnaState, 1, 0);
	if (err != mir_sdr_Success)
	   fprintf (stderr, "Error is gain setting %s\n",
	                           errorCodes (err). toLatin1 (). data ());
	else {
	   GRdBDisplay	-> display (GRdB);
	   lnaGRdBDisplay	-> display (lnaGRdB);
	}
}

//
static
void myStreamCallback (int16_t		*xi,
	               int16_t		*xq,
	               uint32_t		firstSampleNum, 
	               int32_t		grChanged,
	               int32_t		rfChanged,
	               int32_t		fsChanged,
	               uint32_t		numSamples,
	               uint32_t		reset,
	               uint32_t		hwRemoved,
	               void		*cbContext) {
int16_t	i;
sdrplayHandler	*p	= static_cast<sdrplayHandler *> (cbContext);
std::complex<float> localBuf [numSamples];
float	denominator	= (float)(p -> denominator);
int	cnt	= 0;

	if (reset || hwRemoved)
	   return;

	for (i = 0; i < (int)numSamples; i ++) {
	   std::complex<float> temp = 
	                    std::complex<float> (float (xi [i]) / denominator,
	                                          float (xq [i]) / denominator);
	   temp	= temp * p -> oscillatorTable [p -> oscillatorPhase];
	   p -> oscillatorPhase += p -> localShift;
	   if (p -> oscillatorPhase < 0)
	      p -> oscillatorPhase += p -> inputRate;
	   if (p -> oscillatorPhase >= p -> inputRate)
	      p -> oscillatorPhase -= p -> inputRate;

	   if (p -> filter -> Pass (temp, &localBuf [cnt])) 
	      if (localBuf [cnt] == localBuf [cnt])
	         cnt ++;
	}

	p -> _I_Buffer -> putDataIntoBuffer (localBuf, cnt);
	p -> sampleCnt += cnt;
	if (p -> sampleCnt > p -> outputRate / 8) {
	   p -> report_dataAvailable ();
	   p -> sampleCnt = 0;
	}

	(void)	firstSampleNum;
	(void)	grChanged;
	(void)	rfChanged;
	(void)	fsChanged;
	(void)	reset;
}

void	myGainChangeCallback (uint32_t	gRdB,
	                      uint32_t	lnaGRdB,
	                      void	*cbContext) {
	(void)gRdB;
	(void)lnaGRdB;	
	(void)cbContext;
}

bool	sdrplayHandler::restartReader	(void) {
int	gRdBSystem;
int	samplesPerPacket;
mir_sdr_ErrT	err;
int	localGred	= ifgainSlider		-> value ();
int	lnaState	= lnaGainSetting	-> value ();

	if (running. load ())
	   return true;

	err	= my_mir_sdr_StreamInit (&localGred,
	                                 double (inputRate) / MHz (1),
	                                 double (lastFrequency) / Mhz (1),
	                                 mir_sdr_BW_1_536,
	                                 mir_sdr_IF_Zero,
	                                 lnaState,
	                                 &gRdBSystem,
	                                 mir_sdr_USE_RSP_SET_GR,
	                                 &samplesPerPacket,
	                                 (mir_sdr_StreamCallback_t)myStreamCallback,
	                                 (mir_sdr_GainChangeCallback_t)myGainChangeCallback,
	                                 this);
	if (err != mir_sdr_Success) {
	   fprintf (stderr, "error at StreamInit = %s\n",
	                             errorCodes (err). toLatin1 (). data ());
	   return false;
	}
	my_mir_sdr_SetPpm (double (ppmControl -> value ()));
	err		= my_mir_sdr_SetDcMode (4, 1);
	err		= my_mir_sdr_SetDcTrackTime (63);
//
	my_mir_sdr_SetSyncUpdatePeriod ((int)(inputRate / 3));
	my_mir_sdr_SetSyncUpdateSampleNum (samplesPerPacket);
	my_mir_sdr_DCoffsetIQimbalanceControl (0, 1);
	running. store (true);
	return true;
}

void	sdrplayHandler::stopReader	(void) {
	if (!running. load ())
	   return;

	my_mir_sdr_StreamUninit	();
	running. store (false);
}

void	sdrplayHandler::resetBuffer	(void) {
	_I_Buffer	-> FlushRingBuffer ();
}

int16_t	sdrplayHandler::bitDepth	(void) {
	return 12;
}

bool	sdrplayHandler::loadFunctions	(void) {
	my_mir_sdr_StreamInit	= (pfn_mir_sdr_StreamInit)
	                    GETPROCADDRESS (this -> Handle,
	                                    "mir_sdr_StreamInit");
	if (my_mir_sdr_StreamInit == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_StreamInit\n");
	   return false;
	}

	my_mir_sdr_StreamUninit	= (pfn_mir_sdr_StreamUninit)
	                    GETPROCADDRESS (this -> Handle,
	                                    "mir_sdr_StreamUninit");
	if (my_mir_sdr_StreamUninit == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_StreamUninit\n");
	   return false;
	}

	my_mir_sdr_SetRf	= (pfn_mir_sdr_SetRf)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetRf");
	if (my_mir_sdr_SetRf == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetRf\n");
	   return false;
	}

	my_mir_sdr_SetFs	= (pfn_mir_sdr_SetFs)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetFs");
	if (my_mir_sdr_SetFs == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetFs\n");
	   return false;
	}

	my_mir_sdr_SetGr	= (pfn_mir_sdr_SetGr)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetGr");
	if (my_mir_sdr_SetGr == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetGr\n");
	   return false;
	}

	my_mir_sdr_RSP_SetGr	= (pfn_mir_sdr_RSP_SetGr)
	                    GETPROCADDRESS (Handle, "mir_sdr_RSP_SetGr");
	if (my_mir_sdr_RSP_SetGr == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_RSP_SetGr\n");
	   return false;
	}

	my_mir_sdr_SetGrParams	= (pfn_mir_sdr_SetGrParams)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetGrParams");
	if (my_mir_sdr_SetGrParams == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetGrParams\n");
	   return false;
	}

	my_mir_sdr_SetDcMode	= (pfn_mir_sdr_SetDcMode)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetDcMode");
	if (my_mir_sdr_SetDcMode == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetDcMode\n");
	   return false;
	}

	my_mir_sdr_SetDcTrackTime	= (pfn_mir_sdr_SetDcTrackTime)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetDcTrackTime");
	if (my_mir_sdr_SetDcTrackTime == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetDcTrackTime\n");
	   return false;
	}

	my_mir_sdr_SetSyncUpdateSampleNum = (pfn_mir_sdr_SetSyncUpdateSampleNum)
	               GETPROCADDRESS (Handle, "mir_sdr_SetSyncUpdateSampleNum");
	if (my_mir_sdr_SetSyncUpdateSampleNum == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetSyncUpdateSampleNum\n");
	   return false;
	}

	my_mir_sdr_SetSyncUpdatePeriod	= (pfn_mir_sdr_SetSyncUpdatePeriod)
	                GETPROCADDRESS (Handle, "mir_sdr_SetSyncUpdatePeriod");
	if (my_mir_sdr_SetSyncUpdatePeriod == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetSyncUpdatePeriod\n");
	   return false;
	}

	my_mir_sdr_ApiVersion	= (pfn_mir_sdr_ApiVersion)
	                GETPROCADDRESS (Handle, "mir_sdr_ApiVersion");
	if (my_mir_sdr_ApiVersion == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_ApiVersion\n");
	   return false;
	}

	my_mir_sdr_AgcControl	= (pfn_mir_sdr_AgcControl)
	                GETPROCADDRESS (Handle, "mir_sdr_AgcControl");
	if (my_mir_sdr_AgcControl == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_AgcControl\n");
	   return false;
	}

        my_mir_sdr_rspDuo_TunerSel = (pfn_mir_sdr_rspDuo_TunerSel)
                       GETPROCADDRESS (Handle, "mir_sdr_rspDuo_TunerSel");
        if (my_mir_sdr_rspDuo_TunerSel == NULL) {
           fprintf (stderr, "Could not find mir_sdr_rspDuo_TunerSel\n");
           return false;
        }

	my_mir_sdr_Reinit	= (pfn_mir_sdr_Reinit)
	                GETPROCADDRESS (Handle, "mir_sdr_Reinit");
	if (my_mir_sdr_Reinit == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_Reinit\n");
	   return false;
	}

	my_mir_sdr_SetPpm	= (pfn_mir_sdr_SetPpm)
	                GETPROCADDRESS (Handle, "mir_sdr_SetPpm");
	if (my_mir_sdr_SetPpm == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetPpm\n");
	   return false;
	}

	my_mir_sdr_DebugEnable	= (pfn_mir_sdr_DebugEnable)
	                GETPROCADDRESS (Handle, "mir_sdr_DebugEnable");
	if (my_mir_sdr_DebugEnable == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_DebugEnable\n");
	   return false;
	}

	my_mir_sdr_DCoffsetIQimbalanceControl	=
	                     (pfn_mir_sdr_DCoffsetIQimbalanceControl)
	                GETPROCADDRESS (Handle, "mir_sdr_DCoffsetIQimbalanceControl");
	if (my_mir_sdr_DCoffsetIQimbalanceControl == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_DCoffsetIQimbalanceControl\n");
	   return false;
	}


	my_mir_sdr_ResetUpdateFlags	= (pfn_mir_sdr_ResetUpdateFlags)
	                GETPROCADDRESS (Handle, "mir_sdr_ResetUpdateFlags");
	if (my_mir_sdr_ResetUpdateFlags == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_ResetUpdateFlags\n");
	   return false;
	}

	my_mir_sdr_GetDevices		= (pfn_mir_sdr_GetDevices)
	                GETPROCADDRESS (Handle, "mir_sdr_GetDevices");
	if (my_mir_sdr_GetDevices == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_GetDevices");
	   return false;
	}

	my_mir_sdr_GetCurrentGain	= (pfn_mir_sdr_GetCurrentGain)
	                GETPROCADDRESS (Handle, "mir_sdr_GetCurrentGain");
	if (my_mir_sdr_GetCurrentGain == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_GetCurrentGain");
	   return false;
	}

	my_mir_sdr_GetHwVersion	= (pfn_mir_sdr_GetHwVersion)
	                GETPROCADDRESS (Handle, "mir_sdr_GetHwVersion");
	if (my_mir_sdr_GetHwVersion == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_GetHwVersion");
	   return false;
	}

	my_mir_sdr_RSPII_AntennaControl	= (pfn_mir_sdr_RSPII_AntennaControl)
	                GETPROCADDRESS (Handle, "mir_sdr_RSPII_AntennaControl");
	if (my_mir_sdr_RSPII_AntennaControl == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_RSPII_AntennaControl");
	   return false;
	}

	my_mir_sdr_SetDeviceIdx	= (pfn_mir_sdr_SetDeviceIdx)
	                GETPROCADDRESS (Handle, "mir_sdr_SetDeviceIdx");
	if (my_mir_sdr_SetDeviceIdx == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_SetDeviceIdx");
	   return false;
	}

	my_mir_sdr_ReleaseDeviceIdx	= (pfn_mir_sdr_ReleaseDeviceIdx)
	                GETPROCADDRESS (Handle, "mir_sdr_ReleaseDeviceIdx");
	if (my_mir_sdr_ReleaseDeviceIdx == NULL) {
	   fprintf (stderr, "Could not find mir_sdr_ReleaseDeviceIdx");
	   return false;
	}

	return true;
}

void	sdrplayHandler::agcControl_toggled (int agcMode) {
	this	-> agcMode	= agcControl -> isChecked ();
	my_mir_sdr_AgcControl (this -> agcMode,
	                -ifgainSlider -> value (), 0, 0, 0, 0, 1);
	if (agcMode == 0)
	   setExternalGain (ifgainSlider -> value ());
}

void	sdrplayHandler::set_ppmControl (int ppm) {
	if (running. load ()) {
	   my_mir_sdr_SetPpm	((float)ppm);
	   my_mir_sdr_SetRf	((float)lastFrequency, 1, 0);
	}
}

void	sdrplayHandler::report_dataAvailable (void) {
	emit dataAvailable (10);
}

void	sdrplayHandler::set_antennaSelect (const QString &s) {
mir_sdr_ErrT err;

	if (hwVersion < 2)	// should not happen
	   return;

	if (s == "Antenna A")
	   err = my_mir_sdr_RSPII_AntennaControl (mir_sdr_RSPII_ANTENNA_A);
	else
	   err = my_mir_sdr_RSPII_AntennaControl (mir_sdr_RSPII_ANTENNA_B);
	(void)err;
}

void	sdrplayHandler::set_tunerSelect (const QString &s) {
mir_sdr_ErrT err;

	if (hwVersion != 3)	// should not happen
	   return;
	if (s == "Tuner 1") 
	   err	= my_mir_sdr_rspDuo_TunerSel (mir_sdr_rspDuo_Tuner_1);
	else
	   err	= my_mir_sdr_rspDuo_TunerSel (mir_sdr_rspDuo_Tuner_2);

	if (err != mir_sdr_Success) 
	   fprintf (stderr, "error %d in selecting  rspDuo\n", err);
}

QString	sdrplayHandler::errorCodes (mir_sdr_ErrT err) {
	switch (err) {
	   case mir_sdr_Success:
	      return "success";
	   case mir_sdr_Fail:
	      return "Fail";
	   case mir_sdr_InvalidParam:
	      return "invalidParam";
	   case mir_sdr_OutOfRange:
	      return "OutOfRange";
	   case mir_sdr_GainUpdateError:
	      return "GainUpdateError";
	   case mir_sdr_RfUpdateError:
	      return "RfUpdateError";
	   case mir_sdr_FsUpdateError:
	      return "FsUpdateError";
	   case mir_sdr_HwError:
	      return "HwError";
	   case mir_sdr_AliasingError:
	      return "AliasingError";
	   case mir_sdr_AlreadyInitialised:
	      return "AlreadyInitialised";
	   case mir_sdr_NotInitialised:
	      return "NotInitialised";
	   case mir_sdr_NotEnabled:
	      return "NotEnabled";
	   case mir_sdr_HwVerError:
	      return "HwVerError";
	   case mir_sdr_OutOfMemError:
	      return "OutOfMemError";
	   case mir_sdr_HwRemoved:
	      return "HwRemoved";
	   default:
	      return "???";
	}
}

