#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRunoPlugin_drm
 *
 *    drm plugin is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm plugin is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with drm plugin; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
//#include	<windows.h>
#include	"sdc-include.h"
#include	"sdc-streamer.h"
#include	"sdc-processor.h"
#include	"state-descriptor.h"
#include	"referenceframe.h"
#include	"prbs.h"
#include	"viterbi-drm.h"
#include	"qam16-metrics.h"
#include	"qam4-metrics.h"
#include	"mapper.h"
#include	"drm-decoder.h"

#include	"mer4-values.h"
#include	"mer16-values.h"
//
//	the "processor" for extracting the SDC values from the
//	(first) frame of a superframe encoded in QAM4/QAM16
//
//	sdcCRC checking is exactly as with the DAB FIC
//	we need the polynome
static
const uint16_t crcPolynome [] = {
	0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 // MSB .. LSB x16 + x12 + x5 + 1
};
//
//	Implement table 56 of the standard 
int16_t table_56 [] = {
	37,	43,	85,	97,	184,
	17,	20,	41,	47,	91,
	28,	33,	66,	76,	143,
	13,	15,	32,	37,	70,
	-1,	-1,	-1,	68,	-1,
	-1,	-1,	-1,	32,	-1,
	-1,	-1,	-1,	33,	-1,
	-1,	-1,	-1,	15,	-1
};

extern	int16_t	table_56 [];

static	inline
int16_t	getLengthofSDC	(uint8_t Mode, uint8_t Spectrum, uint8_t SDCMode) {
int16_t	index;
	index = (Mode - 1) * 10 +
	         (SDCMode == 1 ? 5 : 0) + Spectrum;
	return 4 + 8 * table_56 [index] + 16;
}

//
//	Mode and Spectrum are given now. nrCells is
//	determined by Mode and spectrum and stateDescriptor
//	refers to the database structure

	sdcProcessor::sdcProcessor (drmDecoder	*m_form,
	                            smodeInfo	*modeInf,
	                            std::vector<sdcCell> &sdcTable,
	                            stateDescriptor	*theState):
	                                  theCRC (16, crcPolynome),
	                                  Y13Mapper (2 * sdcTable. size (), 13),
	                                  Y21Mapper (2 * sdcTable. size () ,21) {
	this	-> m_form	= m_form;
	this	-> sdcTable	= &sdcTable;
	this	-> nrCells	= sdcTable. size ();
	this	-> Mode		= theState	-> Mode;
	this	-> Spectrum	= theState	-> Spectrum;
	this	-> theState	= theState;
//
	rmFlag	= theState	-> RMflag;
	SDCmode	= theState	-> sdcMode;
	qammode	= (rmFlag == 0 && SDCmode == 0) ? QAM16 : QAM4;
	if (qammode == QAM4) {
	   my_qam4_metrics	= new qam4_metrics ();
	   stream_0		= new SDC_streamer (1, 2, &Y21Mapper, nrCells);
	   stream_1		= nullptr;
	   lengthofSDC          = getLengthofSDC (Mode, Spectrum, SDCmode);
	   thePRBS		= new prbs (stream_0 -> lengthOut ());
	}
	else {
	   my_qam16_metrics	= new qam16_metrics ();
	   stream_0		= new SDC_streamer (1, 3, &Y13Mapper, nrCells);
	   stream_1		= new SDC_streamer (2, 3, &Y21Mapper, nrCells);
	   lengthofSDC          = getLengthofSDC (Mode, Spectrum, SDCmode);
	   thePRBS		= new prbs (stream_0 -> lengthOut () +
	                                    stream_1 -> lengthOut ());
	}
	connect (this, SIGNAL (show_sdc_mer (float)),
	         m_form, SLOT (show_sdc_mer (float)));
	connect (this, SIGNAL (set_timeLabel (const QString &)),
	         m_form, SLOT (set_timeLabel (const QString &)));
	connect (this, SIGNAL (set_channel_1 (const QString &)),
	         m_form, SLOT (set_channel_1 (const QString &)));
	connect (this, SIGNAL (set_channel_2 (const QString &)),
	         m_form, SLOT (set_channel_2 (const QString &)));
	connect (this, SIGNAL (set_channel_3 (const QString &)),
	         m_form, SLOT (set_channel_3 (const QString &)));
	connect (this, SIGNAL (set_channel_4 (const QString &)),
	         m_form, SLOT (set_channel_4 (const QString &)));
}

	sdcProcessor::~sdcProcessor (void) {
	delete	thePRBS;
	delete	stream_0;
	if (qammode == QAM16) {
	   delete	my_qam16_metrics;
	   delete	stream_1;
	}
	else
	   delete	my_qam4_metrics;
}

