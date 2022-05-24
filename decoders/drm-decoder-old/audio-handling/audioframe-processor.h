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

#ifndef __POST_PROCESSOR__
#define __POST_PROCESSOR__

#include        "basics.h"
#include        <QObject>
#include        <cstring>
#include        "fir-filters.h"
#include        "message-processor.h"
#include	"decoder-base.h"

class	drmDecoder;
class	aacProcessor;
class	xheaacProcessor;

class	audioFrameProcessor: public QObject{
Q_OBJECT
public:
		audioFrameProcessor	(drmDecoder *,
	                                 drmParameters *, int, int);
		~audioFrameProcessor	();
	void	process  (uint8_t *, bool);
private:
	drmDecoder	*parent;
	drmParameters	*params;
	int		shortId;
	int		streamId;
	uint8_t		*firstBuffer;
	decoderBase	*my_aacDecoder;
	aacProcessor	*my_aacProcessor;
	xheaacProcessor	*my_xheaacProcessor;
	void		processAudio (uint8_t *v, int16_t streamIndex,
	                              int16_t startHigh, int16_t lengthHigh,
	                              int16_t startLow,  int16_t lengthLow);
	int		lengthA_total;
	int		lengthB_total;
signals:
	void		show_audioMode (QString);
};
#endif
