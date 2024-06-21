#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DRM backend
 *
 *    DRM backend is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DRM backend is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 *    along with DRMbackend ; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	"xheaac-processor.h"
#include	"basics.h"
#include	"drm-decoder.h"
#include	"state-descriptor.h"
#include	"rate-converter.h"
//#include	"up-converter.h"
#include	<deque>
#include	<vector>
#include	<complex>
#ifndef	__MINGW32__
#include	<fdk-aac/aacdecoder_lib.h>   
#endif

static
const uint16_t crcPolynome [] = {
        0, 0, 0, 1, 1, 1, 0     // MSB .. LSB x⁸ + x⁴ + x³ + x² + 1
};

//	the 16 bit CRC - computed over bytes - has 
//	as polynome x^16 + x^12 + x^5 + 1
static inline
bool	check_crc_bytes (uint8_t *msg, int32_t len) {
int i, j;
uint16_t	accumulator	= 0xFFFF;
uint16_t	crc;
uint16_t	genpoly		= 0x1021;

	for (i = 0; i < len; i ++) {
	   int16_t data = msg [i] << 8;
	   for (j = 8; j > 0; j --) {
	      if ((data ^ accumulator) & 0x8000)
	         accumulator = ((accumulator << 1) ^ genpoly) & 0xFFFF;
	      else
	         accumulator = (accumulator << 1) & 0xFFFF;
	      data = (data << 1) & 0xFFFF;
	   }
	}
//
//	ok, now check with the crc that is contained
//	in the au
	crc	= ~((msg [len] << 8) | msg [len + 1]) & 0xFFFF;
//	fprintf (stderr, "crc = %X, accu %X\n",
//	           (msg [len] << 8) | msg [len + 1],  ~accumulator & 0xFFFF);
	return (crc ^ accumulator) == 0;
}