//	actual processing of a SDC block. 
bool	sdcProcessor::processSDC (myArray<theSignal> *outbank) {
std::vector<theSignal> sdcVector;
int nrSymbols	=  symbolsperFrame (Mode);
int i, symbol, carrier;

	sdcVector. resize (nrCells);
	try {
	   for (i = 0; i < nrCells; i++) {
	      symbol   = (*sdcTable) [i]. symbol;
	      carrier  = (*sdcTable) [i]. carrier;
              sdcVector [i] =
//	         outbank -> element (symbol)[carrier - Kmin (Mode, Spectrum)];
	         outbank -> elementValue (symbol, carrier - Kmin (Mode, Spectrum));
	   }
        } catch (int e) {
	   std::string err = "error for " + std::to_string (i) +
	                               " " + std::to_string (symbol) + " " +
	                               std::to_string (carrier);
	   fprintf (stderr, "%s \n", err. c_str ());
	}

	if (qammode == QAM4) 
	   return processSDC_QAM4 (sdcVector);
	else
	   return processSDC_QAM16 (sdcVector);
}

bool	sdcProcessor::processSDC_QAM4 (std::vector<theSignal> &v) {
uint8_t	*sdcBits	=
	(uint8_t *)alloca ((4 + stream_0 -> lengthOut ()) * sizeof (uint8_t));
metrics *rawBits	=
	(metrics*)alloca  ((2 * nrCells) * sizeof(metrics));
uint8_t	*reconstructed	=
	(uint8_t *)alloca ((2 * nrCells) * sizeof (uint8_t));
mer4_compute   computeMER;
float   mer     = 10 * log10 (computeMER. computemer (v. data (), nrCells));
        show_sdc_mer (mer);

	my_qam4_metrics -> computemetrics (v. data (), nrCells, rawBits);
	stream_0	-> handle_stream (rawBits, reconstructed,
	                                     &sdcBits [4], false);
//
//	apply PRBS
	thePRBS		-> doPRBS (&sdcBits [4]);

	sdcBits [0] = sdcBits [1] = sdcBits [2] = sdcBits [3] = 0;
	if (!theCRC. doCRC (sdcBits, 4 + lengthofSDC)) {
	   return false;
	}

	interpretSDC (sdcBits, lengthofSDC, theState);
	return true;
}

