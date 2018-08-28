#
/*
 *    Copyright (C) 2011 .. 2017
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

#include	"audiosink.h"
#include	<QDebug>
/*
 *	The class is the sink for the data generated
 */
	audioSink::audioSink	(int32_t rate, int32_t size) {
int32_t	i;

	this	-> CardRate	= rate;
	if ((size & (size - 1)) != 0)
	   this	-> size		= 16384;
	else
	   this	-> size		= size;
	this -> Latency		= 1;
	_O_Buffer		= new RingBuffer<float>(4 * 32768);
	portAudio		= false;
	writerRunning		= false;

	if (Pa_Initialize () != paNoError) {
	   fprintf (stderr, "Initializing Pa for output failed\n");
	   return;
	}

	portAudio	= true;
	qDebug ("Hostapis: %d\n", Pa_GetHostApiCount ());

	for (i = 0; i < Pa_GetHostApiCount (); i ++)
	   qDebug ("Api %d is %s\n", i, Pa_GetHostApiInfo (i) -> name);

	numofDevices	= Pa_GetDeviceCount ();
	ostream		= NULL;
	dumpFile	= NULL;
}

	audioSink::~audioSink	(void) {
	if ((ostream != NULL) && !Pa_IsStreamStopped (ostream)) {
	   paCallbackReturn = paAbort;
	   (void) Pa_AbortStream (ostream);
	   while (!Pa_IsStreamStopped (ostream))
	      Pa_Sleep (1);
	   writerRunning = false;
	}

	if (ostream != NULL)
	   Pa_CloseStream (ostream);

	if (portAudio)
	   Pa_Terminate ();

	delete	_O_Buffer;
}
//
bool	audioSink::selectDevice (int16_t odev) {
PaError err;

	if (!isValidDevice (odev))
	   return false;

	if ((ostream != NULL) && !Pa_IsStreamStopped (ostream)) {
	   paCallbackReturn = paAbort;
	   (void) Pa_AbortStream (ostream);
	   while (!Pa_IsStreamStopped (ostream))
	      Pa_Sleep (1);
	   writerRunning = false;
	}

	if (ostream != NULL)
	   Pa_CloseStream (ostream);

	outputParameters. device		= odev;
	outputParameters. channelCount		= 2;
	outputParameters. sampleFormat		= paFloat32;
	outputParameters. suggestedLatency	= Latency *
	                             Pa_GetDeviceInfo (odev) ->
	                                      defaultHighOutputLatency;
	bufSize	= (int)((float)outputParameters. suggestedLatency * 
	                                         (float)CardRate);

	if (bufSize < 0 || bufSize > 16300)
	   bufSize = 2 * 8192;

	outputParameters. hostApiSpecificStreamInfo = NULL;
//
	fprintf (stderr, "Suggested size for outputbuffer = %d\n", bufSize);
	err = Pa_OpenStream (
	             &ostream,
	             NULL,
	             &outputParameters,
	             CardRate,
	             bufSize,
	             0,
	             this	-> paCallback_o,
	             this
	      );

	if (err != paNoError) {
	   fprintf (stderr, "Open ostream error\n");
	   return false;
	}
	paCallbackReturn = paContinue;
	err = Pa_StartStream (ostream);
	writerRunning	= true;
	return true;
}

void	audioSink::restart	(void) {
PaError err;

//	if (!Pa_IsStreamStopped (ostream))
//	   return;

	paCallbackReturn = paContinue;
	err = Pa_StartStream (ostream);
	if (err == paNoError)
	   writerRunning	= true;

	fprintf (stderr, "writer is %s\n", writerRunning ? "OK": "niet OK");
}

void	audioSink::stop	(void) {
	if (Pa_IsStreamStopped (ostream))
	   return;

	paCallbackReturn	= paAbort;
	(void)Pa_StopStream	(ostream);
	while (!Pa_IsStreamStopped (ostream))
	   Pa_Sleep (1);
	writerRunning		= false;
}

