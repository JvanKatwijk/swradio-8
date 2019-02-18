
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
#ifndef	__FRAME_PROCESSOR__
#define	__FRAME_PROCESSOR__

#include	<QThread>
#include	<QString>
#include	<atomic>
#include	"radio-constants.h"
#include	"ringbuffer.h"
#include	"drm-decoder.h"
#include	"basics.h"
#include	"timesync.h"
#include	"reader.h"
#include	"viterbi-drm.h"

class	equalizer_base;
class	facData;
class	referenceFrame;
class	facProcessor;
class	sdcProcessor;
class	mscProcessor;
class	drmDecoder;
class	mscConfig;
class	wordCollector;
struct facElement {
	int16_t	symbol;
	int16_t	carrier;
};

extern	
bool	isGaincell	(uint8_t, int16_t, int16_t);

class	frameProcessor: public QThread {
Q_OBJECT
public:
			frameProcessor	(drmDecoder *,
	                                 RingBuffer<DSPCOMPLEX> *,
	                                 int32_t,	// samplerate
	                                 int16_t,	// number of symbs
	                                 int8_t,	// windowDepth
	                                 int8_t);	// qam64
			~frameProcessor (void);
	void		stop		(void);
private:
	drmDecoder	*mr;
	RingBuffer<DSPCOMPLEX> *buffer;
	int16_t		nSymbols;
	int32_t		sampleRate;
	int8_t		windowDepth;
	int8_t		qam64Roulette;
	smodeInfo       modeInf;
        std::atomic<bool> taskMayRun;
        void            getMode                 (Reader *, smodeInfo *);
	void		frequencySync 		(drmDecoder *mr,
                                       	         Reader *my_Reader,
	                                         smodeInfo *m);
	DSPCOMPLEX	facError		(DSPCOMPLEX *,
	                                         DSPCOMPLEX *, int16_t);
	void		deleteProcessors	(void);
	void		createProcessors	(smodeInfo *);
	void		run		(void);
	uint8_t		getSpectrum	(facData *);
	int16_t		sdcCells	(smodeInfo *);
	int16_t		mscCells	(smodeInfo *);
	bool		is_bestIndex	(smodeInfo *, int16_t);
	float	getCorr		(smodeInfo *, DSPCOMPLEX *);
	bool		isFirstFrame	(facData *);
	bool		isLastFrame	(facData *);
	bool		isSDCcell	(smodeInfo *, int16_t, int16_t);
	bool		isFACcell	(smodeInfo *, int16_t, int16_t);
	int16_t		extractSDC	(smodeInfo *,
	                                 theSignal **, theSignal *);
	bool		getFACdata	(smodeInfo *, theSignal **, facData *,
	                                 float, DSPCOMPLEX **);
	bool		process_sdc	(smodeInfo *, facData *, theSignal **);
	void		addtoSuperFrame	(smodeInfo *, int16_t);
	bool		isDatacell	(smodeInfo *,
	                                 int16_t, int16_t, int16_t);
        Reader          my_Reader;              // single instance during life

//	viterbi_drm	my_dummy;
	wordCollector	*my_wordCollector;
	referenceFrame	*my_referenceFrame;
	mscProcessor	*my_mscProcessor;
	mscConfig	*my_mscConfig;
	int16_t		symbolsinFrame;
	equalizer_base	*my_Equalizer;
	facData		*my_facData;
	float		corrBank [30];
	DSPCOMPLEX	**inbank;
	theSignal	**outbank;
	DSPCOMPLEX	**refBank;
//
signals:
	void		setTimeSync	(bool);
	void		setFACSync	(bool);
	void		setSDCSync	(bool);
	void		show_Mode	(int);
	void		show_Spectrum	(int);
	void		show_services	(int, int);
	void		show_audioMode	(QString);
	void		showEq		(int);
	void		showSNR		(float);
	void		show_freq2	(float);
};

#endif

