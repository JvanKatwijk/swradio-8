#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the drm receiver
 *
 *    drm receiver is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm receiver is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with drm receiver; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#include	<stdio.h>
#include	<float.h>
#include	<math.h>
#include	"basics.h"
#include	"aac-processor-faad.h"
#include	"drm-decoder.h"
#include	"state-descriptor.h"

static	inline
uint16_t	get_MSCBits (uint8_t *v, int16_t offset, int16_t nr) {
int16_t		i;
uint16_t	res	= 0;

	for (i = 0; i < nr; i ++) 
	   res = (res << 1) | (v [offset + i] & 01);

	return res;
}

	aacProcessor_faad::aacProcessor_faad	(
	                                 stateDescriptor *theState,
	                                 drmDecoder *drm,
	                                 RingBuffer<std::compelx<float>> *b):
	                                      my_messageProcessor (drm),
	                                      upFilter_24000 (5, 12000, 48000),
	                                      upFilter_12000 (5, 6000, 48000) {
	
	this	-> theState		= theState;
	this	-> drmMaster		= drm;
	this	-> audioBuffer		= b;
	this	-> theDecoder		= nullptr;
//
//	these are "previous values"
	SBR_flag        = false;
	audioMode       = 0x77;
        audioRate       = 0x77;

        connect (this, SIGNAL (aacData (QString)),
                 drm,  SLOT (aacData (QString)));
	connect (this, SIGNAL (putSample (float, float)),
	         drmMaster, SLOT (sampleOut (float, float)));
	connect (this, SIGNAL (samplesAvailable ()),
	         drmMaster, SLOT (samplesAvailable ()));
	connect (this, SIGNAL (faadSuccess (bool)),
	         drmMaster, SLOT (faadSuccess (bool)));
}

	aacProcessor_faad::~aacProcessor_faad	(void) {
	closeDecoder ();
}

//	little confusing: start- and length specifications are
//	in bytes, we are working internally in bits
void	aacProcessor_faad::process_aac (uint8_t *v, int16_t mscIndex,
	                                int16_t startHigh,
	                                int16_t lengthHigh,
	                                int16_t startLow,
	                                int16_t lengthLow) {
	if (lengthHigh != 0) 
	   handle_uep_audio (v, mscIndex, startHigh, lengthHigh,
	                            startLow, lengthLow);
	else 
	   handle_eep_audio (v, mscIndex,  startLow, lengthLow);
}

static
int16_t outBuffer [8 * 960];
static
audioFrame f [20];
void	aacProcessor_faad::handle_uep_audio (uint8_t *v, int16_t mscIndex,
	                                int16_t startHigh,
	                                int16_t lengthHigh,
	                                int16_t startLow,
	                                int16_t lengthLow) {
int16_t	headerLength, i, j;
int16_t	usedLength	= 0;
int16_t	crcLength	= 1;
int16_t	payloadLength;

//	first the globals
	numFrames = theState -> streams [mscIndex]. audioSamplingRate == 1 ? 5 : 10;
	headerLength = numFrames == 10 ? (9 * 12 + 4) / 8 : (4 * 12) / 8;
	payloadLength = lengthLow + lengthHigh - headerLength - crcLength;
	
//	Then, read the numFrames - 1 "length"s from the header:
	for (i = 0; i < numFrames - 1; i ++) {
	   f [i]. length = get_MSCBits (v, startHigh * 8 + 12 * i, 12);
	   if (f [i]. length < 0)
	      return;
	   usedLength += f [i]. length;
	}

//	the length of the last segment is to be computed
	f [numFrames - 1]. length = payloadLength - usedLength;

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

	if (theState -> streams [mscIndex]. textFlag) {
           my_messageProcessor.
                           processMessage (v, (startLow + lengthLow - 4) * 8);
           lengthLow -= 4;
        }

	playOut (mscIndex);
}

