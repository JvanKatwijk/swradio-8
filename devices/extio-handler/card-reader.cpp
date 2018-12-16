#
/*
 *    Copyright (C) 2012, 2013
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

#include	<QDebug>
#include	<QComboBox>
#include	"ringbuffer.h"
#include	"card-reader.h"
#include	"extio-handler.h"
/*
 *	The class describes the reading of
 *	data from the soundcard, storing it into the
 *	ringbuffer and making samples available for the user
 */

	cardReader::cardReader 	(RingBuffer<DSPCOMPLEX> *b,
	                         QComboBox	*s,
	                         ExtioHandler	*p,
	                         int32_t	theRate):
	                                 Reader_base (p, theRate) {
int16_t	i;
	this	-> theBuffer		= b;
	streamSelector			= s;
	this		-> myMaster	= p;
	this		-> theRate	= theRate;
	portAudio		= false;
	readerRunning		= false;
	if (Pa_Initialize () != paNoError) {
	   qDebug ("PaInit failed for reader\n");
	   return;
	}

	portAudio		= true;
	qDebug ("Hostapis: %d\n", Pa_GetHostApiCount ());
	for (i = 0; i < Pa_GetHostApiCount (); i ++)
	   qDebug ("Api %d is %s\n", i, Pa_GetHostApiInfo (i) -> name);

	numofDevices	= Pa_GetDeviceCount ();
	inTable		= new int16_t [numofDevices + 1];
	for (i = 0; i < numofDevices; i ++)
	   inTable [i] = -1;
	istream		= NULL;
	setupChannels (streamSelector);
}

	cardReader::~cardReader	(void) {
	if (!portAudio)
	   return;

	if ((istream != NULL) &&!Pa_IsStreamStopped (istream)) {
	   paCallbackReturn = paAbort;
	   (void) Pa_AbortStream (istream);
	   while (!Pa_IsStreamStopped (istream))
	      Pa_Sleep (1);
	   readerRunning = false;
	}

	if (istream != NULL)
	   Pa_CloseStream (istream);

	Pa_Terminate ();
	delete []	inTable;
}

bool	cardReader::worksFine	(void) {
	return portAudio;
}

//
//	for setting up the combobox we need
int16_t	cardReader::numberofDevices	(void) {
	return numofDevices;
}

const char	*cardReader::inputChannelwithRate (int16_t ch, int32_t rate) {
const	PaDeviceInfo *deviceInfo;
const	char	*name	= NULL;

	if ((ch < 0) || (ch >= numofDevices))
	   return name;

	deviceInfo = Pa_GetDeviceInfo (ch);
	if (deviceInfo == NULL)
	   return name;

	if (deviceInfo -> maxInputChannels <= 0)
	   return name;

	if (InputrateIsSupported (ch, rate))
	   name = deviceInfo -> name;
	return name;
}

int16_t	cardReader::invalidDevice	(void) {
	return numofDevices + 128;
}

bool	cardReader::isValidDevice	(int16_t dev) {
	if (dev < 0)
	   return false;
	return (dev < numofDevices);
}

bool	cardReader::selectDevice	(int16_t idev) {
PaError	err;

	qDebug ("selecting device %d\n", idev);
	if (!isValidDevice (idev))
	   return false;

	qDebug ("device %d is valid\n", idev);
	if ((istream != NULL) &&!Pa_IsStreamStopped (istream)) {
	   paCallbackReturn = paAbort;
	   (void) Pa_StopStream (istream);
	   while (!Pa_IsStreamStopped (istream))
	      Pa_Sleep (1);
	   readerRunning = false;
	}

	if (istream != NULL)
	   Pa_CloseStream (istream);

        inputParameters. device = idev;
	inputParameters. channelCount		= 2;	/* I and Q	*/
	inputParameters. sampleFormat		= paFloat32;
	inputParameters. suggestedLatency	= 
	                 Pa_GetDeviceInfo (idev) -> 
	                       defaultLowInputLatency;
	inputParameters. hostApiSpecificStreamInfo = NULL;

	bufSize	= (int)((float)inputParameters. suggestedLatency * 
	                                        (float)theRate);
	qDebug ("suggested bufsize for reader = %d\n", bufSize);
	err = Pa_OpenStream (
	             &istream,
	             &inputParameters,
	             NULL,
	             theRate,
	             bufSize,
	             0,
	             this	-> paCallback_i,
	             this
	      );
	if (err != paNoError) {
	   qDebug ("Open istream error\n");
	   return false;
	}
	fprintf (stderr, "inputstream successfully opened\n");
	return true;
}

