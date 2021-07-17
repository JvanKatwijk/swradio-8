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
#include	"sdc-processor.h"
#include	"drm-decoder.h"
#include	"state-descriptor.h"
#include	"referenceframe.h"
#include	"prbs.h"
#include	"sdc-include.h"
#include	"sdc-streamer.h"
#include	"qam16-metrics.h"
#include	"qam4-metrics.h"
#include	"viterbi-drm.h"

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

	sdcProcessor::sdcProcessor (drmDecoder	*master,
	                            smodeInfo	*modeInf,
	                            std::vector<sdcCell> * sdcTable,
	                            stateDescriptor	*theState):
	                                  theCRC (16, crcPolynome),
	                                  Y13Mapper (2 * sdcTable -> size (), 13),
	                                  Y21Mapper (2 * sdcTable -> size () ,21) {
	this	-> sdcTable	= sdcTable;
	this	-> nrCells	= sdcTable	-> size ();
	this	-> Mode		= theState	-> Mode;
	this	-> Spectrum	= theState	-> Spectrum;
	this	-> theState	= theState;
//
	connect (this, SIGNAL (show_stationLabel (const QString &, int)),
	         master, SLOT (show_stationLabel (const QString &, int)));
	connect (this, SIGNAL (show_timeLabel    (const QString &)),
	         master, SLOT (show_timeLabel    (const QString &)));
	connect (this, SIGNAL (show_mer (float)),
	         master, SLOT (show_mer_sdc (float)));

	rmFlag	= theState	-> RMflag;
	SDCmode	= theState	-> sdcMode;
	qammode	= (rmFlag == 0 && SDCmode == 0) ? QAM16 : QAM4;
	if (qammode == QAM4) {
//	   fprintf (stderr, "qammode = QAM4\n");
	   my_qam4_metrics	= new qam4_metrics ();
	   stream_0		= new SDC_streamer (1, 2, &Y21Mapper, nrCells);
	   stream_1		= nullptr;
	   lengthofSDC          = getLengthofSDC (Mode, Spectrum, SDCmode);
	   thePRBS		= new prbs (stream_0 -> lengthOut ());
	}
	else {
//	   fprintf (stderr, "qammode = QAM16\n");
	   my_qam16_metrics	= new qam16_metrics ();
	   stream_0		= new SDC_streamer (1, 3, &Y13Mapper, nrCells);
	   stream_1		= new SDC_streamer (2, 3, &Y21Mapper, nrCells);
	   lengthofSDC          = getLengthofSDC (Mode, Spectrum, SDCmode);
	   thePRBS		= new prbs (stream_0 -> lengthOut () +
	                                    stream_1 -> lengthOut ());
	}
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
int16_t valueIndex      = 0;
theSignal sdcVector [sdcTable -> size ()];

        for (int i = 0; i < sdcTable -> size (); i ++) {
           int symbol   = (*sdcTable) [i]. symbol;
           int carrier  = (*sdcTable) [i]. carrier;
           sdcVector [valueIndex ++] =
                  outbank -> element (symbol)[carrier - Kmin (Mode, Spectrum)];
        }

	if (qammode == QAM4) 
	   return processSDC_QAM4 (sdcVector);
	else
	   return processSDC_QAM16 (sdcVector);
}

bool	sdcProcessor::processSDC_QAM4 (theSignal *v) {
uint8_t sdcBits [4 + stream_0 -> lengthOut ()];
metrics	rawBits [2 * nrCells];
uint8_t	reconstructed [2 * nrCells];
mer4_compute   computeMER;
float   mer     = 10 * log10 (computeMER. computemer (v, nrCells));
        show_mer (mer);

	my_qam4_metrics -> computemetrics (v, nrCells, rawBits);
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

bool sdcProcessor::processSDC_QAM16 (theSignal *v) {
uint8_t sdcBits [4 + stream_0 -> lengthOut () + stream_1 -> lengthOut ()];
metrics *Y0_stream = new metrics [2 * nrCells];
metrics *Y1_stream = new metrics [2 * nrCells];
uint8_t *level_0   = new uint8_t [2 * nrCells];
uint8_t *level_1   = new uint8_t [2 * nrCells];
int16_t i;
mer16_compute   computeMER;
float mer = 10 * log10 (computeMER. computemer (v, nrCells));
        show_mer (mer);

	for (i = 0; i < 4; i ++) {
	   my_qam16_metrics	-> computemetrics (v, nrCells, 0, 
	                                           Y0_stream, 
	                                           i != 0,
	                                           level_0, level_1);
	   stream_0		-> handle_stream  (Y0_stream,
	                                           level_0,
	                                           &sdcBits [4],
	                                           true);
	   my_qam16_metrics	-> computemetrics (v, nrCells, 1, 
	                                           Y1_stream,
	                                           true,
	                                           level_0, level_1);
	   stream_1		-> handle_stream  (Y1_stream,
	                                           level_1,
	                                           &sdcBits [4 + stream_0 -> lengthOut ()],
	                                           true);
	}
//
//	apply PRBS
	thePRBS -> doPRBS (&sdcBits [4]);
	delete [] Y0_stream; 
	delete [] Y1_stream;
	delete [] level_0;
	delete [] level_1;

	sdcBits [0] = sdcBits [1] = sdcBits [2] = sdcBits [3] = 0;
	if (!theCRC. doCRC (sdcBits, 4 + lengthofSDC)) {
	   return false;
	}

	interpretSDC (sdcBits, lengthofSDC, theState);
	return theState -> set;
}
//

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
	      if ((lengthofBody < 0) || (lengthofBody / 3 > 4)) {
	         sdcCorrect = false;
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
	      s = "";
	      for (i = 0; i < lengthofBody; i ++) {
	          s. append (get_SDCBits (v, 4 + 8 * i, 8));
	      }
	      s. append (char (0));
	      if (lengthofBody > 1) {
//	         char *s2 = s. toLatin1 (). data ();
	         strcpy (theState -> streams [shortId]. serviceName,
	                         s. toLatin1 (). data ());
	      }
	      show_stationLabel (QString (s), shortId);
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
	         QString t = QString::number (hours);
	         t. append (":");
	         t. append (QString::number (minutes));
	         show_timeLabel (t);
	      }
	      return;

	   case 9:	// streams information data entity
	      {  int16_t index = get_SDCBits (v, 2, 2);
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

