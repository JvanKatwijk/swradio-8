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

#ifndef	__XHEAAC_PROCESSOR__
#define	__XHEAAC_PROCESSOR__

#include	"basics.h"
#include        <QObject>
#include	<stdint.h>
#include        <cstring>
#include	<vector>
#include	<deque>
#include	<complex>
#include	"checkcrc.h"
#include	"decoder-base.h"
#include	"message-processor.h"

class	drmDecoder;
class	rateConverter;

class	xheaacProcessor: public QObject {
Q_OBJECT
public:
			xheaacProcessor	(drmDecoder *,
	                                 drmParameters *,
	                                 decoderBase *);
			~xheaacProcessor	();
	void		process_usac	(uint8_t *v, int16_t mscIndex,
                                         int16_t startHigh, int16_t lengthHigh,
                                         int16_t startLow, int16_t lengthLow);
private:
	void		resetBuffers	();
	void		processFrame	(int);
	drmDecoder	*parent;
	drmParameters	*params;
	messageProcessor my_messageProcessor;
	checkCRC	theCRC;
	int		currentRate;
	std::vector<uint8_t>
        		getAudioInformation (drmParameters *drm,
                                                        int streamId);
//	deque<uint8_t>	frameBuffer;
//	vector<uint32_t> borders;
	decoderBase	*my_aacDecoder;
	rateConverter	*theConverter;
	int		numFrames;
	void		writeOut	(int16_t *, int16_t, int32_t);
	void		toOutput	(std::complex<float> *, int16_t);
	void		playOut		(std::vector<uint8_t>);
signals:
	void            putSample       (float, float);
	void            faadSuccess     (bool);
};

#endif

