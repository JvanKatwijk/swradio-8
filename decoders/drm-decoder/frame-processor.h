
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the drm decoder
 *
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
#include	"backend-controller.h"
#include	"state-descriptor.h"
#include        "equalizer-1.h"
#include        "freqsyncer.h"
#include        "word-collector.h"

class	referenceFrame;
class	drmDecoder;

//typedef struct {
//	int	symbol;
//	int	carrier;
//} sdcCell;

extern	
bool	isGaincell	(uint8_t, int16_t, int16_t);

class	frameProcessor: public QThread {
Q_OBJECT
public:
			frameProcessor	(drmDecoder *,
	                                 RingBuffer<DSPCOMPLEX> *,
	                                 RingBuffer<DSPCOMPLEX> *,
	                                 RingBuffer<DSPCOMPLEX> *,
	                                 RingBuffer<std::complex<float>> *,
	                                 int32_t,	// samplerate
	                                 int16_t,	// number of symbs
	                                 int8_t,	// windowDepth
	                                 int8_t);	// qam64
			~frameProcessor (void);
	void		stop		(void);
private:
        Reader          my_Reader;              // single instance during life
	drmDecoder	*theDecoder;
	RingBuffer<std::complex<float>> *buffer;
	RingBuffer<std::complex<float>> *iqBuffer;
	RingBuffer<std::complex<float>> *eqBuffer;
	RingBuffer<std::complex<float>> *audioBuffer;
	backendController	my_backendController;
	int16_t		nSymbols;
	int32_t		sampleRate;
	int8_t		windowDepth;
	smodeInfo       modeInf;
	void		getMode			(Reader *, smodeInfo *);
	void		frequencySync 		(drmDecoder *mr,
                                       	         Reader *my_Reader,
	                                         smodeInfo *m);
	void		set_sdcCells		(smodeInfo *modeInf);
	std::vector<sdcCell> sdcTable;
	void		run		(void);
	uint8_t		getSpectrum	(stateDescriptor *);
	int16_t		sdcCells	(smodeInfo *);
	bool		isFirstFrame	(stateDescriptor *);
	bool		isLastFrame	(stateDescriptor *);
	bool		isSDCcell	(smodeInfo *, int16_t, int16_t);
	bool		isFACcell	(smodeInfo *, int16_t, int16_t);
//	bool		processFac	(float, DSPCOMPLEX **, myArray<theSignal> *);
	void		addtoSuperFrame	(smodeInfo *,
	                                 int16_t, myArray<theSignal> *);
	bool		isDatacell	(smodeInfo *,
	                                 int16_t, int16_t, int16_t);

	referenceFrame	*my_referenceFrame;
	stateDescriptor theState;
	int16_t		getnrAudio	(stateDescriptor *);
	int16_t		getnrData	(stateDescriptor *);
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

