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
#ifndef	__BACKEND_CONTROLLER__
#define	__BACKEND_CONTROLLER__

#include	<QObject>
#include	"basics.h"
#include	"ringbuffer.h"

class   stateDescriptor;
class	drmDecoder;
class	mscProcessor;

#ifdef	__WITH_FDK_AAC__
#ifdef	__MINGW32__
#include	"aac-handler.h"
#endif
#endif

class	backendController: public QObject {
Q_OBJECT
public:
		backendController	(drmDecoder *,
	                                 int8_t,
#ifdef	__WITH_FDK_AAC__
#ifdef	__MINGW32__
	                                 aacHandler	*,
#endif
#endif
	                                 RingBuffer<std::complex<float>> *,
	                                 RingBuffer<std::complex<float>> *);
		~backendController	();
	void	newFrame		(stateDescriptor *);
	void	addtoMux		(int16_t, int, theSignal);
	void	endofFrame		();
	void	reset			(stateDescriptor *);

private:
	drmDecoder	*m_form;
	mscProcessor	*theWorker;
	stateDescriptor	*theState;
	RingBuffer<std::complex<float>> *audioBuffer;
	RingBuffer<std::complex<float>> *iqBuffer;
	int8_t		qam64Roulette;
	uint8_t		mscMode;
	uint8_t		protLevelA;
	uint8_t		protLevelB;
	int16_t		numofStreams;
	uint8_t		QAMMode;
#ifdef	__WITH_FDK_AAC__
#ifdef	__MINGW32__
	aacHandler	*aacFunctions;
#endif
#endif
signals:
	void		showIQ		(int);
};

#endif
