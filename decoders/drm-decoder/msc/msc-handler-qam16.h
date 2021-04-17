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
#ifndef	__MSC_HANDLER_QAM16__
#define	__MSC_HANDLER_QAM16__

#include	"radio-constants.h"
#include	"msc-handler.h"
#include	"qam16-metrics.h"

class	stateDescriptor;
class	MSC_streamer;
class	Mapper;

class	QAM16_SM_Handler : public mscHandler {
public:
		QAM16_SM_Handler	(drmDecoder *, stateDescriptor *);
		~QAM16_SM_Handler	(void);
	void	process			(theSignal *, uint8_t *);
private:
	qam16_metrics	myDecoder;
	stateDescriptor	*theState;
	int16_t		lengthA;
	int16_t		lengthB;
	uint8_t		*out;
	MSC_streamer	*stream_0;
	MSC_streamer	*stream_1;
	Mapper		*Y13mapper_high;
	Mapper		*Y21mapper_high;
	Mapper		*Y13mapper_low;
	Mapper		*Y21mapper_low;
	int16_t		N1, N2;
};
#endif

