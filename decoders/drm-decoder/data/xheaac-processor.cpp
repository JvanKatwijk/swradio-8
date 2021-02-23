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
#include	<deque>
#include	<vector>
#include	<complex>

deque<uint8_t>  frameBuffer;
vector<uint32_t> borders;

static
const uint16_t crcPolynome [] = {
        0, 0, 0, 1, 1, 1, 0     // MSB .. LSB x⁸ + x⁴ + x³ + x² + 1
};


static  inline
uint16_t        get_MSCBits (uint8_t *v, int16_t offset, int16_t nr) {
int16_t         i;
uint16_t        res     = 0;

        for (i = 0; i < nr; i ++)
           res = (res << 1) | (v [offset + i] & 01);

        return res;
}

	xheaacProcessor::xheaacProcessor (stateDescriptor *theState,
	                                  drmDecoder *drm):
	                                    theCRC (8, crcPolynome),
	                                    upFilter_24000 (5, 12000, 48000),
                                            upFilter_12000 (5, 6000, 48000) {
	this	-> theState	= theState;
	this	-> parent	= drm;
	this	-> handle	= aacDecoder_Open (TT_DRM, 3);
	connect (this, SIGNAL (putSample (float, float)),
	         parent, SLOT (sampleOut (float, float)));
	connect (this, SIGNAL (faadSuccess (bool)),
	         parent, SLOT (faadSuccess (bool)));
	currentRate			= 0;
	frameBuffer. resize (0);
	borders. resize (0);
}

	xheaacProcessor::~xheaacProcessor	() {
	if (handle == nullptr)
	   aacDecoder_Close (handle);
}
//
//	actually, we know that lengthHigh == 0, and therefore
//	that startLow = 0 as well
void	xheaacProcessor::process_usac	(uint8_t *v, int16_t streamId,
                                         int16_t startHigh, int16_t lengthHigh,
                                         int16_t startLow, int16_t lengthLow) {
uint16_t	frameBorderCount	= get_MSCBits (v, 0, 4);
int	bitReservoirLevel	= get_MSCBits (v, 4, 4);
int	crc			= get_MSCBits (v, 8, 8);
int	length			= lengthHigh + lengthLow - 4;
int	numChannels		=
	         theState -> streams [streamId]. audioMode == 0 ? 1 : 2;
uint32_t elementsUsed		= 0;

	(void)startHigh; (void)startLow;
	(void)bitReservoirLevel;
	(void)crc;
	(void)numChannels;
	if (!theCRC. doCRC (v, 16)) {
	   fprintf (stderr, "oei\n");
	}
//
//	uint32_t bitResLevel	= (bitReservoirLevel + 1) * 384 *
//	                                               numChannels;
	uint32_t directoryOffset = length - 2 * frameBorderCount;

	if (frameBorderCount > 0) {
	   borders. 	resize	(frameBorderCount);
	   std::vector<uint8_t> audioDescriptor =
	                         getAudioInformation (theState, streamId);
	   reinit (audioDescriptor, streamId);

	   for (int i = 0; i < frameBorderCount; i++) {
	      uint32_t frameBorderIndex =
	                    get_MSCBits (v, 8 * length - 16 - 16 * i, 12);
	      uint32_t frameBorderCountRepeat = 
	                    get_MSCBits (v, 8 * length - 16 - 16 * i + 12, 4);
	      if (frameBorderCountRepeat != frameBorderCount) {
	         resetBuffers ();
	         return;
	      }
#if 0
	      fprintf (stderr, "frameBorderIndex %d, check %d\n",
	                        frameBorderIndex, frameBorderCountRepeat);
#endif
	      borders [i] = frameBorderIndex;
	      if (i == 0) {
//
//	The first frameBorderIndex might point to the last one or
//	two bytes of the previous afs.
	         switch (borders [0]) {
	            case 0xffe: // delayed from previous afs
//	first frame has two bytes in previous afs
	               if (frameBuffer. size () < 2) {
	                  resetBuffers ();
	                  return;
	               }
//
//	if the "frameBuffer" contains more than 2 bytes, there was
//	a non-empty last part in the previous afs
	               if (frameBuffer. size () > 2)
	                  processFrame (frameBuffer. size () - 2);
	               elementsUsed = 0;
	               break;

	            case 0xfff:
//	first frame has one byte in previous afs
	               if (frameBuffer. size () < 1) {
	                  resetBuffers ();
	                  return;
	               }
	               if (frameBuffer. size () > 1)
	                  processFrame (frameBuffer. size () - 1);
	               elementsUsed = 0;
	               break;

	            default: // boundary in this afs
//	boundary in this afs, process the last part of the previous afs
//	together with what is here as audioFrame
	               if (borders [0] < 2) {
	                  resetBuffers ();
	                  return;
	               }
//
//	elementsUsed will be used to keep track on the progress
//	in handling the elements of this afs
	               for (elementsUsed = 0; 
	                    elementsUsed < borders [0]; elementsUsed ++)
	                  frameBuffer.
	                  push_back (get_MSCBits (v, 16 + elementsUsed * 8, 8));
	               processFrame (frameBuffer. size ());
	               break;
	         }
	      }
	      else
	      if (i < frameBorderCount - 1) {
//	just read in the data and process the frame
	         for (; elementsUsed < borders [i]; elementsUsed ++) 
	            frameBuffer. push_back
	                        (get_MSCBits (v, 16 + elementsUsed * 8, 8));
	         processFrame (frameBuffer. size ());
	      }
	      else	// at the end, save for the next afs
	      for ( ; elementsUsed < directoryOffset; elementsUsed ++)
	         frameBuffer.
		       push_back (get_MSCBits (v, 16 + elementsUsed * 8, 8));
	   }
	}
	else {
	   for (uint16_t i = 0; i < directoryOffset; i ++)
	      frameBuffer. push_back (get_MSCBits (v, 16 + 8 * i, 8));
	}
}
//
//	In some cases we do not use the full content
//	of the data gathered in the frameBuffer, so we pass on the size
void	xheaacProcessor::processFrame	(int size) {
std::vector<uint8_t> audioFrame;
#if 0
	fprintf (stderr, "process frame %d\n", size);
#endif
	while (size > 0) {
	   size --;
	   audioFrame. push_back (frameBuffer. front());
	   frameBuffer. pop_front ();
	}
	playOut (audioFrame);
}