static  inline
uint16_t        get_MSCBits (uint8_t *v, int16_t offset, int16_t nr) {
int16_t         i;
uint16_t        res     = 0;

        for (i = 0; i < nr; i ++)
           res = (res << 1) | (v [offset + i] & 01);

        return res;
}

	xheaacProcessor::xheaacProcessor (stateDescriptor *theState,
	                                  drmDecoder *drm,
#ifdef	__MINGW32__
	                                  aacHandler	*aacFunctions,
#endif
	                                  RingBuffer<std::complex<float>> *b):
	                                    theCRC (8, crcPolynome),
	                                    my_messageProcessor (drm) {
	this	-> theState	= theState;
	this	-> parent	= drm;
#ifdef	__MINGW32__
	this	-> aacFunctions	= aacFunctions;
#endif
	this	-> audioOut	= b;
#ifdef	__MINGW32__
	this	-> handle	= aacFunctions -> aacDecoder_Open (TT_DRM, 3);
#else
	this	-> handle	= aacDecoder_Open (TT_DRM, 3);
#endif
	connect (this, SIGNAL (faadSuccess (bool)),
	         parent, SLOT (set_faadSyncLabel (bool)));
	connect (this, SIGNAL (audioAvailable ()),
                 drm, SLOT (audioAvailable ()));
	connect (this, SIGNAL (set_aacDataLabel (const QString &)),
                 drm, SLOT (set_aacDataLabel (const QString &)));


	currentRate			= 0;
	theConverter			= nullptr;
	frameBuffer. resize (0);
	borders. resize (0);
}

	xheaacProcessor::~xheaacProcessor	() {
	if (handle == nullptr)
#ifdef	__MINGW32__
	   aacFunctions -> aacDecoder_Close (handle);
#else
	   aacDecoder_Close (handle);
#endif
	if (theConverter != nullptr)
	   delete theConverter;
}
//
//	actually, we know that lengthHigh == 0, and therefore
//	that startLow = 0 as well
void	xheaacProcessor::process_usac	(uint8_t *v, int16_t streamId,
                                         int16_t startHigh, int16_t lengthHigh,
                                         int16_t startLow, int16_t lengthLow) {
uint16_t	frameBorderCount	= get_MSCBits (v, 0, 4);
int	bitReservoirLevel	= get_MSCBits (v, 4, 4);
int	length			= lengthHigh + lengthLow;
int	numChannels		=
	         theState -> streams [streamId]. audioMode == 0 ? 1 : 2;
int	textFlag		=
	         theState -> streams [streamId]. textFlag;
int	elementsUsed	= 0;
int	success		= 0;
int	failure		= 0;

	(void)startHigh; (void)startLow;
	(void)bitReservoirLevel;
	(void)numChannels;
	if (!theCRC. doCRC (v, 16) || (frameBorderCount == 0)) {
//	   fprintf (stderr, "oei\n");
	   faadSuccess (false);
	   return;
	}

	if (textFlag != 0) {
	   my_messageProcessor.
	                   processMessage (v, (startLow + lengthLow - 4) * 8);
	   length -= 4;
	}

	borders. resize	(frameBorderCount);
	std::vector<uint8_t> audioDescriptor =
	                         getAudioInformation (theState, streamId);
	reinit (audioDescriptor, streamId);

	for (int i = 0; i < frameBorderCount; i++) {
	   uint32_t frameBorderIndex =
	                    get_MSCBits (v, 8 * length - 16 - 16 * i, 12);
	   uint32_t frameBorderCountRepeat = 
	                    get_MSCBits (v, 8 * length - 16 - 16 * i + 12, 4);

	   if (frameBorderCountRepeat != frameBorderCount) {
//	      fprintf (stderr, "fout?\n");
	      faadSuccess (false);
	      return;
	   }
//	   fprintf (stderr, "border [%d] = %d\n",
//	                           i, frameBorderIndex);
	   borders [i] = frameBorderIndex;
	}

	for (int i = 0; i < frameBorderCount - 1; i ++)
	   if (borders [i] >= borders [i + 1]) {
	      faadSuccess (false);
	      return;
	   }
//
//	we do not look at the usac crc

	uint32_t directoryOffset = length - 2 * frameBorderCount - 2;
	if (borders [frameBorderCount - 1] >= directoryOffset) {
	   faadSuccess (false);
	   return;
	}

//	The first frameBorderIndex might point to the last one or
//	two bytes of the previous afs.
	switch (borders [0]) {
	   case 0xffe: // delayed from previous afs
//	first frame has two bytes in previous afs
	      if (frameBuffer. size () < 2) {
	         return;
	      }
//
//	if the "frameBuffer" contains more than 2 bytes, there was
//	a non-empty last part in the previous afs
	      if (frameBuffer. size () > 2) {
	         playOut (frameBuffer, frameBuffer. size (), -2);
	      }
	      elementsUsed = 0;
	      break;

	   case 0xfff:
//	first frame has one byte in previous afs
	      if (frameBuffer. size () < 1) {
	         return;
	      }
	      if (frameBuffer. size () > 1) {
	         playOut (frameBuffer, frameBuffer. size (), -1);
	      }
	      elementsUsed = 0;
	      break;

	   default: // boundary in this afs
//	boundary in this afs, process the last part of the previous afs
//	together with what is here as audioFrame
	      if (borders [0] < 2) {
	         return;
	      }
//
	      for (; elementsUsed < borders [0]; elementsUsed ++)
	            frameBuffer.
	                  push_back (get_MSCBits (v, 16 + elementsUsed * 8, 8));
	      if (check_crc_bytes (frameBuffer. data (),
                                      frameBuffer. size () - 2))
	         success ++;
	      else
	         failure ++;
	      playOut (frameBuffer, frameBuffer. size (), 0);
	      break;
	}

	for (int i = 1; i < frameBorderCount; i ++) {
	   frameBuffer. resize (0);
	   for (;elementsUsed < borders [i]; elementsUsed ++)
	      frameBuffer.
	            push_back (get_MSCBits (v, 16 + elementsUsed * 8, 8));
	   if (check_crc_bytes (frameBuffer. data (), frameBuffer. size () - 2))
	      success ++;
	   else
	      failure ++;
	   playOut (frameBuffer, frameBuffer. size (), i);
	}

//	at the end, save for the next afs
	frameBuffer. resize (0);
	for (int j = borders [frameBorderCount - 1];
	                             j < directoryOffset; j ++)
	   frameBuffer.  push_back (get_MSCBits (v, 16 + j * 8, 8));
//	fprintf (stderr, "%d out of %d OK\n", success, success + failure);
	success = 0; failure = 0;
}
//
void	xheaacProcessor::resetBuffers	() {
	frameBuffer. resize (0);
}