//
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
void	aacProcessor_faad::handle_eep_audio (uint8_t *v,
	                                     int16_t mscIndex,
	                                     int16_t startLow,
	                                     int16_t lengthLow) {
int16_t		i, j;
int16_t		headerLength;
int16_t		crc_start;
int16_t		payLoad_start;
int16_t		payLoad_length = 0;

	numFrames = theState -> streams [mscIndex].
	                             audioSamplingRate == 1 ? 5 : 10;
	headerLength = numFrames == 10 ? (9 * 12 + 4) / 8 : (4 * 12) / 8;

	if (theState -> streams [mscIndex]. textFlag) {
	   fprintf (stderr, "%d\n", startLow + lengthLow - 4);
	   my_messageProcessor.
                           processMessage (v, (startLow + lengthLow - 4) * 8);
           lengthLow -= 4;
        }

//	startLow in bytes!!
	f [0]. startPos = 0;
	for (i = 1; i < numFrames; i ++) {
	   f [i]. startPos = get_MSCBits (v,
	                                  startLow * 8 + 12 * (i - 1), 12);
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


void	aacProcessor_faad::playOut (int16_t	mscIndex) {
int16_t	i;
uint8_t	audioSamplingRate	= theState -> streams [mscIndex].
	                                                   audioSamplingRate;
uint8_t	SBR_flag		= theState -> streams [mscIndex].
	                                                   SBR_flag;
uint8_t	audioMode		= theState -> streams [mscIndex].
	                                                   audioMode;

	if (!checkfor (audioSamplingRate, SBR_flag, audioMode)) {
	   faadSuccess (false);
	   return;
	}

	for (i = 0; i < numFrames; i ++) {
	   int16_t	index = i;
	   bool		convOK;
	   int16_t	cnt;
	   int32_t	rate;
	   decodeFrame ((uint8_t *)(&f [index]. aac_crc),
	                            f [index]. length + 1,
	                            &convOK,
	                            outBuffer,
	                            &cnt, &rate);
	   if (convOK) {
	      faadSuccess (true);
	      writeOut (outBuffer, cnt, rate);
	   }
	   else
	      faadSuccess (false);
	}
}
//
void	aacProcessor_faad::toOutput (std::complex<float>*b, int16_t cnt) {
int16_t	i;
	if (cnt == 0)
	   return;

	audioBuffer	-> putDataIntoBuffer (b, cnt);
	samplesAvailable ();
}

void	aacProcessor_faad::writeOut (int16_t *buffer, int16_t cnt,
	                             int32_t pcmRate) {
int16_t	i;

	if (pcmRate == 48000) {
	   std::complex<float> lbuffer [cnt / 2];
	   for (i = 0; i < cnt / 2; i ++) {
	      lbuffer [i] = std::complex<float> (
	                                 buffer [2 * i] / 32767.0,
	      	                         buffer [2 * i + 1] / 32767.0);
	   }
	   toOutput (lbuffer, cnt / 2);
	   return;
	}

	if (pcmRate == 12000) {
	   std::complex<float> lbuffer [2 * cnt + 4];
	   for (i = 0; i < cnt / 2; i ++) {
	      upFilter_12000. Filter (std::complex<float> (
	                                    buffer [2 * i] / 32767.0,
	                                    buffer [2 * i + 1] / 32767.0),
	                              &lbuffer [4 * i]);
	   }
	   toOutput (lbuffer, 2 * cnt);
	   return;
	}
	if (pcmRate == 24000) {
	   std::complex<float> lbuffer [cnt];
	   for (i = 0; i < cnt / 2; i ++) {
	      upFilter_12000. Filter (std::complex<float> (
	                                    buffer [2 * i] / 32767.0,
	                                    buffer [2 * i + 1] / 32767.0),
	                              &lbuffer [2 * i]);
	   }
	   toOutput (lbuffer, cnt);
	   return;
	}
}

//
//	AudioParams contains the relevant parameters:
//	- audioSampleRate
//	- SBR_used
//	- audioMode
//
bool	aacProcessor_faad::checkfor (uint8_t	audioRate,
	                             bool	newSBR,
	                             uint8_t	audioMode) {
	if (theDecoder == NULL)
	   theDecoder = NeAACDecOpen ();
	if (theDecoder == NULL)		// should not happen
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

bool	aacProcessor_faad::initDecoder (int16_t	audioSampleRate,
	                                bool		SBR_used,
	                                uint8_t	audioMode) {
int16_t aacRate = 12000;
uint8_t	aacMode	= DRMCH_SBR_PS_STEREO;
int	s;
QString	text;

	if (theDecoder == NULL)
	   theDecoder = NeAACDecOpen ();
	if (theDecoder == NULL) 	// should not happen
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
	s = NeAACDecInitDRM (&theDecoder, aacRate, (uint8_t)aacMode);
	return s >= 0;
}
//
//	Later on, we'll change this such that the return
//	vector is for further processing
void	aacProcessor_faad::decodeFrame (uint8_t	*AudioFrame,
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
	if (theDecoder == NULL) 
	   return;
	outBuf = (int16_t*) NeAACDecDecode (theDecoder,
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

void aacProcessor_faad::closeDecoder() {
	if (theDecoder != NULL) {
	   NeAACDecClose (theDecoder);
	   theDecoder = NULL;
	}
}

