#
/*
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
 */
#
#include	"data-processor.h"
#include	"drm-decoder.h"
#include	"msc-config.h"
#include	<stdio.h>
#include	"fec-handler.h"
#include	<float.h>
#include	<math.h>
#include	"packet-assembler.h"

static	inline
uint16_t	get_MSCBits (uint8_t *v, int16_t offset, int16_t nr) {
int16_t		i;
uint16_t	res	= 0;

	for (i = 0; i < nr; i ++) 
	   res = (res << 1) | (v [offset + i] & 01);

	return res;
}
//
static	
uint16_t crc16_bytewise (uint8_t in [], int32_t N) {
int32_t i;
int16_t j;
uint32_t b = 0xFFFF;
uint32_t x = 0x1021;	/* (1) 0001000000100001 */
uint32_t y;

	for (i = 0; i < N - 2; i++) {
	   for (j = 7; j >= 0; j--) {
	      y = ((b >> 15) + ((uint32_t) (in [i] >> j) & 0x01)) & 0x01;
	      if (y == 1)
	         b = ((b << 1) ^ x);
	      else
	         b = (b << 1);
	   }
	}
	for (i = N - 2; i < N; i++) {
	   for (j = 7; j >= 0; j--) {
	      y = (((b >> 15) + ((uint32_t) ((in[i] >> j) & 0x01))) ^ 0x01) & 0x01;	/* extra parent pa0mbo */
	      if (y == 1)
	         b = ((b << 1) ^ x);
	      else
	         b = (b << 1);
	   }
	}
	return (b & 0xFFFF);
}

	dataProcessor::dataProcessor	(mscConfig *msc,
	                                 drmDecoder *drm):
	                                      my_messageProcessor (drm) ,
	                                      my_aacDecoder (),
	                                      upFilter_24000 (5, 12000, 48000),
	                                      upFilter_12000 (5, 6000, 48000) {
	
	this	-> msc			= msc;
	this	-> drmMaster		= drm;
	audioChannel			= 1;
	dataChannel			= 3;
//
//	do not change the order of the next two initializations
//	although here they are dummies
	my_packetAssembler		= new packetAssembler (msc, drm, -1);
	my_fecHandler			= new fecHandler (my_packetAssembler);
	connect (this, SIGNAL (show_audioMode (QString)),
	         drmMaster, SLOT (show_audioMode (QString)));
	connect (this, SIGNAL (putSample (float, float)),
	         drmMaster, SLOT (sampleOut (float, float)));
	connect (this, SIGNAL (faadSuccess (bool)),
	         drmMaster, SLOT (faadSuccess (bool)));
	show_audioMode (" ");
	selectedAudioService		= -1;
	selectedDataService		= -1;
}

	dataProcessor::~dataProcessor	(void) {
	delete my_fecHandler;
	delete	my_packetAssembler;
}