void	cardReader::restartReader (int32_t s) {
PaError	err;
	(void)s;
	fprintf (stderr, "restart(card)reader entered\n");
	if (readerRunning)
	   return;
	paCallbackReturn = paContinue;
	fprintf (stderr, "going to start stream\n");
	err = Pa_StartStream (istream);
	if (err == paNoError)
	   readerRunning	= true;
	else
	   fprintf (stderr, "ERROR = %s\n", Pa_GetErrorText (err));
	fprintf (stderr, "success met restartReader %d \n", readerRunning);
}

void	cardReader::stopReader	(void) {
	if (!readerRunning)
	   return;

	paCallbackReturn = paAbort;
	(void) Pa_StopStream (istream);
	while (!Pa_IsStreamStopped (istream))
	   Pa_Sleep (1);
	readerRunning		= false;
}
//
//	helper
bool	cardReader::InputrateIsSupported (int16_t device, int32_t Rate) {
PaStreamParameters *inputParameters =
	           (PaStreamParameters *)alloca (sizeof (PaStreamParameters)); 

	inputParameters	-> device		= device;
	inputParameters -> channelCount		= 2;	/* I and Q	*/
	inputParameters -> sampleFormat		= paFloat32;
	inputParameters -> suggestedLatency	= 0;
	inputParameters -> hostApiSpecificStreamInfo = NULL;

	return Pa_IsFormatSupported (inputParameters, NULL, Rate) ==
	                                          paFormatIsSupported;
}

int	cardReader::paCallback_i (
		const void*			inputBuffer,
                void*				outputBuffer,
		unsigned long			framesPerBuffer,
		const PaStreamCallbackTimeInfo	*timeInfo,
	        PaStreamCallbackFlags		statusFlags,
	        void				*userData) {
RingBuffer<DSPCOMPLEX>	*inB;
float *inp	= (float *)inputBuffer;
cardReader	*ud	= reinterpret_cast <cardReader *>(userData);

	(void) outputBuffer;
	(void) timeInfo;
	(void) statusFlags;

	if (inputBuffer != NULL) {
	   if (ud -> paCallbackReturn == paContinue) {
	      uint32_t i;
	      DSPCOMPLEX temp [framesPerBuffer];
	      inB =  ud -> theBuffer;
	      for (i = 0; i < framesPerBuffer; i ++)
	         temp [i] = DSPCOMPLEX (inp [2 * i], inp [2 * i + 1]);
	      (void) ((inB) -> putDataIntoBuffer (temp, framesPerBuffer));
	      if ((inB) -> GetRingBufferReadAvailable () > ud -> theRate / 5)
	         ud -> myMaster -> putonQueue (DATA_AVAILABLE);
	   }
	}

	return ud -> paCallbackReturn;
}

bool	cardReader::setupChannels (QComboBox *s) {
int16_t		icnt	= 1;
int16_t		i;

	for (i = 0; i < numofDevices; i ++) {
	   qDebug ("Investigating Device %d\n", i);
	   const char *si = inputChannelwithRate (i, theRate);

	   if (si != NULL) {
	      fprintf (stderr, "%d -> %s\n", inTable [icnt], si);
	      s -> insertItem (icnt, si, QVariant (i));	
	      inTable [icnt] = i;
	      qDebug (" (input): item %d wordt stream %d (%s) \n", 
	                        icnt, i, si);
	      icnt ++;
	   }
	}

	qDebug () << "added items to combobox";
	return icnt > 1;
}

bool	cardReader::set_streamSelector (int idx) {
int16_t	inputDevice;

	inputDevice = inTable [idx + 1];
	qDebug ("with %d going for inputDevice %d\n", idx, inputDevice);

	if (!isValidDevice (inputDevice)) {
	   qDebug ("invalid device\n");
	   return false;
	}

	stopReader	();	// may be superfluous
	if (!selectDevice (inputDevice)) {
	   return false;
	}
	qWarning () << "selected input device " <<idx << inputDevice;
	return true;
}

int16_t	cardReader::bitDepth	(void) {
	return 24;
}

