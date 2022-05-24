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

#include	"basics.h"
#include	"drm-decoder.h"
#include	"aac-processor.h"

static  inline
uint16_t        get_MSCBits (uint8_t *v, int16_t offset, int16_t nr) {
int16_t         i;
uint16_t        res     = 0;

        for (i = 0; i < nr; i ++)
           res = (res << 1) | (v [offset + i] & 01);

        return res;
}

	aacProcessor::aacProcessor	(drmDecoder *drm,
	                                 drmParameters *params,
	                                 decoderBase	*my_aacDecoder):
	                                    my_messageProcessor (drm),
	                                    upFilter_24000 (5, 12000, 48000) {
	this	-> parent	= drm;
	this	-> params	= params;
	connect (this, SIGNAL (putSample (float, float)),
	         parent, SLOT (sampleOut (float, float)));
	connect (this, SIGNAL (faadSuccess (bool)),
	         parent, SLOT (faadSuccess (bool)));
	this	-> my_aacDecoder = my_aacDecoder;
}

	aacProcessor::~aacProcessor	() {
}

//      little confusing: start- and length specifications are
//      in bytes, we are working internally in bits
void	aacProcessor::process_aac (uint8_t *v, int16_t streamId,
                                   int16_t startHigh, int16_t lengthHigh,
                                   int16_t startLow,  int16_t lengthLow) {
	if (!params	-> theStreams [streamId]. audioStream)
	   return;
        if (lengthHigh != 0)
           handle_uep_audio (v, streamId, startHigh, lengthHigh,
                                                startLow, lengthLow - 4);
        else
           handle_eep_audio (v, streamId,  startLow, lengthLow - 4);
	my_messageProcessor.
                 processMessage (v, 8 * (2 * (lengthHigh + lengthLow) - 4));
}

static
int16_t	outBuffer [8 * 960];
static
audioFrame f [20];
void	aacProcessor::handle_uep_audio (uint8_t *v, int16_t mscIndex,
	                                int16_t startHigh, int16_t lengthHigh,
	                                int16_t startLow, int16_t lengthLow) {
int16_t	headerLength, i, j;
int16_t	usedLength	= 0;
int16_t	crcLength	= 1;
int16_t	payloadLength;

//	first the globals
	numFrames = params -> theStreams [mscIndex]. audioSamplingRate == 3 ? 5 : 10;
	headerLength = numFrames == 10 ? (9 * 12 + 4) / 8 : (4 * 12) / 8;
	payloadLength = lengthLow + lengthHigh - headerLength - crcLength;

//	Then, read the numFrames - 1 "length"s from the header:
	int previousBorder = 0;
	for (i = 0; i < numFrames - 1; i ++) {
	   f [i]. length = get_MSCBits (v, startHigh * 8 + 12 * i, 12) -
	                                                    previousBorder;
	   previousBorder = get_MSCBits (v, startHigh * 8 + 12 * i, 12);
	   if (f [i]. length < 0)
	      return;
	   usedLength += f [i]. length;
	}
//	the length of the last segment is to be computed
	f [numFrames - 1]. length = payloadLength - previousBorder;
//	fprintf (stderr, "Length segment [%d] = %d\n",
//	                    numFrames - 1, f [numFrames - 1]. length);
//	OK, now it is getting complicated, we first load the part(s) from the
//	HP part. Length for all parts is equal, i.e. LengthHigh / numFrames
//	the audiopart is one byte shorter, due to the crc
	int16_t segmentinHP	= (lengthHigh - headerLength) / numFrames;
	int16_t audioinHP	= segmentinHP - 1;
	int16_t	entryinHP	= startHigh + headerLength;

	for (i = 0; i < numFrames; i ++) {
	   for (j = 0; j < audioinHP; j ++)
	      f [i]. audio [j] = get_MSCBits (v,
	                        (entryinHP ++) * 8, 8);
	   f [i]. aac_crc = get_MSCBits (v, 
	                        (entryinHP ++) * 8, 8);
	}

//	Now what is left is the part of the frame in the LP part

	int16_t entryinLP	= startLow;
	for (i = 0; i < numFrames; i ++) {
	   for (j = 0;
	        j < f [i]. length - audioinHP;
	        j ++)
	      if (entryinLP < startLow + lengthLow)
	         f [i]. audio [audioinHP + j] =
	                    get_MSCBits (v, (entryinLP++) * 8, 8);
	}
	playOut (mscIndex);
}

