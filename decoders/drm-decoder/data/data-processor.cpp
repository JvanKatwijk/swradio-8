#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the drm backend
 *
 *    drm backend is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm backend is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with drm backend; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#include	<stdio.h>
#include	<float.h>
#include	<math.h>
#include	"basics.h"
#include	"data-processor.h"
#include	"drm-decoder.h"
#include	"state-descriptor.h"
#include	"fec-handler.h"
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

	dataProcessor::dataProcessor	(stateDescriptor *theState,
	                                 drmDecoder *drm):
	                                   my_messageProcessor (drm) ,
	                                   my_aacProcessor (theState, drm)
#ifdef	__WITH_FDK_AAC__
	                                   ,my_xheaacProcessor (theState,
	                                                                drm)
#endif
	{
	this	-> theState		= theState;
	this	-> drmMaster		= drm;
	audioChannel			= 1;
	dataChannel			= 3;
//
//	do not change the order of the next two initializations
//	although here they are dummies
	my_packetAssembler		= new packetAssembler (theState,
	                                                       drm, -1);
	my_fecHandler			= new fecHandler (my_packetAssembler);
	connect (this, SIGNAL (show_audioMode (QString)),
	         drmMaster, SLOT (show_audioMode (QString)));
	connect (this, SIGNAL (show_datacoding (QString)),
	         drmMaster, SLOT (show_datacoding (QString)));
	show_audioMode (" ");
	selectedAudioService		= -1;
	selectedDataService		= -1;
}

	dataProcessor::~dataProcessor	(void) {
	delete my_fecHandler;
	delete	my_packetAssembler;
}

static
QString stringfor (uint8_t qamMode) {
	if (qamMode == QAM16)
	   return "QAM16";
	if (qamMode == QAM64)
	   return "QAM64";
	return "QAM4";
}

