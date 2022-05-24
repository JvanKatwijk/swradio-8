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
//#include	"sdrplayselect.h"
#include	"radio.h"

//
//	lna gain reduction tables, per band.
//	The first number in each row tells the number of valid elements
static
int     RSP1_Table [6] [5] = {{4, 0, 24, 19, 43},
	                      {4, 0, 24, 19, 43},
	                      {4, 0, 24, 19, 43},
	                      {4, 0, 24, 19, 43},
	                      {4, 0,  7, 19, 26},
	                      {4, 0,  5, 19, 24} };

static
int     RSP1A_Table [6] [11] = {
	{7,  0, 6, 12, 18, 37, 42, 61, -1, -1, -1},
	{10, 0, 6, 12, 18, 20, 26, 32, 38, 57, 62},
	{10, 0, 6, 12, 18, 20, 26, 32, 38, 57, 62},
	{10, 0, 6, 12, 18, 20, 26, 32, 38, 57, 62},
	{10, 0, 7, 13, 19, 20, 27, 33, 39, 45, 64},
	{10, 0, 6, 12, 20, 26, 32, 38, 43, 62, -1}};


static
int     RSP2_Table [6] [10] = {
	{9, 0, 10, 15, 21, 24, 34, 39, 45, 64},
	{9, 0, 10, 15, 21, 24, 34, 39, 45, 64},
	{9, 0, 10, 15, 21, 24, 34, 39, 45, 64},
	{9, 0, 10, 15, 21, 24, 34, 39, 45, 64},
	{6, 0,  7, 10, 17, 22, 41, -1, -1, -1},
	{6, 0,  5, 21, 15, 15, 32, -1, -1, -1}};

static
int     get_lnaGRdB (int hwVersion, int lnaState, int band) {
        switch (hwVersion) {
           case 1:
              return RSP1_Table [band][lnaState + 1];

           case 2:
              return RSP2_Table [band][lnaState + 1];

           default:
              return RSP1A_Table [band][lnaState + 1];
        }
}

static inline
int16_t	bankFor_rsp (int32_t freq) {
	if (freq < 60 * MHz (1))
	   return 0;
	if (freq < 120 * MHz (1))
	   return 1;
	if (freq < 250 * MHz (1))
	   return 2;
	if (freq < 420 * MHz (1))
	   return 3;
	if (freq < 1000 * MHz (1))
	   return 4;
	if (freq < 2000 * MHz (1))
	   return 5;
	return -1;
}