//
//	helper
bool	audioSink::OutputrateIsSupported (int16_t device, int32_t Rate) {
PaStreamParameters *outputParameters =
	           (PaStreamParameters *)alloca (sizeof (PaStreamParameters)); 

	outputParameters -> device		= device;
	outputParameters -> channelCount	= 2;	/* I and Q	*/
	outputParameters -> sampleFormat	= paFloat32;
	outputParameters -> suggestedLatency	= 0;
	outputParameters -> hostApiSpecificStreamInfo = NULL;

	return Pa_IsFormatSupported (NULL, outputParameters, Rate) ==
	                                          paFormatIsSupported;
}
/*
 * 	... and the callback
 */
int	audioSink::paCallback_o (
		const void*			inputBuffer,
                void*				outputBuffer,
		unsigned long			framesPerBuffer,
		const PaStreamCallbackTimeInfo	*timeInfo,
	        PaStreamCallbackFlags		statusFlags,
	        void				*userData) {
RingBuffer<float>	*outB;
float	*outp		= (float *)outputBuffer;
audioSink *ud		= reinterpret_cast <audioSink *>(userData);
uint32_t	actualSize;
uint32_t	i;

	(void)statusFlags;
	(void)inputBuffer;
	(void)timeInfo;
	if (statusFlags & (paOutputUnderflow|paOutputOverflow)) {
	    const std::string runtype (statusFlags&paOutputUnderflow ? "Under" : "Over");
	    qDebug("%srun flag in PA callback", runtype.c_str());
	}

	if (ud -> paCallbackReturn == paContinue) {
	   outB = (reinterpret_cast <audioSink *>(userData)) -> _O_Buffer;
	   actualSize = outB -> getDataFromBuffer (outp, 2 * framesPerBuffer);
	   for (i = actualSize; i < 2 * framesPerBuffer; i ++)
	      outp [i] = 0;
	}

	return ud -> paCallbackReturn;
}

static inline
int32_t	minimum (int32_t a, int32_t b) {
	return a > b ? b : a;
}
//
//	Just for my own curiosity I want to know to what degree
//	the buffer is filled
int32_t	audioSink::capacity	(void) {
	return _O_Buffer -> GetRingBufferWriteAvailable () / 2;
}
//
//	putSample output comes from the FM receiver

int32_t	audioSink::putSample	(DSPCOMPLEX v) {
	return putSamples (&v, 1);
}

int32_t	audioSink::putSamples		(DSPCOMPLEX *V, int32_t n) {
float	*buffer = (float *)alloca (2 * n * sizeof (float));
int32_t	i;
int32_t	available = _O_Buffer -> GetRingBufferWriteAvailable ();

	if (2 * n > available)
	   n = (available / 2) & ~01;
	for (i = 0; i < n; i ++) {
	   buffer [2 * i] = real (V [i]);
	   buffer [2 * i + 1] = imag (V [i]);
	}

	if (dumpFile != NULL)
	   sf_writef_float (dumpFile, buffer, n);
	_O_Buffer	-> putDataIntoBuffer (buffer, 2 * n);
	return n;
}

int16_t	audioSink::numberofDevices	(void) {
	return numofDevices;
}

const char	*audioSink::outputChannelwithRate (int16_t ch, int32_t rate) {
const PaDeviceInfo *deviceInfo;
const	char	*name	= NULL;

	if ((ch < 0) || (ch >= numofDevices))
	   return name;

	deviceInfo = Pa_GetDeviceInfo (ch);
	if (deviceInfo == NULL)
	   return name;
	if (deviceInfo -> maxOutputChannels <= 0)
	   return name;

	if (OutputrateIsSupported (ch, rate))
	   name = deviceInfo -> name;
	return name;
}

int16_t	audioSink::invalidDevice	(void) {
	return numofDevices + 128;
}

bool	audioSink::isValidDevice (int16_t dev) {
	fprintf (stderr, "testing dev %d (numofDev = %d)\n", dev, numofDevices);
	return 0 <= dev && dev < numofDevices;
}

bool	audioSink::selectDefaultDevice (void) {
	return selectDevice (Pa_GetDefaultOutputDevice ());
}

void	audioSink::startDumping	(SNDFILE *f) {
	dumpFile	= f;
}

void	audioSink::stopDumping	(void) {
	dumpFile	= NULL;
}

int32_t	audioSink::getSelectedRate	(void) {
	return CardRate;
}