bool sdcProcessor::processSDC_QAM16 (std::vector<theSignal> &v) {
std::vector<uint8_t> sdcBits;
std::vector<metrics> Y0_stream;
std::vector<metrics> Y1_stream;
std::vector<uint8_t> level_0;
std::vector<uint8_t> level_1;
mer16_compute	computeMER;
DRM_FLOAT	mer	= 10 * log10 (computeMER. computemer (v. data (), nrCells));
	show_sdc_mer (mer);
	
	sdcBits. resize (4 + stream_0 -> lengthOut() + stream_1->lengthOut());
	Y0_stream.resize (2 * nrCells);
	Y1_stream.resize (2 * nrCells);
	level_0.resize (2 * nrCells);
	level_1.resize (2 * nrCells);

	for (int i = 0; i < 4; i ++) {
	   my_qam16_metrics	-> computemetrics (v. data (),
	                                           nrCells, 0, 
	                                           Y0_stream. data (), 
	                                           i != 0,
	                                           level_0. data (), level_1. data ());
	   stream_0		-> handle_stream  (Y0_stream. data (),
	                                           level_0. data (),
	                                           &sdcBits [4],
	                                           true);
	   my_qam16_metrics	-> computemetrics (v. data (), nrCells, 1, 
	                                           Y1_stream. data (),
		                                   true,
	                                           level_0. data (), level_1. data ());
	   stream_1	->	handle_stream (Y1_stream. data (),
		                               level_1. data (),
		                               &sdcBits [4 + stream_0 -> lengthOut()],
	                                       true);
	}
//
//	apply PRBS
	thePRBS -> doPRBS (&sdcBits [4]);

	sdcBits [0] = sdcBits [1] = sdcBits [2] = sdcBits [3] = 0;
	if (!theCRC. doCRC (sdcBits. data (), 4 + lengthofSDC)) {
	   return false;
	}
	interpretSDC (sdcBits. data (), lengthofSDC, theState);
	return theState -> set;
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

void	sdcProcessor::interpretSDC (uint8_t *v, int16_t size,
	                                  stateDescriptor *theState) {
uint8_t	afs	= (v [4] << 3) | (v [5] << 2) | (v [6] << 1) | v [7];
int16_t	index	= 8;
int16_t	lengthofBody, versionFlag, dataType;
int16_t	lengthofDatafield	= (size - 16 - 4) / 8;

	sdcCorrect = true;

	while (index < lengthofDatafield * 8 + 8) {
	   lengthofBody = get_SDCBits (v, index, 7);
	   index	+= 7;
	   versionFlag	= get_SDCBits (v, index, 1);
	   index	+= 1;
	   dataType	= get_SDCBits (v, index, 4);
	   index	+= 4;

	   if ((dataType == 0) && (lengthofBody == 0))
	      break;
	   if (index + lengthofBody < lengthofDatafield * 8 + 8) // sanity check
	      set_SDCData (theState,
	                   &v [index],
	                   afs,
	                   dataType, versionFlag, lengthofBody);
//
//	   step over contents
//	   Note that, according to std 6.4.3, the length of the body
//	   is indicated in full bytes, excluding the first 4 bits, i.e.
//	   the bitlength = lengthofBody * 8 + 4
	   index	+= lengthofBody * 8 + 4;
	}
	theState -> set =  sdcCorrect;

}
//
//	SDC data is organized similar to the FIC data in DAB, i.e. a
//	datasegment with, embedded, short vectors with the actual data.
//	We assume the caller has done all CRC checking and passes
//	a vector to be interpreted as SDC data with a given type
void	sdcProcessor::set_SDCData (stateDescriptor *theState,
	                           uint8_t	*v,
	                           uint8_t	afs,
	                           uint8_t	dataType,
	                           uint8_t	versionFlag,
	                           int8_t	lengthofBody) {
uint8_t	shortId, rfu;
int16_t	i;
std::string	s;
uint8_t	language [3], country [2];

	(void)afs;
	if (versionFlag == 1)	// next configuration, skip for now
	   return;
//
//	The bits are sequentially stored in v, so let's go
	switch (dataType) {
//	up to 4 streams can be carried. However, not all
//	of these streams have to be audio!!
	   case 0:	// multiplex description data
	      if ((lengthofBody < 0) || (lengthofBody / 3 > 4)) {
	         sdcCorrect = false;
	         return;
	      }

	      theState	-> numofStreams	= lengthofBody / 3;
	      theState	-> protLevelA	= get_SDCBits (v, 0, 2);
	      theState	-> protLevelB	= get_SDCBits (v, 2, 2);
	      theState	-> dataLength	= 0;
//
//	Note that, at least for now, datastreams are generic, i.e.
//	both Audio and data.
	      for (i = 0; i < theState -> numofStreams; i ++) {
	         theState	-> streams [i]. lengthHigh  =
	                                get_SDCBits (v, 4 + i * 24, 12);
	         theState	-> streams [i]. lengthLow  =
	                                get_SDCBits (v, 16 + i * 24, 12);
	         theState	-> dataLength += 
	                                theState -> streams [i]. lengthHigh +
	                                theState -> streams [i]. lengthLow;
	         theState	-> dataHigh	+=
	                                theState -> streams [i]. lengthHigh;
	         theState	-> streams [i]. inUse = true;
	      }
	
	      return;

	   case 1:	// label entity, first 4 bits are:
	      shortId	= get_SDCBits (v, 0, 2);
	      rfu	= get_SDCBits (v, 2, 2);
//	for now:
	      (void)shortId; (void)rfu;
//	the "full" bits are
	      for (i = 0; i < lengthofBody; i ++) 
	          s. push_back (get_SDCBits (v, 4 + 8 * i, 8));
	      s. push_back (char (0));
	      if (lengthofBody > 1) {
	         for (i = 0; i < theState -> numofStreams; i ++) 
	            if ((theState -> streams [i]. inUse) &&
	                (theState -> streams [i]. shortId == shortId)) {
	               if (theState -> streams [i]. serviceName [0] != 0)
	                  break;
	               strcpy (theState -> streams [i]. serviceName,
	                                          s. c_str ());
	               switch (shortId) {
	                  case 0:
	                     set_channel_1 (QString::fromStdString (s));
	                     break;
	                  case 1:
	                     set_channel_2 (QString::fromStdString (s));
	                     break;
	                  case 2:
	                     set_channel_3 (QString::fromStdString (s));
	                     break;
	                  case 3:
	                     set_channel_4 (QString::fromStdString (s));
	                     break;
	               }
	               return;
	            }
	         }
	      return;

	   case 2:	// conditional access parameters
	      return;

	   case 3:	// alternative frequency signalling
	      return;

	   case 4:	// alternative frequency signalling
	      return;

	   case	5:	// Application information data entity
	      {  int16_t index = get_SDCBits (v, 2, 2);
	         theState -> streams  [index]. soort		=
	                                        stateDescriptor::DATA_STREAM;
	         theState  -> streams [index]. shortId	=
	                                        get_SDCBits (v, 0, 2);
	         theState  -> streams [index]. streamId	=
	                                        get_SDCBits (v, 2, 2);
	         theState  -> streams [index]. packetModeInd	= get_SDCBits (v, 4, 1);
	         if (theState -> streams [index]. packetModeInd == 0) {
	            // synchronous stream Mode
	            theState -> streams [index]. rfa = get_SDCBits (v, 5, 3);
	            theState -> streams [index]. enhancementFlag =
	                                          get_SDCBits (v, 8, 1);
	            theState -> streams [index]. domain =
	                                          get_SDCBits (v, 9, 3);
	         } else {
	            // packet mode
	            theState -> streams [index]. dataUnitIndicator =
	                                          get_SDCBits (v, 5, 1);
	            theState -> streams [index]. packetId =
	                                          get_SDCBits (v, 6, 2);
	            theState -> streams [index]. enhancementFlag =
	                                          get_SDCBits (v, 8, 1);
	            theState -> streams [index]. domain =
	                                          get_SDCBits (v, 9, 3);
	            theState -> streams [index]. packetLength =
	                                          get_SDCBits (v, 12, 8);
	
	            if (theState -> streams [index]. domain == 0)
	               theState -> streams [index]. applicationId =
	                                          get_SDCBits (v, 20, 16);
	            else
	            if (theState -> streams [index]. domain == 1)
	               theState -> streams [index]. applicationId =
	                                          get_SDCBits (v, 25, 11);
	            theState -> streams [index]. startofData = 36;
//	         fprintf (stderr, " stream %d packetLength = %d, domain = %d, (app Id = %x) dataUnit = %d\n",
//	                    index,
//	                    theState -> streams [index]. packetLength,
//	                    theState -> streams [index]. domain,
//	                    theState -> streams [index]. applicationId,
//	                    theState -> streams [index]. dataUnitIndicator);
	         }
	      }
	      return;

	   case 6:
	   case 7:	
	      return;

	   case 8:	// time and date information
	      {  int16_t hours		= get_SDCBits (v, 17, 5);
	         int16_t minutes	= get_SDCBits (v, 22, 6);
	         if (lengthofBody > 30) {
	            int16_t offset = get_SDCBits (v, 31, 5);
	            uint8_t f     = get_SDCBits (v, 30, 1);
	            hours += f ? - offset : offset;
	         }
	         std::string t = std::to_string (hours);
	         t. append (":");
	         t. append (std::to_string (minutes));
	         set_timeLabel (QString::fromStdString (t));
	      }
	      return;

	   case 9:	// streams information data entity
	      {  int16_t index = get_SDCBits (v, 2, 2);
//	         m_form->set_countryLabel(std::to_string(index) + " audiostream");
	         theState -> streams [index]. soort  
	                                      = stateDescriptor::AUDIO_STREAM;
	         theState -> streams [index]. shortId
	                                      = get_SDCBits (v, 0, 2);
	         theState -> streams [index]. streamId
	                                      = get_SDCBits (v, 2, 2);
	         theState -> streams [index]. audioCoding
	                                      = get_SDCBits (v, 4, 2);
	         theState -> streams [index]. SBR_flag
	                                      = get_SDCBits (v, 6, 1);
	         theState -> streams [index]. audioMode	
	                                      = get_SDCBits (v, 7, 2);
	         theState -> streams [index]. audioSamplingRate
	                                      = get_SDCBits (v, 9, 3);
	         theState -> streams [index]. textFlag
	                                      = get_SDCBits (v, 12, 1);
	         theState -> streams [index]. enhancementFlag
	                                      = get_SDCBits (v, 13, 1);
	         theState -> streams [index]. coderField
	                                      = get_SDCBits (v, 14, 5);
	         theState -> streams [index]. rfa
	                                      = get_SDCBits (v, 19, 1);
//
//      if xHE-AAC we need to collect the decoderspecific data
	         if (theState -> streams [index]. audioCoding == 03) {
	            int bytes = (index + 16 + 8 * lengthofBody - 20) / 8;
	            theState -> streams [index]. xHE_AAC. resize (0);
	            for (int i = 0; i < bytes; i ++) {
	               uint8_t t = get_SDCBits (v, 20 + 8 * i, 8);
	               theState -> streams [index]. xHE_AAC. push_back (t);
	            }
	         }
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
	         theState -> streams [streamId]. R	
	                                        = get_SDCBits (v, 4, 8);
	         theState -> streams [streamId]. C
	                                        = get_SDCBits (v, 12, 8);
	         theState -> streams [streamId]. packetLength
	                                        = get_SDCBits (v, 20, 8) - 3;
	         theState  -> streams [streamId]. FEC	= true;
	      }
	      return;

	   default:
	      return;
	}
}

