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
#include	"packet-assembler.h"
#include	"virtual-datahandler.h"
#include	<stdio.h>
#include	"mot-data.h"
#include	"state-descriptor.h"
//
//	The function of the packetAssembler is - as the name suggests -
//	assembling the packets for the various application handlers.
//
//	The assembler hands the packages over to the application handler
//	when completed.
//	The applicationId is a parameter, the applicationhandler is
//	local to the packetassembler

	packetAssembler::packetAssembler	(stateDescriptor *theState,
	                                         drmDecoder *drm,
	                                         uint16_t applicationId) {
	fprintf (stderr, "applicationId = %x\n", applicationId);
	this	-> theState	= theState;
	mscIndex	= 0;
	waitforFirst	= true;
	old_CI		= 11;		// illegal value
	switch (applicationId) {
	   case 02:		// slide
	   case 03:		// website
	   case 60:
	      my_dataHandler	= new motHandler (drm);
	      break;
	   case 0x44a:
//	      my_dataHandler	= new journalineHandler (drm);
//	      break;
	   case 59:
//	      my_dataHandler	= new ip_dataHandler (mr);
//	      break;
	   default:
	       my_dataHandler	= new virtual_dataHandler ();
	}
}

	packetAssembler::~packetAssembler	(void) {
	delete my_dataHandler;
}
//
//	we get in the mscpackets and assemble packets for the application,
//	basically simple, take into account the Continuation Indices
//	and the FL flags
void	packetAssembler::assemble	(uint8_t *packet,
	                                 int16_t length,
	                                 int16_t mscIndex) {
uint8_t firstBit	= (packet [0] & 0x80) >> 7;
uint8_t lastBit		= (packet [0] & 0x40) >> 6;
uint8_t packetId	= (packet [0] & 0x30) >> 4;
uint8_t PPI		= (packet [0] & 0x08) >> 3;
uint8_t	CI		= packet [0] & 07;
int16_t	i;

	(void)packetId;
	this	-> mscIndex	= mscIndex;
	if (CI != ((old_CI + 1) & 07)) {
	   waitforFirst	= true;
	   series. resize (0);
	}
	old_CI = CI;
	if (waitforFirst && !firstBit)	// skip it
	   return;
	
	switch ((firstBit << 1) | lastBit) {
	   default:				// cannot happen
	   case 00:				// continue;
	      { int16_t currentLength = series. size ();
	        int16_t	addition = PPI ? packet [1] : length - 3;
	        int16_t base = PPI ? 2 : 1;
	        series. resize (currentLength + addition);
	        for (i = 0; i < addition; i ++)
	           series [currentLength + i] = packet [base + i];
	      }
	      break;

	   case 02:				// first Segment
	      { int16_t addition = PPI ? packet [1] : length - 3;
	        int16_t base = PPI ? 2 : 1;
	        series. resize (addition);
	        for (i = 0; i < addition; i ++)
	           series [i] = packet [base + i];
	        waitforFirst = false;
	      }
	      break;

	   case 01:				// last Segment
	      { int16_t currentLength = series. size ();
	        int16_t	addition = PPI ? packet [1] : length - 3;
	        int16_t base = PPI ? 2 : 1;
	        series. resize (currentLength + addition);
	        for (i = 0; i < addition; i ++)
	           series [currentLength + i] = packet [base + i];
	      }
	      add_mscDatagroup	(series);
	      waitforFirst = true;
	      break;
	   case 03:				// single segment
	      { int16_t addition = PPI ? packet [1] : length - 3;
	        int16_t base = PPI ? 2 : 1;
	        series. resize (addition);
	        for (i = 0; i < addition; i ++)
	           series [i] = packet [base + i];
	      }
	      add_mscDatagroup	(series);
	      waitforFirst = true;
	      break;
	}
}

void	packetAssembler::add_mscDatagroup	(QByteArray &mscGroup) {
uint8_t *data		= (uint8_t *)(mscGroup. data ());
bool	extensionFlag	= data [0] & 0x80;
bool	crcFlag		= data [0] & 0x40;
bool	segmentFlag	= data [0] & 0x20;
bool	userAccessFlag	= data [0] & 0x10;
uint8_t	groupType	= data [0] & 0x0F;
uint8_t	CI		= (data [1] & 0xF0) >> 4;
int16_t	next		= 2;		// bytes
bool	lastSegment	= false;
uint16_t segmentNumber	= 0;
bool transportIdFlag	= false;
uint16_t transportId	= 0;
uint8_t	lengthInd;

	(void)CI;
	(void)crcFlag;
	if (mscGroup. size () == 0)
	   return;

//	if (crcFlag && !check_CRC_bits (data, mscGroup.size ())) 
//	   return;

	if (extensionFlag)
	   next += 2;

	if (segmentFlag) {
	   lastSegment		= data [next] & 0x80;
	   segmentNumber	= ((data [next] & 0x7F) << 8) | data [next + 1];
	   next += 2;
	}

	if (userAccessFlag) {
	   transportIdFlag	= data [next] & 0x10;
	   lengthInd		= data [next] & 0x0F;
	   next += 1;
	   if (transportIdFlag) {
	      transportId = (data [next] << 8) | data [next + 1];
	   }
	   next	+= lengthInd;
	}


	if (transportIdFlag) 
	   my_dataHandler -> process_mscGroup (&data [next],
	                                       groupType,
	                                       lastSegment,
	                                       segmentNumber,
	                                       transportId);
}