static int good	= 0;
static int fout = 0;

static
int16_t outBuffer [16 * 960];
void	xheaacProcessor::playOut (std::vector<uint8_t> &f,
	                          int size, int index) {
static
bool		convOK = false;
int16_t	cnt;
int32_t	rate;
float ff = 0;

	decodeFrame (f. data (),
	             f. size (),
//	             f. size () - 2,
	             &convOK,
	             outBuffer, &cnt, &rate);
	if (convOK) {
	   good ++;
	   writeOut (outBuffer, cnt, rate);
	}
	else {
	   fout ++;
	}

	if (good + fout >= 10) {
	   faadSuccess (good > fout);
//	   fprintf (stderr, "%d out of %d were good\n", good, good + fout);
	   good = 0; fout = 0;
	}

	if (good + fout >= 10) {
//	   fprintf (stderr, "%d good out of %d\n", good, good + fout);
	   good = 0; fout = 0;
	}
}

void	xheaacProcessor::toOutput (std::complex<float> *b, int16_t cnt) {
        if (cnt == 0)
           return;
	audioOut        -> putDataIntoBuffer (b, cnt);
        if (audioOut -> GetRingBufferReadAvailable () > 4800)
           audioAvailable ();
}
//
//	valid samplerates for xHE-AAC are
//	9.6, 12, 16, 19,2 24, 32, 38,4 and 48 KHz
//	translation factors are
//	5, 4, 3, 5 / 2, 2, 3 / 2, 5/4
void	xheaacProcessor::writeOut (int16_t *buffer, int16_t cnt,
	                           int32_t pcmRate) {
	if (theConverter == nullptr) {
	   theConverter = new rateConverter (pcmRate, 48000, pcmRate / 10);
	   currentRate	= pcmRate;
	}

	if (pcmRate != currentRate) {
	   delete theConverter;
	   theConverter = new rateConverter (pcmRate, 48000, pcmRate / 10);
	   currentRate = pcmRate;
	}
#if 0
	fprintf (stderr, "processing %d samples (rate %d)\n",
	                  cnt, pcmRate);
#endif
	std::complex<float> local [theConverter -> getOutputsize ()];
	for (int i = 0; i < cnt; i ++) {
	   std::complex<float> tmp = 
	                    std::complex<float> (buffer [2 * i] / 8192.0,
	                                         buffer [2 * i + 1] / 8192.0);
	   int amount;
	   bool b = theConverter -> convert (tmp, local, &amount);
	   if (b) {
	      toOutput (local, amount);
	   }
	}
}

void	xheaacProcessor::reinit (std::vector<uint8_t> newConfig,
	                                                 int streamId) {
	if (handle == nullptr)
	   return;
	if (currentConfig. size () != newConfig. size ()) {
	   currentConfig = newConfig;
	   init ();
	   return;
	}

	for (int i = 0; i < newConfig. size (); i ++) {
	   if (currentConfig. at (i) != newConfig. at (i)) {
	      currentConfig = newConfig;
	      init ();
	      return;
	   }
	}
}

void	xheaacProcessor::init	() {
	UCHAR *codecP		= &currentConfig [0];
	uint32_t codecSize	= currentConfig. size ();
	AAC_DECODER_ERROR err =
#ifdef	__MINGW32__
	           aacFunctions -> aacDecoder_ConfigRaw (handle,
	                                                 &codecP, &codecSize);
#else
	           aacDecoder_ConfigRaw (handle, &codecP, &codecSize);
#endif
	if (err == AAC_DEC_OK) {
	   CStreamInfo *pInfo =
#ifdef	__MINGW32__
	              aacFunctions -> aacDecoder_GetStreamInfo (handle);
#else
	              aacDecoder_GetStreamInfo (handle);
#endif
	   if (pInfo == nullptr) {
	      fprintf (stderr, "No stream info\n");
	   }
	}
	else
	   fprintf (stderr, "err\n");
}

static
int16_t	localBuffer [8 * 32768];

