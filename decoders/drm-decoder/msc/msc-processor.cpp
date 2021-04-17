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
/*
 *	MSC processor 
 */
#include	"msc-processor.h"
#include	"drm-decoder.h"
#include	"state-descriptor.h"
#include	"deinterleaver.h"
#include	"msc-handler.h"
#include	"msc-handler-qam16.h"
#include	"msc-handler-qam64.h"
#include	"data-processor.h"
//
//	The mscState allows us to create the required
//	actual processor. The mscConfig is updated regularly
//	and checked once per superframe here
	mscProcessor::mscProcessor	(stateDescriptor *theState,
	                                 drmDecoder	*drm,
	                                 int8_t		qam64Roulette) {
	this	-> theState	= theState;
	drmMaster		= drm;
	this	-> qam64Roulette	= qam64Roulette;
	this	-> mscMode	= theState	-> mscMode;
	this	-> protLevelA	= theState	-> protLevelA;
	this	-> protLevelB	= theState	-> protLevelB;
	this	-> numofStreams	= theState	-> numofStreams;
	this	-> QAMMode	= theState	-> QAMMode;
	my_dataProcessor	= new dataProcessor (theState, drmMaster);

	this	-> muxsampleLength	= theState -> mscCells / 3;
	this	-> muxsampleBuf	= new theSignal [muxsampleLength];
	this	-> tempBuffer	= new theSignal [muxsampleLength];
	if (theState -> interleaverDepth > 1)
	   this -> my_deInterleaver =
	                     new deInterleaver_long (muxsampleLength, 
	                                             theState -> interleaverDepth);
	else
	   this -> my_deInterleaver =
	                     new deInterleaver (muxsampleLength);

	if (QAMMode == QAM64) {
	   switch (mscMode) {
	      case stateDescriptor::SM:
	         my_mscHandler = new QAM64_SM_Handler (drmMaster, theState,
	                                               qam64Roulette);
	         break;

	      default:  
	         fprintf (stderr, "not implemented yet\n");
	         my_mscHandler = new QAM64_SM_Handler  (drmMaster, theState,
	                                                qam64Roulette);
	         break;
	   }
	}
	else
	if (QAMMode == QAM16) 	// mscMode == SM
	   my_mscHandler	= new QAM16_SM_Handler (drmMaster, theState);
	else
	   my_mscHandler	= new mscHandler (drmMaster, theState);
	bufferP		= 0;
}

	mscProcessor::~mscProcessor	(void) {
	delete []       muxsampleBuf;
        delete []       tempBuffer;
        delete          my_deInterleaver;
        delete          my_mscHandler;
	delete		my_dataProcessor;
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

	(void)blockno; (void)cnt;
	tempBuffer [bufferP ++] = v;
	if (bufferP >= muxsampleLength) {
           uint8_t dataBuffer [theState -> dataLength * BITSPERBYTE + 100];
	   my_deInterleaver -> deInterleave (tempBuffer,
	                                     this -> muxsampleBuf);
	   my_mscHandler    -> process (this -> muxsampleBuf, dataBuffer);
	   my_dataProcessor -> process (dataBuffer, theState -> dataLength);
	   bufferP	= 0;
	}
}

//	at the start of the superframe: reset the input pointer
void	mscProcessor::newFrame		(stateDescriptor *theState) {
	bufferP	= 0;
}
//
//	At the end of a superframe, this function is called.
//	simple check:
void	mscProcessor::endofFrame	(void) {
	if (theState -> mscCells != 3 * muxsampleLength + bufferP)
	   fprintf (stderr, " E R R O R %d %d %d\n",
	                   theState -> mscCells,
	                   3 * muxsampleLength + bufferP,
	                   bufferP);
	bufferP	= 0;
	muxNo	= 0;
}

//

