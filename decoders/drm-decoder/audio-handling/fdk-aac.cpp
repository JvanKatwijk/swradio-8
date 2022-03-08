#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DRM+ Decoder
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

#include	<aacdecoder_lib.h>
#include	<stdio.h>
#include	"drm-decoder.h"
#include	"fdk-aac.h"
#include	<vector>

	fdkAAC::fdkAAC	(drmDecoder *p,
	                 drmParameters *drm) {
std::vector<uint8_t> codecInfo;
	handle = aacDecoder_Open (TT_DRM, 3);
	fprintf (stderr, "handle for aacDecoder %s\n", 
	                     handle == nullptr ? "fout" : "goed");
	if (handle == nullptr) {
	   throw (44);
	}
}

	fdkAAC::~fdkAAC	() {
	if (handle != nullptr)
	   aacDecoder_Close (handle);
}

void	fdkAAC::reinit	(std::vector<uint8_t> newConfig, int streamId) {
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

void	fdkAAC::init	() {
	UCHAR *codecP	= &currentConfig [0];
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

void	fdkAAC::decodeFrame (uint8_t    *audioFrame,
	                     uint32_t	frameSize,
	                     bool       *conversionOK,
	                     int16_t	*buffer,
	                     int16_t    *samples,
	                     int32_t    *pcmRate) {
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
	fprintf (stderr, "%X\n", errorStatus);
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
	fprintf (stderr, "channel config %d (rate %d)\n",
	           fdk_info -> channelConfig, fdk_info -> sampleRate);
#endif
	*samples	= fdk_info	-> frameSize;
	*pcmRate	= fdk_info	-> sampleRate;
	*conversionOK	= true;
}

std::vector<uint8_t>
	fdkAAC::getAudioInformation (drmParameters *drm, int streamId) {
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
	for (int i = 0; i < temp. size (); i ++)
	   fprintf (stderr, "%x ", temp. at (i));
	fprintf (stderr, "\n");
	return temp;
}