void	xheaacProcessor::decodeFrame (uint8_t	*audioFrame,
	                              uint32_t	frameSize,
	                              bool	*conversionOK,
	                              int16_t	*buffer,
	                              int16_t	*samples,
	                              int32_t	*pcmRate) {
int	errorStatus;
uint32_t	bytesValid	= 0;
int	flags		= 0;

	UCHAR *bb	= (UCHAR *)audioFrame;
	bytesValid	= frameSize;
	errorStatus =
#ifdef	__MINGW32__
	     aacFunctions -> aacDecoder_Fill (handle, &bb,
	                                          &frameSize, &bytesValid);
#else
	     aacDecoder_Fill (handle, &bb, &frameSize, &bytesValid);
#endif

	if (bytesValid != 0)
	   fprintf (stderr, "bytesValid after fill %d\n", bytesValid);

	if (!*conversionOK)
           flags = AACDEC_INTR;
	errorStatus =
#ifdef	__MINGW32__
	     aacFunctions -> aacDecoder_DecodeFrame (handle,
	                                             localBuffer,
	                                             2048, flags);
#else
	     aacDecoder_DecodeFrame (handle, localBuffer, 2048, flags);
#endif
#if 0
	if (errorStatus != 0)
	   fprintf (stderr, "fdk-aac errorstatus %x\n",
	                                errorStatus);
#endif
	if (errorStatus == AAC_DEC_NOT_ENOUGH_BITS) {
	   *conversionOK	= false;
	   *samples		= 0;
	   faadSuccess (false);
	   return;
	}

	if (errorStatus != AAC_DEC_OK) {
//	if ((errorStatus != AAC_DEC_OK) && (errorStatus & 0x4000 == 0)) {
	   *conversionOK	= false;
	   *samples		= 0;
	   faadSuccess (false);
	   return;
	}

	CStreamInfo *fdk_info =
#ifdef	__MINGW32__
	                aacFunctions -> aacDecoder_GetStreamInfo (handle);
#else
	                aacDecoder_GetStreamInfo (handle);
#endif

	if (fdk_info -> numChannels == 1) {
	   for (int i = 0; i < fdk_info -> frameSize; i ++) {
	      buffer [2 * i] 	= localBuffer [i];
	      buffer [2 * i + 1]= localBuffer [i];
	   }
	}
	else
	if (fdk_info -> numChannels == 2) {
	   for (int i = 0; i < fdk_info -> frameSize; i ++) {
	      buffer [2 * i] 	= localBuffer [2 * i];
	      buffer [2 * i + 1] = localBuffer [2 * i + i];
	   }
	}

	QString text = QString::number (fdk_info -> sampleRate);
	text += fdk_info -> numChannels == 2 ? " stereo" : " mono";
	set_aacDataLabel (text);
	
#if 0
	fprintf (stderr, "frameSize %d, samplerate %d\n",
	               fdk_info -> frameSize, fdk_info -> sampleRate);
	fprintf (stderr, "channel config %d (rate %d)\n",
	           fdk_info -> channelConfig, fdk_info -> sampleRate);
#endif
	*samples	= fdk_info	-> frameSize;
	*pcmRate	= fdk_info	-> sampleRate;
	*conversionOK	= true;
}

std::vector<uint8_t>
	xheaacProcessor::getAudioInformation (stateDescriptor *theState,
	                                                int streamId) {
std::vector<uint8_t> temp;
uint8_t	xxx	= 0;

	xxx	=  theState -> streams [streamId]. audioCoding << 6;
	xxx	|= theState -> streams [streamId]. SBR_flag << 5;
	xxx	|= theState -> streams [streamId]. audioMode << 3;
	xxx	|= theState -> streams [streamId]. audioSamplingRate;
	temp. push_back (xxx);
	xxx	=  theState -> streams [streamId]. textFlag << 7;
	xxx	|= theState -> streams [streamId]. enhancementFlag << 6;
	xxx	|= theState -> streams [streamId]. coderField << 1;
	temp. push_back (xxx);
	for (int i = 0;
	     i < theState -> streams [streamId]. xHE_AAC. size (); i ++)
	   temp. push_back (theState -> streams [streamId]. xHE_AAC. at (i));
	return temp;
}