static	inline
int16_t	bank_rsp1 (int32_t freq) {
	if (freq < 12 * MHz (1))
	   return mir_sdr_BAND_AM_LO;
	if (freq < 30 * MHz (1))
	   return mir_sdr_BAND_AM_MID;
	if (freq < 60 * MHz (1))
	   return mir_sdr_BAND_AM_HI;
        if (freq < 120 * MHz (1))
	   return mir_sdr_BAND_VHF;
        if (freq < 250 * MHz (1))
	   return mir_sdr_BAND_3;
        if (freq < 420 * MHz (1))
	   return mir_sdr_BAND_X;
        if (freq < 1000 * MHz (1))
	   return mir_sdr_BAND_4_5;
        if (freq < 2000 * MHz (1))
           return mir_sdr_BAND_L;;
        return -1;
}

	sdrplayHandler::sdrplayHandler  (RadioInterface *mr,
	                                 int32_t	outputRate,
	                                 RingBuffer<DSPCOMPLEX> *r,
	                                 QSettings	*s):
	                                              deviceHandler (mr) {
mir_sdr_ErrT err;
float	ver;
mir_sdr_DeviceT devDesc [4];
//sdrplaySelect	*sdrplaySelector;

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

	wchar_t *libname = (wchar_t *)L"mir_sdr_api.dll";
	Handle  = LoadLibrary (libname);
	if (Handle == NULL) {
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
	   wchar_t *x =
	          wcscat (APIkeyValue, (wchar_t *)L"\\x86\\mir_sdr_api.dll");
	   RegCloseKey(APIkey);

	   Handle	= LoadLibrary (x);
	   if (Handle == NULL) {
	      fprintf (stderr, "Failed to open mir_sdr_api.dll\n");
	      delete myFrame;
	      throw (22);
	   }
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
	GRdBSelector      -> setValue (
                    sdrplaySettings -> value ("sdrplay-ifgrdb", 40). toInt ());
	lnaGainSetting          -> setValue (
                   sdrplaySettings -> value ("sdrplay-lnastate", 0). toInt ());

        ppmControl      -> setValue (
                    sdrplaySettings -> value ("ppmOffset", 0). toInt ());

	agcMode         =
                    sdrplaySettings -> value ("sdrplay-agcMode", 0). toInt ();
        if (agcMode) {
           agcControl -> setChecked (true);
           GRdBSelector         -> hide ();
           gainsliderLabel      -> hide ();
        }

        sdrplaySettings -> endGroup ();

	oscillatorTable	= new std::complex<float> [inputRate];
	for (int32_t i = 0; i < (int)inputRate; i ++)
	   oscillatorTable [i] = std::complex<float> (
	                           cos ((float) i * 2 * M_PI / inputRate),
	                           sin ((float) i * 2 * M_PI / inputRate));
	localShift	= 0;
	oscillatorPhase	= 0;
	filter		= new decimatingFIR (inputRate / outputRate * 5 - 1,
	                                     + outputRate / 2,
	                                     inputRate,
	                                     inputRate / outputRate);
	api_version	-> display (ver);
	lastFrequency	= Khz (14070);
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

//	if (numofDevs > 1) {
//           sdrplaySelector       = new sdrplaySelect ();
//           for (deviceIndex = 0; deviceIndex < numofDevs; deviceIndex ++) {
//#ifndef	__MINGW32__
//              sdrplaySelector ->
//                   addtoList (devDesc [deviceIndex]. DevNm);
//#else
//              sdrplaySelector ->
//                   addtoList (devDesc [deviceIndex]. SerNo);
//#endif
//           }
//           deviceIndex = sdrplaySelector -> QDialog::exec ();
//           delete sdrplaySelector;
//        }
//	else
	if (numofDevs >= 1)
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
	      nrBits		= 12;
	      denominator	= 2048;
              break;
           case 2:
              lnaGainSetting    -> setRange (0, 8);
              deviceLabel       -> setText ("RSP-II");
	      nrBits		= 12;
	      denominator	= 2048;
              antennaSelector -> show ();
              err = my_mir_sdr_RSPII_AntennaControl (mir_sdr_RSPII_ANTENNA_A);
              if (err != mir_sdr_Success)
                 fprintf (stderr, "error %d in setting antenna\n", err);
              connect (antennaSelector, SIGNAL (activated (const QString &)),
                       this, SLOT (set_antennaSelect (const QString &)));
              break;
           case 3:
              lnaGainSetting    -> setRange (0, 8);
              deviceLabel       -> setText ("RSP-DUO");
	      nrBits		= 14;
	      denominator	= 8192;
              tunerSelector        -> show ();
              err  = my_mir_sdr_rspDuo_TunerSel (mir_sdr_rspDuo_Tuner_1);
              if (err != mir_sdr_Success)
                 fprintf (stderr, "error %d in setting of rspDuo\n", err);
              connect (tunerSelector, SIGNAL (activated (const QString &)),
                       this, SLOT (set_tunerControl (const QString &)));
              break;
           default:
              lnaGainSetting    -> setRange (0, 9);
              deviceLabel       -> setText ("RSP-1A");
	      nrBits		= 14;
	      denominator	= 8192;
              break;
        }

	lnaGRdBDisplay -> display (get_lnaGRdB (hwVersion,
	                           lnaGainSetting -> value (),
	                           bankFor_rsp (lastFrequency)));

//      and be prepared for future changes in the settings
	connect (GRdBSelector, SIGNAL (valueChanged (int)),
	         this, SLOT (set_ifgainReduction (int)));
	connect (lnaGainSetting, SIGNAL (valueChanged (int)),
	         this, SLOT (set_lnagainReduction (int)));
	connect (agcControl, SIGNAL (stateChanged (int)),
	         this, SLOT (agcControl_toggled (int)));
	connect (debugControl, SIGNAL (stateChanged (int)),
	         this, SLOT (debugControl_toggled (int)));
	connect (ppmControl, SIGNAL (valueChanged (int)),
	         this, SLOT (set_ppmControl (int)));

	running. store (false);	
	sampleCnt	= 0;
}

	sdrplayHandler::~sdrplayHandler	(void) {
	stopReader ();
	sdrplaySettings -> beginGroup ("sdrplaySettings");
	sdrplaySettings -> setValue   ("ppmOffset",
	                                ppmControl -> value ());
	sdrplaySettings -> setValue ("sdrplay-ifgrdb",
	                              GRdBSelector -> value ());
        sdrplaySettings -> setValue ("sdrplay-lnastate",
                                            lnaGainSetting -> value ());
        sdrplaySettings -> setValue ("sdrplay-agcMode",
                                          agcControl -> isChecked () ? 1 : 0);

	sdrplaySettings -> endGroup ();

	if (!libraryLoaded)	// should not happen
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

static	inline
int16_t	bank_rsp2 (int32_t freq) {
	if (freq < 12 * MHz (1))
	   return mir_sdr_BAND_AM_LO;
	if (freq < 30 * MHz (1))
	   return mir_sdr_BAND_AM_MID;
	if (freq < 60 * MHz (1))
	   return mir_sdr_BAND_AM_HI;
        if (freq < 120 * MHz (1))
	   return mir_sdr_BAND_VHF;
        if (freq < 250 * MHz (1))
	   return mir_sdr_BAND_3;
        if (freq < 420 * MHz (1))
	   return mir_sdr_BAND_X;
        if (freq < 1000 * MHz (1))
	   return mir_sdr_BAND_4_5;
        if (freq < 2000 * MHz (1))
           return mir_sdr_BAND_L;;
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
	      return rsp1Tables [band];
	   case 2:
	      return rsp2Tables [band];
	   case 3:
	      return rspduoTables [band];
	   default:
	      return rsp1ATables [band];
	}
}

