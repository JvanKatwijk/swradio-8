#
/*
 *    Copyright (C) 2016
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
#
#include	<QFileDialog>
#include	<QSettings>
#include	<QMessageBox>
#include	"extio-handler.h"
#include	"reader-base.h"
#include	"card-reader.h"

using namespace std;
#define	GETPROCADDRESS	GetProcAddress
#define	FREELIBRARY	FreeLibrary
//

//	Interface routines
//	The callback from the Extio*.dll does not have a "context"
//	as parameter, typical example of non OO work.
//	We just do not know what the context of the call is,
//	so we have to create one artificially in order to access
//	our functions
//	Our context here is the instance of the class, stored in
//	a static variable
//
//	When a signal arrives, we pass it on to a (simple) thread
//	that will send a (Qt) signal to the GUI.
//	We create a thread, since sending an external signal directly
//	may happen in the GUI thread in a non-transparant way. The thread
//	will handle the real signalling
//
//	The signals are - obviously - the exthwXXX values, however, we
//	add a signal "DATA_AVAILABLE" to signal the GUI that there
//	is data
static
ExtioHandler	*myContext	= NULL;

static
int	extioCallback (int cnt, int status, float IQoffs, void *IQData) {
	if (cnt > 0) { 	//	we got data
	   if (myContext != NULL && myContext -> isStarted) 
	      myContext -> theReader -> processData (IQoffs, IQData, cnt);
	}
	else	// we got something we cannot deal with
	if (cnt > 0)
	   fprintf (stderr, "XXXXXXXXXXXX%d (%d)\n", cnt, status);
	else
	if (cnt < 0) {
//	there is a signal to do something,
//	queue the signal (and wake up the handling thread)
	   if (myContext != NULL) {
	      myContext -> putonQueue (status);
	   }
	}
	return 1;
}
//
//	the seemingly additional complexity is caused by the fact
//	that naming of files in windows is in MultiBytechars
//
//	We assume that if there are settings possible, they
//	are dealt with by the producer of the extio, so here 
//	no frame whatsoever.
	extioHandler::extioHandler (RadioInterface *theRadio,
                                    int32_t outputRate,
                                    RingBuffer<std::complex<float>> *data,
                                    QSettings *s);

char	temp [256];
wchar_t	*windowsName;
int16_t	wchars_num;
	(void)theRadio;
	this	-> outputRate	= outputRate;
	this	-> theBuffer	= data;
	this	-> settings	= s;

	lastFrequency	= Khz (25000);
	isStarted	= false;
	theReader	= NULL;
	this -> myFrame		= myFrame;
	setupUi (myFrame);
	theSelector	-> hide ();
	myContext	= (ExtioHandler *)this;

	dll_open	= false;
	running		= false;

	QString	dll_file	= "foute boel";
	dll_file = QFileDialog::
	           getOpenFileName (NULL,
	                            tr ("load file .."),
	                            QDir::currentPath (),
	                            tr ("libs (Extio*.dll)"));
	dll_file	= QDir::toNativeSeparators (dll_file);
	if (dll_file == QString ("")) {
	   QMessageBox::warning (NULL, tr ("sdr"),
	                               tr ("incorrect filename\n"));
	   status	-> setText ("no filename");
	   return false;
	}

	wchars_num = MultiByteToWideChar (CP_UTF8, 0,
	                              dll_file. toLatin1 (). data (),
	                              -1, NULL, 0);
	windowsName = new wchar_t [wchars_num];
	MultiByteToWideChar (CP_UTF8, 0,
	                     dll_file. toLatin1 (). data (),
	                     -1, windowsName, wchars_num);
	wcstombs (temp, windowsName, 128);
	Handle		= LoadLibrary (windowsName);
//	fprintf (stderr, "Last error = %ld\n", GetLastError ());
	if (Handle == NULL) {
	   QMessageBox::warning (NULL, tr ("sdr"),
	                               tr ("loading dll failed\n"));
	   status	-> setText ("incorrect dll?");
	   throw (21);
	}

	if (!loadFunctions ()) {
	   QMessageBox::warning (NULL, tr ("sdr"),
	                               tr ("loading functions failed\n"));
	   status	-> setText ("could not load all functions");
	   throw (22);
	}

//	apparently, the library is open, so record that
	dll_open	= true;
//	
//	and start the rig
	rigName		= new char [128];
	rigModel	= new char [128];
	if (!((*InitHW) (rigName, rigModel, hardwareType))) {
	   QMessageBox::warning (NULL, tr ("sdr"),
	                               tr ("init failed\n"));
	   status	-> setText ("could not init device");
	   return false;
	}

	fprintf (stderr, "hardware type = %d\n", hardwareType);
//
//	for the sw receiver, we only support card reader-based
//	devices
//	theSelector	-> hide ();
	switch (hardwareType) {
	   default:
	      QMessageBox::warning (NULL, tr ("sdr"),
	                               tr ("device not supported\n"));
	      status	-> setText ("unsupported device");
	      return false;

	   case exthwSCdata:
	      theSelector -> show ();
	      theReader	= new paReader (inputRate, theSelector);
	      connect (theSelector, SIGNAL (activated (int)),
	               this, SLOT (set_streamSelector (int)));
	      connect (theReader, SIGNAL (samplesAvailable (int)),
	               this, SLOT (samplesAvailable (int)));
	      break;
	}

	SetCallback (extioCallback);
	if (!(*OpenHW)()) {
	   QMessageBox::warning (NULL, tr ("sdr"),
	                               tr ("Opening hardware failed\n"));
	   status	-> setText ("Opening hardware failed");
	   return false;
	}

	status	-> setText ("It seems we are running");
	theReader	-> setInputrate (get_deviceRate ());

	ShowGUI ();
	fprintf (stderr, "Hw open successful\n");
	start ();
	return true;
}

	ExtioHandler::~ExtioHandler	(void) {
	if (dll_open) {
	   HideGUI ();
	   StopHW ();
	   CloseHW ();
	}
//
//	The external device should be quiet now
	if (running) {
	   commandQueue. clear	();
	   commandQueue. enqueue (-1);
	   commandHandler. wakeAll ();
	   while (!isFinished ())
	      usleep (100);
	}
//
//	The signal handling task is now terminated
	fprintf (stderr, "extio thread is finished now\n");
	if (Handle != NULL)
	   FREELIBRARY (Handle);
	if (theReader != NULL)
	   delete theReader;
//	just in case
	theSelector	-> hide ();
}

bool	ExtioHandler::loadFunctions (void) {
//	start binding addresses, 
	InitHW		= (pfnInitHW)GETPROCADDRESS (Handle, "InitHW");
	if (InitHW == NULL) {
	   fprintf (stderr, "Failed to load InitHW\n");
	   return false;
	}

	OpenHW		= (pfnOpenHW)GETPROCADDRESS (Handle, "OpenHW");
	if (OpenHW == NULL) {
	   fprintf (stderr, "Failed to load OpenHW\n");
	   return false;
	}

	StartHW		= (pfnStartHW)GETPROCADDRESS (Handle, "StartHW");
	if (StartHW == NULL) {
	   fprintf (stderr, "Failed to load StartHW\n");
	   return false;
	}

	StopHW		= (pfnStopHW)GETPROCADDRESS (Handle, "StopHW");
	if (StopHW == NULL) {
	   fprintf (stderr, "Failed to load StopHW\n");
	   return false;
	}

	CloseHW		= (pfnCloseHW)GETPROCADDRESS (Handle, "CloseHW");
	if (CloseHW == NULL) {
	   fprintf (stderr, "Failed to load CloseHW\n");
	   return false;
	}

	GetHWLO		= (pfnGetHWLO)GETPROCADDRESS (Handle, "GetHWLO");
	if (GetHWLO == NULL) {
	   fprintf (stderr, "Failed to load GetHWLO\n");
	   return false;
	}

	SetHWLO		= (pfnSetHWLO)GETPROCADDRESS (Handle, "SetHWLO");
	if (SetHWLO == NULL) {
	   fprintf (stderr, "Failed to load SetHWLO\n");
	   return false;
	}

	GetStatus	= (pfnGetStatus)GETPROCADDRESS (Handle, "GetStatus");
	if (GetStatus == NULL) {
	   fprintf (stderr, "Failed to load GetStatus\n");
	   return false;
	}

	SetCallback	= (pfnSetCallback)
	                      GETPROCADDRESS (Handle, "SetCallback");
	if (SetCallback == NULL) {
	   fprintf (stderr, "Failed to load SetCallback\n");
	   return false;
	}
//
//	the "non essentials", packed in envelope functions:
	L_ShowGUI	= (pfnShowGUI)GETPROCADDRESS (Handle, "ShowGUI");
	L_HideGUI	= (pfnHideGUI)GETPROCADDRESS (Handle, "HideGUI");
	L_GetHWSR	= (pfnGetHWSR)GETPROCADDRESS (Handle, "GetHWSR");
	L_GetFilters	= (pfnGetFilters)GETPROCADDRESS (Handle, "GetFilters");
	L_GetTune	= (pfnGetTune)GETPROCADDRESS (Handle, "GetTune");
	L_GetMode	= (pfnGetMode)GETPROCADDRESS (Handle, "GetMode");
//
	return true;
}

int32_t	ExtioHandler::get_deviceRate	(void) {
	return GetHWSR ();
}

void	ExtioHandler::setVFOFrequency (int32_t f) {
	fprintf (stderr, "setting freq to %d\n", f);
	(void)((*SetHWLO) ((int)f));
	lastFrequency = f;
}

int32_t	ExtioHandler::getVFOFrequency (void) {
	return lastFrequency;
}

bool	ExtioHandler::legalFrequency	(int32_t f) {
	(void)f;
	return true;
}

int32_t	ExtioHandler::defaultFrequency	(void) {
	return Khz (14070);
}
//
//	envelopes for functions that might or might not
//	be available
void	ExtioHandler::ShowGUI		(void) {
	if (L_ShowGUI != NULL)
	   (*L_ShowGUI) ();
}

void	ExtioHandler::HideGUI		(void) {
	if (L_HideGUI != NULL)
	   (*L_HideGUI) ();
}

long	ExtioHandler::GetHWSR		(void) {
	return L_GetHWSR != NULL ? (*L_GetHWSR) () : outputRate;
}

void	ExtioHandler::GetFilters		(int &x, int &y, int &z) {
	if (L_GetFilters != NULL)
	   L_GetFilters (x, y, z);
}

long	ExtioHandler::GetTune		(void) {
	return L_GetTune != NULL ? (*L_GetTune) () : MHz (25);
}

uint8_t	ExtioHandler::GetMode		(void) {
	return L_GetMode != NULL ? (*L_GetMode)() : 0;
}
//
//
//	Handling the data
bool	ExtioHandler::restartReader	(void) {
	fprintf (stderr, "restart reader entered (%d)\n", lastFrequency);
int32_t	size	= (*StartHW)(lastFrequency);
	fprintf (stderr, "restart reader returned with %d\n", size);
	theReader -> restartReader (size);
	fprintf (stderr, "now we have restarted the reader\n");
	isStarted	= true;
	return true;
}

void	ExtioHandler::stopReader	(void) {
	if (isStarted) {
	   (*StopHW)();
	   theReader	-> stopReader ();
	   isStarted = false;
	}
}

//
void	ExtioHandler::set_Changed_LO	(int32_t newFreq) {
	emit set_ExtLO (newFreq);
}

void	ExtioHandler::set_Changed_TUNE	(int32_t newFreq) {
	emit set_ExtFrequency (newFreq);
}

void	ExtioHandler::set_Lock_LO	(void) {
	emit set_lockLO ();
}

void	ExtioHandler::set_Unlock_LO	(void) {
	emit set_unlockLO ();
}

void	ExtioHandler::set_StartHW	(void) {
	emit set_startHW ();
}

void	ExtioHandler::set_StopHW		(void) {
	emit set_stopHW ();
}

int16_t	ExtioHandler::bitDepth		(void) {
	return	theReader	-> bitDepth ();
}

void	ExtioHandler::samplesAvailable  (int n) {
std::complex<float> b [512];
int16_t i;

        (void)n;
        while (theReader -> Samples () > 512) {
           myReader -> getSamples (b, 512, Mode);
           for (i = 0; i < 512; i ++)
              b [i] = cmul (b [i], gainValue / 10.0);
           theBuffer -> putDataIntoBuffer (b, 512);
        }

        if (theBuffer -> GetRingBufferReadAvailable () > 1024)
           dataAvailable (1024);
}


void	ExtioHandler::set_streamSelector (int idx) {
	fprintf (stderr, "Setting stream %d\n", idx);
	if (!((cardReader *)theReader) -> set_streamSelector (idx))
	   QMessageBox::warning (NULL, tr ("sdr"),
	                               tr ("Selecting  input stream failed\n"));
}
//
//	It is unclear where the external events come from. There were
//	some problems with two events occurring directly after each other,
//	blowing up the GUI thread. We therefore queue the events here and
//	send them to the gui.
void	ExtioHandler::run	(void) {

	running		= true;
	while (running) {
	   helper. lock ();
	   commandHandler. wait (&helper, 100);
	   helper. unlock ();
	   while (!commandQueue. isEmpty ()) {
	      int command = commandQueue. dequeue ();
	      switch (command) {
	         case -1:
	            running = false;
	            commandQueue. clear ();
	            break;
	      // for the SDR14
	         case extHw_Disconnected:
	         case extHw_READY:
	         case extHw_RUNNING:
	         case extHw_ERROR:
	         case extHw_OVERLOAD:
	         case extHw_Changed_SampleRate:	// 100
	                   ;			// for now
	            break;

	         case extHw_Changed_LO:		// 101
	            set_Changed_LO (GetHWLO ()); 
	            break;
	         case extHw_Lock_LO:		// 102
	            set_Lock_LO ();
	            break;
	         case extHw_Unlock_LO:		// 103
	            set_Unlock_LO ();
	            break;
	         case extHw_Changed_LO_Not_TUNE:	// not implemented
	            break;
	         case extHw_Changed_TUNE:		// 105
	            {  long t =  GetTune ();
	               if (t != -1)
	                   set_Changed_TUNE (t);
	            }
	            break;
//
//	we might, in the future, handle the changed mode by
//	changing the settings of the bandwidth
	         case extHw_Changed_MODE:		// 106
	            fprintf (stderr, "Mode is changed to %d\n", GetMode ());
	            break;
	         case extHw_Start:			// 107
	            set_StartHW ();
	            break;
	         case extHw_Stop:			// 108
	            set_StopHW ();
	            break;
//
//	do not know how to handle:
	         case extHw_Changed_FILTER:		// 109
	            {  int x1, x2, x3;
	               GetFilters (x1, x2, x3);
	               fprintf (stderr, "Filters to be changed %d %d %d\n",
	                                                      x1, x2, x3);
	            }
	            break;
//
//	this is not an official extio signal, however, we add
//	it to the list. Fast going devices with small buffers
//	might overload the system, so we adapt the rate with which
//	the signals are sent.
	         case DATA_AVAILABLE:
	            emit samplesAvailable (100);	// number is dummy
	            break;

	         default:
	            break;
	      }
	   }
	}
L1:;
}

void	ExtioHandler::putonQueue	(int s) {
	commandQueue. enqueue (s);
	commandHandler. wakeAll ();
}

void	ExtioHandler::exit		(void) {
	running	= false;
}

bool	ExtioHandler::isOK		(void) {
	return Handle != NULL;
}

int32_t	ExtioHandler::getRate		(void) {
	return outputRate;
}

