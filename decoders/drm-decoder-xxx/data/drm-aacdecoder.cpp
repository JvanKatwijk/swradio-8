#
/*
 *
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDR-J (JSDR).
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
 *
 */
#
#include	"drm-aacdecoder.h"
//#include	<faad.h>
#include	<stdio.h>
#include	"drm-decoder.h"

	DRM_aacDecoder::DRM_aacDecoder (drmDecoder *drm) {
	the_drmDecoder	= drm;
	the_aacDecoder	= NULL;
	SBR_flag	= false;
	audioMode	= 0x77;
	audioRate	= 0x77;
	connect (this, SIGNAL (aacData (QString)),
	         drm,  SLOT (aacData (QString)));
}

	DRM_aacDecoder::~DRM_aacDecoder (void) {
	closeDecoder ();
}
//
//	AudioParams contains the relevant parameters:
//	- audioSampleRate
//	- SBR_used
//	- audioMode
//
bool	DRM_aacDecoder::checkfor (uint8_t	audioRate,
	                          bool		newSBR,
	                          uint8_t	audioMode) {
	if (the_aacDecoder == NULL)
	   the_aacDecoder = NeAACDecOpen ();
	if (the_aacDecoder == NULL)
	   return false;

	if (newSBR ==  this -> SBR_flag &&
	    audioMode == this -> audioMode &&
	    audioRate == this -> audioRate)
	   return true;
	this	-> audioRate		= audioRate;
	this	-> SBR_flag		= newSBR;
	this	-> audioMode		= audioMode;
	return initDecoder (this -> audioRate,
	                    this -> SBR_flag, this -> audioMode);
}

bool DRM_aacDecoder::initDecoder (int16_t	audioSampleRate,
	                          bool		SBR_used,
	                          uint8_t	audioMode) {
int16_t aacRate = 12000;
uint8_t	aacMode	= DRMCH_SBR_PS_STEREO;
int	s;
QString	text;

	if (the_aacDecoder == NULL)
	   the_aacDecoder = NeAACDecOpen ();
	if (the_aacDecoder == NULL) 	// should not happen
	   return false;

// Only 12 kHz and 24 kHz with DRM
	aacRate = audioSampleRate == 01 ? 12000 : 24000;
	text = QString::number (aacRate);
	text. append (" ");

// Number of channels for AAC: Mono, PStereo, Stereo 
	switch (audioMode) {
	   case 0:		// audioMode == 0, MONO
	      if (SBR_used) 
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
	      if (SBR_used) 
	         aacMode = DRMCH_SBR_STEREO;
	      else
	         aacMode = DRMCH_STEREO;
	      text. append ("stereo");
	      break;
	}

	aacData (text);
	s = NeAACDecInitDRM (&the_aacDecoder, aacRate, (uint8_t)aacMode);
	return s >= 0;
}
//
//	Later on, we'll change this such that the return
//	vector is for further processing
void	DRM_aacDecoder::decodeFrame (uint8_t	*AudioFrame,
	                             int16_t	frameSize,
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

	if (hInfo. error != 0) {
	   fprintf (stderr, "Warning %s\n",
	                     faacDecGetErrorMessage (hInfo. error));
	   *samples	= 0;
	   return;
	}

	if (hInfo. channels == 2)
	   for (i = 0; i < hInfo. samples; i ++)
//	   for (i = 0; i < 2 * hInfo. samples; i ++)
	      buffer [i] = outBuf [i];
	else
	if (hInfo. channels == 1)
	   for (i = 0; i < hInfo. samples; i ++) {
	      buffer [2 * i]	= ((int16_t *)outBuf) [i];
	      buffer [2 * i + 1] = buffer [2 * i];
	   }
	*samples = hInfo. samples;
	return;
}

void DRM_aacDecoder::closeDecoder() {
	if (the_aacDecoder != NULL) {
	   NeAACDecClose (the_aacDecoder);
	   the_aacDecoder = NULL;
	}
}

