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
/*
 *	shield for mscProcessor 
 */
#include	"msc-processor.h"
#include	"drm-decoder.h"
#include	"state-descriptor.h"
#include	"backend-controller.h"

	backendController::
	           backendController	(drmDecoder	*m_form,
	                                 int8_t		qam64Roulette,
#ifdef	__WITH_FDK_AAC__
#ifdef	__MINGW32__
	                                 aacHandler	*aacFunctions,
#endif
#endif
	                                 RingBuffer<std::complex<float>> *b,
	                                 RingBuffer<std::complex<float>> *iq) {
	this	-> m_form		= m_form;

	this	-> qam64Roulette	= qam64Roulette;
#ifdef	__WITH_FDK_AAC__
#ifdef	__MINGW32__
	this	-> aacFunctions		= aacFunctions;
#endif
#endif
	this	-> audioBuffer		= b;
	this	-> iqBuffer		= iq;

	connect (this, SIGNAL (showIQ (int)),
	         m_form, SLOT (showIQ (int)));


	theWorker	= nullptr;
	mscMode		= 0;
	protLevelA	= 0;
	protLevelB	= 0;
	numofStreams	= 0;
	QAMMode		= 0;
}

	backendController::~backendController	() {
	if (theWorker != nullptr)
	   delete theWorker;
}

//
//	Reset is called whenever we have the start of a new stream of
//	superframe data
void	backendController::reset	(stateDescriptor *theState) {
	if (theWorker != nullptr)
	   delete theWorker;
#ifdef  __WITH_FDK_AAC__ 
	theWorker = new mscProcessor (theState, m_form, 4,
#ifdef	__MINGW32__
                                                 aacFunctions,
#endif
	                                         audioBuffer);
#else
	theWorker = new mscProcessor (theState, m_form, 4, audioBuffer);
#endif
}

void	backendController::newFrame	(stateDescriptor *theState) {
	if (theWorker == nullptr) 
#ifdef __WITH_FDK_AAC__ 
	   theWorker = new mscProcessor (theState, m_form, 4,
#ifdef	__MINGW32__
	                                           aacFunctions,
#endif
	                                           audioBuffer);
#else
	   theWorker = new mscProcessor (theState, m_form, 6, audioBuffer);
#endif
	theWorker	-> newFrame (theState);
}

void	backendController::addtoMux	(int16_t blockno,
	                                 int cnt, theSignal v) {
	if (theWorker == nullptr)	// should not/cannot happen
	   return;

	std::complex<float> temp = v. signalValue;
	iqBuffer        -> putDataIntoBuffer (&temp, 1);
	if (iqBuffer -> GetRingBufferReadAvailable () > 512)
	   showIQ (512);
	theWorker	-> addtoMux (blockno, cnt, v);
}

void	backendController::endofFrame		() {
	if (theWorker == nullptr)	// should not/cannot happen
	   return;
	theWorker	-> endofFrame ();
}

