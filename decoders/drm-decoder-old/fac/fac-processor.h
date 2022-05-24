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
#ifndef	__FAC_PROCESSOR__
#define	__FAC_PROCESSOR__

#include	<QObject>
#include	"radio-constants.h"
#include	"basics.h"
#include	"mapper.h"
#include	"qam4-metrics.h"
#include	"prbs.h"
#include	"checkcrc.h"
#include	"viterbi-drm.h"
#include	"my-array.h"

class	drmDecoder;
class	stateDescriptor;
class	equalizer;		// very bad name

class	facProcessor : public QObject {
Q_OBJECT
public:
	                facProcessor	(drmDecoder *, smodeInfo *);
			~facProcessor	(void);
	bool		processFAC	(float        meanEnergy,
                                         std::complex<float> **H,
                                         myArray<theSignal> *outbank,
                                         stateDescriptor *theState);

private:
	drmDecoder	*theDecoder;
	smodeInfo	*modeInf;
	uint8_t		Mode;
	uint8_t		Spectrum;
	struct facElement	*facTable;
	Mapper		myMapper;
	prbs		thePRBS;
	checkCRC	theCRC;
	qam4_metrics	myMetrics;
	viterbi_drm	deconvolver;
	void		fromSamplestoBits	(theSignal *, uint8_t *);
//	void		fac_Metrics		(theSignal *, int32_t, metrics *);
	void		interpretFac		(uint8_t *, stateDescriptor *);
	void		set_serviceParameters	(uint8_t *, stateDescriptor *);
	void		set_channelParameters	(uint8_t *, stateDescriptor *);
	void		set_serviceLanguage	(int, uint8_t *, stateDescriptor *);
	void		set_serviceDescriptor	(int, uint8_t *, stateDescriptor *);
	int16_t		mscCells		(uint8_t, uint8_t);
signals:
	void		showSNR		(float);
};

#endif

