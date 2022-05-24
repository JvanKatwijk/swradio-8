#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DRM+ decoder
 *
 *    DRM+ decoder is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DRM+ decoder is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DRM+ decoder; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef	__AAC_PROCESSOR__
#define	__AAC_PROCESSOR__

#include	"basics.h"
#include        <QObject>
#include        <cstring>
#include	<vector>
#include	"fir-filters.h"
#include	"decoder-base.h"
#include	"message-processor.h"

typedef struct frame {
	int16_t length, startPos;
	uint8_t aac_crc;
	uint8_t audio [512];
} audioFrame;

class	drmDecoder;

class	aacProcessor: public QObject {
Q_OBJECT
public:
			aacProcessor	(drmDecoder *,
	                                 drmParameters *,
	                                 decoderBase *);
			~aacProcessor	();
	void		process_aac     (uint8_t *, int16_t, int16_t,
	                                 int16_t, int16_t, int16_t);
private:
	std::vector<uint8_t>
        		getAudioInformation (drmParameters *drm, int streamId);

	drmDecoder	*parent;
	drmParameters	*params;
	messageProcessor my_messageProcessor;
	decoderBase	*my_aacDecoder;
	LowPassFIR      upFilter_24000;
	int		numFrames;
	void		handle_uep_audio (uint8_t *, int16_t,
	                                 int16_t, int16_t, int16_t, int16_t);
	void		handle_eep_audio (uint8_t *, int16_t,
	                                          int16_t, int16_t);
	void		writeOut	(int16_t *, int16_t, int32_t);
	void		toOutput	(float *, int16_t);
	void		playOut		(int16_t);
signals:
	void            putSample       (float, float);
	void            faadSuccess     (bool);
};

#endif