//	Processing audio, in an EEP segment, proceeds as follows
//	first, the number of segments is determined by looking at
//	the audioRate. A rate of 12000 gives 5 frames, 24000 gives
//	10 frames.
//	second, the header is determined from which the startPositions
//	of the subsequent aac segments can be determined. The header
//	consists of (numFrames - 1) 12 bits words. For a 10 frame
//	header, we add 4 bits such that the end is a byte multiple (112)
//	Then the crc's are read in, these are 8 bit values, one for 
//	each frame.
//	Finally, the aac encodings of the frames themselves is readin
//	Note that positions where the segments are to be found
//	can be deduced from the start positions: just add the size
//	of the header and the size of the crc block to the start position.
void	aacProcessor::handle_eep_audio (uint8_t *v,
	                                int16_t mscIndex,
	                                int16_t startLow,
	                                int16_t lengthLow) {
int16_t		i, j;
int16_t		headerLength;
int16_t		crc_start;
int16_t		payLoad_start;
int16_t		payLoad_length = 0;

//	we could assert that startLow == 0
//	in mode E we have 24 Khz -> 5, 48 Khz -> 10
	numFrames = params -> theStreams [mscIndex].
	                           audioSamplingRate == 03 ? 5 : 10;
	headerLength = numFrames == 10 ? (9 * 12 + 4) / 8 : (4 * 12) / 8;
//	length in bytes

	f [0]. startPos = 0;
	for (i = 1; i < numFrames; i ++) {
	   f [i]. startPos = get_MSCBits (v, 12 * (i - 1), 12);
	}

	for (i = 1; i < numFrames; i ++) {
	   f [i - 1]. length = f [i]. startPos - f [i - 1]. startPos;
	   if (f [i - 1]. length < 0 ||
	       f [i - 1]. length >= lengthLow) {
	      faadSuccess (false);
	      return;
	   }
	   payLoad_length += f [i - 1]. length;
	}

	f [numFrames - 1]. length = lengthLow - 
	                            (headerLength + numFrames) -
	                            payLoad_length;
	if (f [numFrames - 1]. length < 0) {
	   faadSuccess (false);
	   return;
	}
//
//	crc_start in bytes
	crc_start	= startLow + headerLength;
	for (i = 0; i < numFrames; i ++)
	   f [i]. aac_crc = get_MSCBits (v, (crc_start + i) * 8, 8);

//	The actual audiobits (bytes) starts at crc_start + numFrames
	payLoad_start	= crc_start + numFrames; // the crc's

	for (i = 0; i < numFrames; i ++) {
	   for (j = 0; j < f [i]. length; j ++) {
	      int16_t in2 = (payLoad_start + f [i]. startPos + j);
	      f [i]. audio [j] = get_MSCBits (v, in2 * 8, 8);
	   }
	}
	playOut (mscIndex);
}
//

void	aacProcessor::playOut (int16_t	streamId) {
int16_t	i;
uint8_t	audioSamplingRate	= params -> theStreams [streamId].
	                                                   audioSamplingRate;
uint8_t	SBR_flag		= params -> theStreams [streamId].
	                                                   sbrFlag;
uint8_t	audioMode		= params -> theStreams [streamId].
	                                                   audioMode;
std::vector<uint8_t> audioDescriptor =
		getAudioInformation (params, streamId);

	my_aacDecoder -> reinit (audioDescriptor, streamId);
	for (i = 0; i < numFrames; i ++) {
	   int16_t	index = i;
	   bool		convOK;
	   int16_t	cnt;
	   int32_t	rate;
	   if (f [index]. length < 0)
	      continue;
#if 0
	   fprintf (stderr, "Frame %d (numFrames %d) length %d\n",
	                          index, numFrames, f [index]. length + 1);
#endif
	   my_aacDecoder ->  decodeFrame ((uint8_t *)(&f [index]. aac_crc),
	                                 f [index]. length + 1,
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
}
//

void	aacProcessor::toOutput (float *b, int16_t cnt) {
int16_t	i;
	if (cnt == 0)
	   return;
	for (i = 0; i < cnt / 2; i ++)
	   putSample (b [2 * i], b [2 * i + 1]);
}

void	aacProcessor::writeOut (int16_t *buffer, int16_t cnt,
	                          int32_t pcmRate) {
int16_t	i;

	if (pcmRate == 48000) {
	   float lbuffer [2 * cnt];
	   for (i = 0; i < cnt; i ++) {
	      lbuffer [2 * i]     = float (buffer [2 * i] / 32767.0);
	      lbuffer [2 * i + 1] = float (buffer [2 * i + 1] / 32767.0);
	   }
	   toOutput (lbuffer, 2 * cnt);
	   return;
	}


	if (pcmRate == 24000) {
	   float lbuffer [4 * cnt];
	   for (i = 0; i < cnt; i ++) {
	      std::complex<float> help =
	           upFilter_24000.
	                Pass (std::complex<float> (buffer [2 * i] / 32767.0,
	                                           buffer [2 * i + 1] / 32767.0));
	      lbuffer [4 * i + 0] = real (help);
	      lbuffer [4 * i + 1] = imag (help);
	      help = upFilter_24000. Pass (std::complex<float> (0, 0));
	      lbuffer [4 * i + 2] = real (help);
	      lbuffer [4 * i + 3] = imag (help);
	   }
	   toOutput (lbuffer, 4 * cnt);
	   return;
	}
}

std::vector<uint8_t>
	aacProcessor::getAudioInformation (drmParameters *drm, int streamId) {
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