void	xheaacProcessor::resetBuffers	() {
	frameBuffer. resize (0);
}

static
int16_t outBuffer [16 * 960];
void	xheaacProcessor::playOut (std::vector<uint8_t> f) {
bool		convOK;
int16_t	cnt;
int32_t	rate;
	decodeFrame (f. data (), f. size (), &convOK,
	                   outBuffer, &cnt, &rate);
	if (convOK) {
	   faadSuccess (true);
	   if (cnt > 0)
	      writeOut (outBuffer, cnt, rate);
	}
	else {
	   faadSuccess (false);
	}
}
//
void	xheaacProcessor::toOutput (float *b, int16_t cnt) {
int16_t i;
        if (cnt == 0)
           return;

        for (i = 0; i < cnt / 2; i ++)
           putSample (b [2 * i], b [2 * i + 1]);
}

void	xheaacProcessor::writeOut (int16_t *buffer, int16_t cnt,
	                           int32_t pcmRate) {
int16_t	i;
	if (pcmRate == 48000) {
	   float lbuffer [cnt];
	   for (i = 0; i < cnt / 2; i ++) {
	      lbuffer [2 * i]     = float (buffer [2 * i] / 32767.0);
	      lbuffer [2 * i + 1] = float (buffer [2 * i + 1] / 32767.0);
	   }
	   toOutput (lbuffer, cnt);
	   return;
	}

	if (pcmRate == 12000) {
	   float lbuffer [4 * cnt];
	   for (i = 0; i < cnt / 2; i ++) {
	      std::complex<float> help =
	           upFilter_12000. Pass (std::complex<float> (
	                                           buffer [2 * i] / 32767.0,
	                                           buffer [2 * i + 1] / 32767.0));
	      lbuffer [8 * i + 0] = real (help);
	      lbuffer [8 * i + 1] = imag (help);
	      help = upFilter_12000. Pass (std::complex<float> (0, 0));
	      lbuffer [8 * i + 2] = real (help);
	      lbuffer [8 * i + 3] = imag (help);
	      help = upFilter_12000. Pass (std::complex<float> (0, 0));
	      lbuffer [8 * i + 4] = real (help);
	      lbuffer [8 * i + 5] = imag (help);
	      help = upFilter_12000. Pass (std::complex<float> (0, 0));
	      lbuffer [8 * i + 6] = real (help);
	      lbuffer [8 * i + 7] = imag (help);
	   }
	   toOutput (lbuffer, 4 * cnt);
	   return;
	}
	if (pcmRate == 24000) {
	   float lbuffer [2 * cnt];
	   for (i = 0; i < cnt / 2; i ++) {
	      std::complex<float> help =
	           upFilter_24000. Pass (std::complex<float> (
                                                   buffer [2 * i] / 32767.0,
	                                           buffer [2 * i + 1] / 32767.0));
	      lbuffer [4 * i + 0] = real (help);
	      lbuffer [4 * i + 1] = imag (help);
	      help = upFilter_24000. Pass (std::complex<float> (0, 0));
	      lbuffer [4 * i + 2] = real (help);
	      lbuffer [4 * i + 3] = imag (help);
	   }
	   toOutput (lbuffer, 2 * cnt);
	   return;
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
	              aacDecoder_ConfigRaw (handle, &codecP, &codecSize);
	if (err == AAC_DEC_OK) {
	   CStreamInfo *pInfo = aacDecoder_GetStreamInfo (handle);
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

	UCHAR *bb	= (UCHAR *)audioFrame;
	bytesValid	= frameSize;
	errorStatus =
	     aacDecoder_Fill (handle, &bb, &frameSize, &bytesValid);

	if (bytesValid != 0)
	   fprintf (stderr, "bytesValid after fill %d\n", bytesValid);
	errorStatus =
	     aacDecoder_DecodeFrame (handle, localBuffer, 16 * 980, 0);
#if 0
	fprintf (stderr, "fdk-aac errorstatus %x\n",
	                       errorStatus);
#endif
	if (errorStatus == AAC_DEC_NOT_ENOUGH_BITS) {
	   *conversionOK	= false;
	   *samples		= 0;
	   return;
	}
	if (errorStatus != AAC_DEC_OK) {
	   *conversionOK	= false;
	   *samples		= 0;
	   return;
	}

	CStreamInfo *fdk_info = aacDecoder_GetStreamInfo (handle);
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
#if 0
	fprintf (stderr, "frameSize %d, samplerate %d\n",
	               fdk_info -> frameSize, fdk_info -> sampleRate);
#endif
//	fprintf (stderr, "channel config %d (rate %d)\n",
//	           fdk_info -> channelConfig, fdk_info -> sampleRate);
	*samples	= fdk_info	-> frameSize * 2;
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
//	for (int i = 0; i < sp -> xHE_AAC. size (); i ++)
//	   temp. push_back (sp -> xHE_AAC. at (i));
	return temp;
}

