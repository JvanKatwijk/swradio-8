#
/*
 *    Copyright (C) 2013 .. 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDunoPlugin_drm
 *
 *    DRM plugin is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DRM plugin is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DRM plugin; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#ifndef	__DATA_PROCESSOR__
#define	__DATA_PROCESSOR__

#include	<QObject>
#include	<cstring>
<<<<<<< HEAD
#include	"ringbuffer.h"
#ifdef  __WITH_FDK_AAC__
#ifdef	__MINGW32__
#include	"aac-handler.h"
#endif
#include        "aac-processor-fdk.h"
#include        "xheaac-processor.h"
=======
#include	"fec-handler.h"
#include	"fir-filters.h"
#include	"ringbuffer.h"
#ifdef	__WITH_FDK_AAC__
#include	"aac-processor-fdk.h"
#include	"xheaac-processor.h"
>>>>>>> 13f7ded765b6129a07793d5b0c777ece446b44e3
#else
#include	"aac-processor-faad.h"
#endif

class	stateDescriptor;
class	drmDecoder;

class	dataProcessor: public QObject {
Q_OBJECT
public:
		dataProcessor	(stateDescriptor *,
<<<<<<< HEAD
	                         drmDecoder *,
=======
	                         drmDecoder *, 
	                         RingBuffer<std::complex<float>> *);
		~dataProcessor	();
	void	process		(uint8_t *, int16_t);
enum	{S_AAC, S_CELPT, S_HVXC};
	void	selectDataService	(int16_t);
	void	selectAudioService	(int16_t);
private:
	stateDescriptor	*theState;
	drmDecoder	*drmMaster;
>>>>>>> 13f7ded765b6129a07793d5b0c777ece446b44e3
#ifdef	__WITH_FDK_AAC__
#ifdef	__MINGW32__
	                         aacHandler *,
#endif
#endif
	                         RingBuffer<std::complex<float>> *);
		~dataProcessor	();
	void	process		(uint8_t *, int16_t);
//	void	selectAudioService	(int16_t);
private:
	stateDescriptor		*theState;
	drmDecoder		*m_form;
	messageProcessor	my_messageProcessor;
#ifdef  __WITH_FDK_AAC__
        aacProcessor_fdk        my_aacProcessor;
        xheaacProcessor         my_xheaacProcessor;
#else
        aacProcessor_faad       my_aacProcessor;
#endif
	int16_t		numFrames;
	int		audioChannel;
	void	process_audio		(uint8_t *, int16_t,
	                         	int16_t, int16_t, int16_t, int16_t);
signals:
	void		set_audioModeLabel (const QString &);
	void		audioAvailable	();
};
#endif