//	The first thing we do is to define the borders for the
//	streams and dispatch on the kind of stream
void	dataProcessor::process		(uint8_t *v, int16_t size) {
int16_t	i;
int16_t	startPosA	= 0;
int16_t	startPosB	= 0;
int16_t	new_dataChannel	= drmMaster	-> getDataChannel ();
	audioChannel	= drmMaster	-> getAudioChannel ();
	if (new_dataChannel != dataChannel) {
	   uint16_t applicationId = theState -> streams [new_dataChannel].
	                                                      applicationId;
	   delete my_packetAssembler;
	   delete my_fecHandler;
//	   fprintf (stderr, "the state is %d\n", theState -> mscMode);
	   show_datacoding (stringfor (theState -> QAMMode));
	   my_packetAssembler		= new packetAssembler (theState,
	                                                       drmMaster,
	                                                       applicationId);
	   my_fecHandler		= new fecHandler (my_packetAssembler);
	   dataChannel			= new_dataChannel;
	}

	(void)size;
//	we first compute the length of the HP part, which - apparently -
//	is the start of the LP part
	for (i = 0; i < theState -> numofStreams; i ++) 
	   startPosB += theState -> streams [i]. lengthHigh;

	for (i = 0; i < theState -> numofStreams; i ++) {
	   int16_t lengthA = theState -> streams [i]. lengthHigh;
	   int16_t lengthB = theState -> streams [i]. lengthLow;
	   if ((theState -> streams [i]. soort ==
	                           stateDescriptor::AUDIO_STREAM) &&
	                               (audioChannel == i)) {
	      process_audio (v, i, startPosA, lengthA,
	                           startPosB, lengthB);
	      my_messageProcessor.  processMessage (v,
	                                       8 * (startPosB + lengthB - 4));
	   }
	   else
	   if ((theState -> streams [i]. soort ==
	                           stateDescriptor::DATA_STREAM) &&
	                                (dataChannel == i)) {
//
//	just a reminder:
//	if the packetmode indicator == 1 we have an asynchronous stream
//	if 0, we have a synchronous stream which we don't handle yet
	      if (theState -> streams [i]. packetModeInd == 1) {
	         process_packets (v, i,
	                          startPosA, lengthA,
	                          startPosB, lengthB);
	      }
	      else {	// synchronous stream
	         process_syncStream (v, i,
	                             startPosA, lengthA,
	                             startPosB, lengthB);
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
uint8_t	audioCoding		= theState -> streams [mscIndex]. audioCoding;

	switch (audioCoding) {
	   case 0:		// AAC
	      show_audioMode (QString ("AAC"));
	      my_aacProcessor. process_aac (v, mscIndex,
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

	   case 3:
	      show_audioMode (QString ("xHE-AAC"));
#ifdef	__WITH_FDK_AAC__
	      my_xheaacProcessor. process_usac (v, mscIndex,
	                                        startHigh, lengthHigh,
	                                        startLow, lengthLow);
#endif
	      return;

	   default:
	      fprintf (stderr, "unknown audiomode %d\n", audioCoding);
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

void	dataProcessor::process_syncStream (uint8_t *v, int16_t mscIndex,
	                                   int16_t startHigh,
	                                   int16_t lengthHigh,
	                                   int16_t startLow,
	                                   int16_t lengthLow) {
	if (lengthHigh != 0) 
	   handle_uep_syncStream (v, mscIndex, startHigh, lengthHigh,
	                            startLow, lengthLow);
	else
	   handle_eep_syncStream (v, mscIndex, startLow, lengthLow);
}

void	dataProcessor::handle_uep_syncStream (uint8_t *v, int16_t mscIndex,
	                           int16_t startHigh, int16_t lengthHigh,
	                           int16_t startLow, int16_t lengthLow) {
int16_t	i;
uint8_t dataBuffer [lengthLow + lengthHigh];
	
	for (i = 0; i < lengthHigh; i ++)
	   dataBuffer [i] = get_MSCBits (v, (startHigh + i) * 8, 8);
	for (i = 0; i < lengthLow; i ++)
	   dataBuffer [lengthHigh + i] =
	                    get_MSCBits (v, (startLow + i) * 8, 8);
	handle_syncStream (dataBuffer, lengthLow + lengthHigh, mscIndex);
}

void	dataProcessor::handle_eep_syncStream (uint8_t *v, int16_t mscIndex,
	                                      int16_t startLow,
	                                      int16_t lengthLow) {
uint8_t	dataBuffer [lengthLow];
int16_t i;

	for (i = 0; i < lengthLow; i ++)
	   dataBuffer [i] = get_MSCBits (v, (startLow + i) * 8, 8);
	handle_syncStream (dataBuffer, lengthLow, mscIndex);
}
//

void	dataProcessor::handle_syncStream (uint8_t *dataBuffer,
	                                  int16_t length,
	                                  int16_t index) {
//	fprintf (stderr, "dataBuffer met lengte %d\n", length);
//	for (int i = 0; i < 40; i ++)
//	   fprintf (stderr, "%x ", dataBuffer [i]);
//	fprintf (stderr, "\n");
}

void	dataProcessor::handle_uep_packets (uint8_t *v, int16_t mscIndex,
	                                   int16_t startHigh,
	                                   int16_t lengthHigh,
	                                   int16_t startLow,
	                                   int16_t lengthLow) {
int16_t	i;
uint8_t dataBuffer [lengthLow + lengthHigh];
	
	for (i = 0; i < lengthHigh; i ++)
	   dataBuffer [i] = get_MSCBits (v, (startHigh + i) * 8, 8);
	for (i = 0; i < lengthLow; i ++)
	   dataBuffer [lengthHigh + i] =
	                    get_MSCBits (v, (startLow + i) * 8, 8);

	if (theState -> streams [mscIndex]. FEC)
	   handle_packets_with_FEC (dataBuffer,
	                            lengthLow + lengthHigh, mscIndex);
	else
	   handle_packets (dataBuffer, lengthLow + lengthHigh, mscIndex);
}

void	dataProcessor::handle_eep_packets (uint8_t *v, int16_t mscIndex,
	                                   int16_t startLow,
	                                   int16_t lengthLow) {
uint8_t	dataBuffer [lengthLow];
int16_t i;

	for (i = 0; i < lengthLow; i ++)
	   dataBuffer [i] = get_MSCBits (v, (startLow + i) * 8, 8);

	if (theState -> streams [mscIndex]. FEC)
	   handle_packets_with_FEC (dataBuffer, lengthLow, mscIndex);
	else
	   handle_packets (dataBuffer, lengthLow, mscIndex);
}

void	dataProcessor::handle_packets_with_FEC (uint8_t *v,
	                                        int16_t length,
	                                        uint8_t mscIndex) {
uint8_t *packetBuffer;
int16_t	packetLength = theState -> streams [mscIndex]. packetLength + 3;
int16_t	i;
static	int	cnt	= 0;
//
//	first check the RS decoder
	my_fecHandler -> checkParameters (
	          theState -> streams [mscIndex]. R,
	          theState -> streams [mscIndex]. C, 
	          theState -> streams [mscIndex]. packetLength + 3,
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
int16_t	packetLength = theState -> streams [mscIndex]. packetLength + 3;
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

void	dataProcessor::process_celp (uint8_t *v, int16_t mscIndex,
	                            int16_t startHigh, int16_t lengthHigh,
	                            int16_t startLow,  int16_t lengthLow) {
uint8_t	audioCoding	= theState -> streams [mscIndex]. audioCoding;
uint8_t	SBR_flag	= theState -> streams [mscIndex]. SBR_flag;
//uint8_t audioMode	= theState -> streams [mscIndex]. audioMode;
//uint8_t audioSamplingRate	= theState -> streams [mscIndex].
//	                                                   audioSamplingRate;
//uint8_t textFlag	= theState -> streams [mscIndex]. textFlag;
//uint8_t enhancementFlag	= theState -> streams [mscIndex].
//	                                                   enhancementFlag;
//uint8_t coderField	= theState -> streams [mscIndex]. coderField;

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
uint8_t	audioCoding	= theState -> streams [mscIndex]. audioCoding;
//uint8_t SBR_flag	= theState -> streams [mscIndex]. SBR_flag;
//uint8_t audioMode	= theState -> streams [mscIndex]. audioMode;
//uint8_t audioSamplingRate	= theState -> streams [mscIndex].
//	                                                     audioSamplingRate;
//uint8_t textFlag	= theState -> streams [mscIndex]. textFlag;
//uint8_t enhancementFlag	= theState -> streams [mscIndex].
//	                                                     enhancementFlag;
//uint8_t coderField	= theState -> streams [mscIndex]. coderField;
	(void)v; (void)mscIndex;
	(void)startHigh; (void)startLow;
	(void)lengthHigh; (void)lengthLow;
	   fprintf (stderr, "HVXC_rate = %d, HVXC_CRC = %d\n",
	                    (audioCoding & 02) >> 1, audioCoding & 01);
}

