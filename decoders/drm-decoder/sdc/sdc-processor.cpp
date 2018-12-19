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
#include	"sdc-processor.h"
#include	"fac-data.h"
#include	"prbs.h"
#include	"sdc-include.h"
#include	"sdc-streamer.h"
#include	"qam16-metrics.h"
#include	"qam4-metrics.h"
#include	"msc-config.h"
#include	"viterbi-drm.h"
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

static	inline
int16_t	getLengthofSDCData (uint8_t Mode, uint8_t Spectrum, uint8_t SDCMode) {
int16_t	index;
	index = (Mode - 1) * 10 + (SDCMode == 1 ? 5 : 0) + Spectrum;
	return table_56 [index];
}
//
//	Mode and Spectrum are given now. nrCells could be
//	determined by Mode and spectrum and facData
//	refers to the database structure
	sdcProcessor::sdcProcessor (uint8_t	Mode,
	                            uint8_t	Spectrum,
	                            viterbi_drm	*v,
	                            facData	*my_facData,
	                            int16_t 	nrCells):
	                                  theCRC (16, crcPolynome),
	                                  Y13Mapper (2 * nrCells, 13),
	                                  Y21Mapper (2 * nrCells, 21) {
	this	-> Mode		= Mode;
	this	-> Spectrum	= Spectrum;
	this	-> my_facDB	= my_facData;
	this	-> nrCells	= nrCells;
//
//	default settings will be overruled later on
	this	-> qammode	= mscConfig::QAM16;
	this	-> rmFlag	= 0;
	this	-> SDCmode	= 0;
	this	-> deconvolver	= v;
	lengthofSDC		= getLengthofSDC (Mode, Spectrum, getSDCmode ());
	my_qam16_metrics	= new qam16_metrics ();
	stream_0		= new SDC_streamer (deconvolver,
	                                            1, 3, &Y13Mapper, nrCells);
	stream_1		= new SDC_streamer (deconvolver,
	                                            2, 3, &Y21Mapper, nrCells);
	thePRBS			= new prbs (stream_0 -> lengthOut () +
	                                    stream_1 -> lengthOut ());
}

	sdcProcessor::~sdcProcessor (void) {
	delete	thePRBS;
	delete	stream_0;
	if (qammode == mscConfig::QAM16) {
	   delete	my_qam16_metrics;
	   delete	stream_1;
	}
	else
	   delete	my_qam4_metrics;
}

void	sdcProcessor::checkConfig (void) {
uint8_t	newrmFlag	= getRMflag ();
uint8_t	newSDCmode	= getSDCmode ();

	if (newrmFlag == rmFlag && newSDCmode == SDCmode)
	   return;

//	some serious changes in the configuration,
//	delete the old stuff and recompute the handlers
	delete	thePRBS;
	delete	stream_0;
	if (qammode == mscConfig::QAM16) {
	   delete	my_qam16_metrics;
	   delete	stream_1;
	}
	else
	   delete	my_qam4_metrics;
//
//	and rebuild
	rmFlag	= newrmFlag;
	SDCmode	= newSDCmode;
	qammode	= (rmFlag == 0 && SDCmode == 0) ? mscConfig::QAM16 :
	                                          mscConfig::QAM4;
	if (qammode == mscConfig::QAM4) {
	   fprintf (stderr, "qammode = QAM4\n");
	   my_qam4_metrics	= new qam4_metrics ();
	   stream_0		= new SDC_streamer (deconvolver,
	                                            1, 2, &Y21Mapper, nrCells);
	   lengthofSDC          = getLengthofSDC (Mode, Spectrum, SDCmode);

	   stream_1		= NULL;
	   thePRBS		= new prbs (stream_0 -> lengthOut ());
	}
	else {
	   fprintf (stderr, "qammode = QAM16\n");
	   my_qam16_metrics	= new qam16_metrics ();
	   stream_0		= new SDC_streamer (deconvolver,
	                                            1, 3, &Y13Mapper, nrCells);
	   stream_1		= new SDC_streamer (deconvolver,
	                                            2, 3, &Y21Mapper, nrCells);
	   lengthofSDC          = getLengthofSDC (Mode, Spectrum, SDCmode);
	   thePRBS		= new prbs (stream_0 -> lengthOut () +
	                                    stream_1 -> lengthOut ());
	}
}

//	actual processing of a SDC block. 
bool	sdcProcessor::processSDC (theSignal *v) {

	checkConfig ();
	if (qammode == mscConfig::QAM4) 
	   return processSDC_QAM4 (v);
	else
	   return processSDC_QAM16 (v);
}

bool	sdcProcessor::processSDC_QAM4 (theSignal *v) {
uint8_t sdcBits [4 + stream_0 -> lengthOut ()];
metrics	rawBits [2 * nrCells];
uint8_t	reconstructed [2 * nrCells];

	my_qam4_metrics -> computemetrics (v, nrCells, rawBits);
	stream_0	-> handle_stream (rawBits, reconstructed,
	                                     &sdcBits [4], false);
//
//	apply PRBS
	thePRBS		-> doPRBS (&sdcBits [4]);

	sdcBits [0] = sdcBits [1] = sdcBits [2] = sdcBits [3] = 0;
	if (!theCRC. doCRC (sdcBits, 4 + lengthofSDC)) {
	   my_facDB	-> set_SDC_crc (false);
	   return false;
	}

	my_facDB	-> interpretSDC (sdcBits, lengthofSDC);
	my_facDB	-> set_SDC_crc (true);
	return true;
}

bool sdcProcessor::processSDC_QAM16 (theSignal *v) {
uint8_t sdcBits [4 + stream_0 -> lengthOut () + stream_1 -> lengthOut ()];
metrics Y0_stream	[2 * nrCells];
metrics Y1_stream	[2 * nrCells];
uint8_t level_0		[2 * nrCells];
uint8_t level_1		[2 * nrCells];
int16_t i;

	for (i = 0; i < 5; i ++) {
	   my_qam16_metrics -> computemetrics (v, nrCells, 0, 
	                                       Y0_stream, 
	                                       i != 0,
	                                       level_0, level_1);

	   stream_0	-> handle_stream (Y0_stream,
	                                  level_0,
	                                  &sdcBits [4],
	                                  true);
	   my_qam16_metrics -> computemetrics (v, nrCells, 1, 
	                                        Y1_stream,
	                                        true,
	                                        level_0, level_1);
	   stream_1	-> handle_stream (Y1_stream,
	                                  level_1,
	                                  &sdcBits [4 + stream_0 -> lengthOut ()],
	                                  true);
	}
//
//	apply PRBS
	thePRBS -> doPRBS (&sdcBits [4]);

	sdcBits [0] = sdcBits [1] = sdcBits [2] = sdcBits [3] = 0;
	if (!theCRC. doCRC (sdcBits, 4 + lengthofSDC)) {
	   my_facDB	-> set_SDC_crc (false);
	   return false;
	}

	my_facDB	-> interpretSDC (sdcBits, lengthofSDC);
	my_facDB	-> set_SDC_crc (true);
	return true;
}
//

uint8_t	sdcProcessor::getSDCmode	(void) {
	return my_facDB	-> myChannelParameters.getSDCmode ();
}

uint8_t	sdcProcessor::getRMflag		(void) {
	return my_facDB	-> myChannelParameters. getRMflag ();
}

