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
#ifndef	__DATA_PROCESSOR
#define	__DATA_PROCESSOR

#include	<QObject>
#include	"radio-constants.h"
#include	<cstring>
#include	"fec-handler.h"
#include	"fir-filters.h"
#include	"drm-aacdecoder.h"
#include	"message-processor.h"

class	drmDecoder;
class	mscConfig;
class	packetAssembler;

typedef	struct frame {
	int16_t length, startPos;
	uint8_t	aac_crc;
	uint8_t audio [512];
} audioFrame; 
	
class	dataProcessor: public QObject {
Q_OBJECT
public:
		dataProcessor	(mscConfig *, drmDecoder *);
		~dataProcessor	(void);
	void	process		(uint8_t *, int16_t);
enum	{
	S_AAC, S_CELPT, S_HVXC
};
	void	selectDataService	(int16_t);
	void	selectAudioService	(int16_t);
private:
	messageProcessor my_messageProcessor;
	DRM_aacDecoder	my_aacDecoder;
	LowPassFIR	upFilter_24000;
	LowPassFIR	upFilter_12000;
	int16_t		numFrames;
	int16_t		selectedDataService;
	int16_t		selectedAudioService;
	fecHandler	*my_fecHandler;
	mscConfig	*msc;
	drmDecoder	*drmMaster;
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
	void	process_aac	(uint8_t *, int16_t, int16_t,
	                         int16_t, int16_t, int16_t);
	void	handle_uep_audio	(uint8_t *, int16_t,
	                         int16_t, int16_t, int16_t, int16_t);
	void	handle_eep_audio	(uint8_t *, int16_t, int16_t, int16_t);
	void	handle_uep_packets	(uint8_t *, int16_t,
	                         int16_t, int16_t, int16_t, int16_t);
	void	handle_eep_packets	(uint8_t *, int16_t, int16_t, int16_t);
	void	handle_packets_with_FEC	(uint8_t *, int16_t, uint8_t);
	void	handle_packets	(uint8_t *, int16_t, uint8_t);
	void	writeOut	(int16_t *, int16_t, int32_t);
	void	toOutput	(float *, int16_t);
	void	playOut		(int16_t);
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
};

#endif

