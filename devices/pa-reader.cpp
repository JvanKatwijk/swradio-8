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
#include	"pa-reader.h"
/*
 *	The class describes the reading of
 *	data from the soundcard, storing it into the
 *	ringbuffer and making samples available for the user
 *	This version is special in that it assumes to be in a Qt environment
 *	such that the names of the channels can be put in a combobox.
 */

#define	INPUT_BUFFER	4 * 16384

	paReader::paReader 	(int32_t cardRate, QComboBox *s) {
int16_t	i;

	this	-> inputRate	= cardRate;
	_I_Buffer		= NULL;
	portAudio		= false;
	readerRunning		= false;
	if (Pa_Initialize () != paNoError) {
	   qDebug ("PaInit failed for reader\n");
	   return;
	}

	portAudio		= true;
	_I_Buffer		= new RingBuffer<float>(INPUT_BUFFER);
	qDebug ("Hostapis: %d\n", Pa_GetHostApiCount ());
	for (i = 0; i < Pa_GetHostApiCount (); i ++)
	   qDebug ("Api %d is %s\n", i, Pa_GetHostApiInfo (i) -> name);

	numofDevices	= Pa_GetDeviceCount ();
	inTable		= new int16_t [numofDevices + 1];
	for (i = 0; i < numofDevices; i ++)
	   inTable [i] = -1;
	fprintf (stderr, "numof devices = %d\n", numofDevices);
	istream		= NULL;
	setupChannels (s);
}

	paReader::~paReader	(void) {
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
	delete		_I_Buffer;
}
//
//	for setting up the combobox we need
int16_t	paReader::numberofDevices	(void) {
	return numofDevices;
}

const char	*paReader::inputChannelwithRate (int16_t ch, int32_t rate) {
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

int16_t	paReader::invalidDevice	(void) {
	return numofDevices + 128;
}

bool	paReader::isValidDevice	(int16_t dev) {
	if (dev < 0)
	   return false;
	return (dev < numofDevices);
}

bool	paReader::selectDevice	(int16_t idev) {
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
	                                        (float)inputRate);
	qDebug ("suggested bufsize for reader = %d\n", bufSize);
	err = Pa_OpenStream (
	             &istream,
	             &inputParameters,
	             NULL,
	             inputRate,
	             bufSize,
	             0,
	             this	-> paCallback_i,
	             this
	      );
	if (err != paNoError) {
	   qDebug ("Open istream error\n");
	   return false;
	}
	return true;
}

bool	paReader::restartReader (void) {
PaError	err;

	if (readerRunning ||!Pa_IsStreamStopped (istream))
	   return true;
	paCallbackReturn	= paContinue;
	err = Pa_StartStream (istream);
	if (err == paNoError)
	   readerRunning	= true;
	else {
	   const char *s	= Pa_GetErrorText (err);
	   fprintf (stderr, "Error %s\n", s);
	}
	return readerRunning;
}

void	paReader::stopReader	(void) {
	if (!readerRunning || Pa_IsStreamStopped (istream))
	   return;

	paCallbackReturn = paAbort;
	(void) Pa_StopStream (istream);
	while (!Pa_IsStreamStopped (istream))
	   Pa_Sleep (1);
	readerRunning		= false;
}

bool	paReader::isRunning	(void) {
	return readerRunning;
}

/*
 * 	Notice that, in order to be consistent, outside the reader
 * 	area, we mean with samples full IQ pairs, while in reader/writer
 * 	we mean single values
 */
int32_t	paReader::Samples () {
	return _I_Buffer -> GetRingBufferReadAvailable () / 2;
}

int32_t	paReader::getSamples	(std::complex<float> *V,
	                         int32_t n, uint8_t Mode) {
float	*buf = (float *)alloca (2 * n * sizeof (float));
int32_t	i, realAmount;

	if (!portAudio)
	   return 0;

	while (_I_Buffer -> GetRingBufferReadAvailable () < (uint32_t)(2 * n)) {
	   if (!readerRunning)
	      return 0;
	   Pa_Sleep (1);
	}
	realAmount = _I_Buffer	-> getDataFromBuffer (buf, 2 * n);

	for (i = 0; i < realAmount / 2; i ++) {
	    switch (Mode) {
	       case IandQ:
	          V [i] = std::complex<float> (buf [2 * i], buf [2 * i + 1]);
	          break;
	       case QandI:
	          V [i] = std::complex<float> (buf [2 * i + 1], buf [2 * i]);
	          break;
	    }
	}
	return realAmount / 2;
}
//
//	helper
bool	paReader::InputrateIsSupported (int16_t device, int32_t Rate) {
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

int	paReader::paCallback_i (
		const void*			inputBuffer,
                void*				outputBuffer,
		unsigned long			framesPerBuffer,
		const PaStreamCallbackTimeInfo	*timeInfo,
	        PaStreamCallbackFlags		statusFlags,
	        void				*userData) {
RingBuffer<float>	*inB;
float *inp	= (float *)inputBuffer;
paReader	*ud	= reinterpret_cast <paReader *>(userData);

	(void) outputBuffer;
	(void) timeInfo;
	(void) statusFlags;

	if (inputBuffer != NULL) {
	   if (ud -> paCallbackReturn == paContinue) {
	      inB =  ud -> _I_Buffer;
	      (void) ((inB) -> putDataIntoBuffer (inp, 2 * framesPerBuffer));
	      if ((inB) -> GetRingBufferReadAvailable () >
	                      (uint32_t)(ud -> inputRate / 10)) 
	         emit ud ->  samplesAvailable (ud -> inputRate / 10);
	   }
	}
	return ud -> paCallbackReturn;
}

bool	paReader::setupChannels (QComboBox *s) {
int16_t		icnt	= 1;
int16_t		i;

	s -> insertItem (icnt++, "select stream", QVariant (0));
	for (i = 0; i < numofDevices; i ++) {
	   qDebug ("Investigating Device %d\n", i);
	   const char *si = inputChannelwithRate (i, inputRate);
	   if (si != NULL) {
	      fprintf (stderr, "channels: %d\n", s -> count ());
	      fprintf (stderr, "%d  (%d) -> %s\n", icnt, inTable [icnt], si);
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

bool	paReader::set_StreamSelector (int idx) {
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

