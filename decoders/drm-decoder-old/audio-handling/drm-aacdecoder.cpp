#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DRM+ Decoder
 *
 *    DRM+ Decoder is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DRM+ Decoder is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DRM+ Decoder; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#include	"drm-aacdecoder.h"
//#include	<faad.h>
#include	<stdio.h>
#include	"drm-decoder.h"

	DRM_aacDecoder::DRM_aacDecoder (drmDecoder *drm, 
	                                drmParameters *params) {
	the_drmDecoder		= drm;
	this	-> params	= params;
	the_aacDecoder		= nullptr;
	connect (this, SIGNAL (aacData (QString)),
	         drm,  SLOT (aacData (QString)));
}

	DRM_aacDecoder::~DRM_aacDecoder (void) {
	closeDecoder ();
}

static
std::vector <uint8_t> theDescriptor;

void	DRM_aacDecoder::reinit	(std::vector<uint8_t> audioD, int streamId) {
uint8_t audioRate		= params -> theStreams [streamId].
                                                           audioSamplingRate;
uint8_t SBR_flag                = params -> theStreams [streamId].
                                                           sbrFlag;
uint8_t audioMode               = params -> theStreams [streamId].
                                                           audioMode;
	if (the_aacDecoder == nullptr)
	   the_aacDecoder = NeAACDecOpen ();

	if (theDescriptor. size () != audioD. size ()) {
	   theDescriptor = audioD;
	   initDecoder (audioRate, SBR_flag, audioMode);
	   return;
	}

	for (int i = 0; i < theDescriptor. size (); i ++) {
	   if (theDescriptor. at (i) != audioD. at (i)) {
	      theDescriptor = audioD;
	      initDecoder (audioRate, SBR_flag, audioMode);
	      return;
	   }
	}
}

void	DRM_aacDecoder::initDecoder (int16_t	audioRate,
	                             bool	SBR_flag,
	                             uint8_t	audioMode) {
int16_t aacRate = 24000;
uint8_t	aacMode	= DRMCH_SBR_PS_STEREO;
int	s;
QString	text;

	if (the_aacDecoder == nullptr)
	   the_aacDecoder = NeAACDecOpen ();

// Only 24 kHz and 48 kHz with DRM+
	aacRate = audioRate == 03 ? 24000 : 48000;
	text = QString::number (aacRate);
	text. append (" ");

// Number of channels for AAC: Mono, PStereo, Stereo 
	switch (audioMode) {
	   case 0:		// audioMode == 0, MONO
	      if (SBR_flag) 
	         aacMode = DRMCH_SBR_MONO;
	      else 
	         aacMode = DRMCH_MONO;
	      text. append ("mono");
	      break;

	   case 1:
/* Low-complexity stereo only defined in SBR mode */
	      aacMode = DRMCH_SBR_PS_STEREO;
	      text. append ("stereo");
	      break;
	
	   default:
	   case 2:
	      if (SBR_flag) 
	         aacMode = DRMCH_SBR_STEREO;
	      else
	         aacMode = DRMCH_STEREO;
	      text. append ("stereo");
	      break;
	}

	aacData (text);
	s = NeAACDecInitDRM (&the_aacDecoder, aacRate, (uint8_t)aacMode);
}
//
//	Later on, we'll change this such that the return
//	vector is for further processing
void	DRM_aacDecoder::decodeFrame (uint8_t	*AudioFrame,
	                             uint32_t	frameSize,
	                             bool	*conversionOK,
	                             int16_t	*buffer,
	                             int16_t	*samples,
	                             int32_t	*pcmRate) {
int16_t* outBuf = NULL;
NeAACDecFrameInfo hInfo;
uint16_t	i;
	hInfo.channels	= 1;
	hInfo.error	= 1;
//	ensure we have a decoder:
	if (the_aacDecoder == NULL) 
	   return;

	outBuf = (int16_t*) NeAACDecDecode (the_aacDecoder,
	                                    &hInfo,
	                                    &AudioFrame[0],
	                                    frameSize);

	*pcmRate	= hInfo. samplerate;
	*conversionOK	= !hInfo. error;

	fprintf (stderr, "samples %d, sbr %d ps %d ch %d sr %d consumed %d\n",
	                 hInfo. samples, hInfo. sbr,
	                 hInfo. ps, hInfo. channels, 
	                 hInfo. samplerate, hInfo. bytesconsumed);
	if (hInfo. error != 0) {
	   fprintf (stderr, "Warning %s\n",
	                     faacDecGetErrorMessage (hInfo. error));
	   *samples	= 0;
	   return;
	}

	if (hInfo. channels == 2) {
	   for (i = 0; i < hInfo. samples; i ++)
	      buffer [i] = outBuf [i];
	   *samples = hInfo. samples / 2;
	}
	else
	if (hInfo. channels == 1) {
	   for (i = 0; i < hInfo. samples; i ++) {
	      buffer [2 * i]	= ((int16_t *)outBuf) [i];
	      buffer [2 * i + 1] = buffer [2 * i];
	   *samples = hInfo. samples;
	   }
	}
}

void DRM_aacDecoder::closeDecoder() {
	if (the_aacDecoder != NULL) {
	   NeAACDecClose (the_aacDecoder);
	   the_aacDecoder = NULL;
	}
}

