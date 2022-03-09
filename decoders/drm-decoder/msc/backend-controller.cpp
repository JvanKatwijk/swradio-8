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
 *	shield for mscProcessor 
 */
#include	"msc-processor.h"
#include	"drm-decoder.h"
#include	"state-descriptor.h"
#include	"deinterleaver.h"
#include	"msc-handler.h"
#include	"msc-handler-qam16.h"
#include	"msc-handler-qam64.h"
#include	"backend-controller.h"

	backendController::
	           backendController	(drmDecoder	*drmDecoder,
	                                 RingBuffer<std::complex<float>> *iqBuffer,
	                                 RingBuffer<std::complex<float>> *audioBuffer,
	                                 int8_t		qam64Roulette) {
	drmMaster	= drmDecoder;
	this	-> iqBuffer	= iqBuffer;
	this	-> audioBuffer	= audioBuffer;
	this	-> qam64Roulette	= qam64Roulette;

	connect (this, SIGNAL (showIQ (int)),
	         drmMaster, SLOT (showIQ (int)));
	theWorker	= NULL;
	mscMode		= 0;
	protLevelA	= 0;
	protLevelB	= 0;
	numofStreams	= 0;
	QAMMode		= 0;
}

	backendController::~backendController	() {
	if (theWorker != NULL)
	   delete theWorker;
}
//
//	Reset is called whenever we have the start of a new stream of
//	superframe data
void	backendController::reset	(stateDescriptor *theState) {
	if (theWorker != NULL)
	   delete theWorker;
	theWorker = new mscProcessor (theState, drmMaster, 6, audioBuffer);
	
}

void	backendController::newFrame	(stateDescriptor *theState) {
	if (theWorker == NULL)	// should not happen
	   theWorker = new mscProcessor (theState, drmMaster, 6, audioBuffer);
	theWorker -> newFrame (theState);
}

void	backendController::addtoMux	(int16_t blockno,
	                                 int cnt, theSignal v) {
	if (theWorker == NULL)	// should not happen
	   return;

	std::complex<float> temp = v. signalValue;
	iqBuffer	-> putDataIntoBuffer (&temp, 1);
	if (iqBuffer -> GetRingBufferReadAvailable () > 512)
	   showIQ (512);
	theWorker	-> addtoMux (blockno, cnt, v);
}

void	backendController::endofFrame		() {
	if (theWorker == NULL)	// should not happen
	   return;
	theWorker	-> endofFrame ();
}

