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

#include	"drm-decoder.h"
#include	"audioframe-processor.h"
#include	"aac-processor.h"
#include	"xheaac-processor.h"
#ifdef  __WITH_FDK_AAC__
#include        "fdk-aac.h"
#elif   __WITH_FAAD__
#include        "drm-aacdecoder.h"
#endif

	audioFrameProcessor::audioFrameProcessor (drmDecoder *drm,
	                                          drmParameters *params,
	                                          int	shortId,
	                                          int	streamId) {
	this	-> parent	= drm;
	this	-> params	= params;
	this	-> shortId	= shortId;
	this	-> streamId	= streamId;

	fprintf (stder,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
#ifdef  __WITH_FDK_AAC__
	fprintf (stderr, "er is een fdkAAC gealloceerd\n");
        my_aacDecoder           = new fdkAAC    (drm, params);
#elif   __WITH_FAAD__
        my_aacDecoder           = new DRM_aacDecoder (drm, params);
#else
	fprintf (stderr, "een lege decoderbase?\n");
        my_aacDecoder           = new decoderBase ();
#endif

	lengthA_total	= 0;
	lengthB_total	= 0;
	for (int i = 0; i < 4; i ++) {
	   lengthA_total += params -> theStreams [i]. lengthHigh * 8;
	   lengthB_total += params -> theStreams [i]. lengthLow  * 8;
	}

	firstBuffer		= new uint8_t [lengthA_total + lengthB_total];
	my_aacProcessor		= new aacProcessor	(drm, params,
	                                                      my_aacDecoder);
	my_xheaacProcessor	= new xheaacProcessor	(drm, params,
	                                                      my_aacDecoder);
	connect (this, SIGNAL (show_audioMode (QString)),
                 parent, SLOT (show_audioMode (QString)));

}

	audioFrameProcessor::~audioFrameProcessor	() {
	delete	my_aacDecoder;
	delete	my_aacProcessor;
	delete	my_xheaacProcessor;
	delete[] firstBuffer;
}

void	audioFrameProcessor::
	               process	(uint8_t *buf_2, bool toggle) {
int	startPosA	= params -> theStreams [streamId]. offsetHigh;
int	startPosB	= params -> theStreams [streamId]. offsetLow;
int	lengthA		= params -> theStreams [streamId]. lengthHigh;
int	lengthB		= params -> theStreams [streamId]. lengthLow;

	if (!toggle) {
	   memcpy (firstBuffer, buf_2, lengthA_total + lengthB_total);
	   return;
	}

	uint8_t dataVec [2 * 8 * (lengthA + lengthB)];
	memcpy (dataVec, &firstBuffer [startPosA * 8], lengthA * 8);
	memcpy (&dataVec [lengthA * 8],
	                   &buf_2 [startPosA * 8], lengthA * 8);
	memcpy (&dataVec [2 * lengthA * 8],
	                   &firstBuffer [startPosB * 8], lengthB * 8);
	memcpy (&dataVec [(2 * lengthA + lengthB) * 8],
	                   &buf_2 [startPosB * 8], lengthB * 8);
	if (params -> theStreams [streamId]. audioStream)
	   processAudio (dataVec, streamId,
	                 0,           2 * lengthA,
	                 2 * lengthA, 2 * lengthB);
}

void	audioFrameProcessor::
	           processAudio (uint8_t *v, int16_t streamIndex,
	                         int16_t startHigh, int16_t lengthHigh,
	                         int16_t startLow,  int16_t lengthLow) {
uint8_t	audioCoding	= params -> theStreams [streamIndex]. audioCoding;

	switch (audioCoding) {
	   case 0:		// AAC
	      show_audioMode (QString ("AAC"));
	      my_aacProcessor -> process_aac (v, streamIndex,
	                                      startHigh, lengthHigh,
	                                      startLow,  lengthLow);
	      return;

	   case 3:		// xHE_AAC
	      show_audioMode (QString ("xHE-AAC"));
//#ifdef	__WITH_FDK_AAC__
	      my_xheaacProcessor -> process_usac (v, streamIndex,
	                                          startHigh, lengthHigh,
	                                          startLow,  lengthLow);
//#endif
	      return;

	   default:
	      fprintf (stderr,
	               "unsupported format audioCoding (%d)\n", audioCoding);
	      return;
	}
}

