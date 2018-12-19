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
#include	"fac-data.h"
#include	"drm-decoder.h"
#include	"sdc-include.h"
//
//	The "frame" with all the extracted  system data
//	in a drmFrame
//
//	There are three components:
//	1. the channel parameters, delivered by the FAC
//	2. the service parameters, deliverd by the FAC
//	3. the SDC parameters, deliverd by the SDC

	facData::facData (drmDecoder *mr, mscConfig *msc):
	                                        myChannelParameters (msc){
	this	-> master	= mr;
	this	-> FAC_crc	= false;
	this	-> SDC_crc	= false;
	this	-> msc		= msc;
}

	facData::~facData (void) {
}

void	facData::set_FAC_crc	(bool b) {
	FAC_crc	= b;
}

bool	facData::is_FAC_crc	(void) {
	return FAC_crc;
}

void	facData::set_SDC_crc	(bool b) {
	SDC_crc	= b;
}

bool	facData::is_SDC_crc	(void) {
	return SDC_crc;
}

//
//	The data in the SDC vector is organised as
//	4 bits are zero (just inserted for the CRC)
//	4 bits AFS
//	N bytes data
//	16 bits CRC
//	some padding bits

//	The data is organized as packages with a 12 bits header
//	that contains (in the first 7 bits) the length of
//	the data (excluding the header).
//	We pass on the length in bits, converting back to the
//	number of bytes in the datafield requires subtracting
//	the 4 bits (put before the afs field) and the 16 crc bits
//	and dividing by the number of bits per byte.
void	facData::interpretSDC (uint8_t *v, int16_t size) {
uint8_t	afs	= (v [4] << 3) | (v [5] << 2) | (v [6] << 1) | v [7];
int16_t	index	= 8;
int16_t	lengthofBody, versionFlag, dataType;
int16_t	lengthofDatafield	= (size - 16 - 4) / 8;

	while (index < lengthofDatafield * 8 + 8) {
	   lengthofBody = get_SDCBits (v, index, 7);
	   index	+= 7;
	   versionFlag	= get_SDCBits (v, index, 1);
	   index	+= 1;
	   dataType	= get_SDCBits (v, index, 4);
	   index	+= 4;

	   if (dataType == 0 && lengthofBody == 0)
	      break;
	   if (index + lengthofBody < lengthofDatafield * 8 + 8) // sanity check
	      set_SDCData (&v [index], afs,
	                               dataType, versionFlag, lengthofBody);
//
//	   step over contents
//	   Note that, according to std 6.4.3, the length of the body
//	   is indicated in full bytes, excluding the first 4 bits, i.e.
//	   the bitlength = lengthofBody * 8 + 4
	   index	+= lengthofBody * 8 + 4;
	}
}
//
//
//	SDC data is organized similar to the FIC data in DAB, i.e. a
//	datasegment with, embedded, short vectors with the actual data.
//	We assume the caller has done all CRC checking and passes
//	a vector to be interpreted as SDC data with a given type
void	facData::set_SDCData (uint8_t *v,
	                      uint8_t afs,
	                      uint8_t dataType,
	                      uint8_t versionFlag,
	                      int8_t lengthofBody) {
uint8_t	shortId, rfu;
int16_t	i;
QString s = QString();
uint8_t	language [3], country [2];

	(void)afs;
	if (versionFlag == 1)	// next configuration, skip for now
	   return;
//
//	The bits are sequentially stored in v, so let's go

//	fprintf (stderr, "SDC data with %d\n", dataType);
	switch (dataType) {
//	up to 4 streams can be carried. However, not all
//	of these streams have to be audio!!
	   case 0:	// multiplex description data
	      msc	-> numofStreams	= lengthofBody / 3;
	      msc	-> protLevelA	= get_SDCBits (v, 0, 2);
	      msc	-> protLevelB	= get_SDCBits (v, 2, 2);
	      msc	-> dataLength	= 0;
//
//	Note that, at least for now, datastreams are generic, i.e.
//	both Audio and data.
	      for (i = 0; i < msc -> numofStreams; i ++) {
	         msc	-> streams [i]. lengthHigh  =
	                           get_SDCBits (v, 4 + i * 24, 12);
	         msc	-> streams [i]. lengthLow  =
	                           get_SDCBits (v, 16 + i * 24, 12);
	         msc	-> dataLength += 
	                     msc -> streams [i]. lengthHigh +
	                     msc -> streams [i]. lengthLow;
	         msc	-> dataHigh	+=
	                     msc -> streams [i]. lengthHigh;
	      }
	      return;

	   case 1:	// label entity, first 4 bits are:
	      shortId	= get_SDCBits (v, 0, 2);
	      rfu	= get_SDCBits (v, 2, 2);
//	for now:
	      (void)shortId; (void)rfu;
//	the "full" bits are
	      for (i = 0; i < lengthofBody; i ++) 
	          s. append (get_SDCBits (v, 4 + 8 * i, 8));
	      s. append (char (0));
	      master -> show_stationLabel (s);
	      return;

	   case 2:	// conditional access parameters
	      return;

	   case 3:	// alternative frequency signalling
	      return;

	   case 4:	// alternative frequency signalling
	      return;

	   case	5:	// Application information data entity
	      {  int16_t index = get_SDCBits (v, 2, 2);
	         msc -> streams  [index]. soort		=
	                                        mscConfig::DATA_STREAM;
	         msc  -> streams [index]. shortId	=
	                                        get_SDCBits (v, 0, 2);
	         msc  -> streams [index]. streamId	=
	                                        get_SDCBits (v, 2, 2);
	         msc  -> streams [index]. packetModeInd	= get_SDCBits (v, 4, 1);
	         if (msc -> streams [index]. packetModeInd == 0) {
	            // synchronous stream Mode
	            msc -> streams [index]. rfa = get_SDCBits (v, 5, 3);
	            msc -> streams [index]. enhancementFlag =
	                                          get_SDCBits (v, 8, 1);
	            msc -> streams [index]. domain =
	                                          get_SDCBits (v, 9, 3);
	         } else {
	            // packet mode
	            msc -> streams [index]. dataUnitIndicator =
	                                          get_SDCBits (v, 5, 1);
	            msc -> streams [index]. packetId =
	                                          get_SDCBits (v, 6, 2);
	            msc -> streams [index]. enhancementFlag =
	                                          get_SDCBits (v, 8, 1);
	            msc -> streams [index]. domain =
	                                          get_SDCBits (v, 9, 3);
	            msc -> streams [index]. packetLength =
	                                          get_SDCBits (v, 12, 8);
	
	            if (msc -> streams [index]. domain == 0)
	               msc -> streams [index]. applicationId =
	                                          get_SDCBits (v, 20, 16);
	            else
	            if (msc -> streams [index]. domain == 1)
	               msc -> streams [index]. applicationId =
	                                          get_SDCBits (v, 25, 11);
	            msc -> streams [index]. startofData = 36;
//	         fprintf (stderr, " stream %d packetLength = %d, domain = %d, (app Id = %x) dataUnit = %d\n",
//	                    index,
//	                    msc -> streams [index]. packetLength,
//	                    msc -> streams [index]. domain,
//	                    msc -> streams [index]. applicationId,
//	                    msc -> streams [index]. dataUnitIndicator);
	         }
	      }
	      return;

	   case 6:
	   case 7:	
	      return;

	   case 8:	// time and date information
	      {  int16_t hours	= get_SDCBits (v, 17, 5);
	         int16_t minutes	= get_SDCBits (v, 22, 6);
	         if (lengthofBody > 30) {
	            int16_t offset = get_SDCBits (v, 31, 5);
	            uint8_t f     = get_SDCBits (v, 30, 1);
	            hours += f ? - offset : offset;
	         }
	         fprintf (stderr, "time = %d %d\n", hours, minutes);
	      }
	      return;

	   case 9:	// streams information data entity
	      {  int16_t index = get_SDCBits (v, 2, 2);
	         msc -> streams  [index]. soort		= mscConfig::AUDIO_STREAM;
	         msc  -> streams [index]. shortId	= get_SDCBits (v, 0, 2);
//	         fprintf (stderr, "shortId = %d\n", get_SDCBits (v, 0, 2));
	         msc  -> streams [index]. streamId	= get_SDCBits (v, 2, 2);
//	         fprintf (stderr, "streamId = %d\n", get_SDCBits (v, 2, 2));
	         msc  -> streams [index]. audioCoding	= get_SDCBits (v, 4, 2);
//	         fprintf (stderr, "audioCoding = %d\n", get_SDCBits (v, 4, 2));
	         msc  -> streams [index]. SBR_flag	= get_SDCBits (v, 6, 1);
//	         fprintf (stderr, "SBR_flag = %d\n", get_SDCBits (v, 6, 1));
	         msc  -> streams [index]. audioMode	= get_SDCBits (v, 7, 2);
//	         fprintf (stderr, "audioMode = %d\n", get_SDCBits (v, 7, 2));
	         msc  -> streams [index]. audioSamplingRate	= get_SDCBits (v, 9, 3);
//	         fprintf (stderr, "audioSamplingRate = %d\n", get_SDCBits (v, 9, 3));
	         msc  -> streams [index]. textFlag	= get_SDCBits (v, 12, 1);
	         msc  -> streams [index]. enhancementFlag	= get_SDCBits (v, 13, 1);
	         msc  -> streams [index]. coderField	= get_SDCBits (v, 14, 5);
	         msc  -> streams [index]. rfa		= get_SDCBits (v, 19, 1);
	      }
	      return;

	   case 10:	// FAC channel parameters data entity
	      return;

	   case 11:	// Alternative frequency signalling
	      return;

	   case 12:	// Language and country data entity
	      shortId		= get_SDCBits (v, 0, 2);
	      rfu		= get_SDCBits (v, 2, 2);
	      language [0]	= get_SDCBits (v, 4, 8);
	      language [1]	= get_SDCBits (v, 12, 8);
	      language [2]	= get_SDCBits (v, 20, 8);
	      country [0]	= get_SDCBits (v, 28, 8);
	      country [1]	= get_SDCBits (v, 36, 8);
	      (void)shortId;
	      (void)rfu;
	      (void)language;
	      (void)country;
	      return;

	   case 13:
	      return;

	   case 14:
	      {  int16_t streamId = get_SDCBits	(v, 0, 2);
	         rfu		  = get_SDCBits	(v, 2, 2);
	      msc -> streams [streamId]. R	= get_SDCBits (v, 4, 8);
	      msc -> streams [streamId]. C	= get_SDCBits (v, 12, 8);
	      msc -> streams [streamId]. packetLength =
	                             get_SDCBits (v, 20, 8) - 3;
	      msc -> streams [streamId]. FEC	= true;
	      }
	      return;

	   default:
	      return;
	}
}

void	facData::FAC_channelParameters (uint8_t *v) {
	myChannelParameters. setBase (&v [0]);
	myChannelParameters. setIdentity (&v [1]);
	myChannelParameters. setRMflag (&v [3]);
	myChannelParameters. setSpectrumOccupancy (&v [4]);
	myChannelParameters. setInterleaverdepthflag (&v [7]);
	myChannelParameters. setMSCmode (&v [8]);
	myChannelParameters. setSDCmode (&v [10]);
	myChannelParameters. Numberofservices (&v [11]);
	myChannelParameters. ReconfigurationIndex (&v [15]);
	myChannelParameters. Toggleflag (&v [18]);
//	rfu is reserved for future use
//	channelParameters. rfu (&v [19]);
}

void	facData::FAC_serviceParameters (uint8_t *v) {
	myServiceParameters. ServiceIdentifier (&v [0]);
	myServiceParameters. ShortId (&v [24]);
	myServiceParameters. AudioCA (&v [26]);
	myServiceParameters. Language (&v [27]);
	myServiceParameters. AudioDataflag (&v [31]);
	myServiceParameters. Servicedescriptor (&v [32]);
	myServiceParameters. DataCAindication (&v [37]);
//	last 6 bits, rfa is for future use
}