//
//	The first thing we do is to define the borders for the
//	streams and dispatch on the kind of stream
void	dataProcessor::process		(uint8_t *v, int16_t size) {
int16_t	i;
int16_t	startPosA	= 0;
int16_t	startPosB	= 0;
int16_t	new_dataChannel	= drmMaster	-> getDataChannel ();
	audioChannel	= drmMaster	-> getAudioChannel ();
	if (new_dataChannel != dataChannel) {
	   uint16_t applicationId = msc -> streams [new_dataChannel]. applicationId;
	   delete my_packetAssembler;
	   delete my_fecHandler;
	   my_packetAssembler		= new packetAssembler (msc,
	                                                       drmMaster,
	                                                       applicationId);
	   my_fecHandler		= new fecHandler (my_packetAssembler);
	   dataChannel = new_dataChannel;
	}
	   
	(void)size;
//	we first compute the length of the HP part, which - apparently -
//	is the start of the LP part
	for (i = 0; i < msc -> numofStreams; i ++) 
	   startPosB += msc -> streams [i]. lengthHigh;

	for (i = 0; i < msc -> numofStreams; i ++) {
	   int16_t lengthA = msc -> streams [i]. lengthHigh;
	   int16_t lengthB  = msc -> streams [i]. lengthLow;
	   if (msc -> streams [i]. soort == mscConfig::AUDIO_STREAM &&
	                               (audioChannel == i)) {
	      process_audio (v, i,
	                     startPosA, lengthA,
	                     startPosB, lengthB);
	      my_messageProcessor.  processMessage (v,
	                                       8 * (startPosB + lengthB - 4));
	   }
	   else
	   if (msc -> streams [i]. soort == mscConfig::DATA_STREAM &&
	                                (dataChannel == i)) {
//
//	just a reminder:
//	if the packetmode indicator == 1 we have an asynchronous stream
//	if 0, we have a synchronous stream which we don't handle yet
	      if (msc -> streams [i]. packetModeInd == 1) {
	         process_packets (v, i,
	                          startPosA, lengthA,
	                          startPosB, lengthB);
	      }
	      else {;	// synchronous stream
	      }
	   }
	
	   startPosA	+= lengthA;
	   startPosB	+= lengthB;
	}
}
//
//	Finally, we are here. We have a segment v with length l
//	that contains audio. Parameters are to be found
//	at the mscIndex's description in the mscConfig
void	dataProcessor::process_audio (uint8_t *v, int16_t mscIndex,
	                              int16_t startHigh, int16_t lengthHigh,
	                              int16_t startLow,  int16_t lengthLow) {
uint8_t	audioCoding		= msc -> streams [mscIndex]. audioCoding;

	switch (audioCoding) {
	   case 0:		// AAC
	      show_audioMode (QString ("AAC"));
	      process_aac (v, mscIndex,
	                   startHigh, lengthHigh,
	                   startLow,  lengthLow);
	      return;

	   case 1:		// CELP
	      show_audioMode (QString ("CELP"));
	      process_celp (v, mscIndex,
	                    startHigh, lengthHigh,
	                    startLow,  lengthLow);
	      return;

	   case 2:		// HVXC
	      show_audioMode (QString ("HVXC"));
	      process_hvxc (v, mscIndex,
	                    startHigh, lengthHigh,
	                    startLow,  lengthLow);
	      return;

	   default:
	   case 3:
	      fprintf (stderr,
	               "unsupported format audioCoding (%d)\n", audioCoding);
	      return;
	}
}

void	dataProcessor::process_packets (uint8_t *v, int16_t mscIndex,
	                                int16_t startHigh, int16_t lengthHigh,
	                                int16_t startLow,  int16_t lengthLow) {
	if (lengthHigh != 0) 
	   handle_uep_packets (v, mscIndex, startHigh, lengthHigh,
	                            startLow, lengthLow);
	else
	   handle_eep_packets (v, mscIndex, startLow, lengthLow);
}
//
//
//	little confusing: start- and length specifications are
//	in bytes, we are working internally in bits
void	dataProcessor::process_aac (uint8_t *v, int16_t mscIndex,
	                            int16_t startHigh, int16_t lengthHigh,
	                            int16_t startLow,  int16_t lengthLow) {
	if (lengthHigh != 0) 
	   handle_uep_audio (v, mscIndex, startHigh, lengthHigh,
	                            startLow, lengthLow - 4);
	else 
	   handle_eep_audio (v, mscIndex,  startLow, lengthLow - 4);
}

static
int16_t outBuffer [8 * 960];
static
audioFrame f [20];
void	dataProcessor::handle_uep_audio (uint8_t *v, int16_t mscIndex,
	                           int16_t startHigh, int16_t lengthHigh,
	                           int16_t startLow, int16_t lengthLow) {
int16_t	headerLength, i, j;
int16_t	usedLength	= 0;
int16_t	crcLength	= 1;
int16_t	payloadLength;

//	first the globals
	numFrames = msc -> streams [mscIndex]. audioSamplingRate == 1 ? 5 : 10;
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
	playOut (mscIndex);
}

void	dataProcessor::handle_uep_packets (uint8_t *v, int16_t mscIndex,
	                           int16_t startHigh, int16_t lengthHigh,
	                           int16_t startLow, int16_t lengthLow) {
int16_t	i;
uint8_t dataBuffer [lengthLow + lengthHigh];
	
	for (i = 0; i < lengthHigh; i ++)
	   dataBuffer [i] = get_MSCBits (v, (startHigh + i) * 8, 8);
	for (i = 0; i < lengthLow; i ++)
	   dataBuffer [lengthHigh + i] =
	                    get_MSCBits (v, (startLow + i) * 8, 8);
	if (msc -> streams [mscIndex]. FEC)
	   handle_packets_with_FEC (dataBuffer,
	                                lengthLow + lengthHigh, mscIndex);
	else
	   handle_packets (dataBuffer, lengthLow + lengthHigh, mscIndex);
}

