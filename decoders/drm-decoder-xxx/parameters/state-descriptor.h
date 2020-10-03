#
/*
 *    Copyright (C) 2019
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
#ifndef	__STATE_DESCRIPTOR__
#define	__STATE_DESCRIPTOR__

#include	"radio-constants.h"
class	drmDecoder;

//
//	The "frame" with all derived components are maintained
//	in a structure drmFrame

class	stateDescriptor {
public:
enum	{
	QAM4, QAM16, QAM64
};

enum	{
	SM, HM, HMI, HMIQ
};

enum	{
	AUDIO_STREAM, DATA_STREAM
};

		stateDescriptor	(drmDecoder *, uint8_t, uint8_t);
		~stateDescriptor	(void);
	void		cleanUp		(void);
	int16_t		getIdentity	(void);
	uint8_t		getRMflag	(void);
	uint8_t		getSpectrumBits	(void);
	uint8_t		getSDCmode	(void);
	int16_t		muxSize		(void);
	int16_t		muxDepth	(void);
	int16_t		mscCells	(void);
	int16_t		getnrAudio	(void);
	int16_t		getnrData	(void);

	void		protLevel	(uint8_t, int16_t, int16_t *, int16_t *);
	float		getRp		(uint8_t, int16_t);
	int16_t		getRYlcm_64	(int16_t);
	int16_t		getRYlcm_16	(int16_t);

	bool		set;
	drmDecoder	*master;
	uint8_t		Mode;		// set by constructor
	uint8_t		Spectrum;	// set by constructor

	bool		FAC_crc;
	bool		SDC_crc;

	uint8_t		SDCMode;
	bool		RMflag;
//	bool		notSupported;
	int16_t		interleaving;
	int16_t		MSC_QAM;
//	int16_t		hierarchical;
	int16_t		SDC_QAM;
	int16_t		reconfigurationhappens;
	int16_t		FrameIdentity;
	uint8_t		spectrumBits;

	int16_t		mscMode;	// gezet door FAC
	bool		currentConfig;	// gezet door FAC
	uint8_t		protLevelA;	// gezet door SDC
	uint8_t		protLevelB;	// gezet door SDC
	int16_t		QAMMode;	// gezet door FAC
	int16_t		numofStreams;	// computed by SDC
	int16_t		dataLength;	// computed by SDC
	int16_t		dataHigh;	// computed by SDC
	int16_t		interleaverDepth;// gezet door FAC
	int16_t		audioServices;
	int16_t		dataServices;
	const char	*theLanguage;
	const char	*theProgrammeType;
	bool		AudioService;
	struct {
	   uint8_t	soort;		// AUDIO or DATA
	   uint8_t	shortId;	// identification
	   bool		selected;
	   uint8_t	streamId;
	   int16_t	lengthHigh;
	   int16_t	lengthLow;
	   uint8_t	audioCoding;
	   uint8_t	SBR_flag;
	   uint8_t	audioMode;
	   uint8_t	audioSamplingRate;
	   uint8_t	textFlag;
	   uint8_t	enhancementFlag;
	   uint8_t	coderField;
	   uint8_t	rfa;
	   uint8_t	domain;		//for data transmission
	   uint8_t	packetModeInd;
	   uint8_t	dataUnitIndicator;
	   uint8_t	packetId;
	   int16_t	R;
	   int16_t	C;
	   uint8_t	FEC;
	   int16_t	packetLength;
	   int16_t	applicationId;
	   int16_t	startofData;
	} streams [4];
};

#endif

