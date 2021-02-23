#
/*
 *    Copyright (C) 2013 .. 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DRM backend
 *
 *    DRM backend is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DRM backend is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DRM backend; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#ifndef	__DATA_PROCESSOR__
#define	__DATA_PROCESSOR__

#include	<QObject>
#include	"radio-constants.h"
#include	<cstring>
#include	"fec-handler.h"
#include	"fir-filters.h"
#ifdef	__WITH_FDK_AAC__
#include	"aac-processor-fdk.h"
#include	"xheaac-processor.h"
#else
#include	"aac-processor-faad.h"
#endif
#include	"message-processor.h"

class	drmDecoder;
class	stateDescriptor;
class	packetAssembler;

class	dataProcessor: public QObject {
Q_OBJECT
public:
		dataProcessor	(stateDescriptor *, drmDecoder *);
		~dataProcessor	(void);
	void	process		(uint8_t *, int16_t);
enum	{S_AAC, S_CELPT, S_HVXC};
	void	selectDataService	(int16_t);
	void	selectAudioService	(int16_t);
private:
	stateDescriptor	*theState;
	drmDecoder	*drmMaster;
	messageProcessor my_messageProcessor;
#ifdef	__WITH_FDK_AAC__
	aacProcessor_fdk	my_aacProcessor;
	xheaacProcessor		my_xheaacProcessor;
#else
	aacProcessor_faad	my_aacProcessor;
#endif
	int16_t		numFrames;
	int16_t		selectedDataService;
	int16_t		selectedAudioService;
	fecHandler	*my_fecHandler;
	packetAssembler	*my_packetAssembler;
	int16_t		old_CI;
	void	process_audio	(uint8_t *, int16_t,
	                         int16_t, int16_t, int16_t, int16_t);
	void	process_packets	(uint8_t *, int16_t,
	                         int16_t, int16_t, int16_t, int16_t);
	void	process_celp	(uint8_t *, int16_t,
	                         int16_t, int16_t, int16_t, int16_t);
	void	process_hvxc	(uint8_t *, int16_t,
	                         int16_t, int16_t, int16_t, int16_t);
	void	handle_uep_packets	(uint8_t *, int16_t,
	                         int16_t, int16_t, int16_t, int16_t);
	void	handle_eep_packets	(uint8_t *, int16_t, int16_t, int16_t);
	void	handle_packets_with_FEC	(uint8_t *, int16_t, uint8_t);
	void	handle_packets	(uint8_t *, int16_t, uint8_t);
	void	process_syncStream (uint8_t *v, int16_t mscIndex,
                                    int16_t startHigh,
                                    int16_t lengthHigh,
                                    int16_t startLow,
                                    int16_t lengthLow);
	void	handle_uep_syncStream (uint8_t *v, int16_t mscIndex,
                                   int16_t startHigh, int16_t lengthHigh,
                                   int16_t startLow, int16_t lengthLow);
	void	handle_eep_syncStream (uint8_t *v, int16_t mscIndex,
                                   int16_t startLow, int16_t lengthLow);
	void	handle_syncStream	(uint8_t *v, int16_t, int16_t);
	
	uint8_t		aac_isInit;
	uint8_t		prev_audioSamplingRate;
	uint8_t		prevSBR_flag;
	uint8_t		prev_audioMode;
	int16_t		audioChannel;
	int16_t		dataChannel;
signals:
	void		show_audioMode	(QString);
	void		putSample	(float, float);
	void		faadSuccess	(bool);
	void		show_datacoding	(QString);
};

#endif