void	dataProcessor::handle_eep_packets (uint8_t *v, int16_t mscIndex,
	                           int16_t startLow, int16_t lengthLow) {
uint8_t	dataBuffer [lengthLow];
int16_t i;

	for (i = 0; i < lengthLow; i ++)
	   dataBuffer [i] = get_MSCBits (v, (startLow + i) * 8, 8);
	if (msc -> streams [mscIndex]. FEC)
	   handle_packets_with_FEC (dataBuffer, lengthLow, mscIndex);
	else
	   handle_packets (dataBuffer, lengthLow, mscIndex);
}

void	dataProcessor::handle_packets_with_FEC (uint8_t *v,
	                                        int16_t length,
	                                        uint8_t mscIndex) {
uint8_t *packetBuffer;
int16_t	packetLength = msc -> streams [mscIndex]. packetLength + 3;
int16_t	i;
static	int	cnt	= 0;
//
//	first check the RS decoder
	my_fecHandler -> checkParameters (
	          msc -> streams [mscIndex]. R,
	          msc -> streams [mscIndex]. C, 
	          msc -> streams [mscIndex]. packetLength + 3,
	          mscIndex);

	for (i = 0; i < length / packetLength; i ++) {
	   packetBuffer = &v [i * packetLength];
//	Fetch relevant info from the stream

//	packetBuffer [0] contains the header
	   uint8_t header	= packetBuffer [0];
//	   uint8_t firstBit	= (header & 0x80) >> 7;
//	   uint8_t lastBit	= (header & 0x40) >> 6;
	   uint8_t packetId	= (header & 0x30) >> 4;
	   uint8_t PPI		= (header & 0x8) >> 3;
//	   uint8_t CI		= header & 0x7;
//
	   if ((packetId == 3) && (PPI == 0)) {
	      my_fecHandler -> fec_packet (packetBuffer, packetLength);
	      cnt = 0;
	   }
	   else 
//	   if (packetId != 0) 
//	      if (PPI)
//	         fprintf (stderr, "filler with %d\n", packetBuffer [1]);
//	      else
//	      fprintf (stderr, "rommelpacket ertussen PPI = %d, CI = %d\n",
//	                               PPI, CI);
	   if (packetId == 0) {
	      my_fecHandler -> data_packet (packetBuffer, packetLength);
	      cnt ++;
	   }
	}
}

