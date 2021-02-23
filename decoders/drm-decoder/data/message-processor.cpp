#
/*
 *    Copyright (C) 2015
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
#include	<QObject>
#include	"radio-constants.h"
#include	<cstring>
#include	"drm-decoder.h"
#include	"drm-aacdecoder.h"
#include	"message-processor.h"

#define	M_IDLE		0100
#define	M_HEADER	0101
#define	M_BODY		0102
#define	M_CRC		0103

static
uint16_t crc16_bytewise (uint8_t in [], int32_t N);

	messageProcessor::messageProcessor	(drmDecoder *drm) {
	this	-> drm	= drm;
	messageState	= M_IDLE;
	connect (this, SIGNAL (sendMessage (QString)),
	         drm, SLOT (showMessage (QString)));
}

	messageProcessor::~messageProcessor	(void) {
}
//
//	we have 4 byte segments, forming a "segment"
void	messageProcessor::processMessage (uint8_t *v, int16_t pos) {
uint8_t messagePart [4];
int16_t	i;
uint8_t firstFlag, lastFlag, commandFlag;

	for (i = 0; i < 4; i ++)
	   messagePart [i] = 0;
	for (i = 0; i < 4 * 8;  i ++) {
	   setBit (messagePart, i, v [pos + i]);
	}

	switch (messageState) {
	   case M_IDLE:
	      for (i = 0; i < 4; i ++)
	         if (messagePart [i] != 0xFF)
	            return;
	      messageState = M_HEADER;
	      break;

	   case M_HEADER:
	      firstFlag		= (messagePart [0] >> 6) & 01;
	      lastFlag		= (messagePart [0] >> 5) & 01;
	      commandFlag	= (messagePart [0] >> 4) & 01;
	      if (commandFlag == 0)
	         byteCount = (messagePart [0] & 0xF) + 1;
	      else
	         byteCount = 16;
	      (void)firstFlag; (void)lastFlag;
	      messageState = M_BODY;
	      byteTeller		= 0;
	      theMessage [byteTeller ++] = messagePart [0];
	      theMessage [byteTeller ++] = messagePart [1];
	      theMessage [byteTeller ++] = messagePart [2];
	      theMessage [byteTeller ++] = messagePart [3];
	      break;

	   case M_BODY:
	      for (i = 0; i < 4; i ++)  {
	         theMessage [byteTeller ++] = messagePart [i];
	         if (byteTeller >= 2 + byteCount + 2) {
	            if (check_CRC (theMessage, byteTeller))
	               addSegment (theMessage, byteTeller);
	            messageState = M_IDLE;
	         }
	      }
	      break;

	   default:
	      messageState = M_IDLE;
	}
}

void	messageProcessor::setBit (uint8_t *v, int16_t ind, uint8_t bit) {
int16_t	byte = ind / 8;
int16_t theBit = 01 << (7 - ind % 8);

	if (bit == 01)
	   v [byte] |= theBit;
}

bool	messageProcessor::check_CRC (uint8_t *v, int16_t cnt) {
	return (crc16_bytewise (v, cnt) == 0);
}

void	messageProcessor::addSegment (uint8_t *v, int16_t cnt) {
int16_t	i;
uint8_t firstFlag	= (v [0] >> 6) & 01;
uint8_t lastFlag	= (v [0] >> 5) & 01;

	if (firstFlag != 0)
	   myMessage	= QString ();
	for (i = 0; i < cnt - 4; i ++)
	   myMessage = myMessage. append (QChar (v [2 + i]));
	if (lastFlag != 0)
	   myMessage = myMessage. append ("<<<");
	sendMessage (myMessage);
}

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
	      y = (((b >> 15) + ((uint32_t) ((in[i] >> j)) & 0x01)) ^ 0x01) & 0x01;	/* extra parent pa0mbo */
	      if (y == 1)
	         b = ((b << 1) ^ x);
	      else
	         b = (b << 1);
	   }
	}
	return (b & 0xFFFF);
}

