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
/*
 *	SDC processor for QAM4/QAM16 encoded SDC's
 */
#ifndef	__SDC_PROCESSOR
#define	__SDC_PROCESSOR

#include	"radio-constants.h"
#include	"basics.h"
#include	"mapper.h"
#include	"checkcrc.h"

class	facData;
class	prbs;
class	SDC_streamer;
class	qam4_metrics;
class	qam16_metrics;
class	viterbi_drm;
class	Mapper;

class	sdcProcessor {
public:
		sdcProcessor	(uint8_t,
	                 	uint8_t,
	                        viterbi_drm *,
	                        facData	*,
	                 	int16_t);
		~sdcProcessor	(void);
	bool	processSDC	        (theSignal *v);
	uint8_t	getSDCmode		(void);
	uint8_t	getRMflag		(void);
private:
	bool	processSDC_QAM4		(theSignal *v);
	bool	processSDC_QAM16	(theSignal *v);
	qam16_metrics	*my_qam16_metrics;
	qam4_metrics	*my_qam4_metrics;
	void		checkConfig	(void);
	SDC_streamer	*stream_0;
	SDC_streamer	*stream_1;
	checkCRC	theCRC;
	Mapper		Y13Mapper;
	Mapper		Y21Mapper;
	viterbi_drm	*deconvolver;
	uint8_t		Mode;
	uint8_t		Spectrum;
	facData		*my_facDB;
	int16_t		nrCells;
	int16_t		lengthofSDC;
	prbs		*thePRBS;
	uint8_t		qammode;
	uint8_t		rmFlag;
	uint8_t		SDCmode;
};


#endif


	