void	dataProcessor::handle_packets (uint8_t *v, int16_t length,
	                               uint8_t mscIndex) {
uint8_t *packetBuffer;
int16_t	packetLength = msc -> streams [mscIndex]. packetLength + 3;
int16_t	i;

	for (i = 0; i < length / packetLength; i ++) {
	   packetBuffer = &v [i * packetLength];
//	Fetch relevant info from the stream
//
//	first a crc check
	   if (crc16_bytewise (packetBuffer, packetLength) != 0)
	      continue;

	   my_packetAssembler -> assemble (packetBuffer,
	                                   packetLength, mscIndex);
	}
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
void	dataProcessor::handle_eep_audio (uint8_t *v,
	                                 int16_t mscIndex,
	                                 int16_t startLow,
	                                 int16_t lengthLow) {
int16_t		i, j;
int16_t		headerLength;
int16_t		crc_start;
int16_t		payLoad_start;
int16_t		payLoad_length = 0;

	numFrames = msc -> streams [mscIndex]. audioSamplingRate == 1 ? 5 : 10;
	headerLength = numFrames == 10 ? (9 * 12 + 4) / 8 : (4 * 12) / 8;

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


void	dataProcessor::playOut (int16_t	mscIndex) {
int16_t	i;
uint8_t	audioSamplingRate	= msc -> streams [mscIndex]. audioSamplingRate;
uint8_t	SBR_flag		= msc -> streams [mscIndex]. SBR_flag;
uint8_t	audioMode		= msc -> streams [mscIndex]. audioMode;

	if (!my_aacDecoder.  checkfor (audioSamplingRate,
	                                        SBR_flag, 
	                                        audioMode)) {
	   faadSuccess (false);
	   return;
	}

	for (i = 0; i < numFrames; i ++) {
	   int16_t	index = i;
	   bool		convOK;
	   int16_t	cnt;
	   int32_t	rate;
	   my_aacDecoder.  decodeFrame ((uint8_t *)(&f [index]. aac_crc),
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
//
void	dataProcessor::process_celp (uint8_t *v, int16_t mscIndex,
	                            int16_t startHigh, int16_t lengthHigh,
	                            int16_t startLow,  int16_t lengthLow) {
uint8_t	audioCoding	= msc -> streams [mscIndex]. audioCoding;
uint8_t	SBR_flag	= msc -> streams [mscIndex]. SBR_flag;
//uint8_t	audioMode	= msc -> streams [mscIndex]. audioMode;
//uint8_t	audioSamplingRate	= msc -> streams [mscIndex]. audioSamplingRate;
//uint8_t	textFlag	= msc -> streams [mscIndex]. textFlag;
//uint8_t	enhancementFlag	= msc -> streams [mscIndex]. enhancementFlag;
//uint8_t	coderField	= msc -> streams [mscIndex]. coderField;

	(void)v; (void)mscIndex;
	(void)startHigh; (void)startLow;
	(void)lengthHigh; (void)lengthLow;
	if (SBR_flag == 0)
	   fprintf (stderr, "rfa = %d, CELP_CRC = %d\n",
	                     (audioCoding & 02) >> 1, audioCoding & 01);
	else
	   fprintf (stderr, "sbr_header_flag = %d, CELP_CRC = %d\n",
	                    (audioCoding & 02) >> 1, audioCoding & 01);
}

void	dataProcessor::process_hvxc (uint8_t *v, int16_t mscIndex,
	                            int16_t startHigh, int16_t lengthHigh,
	                            int16_t startLow,  int16_t lengthLow) {
uint8_t	audioCoding	= msc -> streams [mscIndex]. audioCoding;
//uint8_t	SBR_flag	= msc -> streams [mscIndex]. SBR_flag;
//uint8_t	audioMode	= msc -> streams [mscIndex]. audioMode;
//uint8_t	audioSamplingRate	= msc -> streams [mscIndex]. audioSamplingRate;
//uint8_t	textFlag	= msc -> streams [mscIndex]. textFlag;
//uint8_t	enhancementFlag	= msc -> streams [mscIndex]. enhancementFlag;
//uint8_t	coderField	= msc -> streams [mscIndex]. coderField;
	(void)v; (void)mscIndex;
	(void)startHigh; (void)startLow;
	(void)lengthHigh; (void)lengthLow;
	   fprintf (stderr, "HVXC_rate = %d, HVXC_CRC = %d\n",
	                    (audioCoding & 02) >> 1, audioCoding & 01);
}

void	dataProcessor::toOutput (float *b, int16_t cnt) {
int16_t	i;
	if (cnt == 0)
	   return;

	for (i = 0; i < cnt / 2; i ++)
	   putSample (b [2 * i], b [2 * i + 1]);
}

void	dataProcessor::writeOut (int16_t *buffer, int16_t cnt,
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
	      DSPCOMPLEX help =
	           upFilter_12000. Pass (DSPCOMPLEX (buffer [2 * i] / 32767.0,
	                                           buffer [2 * i + 1] / 32767.0));
	      lbuffer [8 * i + 0] = real (help);
	      lbuffer [8 * i + 1] = imag (help);
	      help = upFilter_12000. Pass (DSPCOMPLEX (0, 0));
	      lbuffer [8 * i + 2] = real (help);
	      lbuffer [8 * i + 3] = imag (help);
	      help = upFilter_12000. Pass (DSPCOMPLEX (0, 0));
	      lbuffer [8 * i + 4] = real (help);
	      lbuffer [8 * i + 5] = imag (help);
	      help = upFilter_12000. Pass (DSPCOMPLEX (0, 0));
	      lbuffer [8 * i + 6] = real (help);
	      lbuffer [8 * i + 7] = imag (help);
	   }
	   toOutput (lbuffer, 4 * cnt);
	   return;
	}
	if (pcmRate == 24000) {
	   float lbuffer [2 * cnt];
	   for (i = 0; i < cnt / 2; i ++) {
	      DSPCOMPLEX help =
	           upFilter_24000. Pass (DSPCOMPLEX (buffer [2 * i] / 32767.0,
	                                           buffer [2 * i + 1] / 32767.0));
	      lbuffer [4 * i + 0] = real (help);
	      lbuffer [4 * i + 1] = imag (help);
	      help = upFilter_24000. Pass (DSPCOMPLEX (0, 0));
	      lbuffer [4 * i + 2] = real (help);
	      lbuffer [4 * i + 3] = imag (help);
	   }
	   toOutput (lbuffer, 2 * cnt);
	   return;
	}
}


