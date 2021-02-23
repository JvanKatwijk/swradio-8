#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DRM+ decoder
 *
 *    DRM+ decoder is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DRM+ decoder is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DRM+ decoder; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	"xheaac-processor.h"
#include	"basics.h"
#include	"drm-decoder.h"
#include	"rate-converter.h"
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

	xheaacProcessor::xheaacProcessor (drmDecoder *drm,
	                                  drmParameters *params,
	                                  decoderBase *my_aacDecoder):
	                                    my_messageProcessor (drm),
	                                    theCRC (8, crcPolynome) {
	this	-> parent	= drm;
	this	-> params	= params;
	connect (this, SIGNAL (putSample (float, float)),
	         parent, SLOT (sampleOut (float, float)));
	connect (this, SIGNAL (faadSuccess (bool)),
	         parent, SLOT (faadSuccess (bool)));
	this	-> my_aacDecoder	= my_aacDecoder;
	this	-> theConverter		= nullptr;
	currentRate			= 0;
	frameBuffer. resize (0);
	borders. resize (0);
}

	xheaacProcessor::~xheaacProcessor	() {
	if (theConverter != nullptr)
	   delete theConverter;
}
//
//	actually, we know that lengthHigh == 0, and therefore
//	that startLow = 0 as well
void	xheaacProcessor::process_usac	(uint8_t *v, int16_t streamId,
                                         int16_t startHigh, int16_t lengthHigh,
                                         int16_t startLow, int16_t lengthLow) {
int	frameBorderCount	= get_MSCBits (v, 0, 4);
int	bitReservoirLevel	= get_MSCBits (v, 4, 4);
int	crc			= get_MSCBits (v, 8, 8);
int	length			= lengthHigh + lengthLow - 4;
int	numChannels		=
	         params -> theStreams [streamId]. audioMode == 0 ? 1 : 2;
int	elementsUsed		= 0;

	my_messageProcessor.
                 processMessage (v, 8 * length);

	if (!theCRC. doCRC (v, 16)) {
	   fprintf (stderr, "oei\n");
	}
//
	uint32_t bitResLevel	= (bitReservoirLevel + 1) * 384 *
	                                               numChannels;
	uint32_t directoryOffset = length - 2 * frameBorderCount;

	if (frameBorderCount > 0) {
	   borders. 	resize	(frameBorderCount);
	   std::vector<uint8_t> audioDescriptor =
	                         getAudioInformation (params, streamId);
	   my_aacDecoder -> reinit (audioDescriptor, streamId);

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
	   for (int i = 0; i < directoryOffset; i ++)
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
	my_aacDecoder ->  decodeFrame (f. data (),
	                               f. size (),
	                               &convOK,
	                               outBuffer,
	                               &cnt, &rate);
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
void	xheaacProcessor::toOutput (std::complex<float> *b, int16_t cnt) {
int16_t i;
        if (cnt == 0)
           return;

        for (i = 0; i < cnt; i ++)
           putSample (real (b [i]), imag (b [i]));
}

void	xheaacProcessor::writeOut (int16_t *buffer, int16_t cnt,
	                           int32_t pcmRate) {
int16_t	i;

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
	   if (b)
	      toOutput (local, amount);
	}
}

std::vector<uint8_t>
	xheaacProcessor::getAudioInformation (drmParameters *drm,
	                                                int streamId) {
std::vector<uint8_t> temp;
streamParameters *sp = &(drm -> theStreams [streamId]);
uint8_t	xxx	= 0;

	xxx	= sp -> audioCoding << 6;
	xxx	|= (sp -> sbrFlag << 5);
	xxx	|= (sp -> audioMode << 3);
	xxx	|= sp -> audioSamplingRate;
	temp. push_back (xxx);
	xxx	= 0;
	xxx	= sp -> textFlag << 7;
	xxx	|= (sp -> enhancementFlag << 6);
	xxx	|= (sp -> coderField) << 1;
	temp. push_back (xxx);
	for (int i = 0; i < sp -> xHE_AAC. size (); i ++)
	   temp. push_back (sp -> xHE_AAC. at (i));
	return temp;
}

