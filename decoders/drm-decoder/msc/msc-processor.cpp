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
/*
 *	MSC processor 
 */
#include	"msc-processor.h"
#include	"drm-decoder.h"
#include	"msc-config.h"
#include	"deinterleaver.h"
#include	"msc-handler.h"
#include	"msc-handler-qam16.h"
#include	"msc-handler-qam64.h"
#include	"data-processor.h"
//
//	The mscState allows us to create the required
//	actual processor. The mscConfig is updated regularly
//	and checked once per superframe here
	mscProcessor::mscProcessor	(mscConfig	*msc,
	                                 drmDecoder	*drm,
	                                 int8_t		qam64Roulette,
	                                 viterbi_drm	*v) {
	drmMaster		= drm;
	create_Handlers	(msc);
	this	-> qam64Roulette	= qam64Roulette;
	viterbiDecoder		= v;
	my_dataProcessor	= new dataProcessor (msc, drmMaster);
	muxNo		= 0;
}

	mscProcessor::~mscProcessor	(void) {
	delete_Handlers ();
	delete	my_dataProcessor;
}
//
//	This function is the one maintaining an eye on the
//	potentially changing environment. This function
//	is called once per superframe (after decoding the SDC)
//
void	mscProcessor::check_mscConfig	(mscConfig *msc) {
	if ((mscMode	== msc -> mscMode) &&
	    (protLevelA == msc -> protLevelA) &&
	    (protLevelB == msc -> protLevelB) &&
	    (numofStreams == msc -> numofStreams) &&
	    (QAMMode	== msc -> QAMMode))
	return;		// no  (relevant) changes whatsoever

//	Otherwise: be cruel
	delete_Handlers	();	
	create_Handlers	(msc);
}
//
//	Processing is easy as long as we can delegate
//	We accept the datacells one at the time,
//	and when the Mux is filled, we pass the mux
//	to the selected msc-handler. That handler returns a
//	bitvector of the length specified in the SDC, however,
//	still packed one bit per uint8
//	Note that the first frame of a superframe contributes less
//	to the Mux, due to the cells needed for the SDC
//	(they all have FAC cells)
//
//	actions are split up in
//	- deinterleaving
//	- mscHandling, i.e from samples to (hard) bits
//	-- processing a bitstream
void	mscProcessor::addtoMux	(int16_t blockno, int32_t cnt, theSignal v) {
uint8_t *dataBuffer = new uint8_t[msc -> dataLength * BITSPERBYTE + 100];

	(void)blockno; (void)cnt;
	tempBuffer [bufferP ++] = v;
	if (bufferP >= muxsampleLength) {
	   my_deInterleaver -> deInterleave (tempBuffer,
	                                     this -> muxsampleBuf);
	   my_mscHandler    -> process (this -> muxsampleBuf, dataBuffer);
	   my_dataProcessor -> process (dataBuffer, msc -> dataLength);
	   bufferP	= 0;
	}
	delete [] dataBuffer;
}
//
//	At the end of a superframe, this function is called.
//	simple check:
void	mscProcessor::endofFrame	(void) {
	if (msc -> mscCells () != 3 * muxsampleLength + bufferP)
	   fprintf (stderr, " E R R O R %d %d\n", muxsampleLength, bufferP);
	bufferP	= 0;
	muxNo	= 0;
}

void	mscProcessor::newFrame		(void) {
	bufferP	= 0;
	muxNo	= 0;
}
//
void	mscProcessor::create_Handlers (mscConfig *msc) {
	this	-> msc		= msc;
	this	-> mscMode	= msc	-> mscMode;
	this	-> protLevelA	= msc	-> protLevelA;
	this	-> protLevelB	= msc	-> protLevelB;
	this	-> numofStreams	= msc	-> numofStreams;
	this	-> QAMMode	= msc	-> QAMMode;

	this	-> muxsampleLength	= msc -> mscCells  () / 3;
	this	-> muxsampleBuf	= new theSignal [muxsampleLength];
	this	-> tempBuffer	= new theSignal [muxsampleLength];
	if (msc -> muxDepth () > 1)
	   this -> my_deInterleaver =
	                     new deInterleaver_long (muxsampleLength,
	                                             msc -> muxDepth ());
	else
	   this -> my_deInterleaver =
	                     new deInterleaver (muxsampleLength);

	if (QAMMode	== mscConfig::QAM64) {
	   switch (mscMode) {
	      case mscConfig::SM:
	         my_mscHandler = new QAM64_SM_Handler (msc,
	                                               qam64Roulette,
	                                               viterbiDecoder);
	         break;

	      default:  
	         fprintf (stderr, "not implemented yet\n");
	         my_mscHandler = new QAM64_SM_Handler  (msc,
	                                               qam64Roulette,
	                                               viterbiDecoder);
	         break;
	   }
	}
	else
	if (QAMMode	== mscConfig::QAM16) 	// mscMode == SM
	   my_mscHandler	= new QAM16_SM_Handler (msc, viterbiDecoder);
	else
	   my_mscHandler	= new mscHandler (msc);
	bufferP		= 0;
}

void	mscProcessor::delete_Handlers (void) {
	delete []	muxsampleBuf;
	delete []	tempBuffer;
	delete		my_deInterleaver;
	delete		my_mscHandler;
}