void	sdrplayHandler::setVFOFrequency		(quint64 newFrequency) {
int	gRdBSystem;
int	samplesPerPacket;
mir_sdr_ErrT	err;
int     GRdB            = GRdBSelector          -> value ();
int     lnaState        = lnaGainSetting        -> value ();

	if (bank_rsp1 ((uint32_t)newFrequency) == -1)
	   return;
	
	if (newFrequency < inputRate / 2) {
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

	if (bank_rsp1 ((uint32_t)newFrequency) ==
	                        bank_rsp1 ((uint32_t)lastFrequency)) {
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
	int	bank	= bankFor_rsp (lastFrequency);
	lnaGRdBDisplay -> display (get_lnaGRdB (hwVersion, lnaState, bank));
}

quint64	sdrplayHandler::getVFOFrequency	(void) {
	return lastFrequency - localShift;
}

void	sdrplayHandler::set_ifgainReduction (int newGain) {
	(void)newGain;
int	GRdB		= GRdBSelector	-> value ();
int	lnaState	= lnaGainSetting -> value ();
mir_sdr_ErrT err;
int	bank		= bankFor_rsp (getVFOFrequency ());
	if (agcMode)
	   return;

	err = my_mir_sdr_RSP_SetGr (GRdB, lnaState, 1, 0);
	if (err != mir_sdr_Success)
	   fprintf (stderr, "Error is gain setting %s\n",
	                           errorCodes (err). toLatin1 (). data ());
	else {
	   lnaGRdBDisplay -> display (get_lnaGRdB (hwVersion, lnaState, bank));
	}
}

void    sdrplayHandler::set_lnagainReduction (int dummy) {
mir_sdr_ErrT err;
int	bank	= bankFor_rsp (getVFOFrequency ());
int	lnaState	= lnaGainSetting -> value ();

	(void)dummy;
        if (!agcControl -> isChecked ()) {
           set_ifgainReduction (0);
           return;
        }
	
        err     = my_mir_sdr_AgcControl (true, -30, 0, 0, 0, 0, lnaState);
        if (err != mir_sdr_Success)
           fprintf (stderr, "Error at set_lnagainReduction %s\n",
                               errorCodes (err). toLatin1 (). data ());
	else
	   lnaGRdBDisplay       -> display (get_lnaGRdB (hwVersion,
	                                                 lnaState,
	                                                 bank));
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
	   if (p -> oscillatorPhase >=  (int)p -> inputRate)
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

void	myGainChangeCallback (uint32_t	GRdB,
	                      uint32_t	lnaGRdB,
	                      void	*cbContext) {
sdrplayHandler	*p	= static_cast<sdrplayHandler *> (cbContext);
	(void)lnaGRdB;
//	p -> lnaGRdBDisplay	-> display ((int)lnaGRdB);
}

bool	sdrplayHandler::restartReader	(void) {
int	gRdBSystem;
int	samplesPerPacket;
mir_sdr_ErrT	err;
int	localGred	= GRdBSelector		-> value ();
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
        if (agcControl -> isChecked ()) {
           my_mir_sdr_AgcControl (this -> agcMode,
                                  -30,
                                  0, 0, 0, 0, lnaGainSetting -> value ());
           GRdBSelector         -> hide ();
           gainsliderLabel      -> hide ();
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
	                -30, 0, 0, 0, 0, lnaGainSetting -> value ());
	if (agcMode == 0) {
	   GRdBSelector         -> show ();
	   gainsliderLabel      -> show ();
	   set_ifgainReduction (0);
        }
        else {
	   GRdBSelector         -> hide ();
	   gainsliderLabel      -> hide ();
	}
}

void    sdrplayHandler::debugControl_toggled (int debugMode) {
	(void)debugMode;
        my_mir_sdr_DebugEnable (debugControl -> isChecked () ? 1 : 0);
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

