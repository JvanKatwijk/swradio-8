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
#include	"msc-config.h"
#include	<string.h>

		mscConfig::mscConfig	(uint8_t Mode, uint8_t Spectrum) {
int16_t	i;
	this	-> Mode			= Mode;
	this	-> Spectrum		= Spectrum;
	this	-> currentConfig	= true;
	for (i = 0; i < 3; i ++)
	   memset (&streams [i], 0, sizeof (streams [0]));
}

		mscConfig::~mscConfig	(void) {
}

//
//	return the actual protection level from prot, for stream nr
void		mscConfig::protLevel	(uint8_t prot, int16_t nr,
	                                 int16_t *RX, int16_t *RY) {

	if (QAMMode == QAM16) {
	   switch (prot){
	      case 0:
	         if (nr == 0) {
	            *RX	= 1; *RY = 3;
	         }
	         else {		// nr better be '1'
	            *RX = 2; *RY = 3;
	         }
	         return;
	      default:		// should not happen
	      case 1:
	         if (nr == 0) {
	            *RX = 1; *RY = 2;
	          }
	          else {	// nr better be '1'
	             *RX = 3; *RY = 4;
	          }
	          return;
	   }
	   return;
	}

	if (QAMMode == QAM64) {
	   switch (prot) {
	      default:		// should not happen
	      case 0:
	         if (nr == 0) {
	            *RX = 1; *RY = 4;
	         }
	         else 
	         if (nr == 1) {
	            *RX = 1; *RY = 2;
	         }
	         else {		// nr better be '2'
	            *RX = 3; *RY = 4;
	         }
	         return;
	      case 1:
	         if (nr == 0) {
	            *RX = 1; *RY = 3;
	         }
	         else 
	         if (nr == 1) {
	            *RX = 2; *RY = 3;
	         }
	         else {		// nr better be '2'
	            *RX = 4; *RY = 5;
	         }
	         return;
	      case 2:
	         if (nr == 0) {
	            *RX = 1; *RY = 2;
	         }
	         else 
	         if (nr == 1) {
	            *RX = 3; *RY = 4;
	         }
	         else {		// nr better be '2'
	            *RX = 7; *RY = 8;
	         }
	         return;
	      case 3:
	         if (nr == 0) {
	            *RX = 2; *RY = 3;
	         }
	         else 
	         if (nr == 1) {
	            *RX = 4; *RY = 5;
	         }
	         else {		// nr better be '2'
	            *RX = 8; *RY = 9;
	         }
	         return;
	   }
	}
}

float		mscConfig::getRp	(uint8_t prot, int16_t nr) {
int16_t	RX, RY;

	protLevel (prot, nr, &RX, &RY);
	return (float (RX))/RY;
}

int16_t		mscConfig::getRYlcm_64	(int16_t protLevel) {
	if (protLevel == 3)
	   return 45;
	if (protLevel == 2)
	   return 8;
	if (protLevel == 1)
	   return 15;
	return 4;
}

int16_t		mscConfig::getRYlcm_16	(int16_t protLevel) {
	if (protLevel == 1)
	   return 4;
	return 3;
}

//
//	size of Mux (in samples) of the current Mux
int16_t		mscConfig::muxSize	(void) {
	return mscCells () / 3;
}
//
//	interleaver depth for the current configuration
int16_t		mscConfig::muxDepth	(void) {
	if (interleaverDepth == 1)
	   return 1;
	if (Mode == 4)
	   return 6;
	return 5;
}

int16_t		mscConfig::mscCells  (void) {
	switch (Mode) {
	   case 1:
	      switch (Spectrum) {
	         case	0:	return 3778;
	         case	1:	return 4368;
	         case	2:	return 7897;
	         default:
	         case	3:	return 8877;
	         case	4:	return 16394;
	      }

	   default:
	   case 2:
	      switch (Spectrum) {
	         case	0:	return 2900;
	         case	1:	return 3330;
	         case	2:	return 6153;
	         default:
	         case	3:	return 7013;
	         case	4:	return 12747;
	      }

	   case 3:
	      return 5532;

	   case 4:
	      return 3679;
	}
}

int16_t	mscConfig::getnrAudio	(void) {
int16_t i;
int16_t	amount	= 0;

	for (i = 0; i < numofStreams; i ++)
	   if (streams [i]. soort == mscConfig::AUDIO_STREAM)
	      amount ++;
	return amount;
}

int16_t	mscConfig::getnrData	(void) {
int16_t i;
int16_t	amount	= 0;

	for (i = 0; i < numofStreams; i ++)
	   if (streams [i]. soort == mscConfig::DATA_STREAM)
	      amount ++;
	return amount;
}

