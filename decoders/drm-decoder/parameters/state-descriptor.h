#
/*
 *    Copyright (C) 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRuno plugin for drm
 *
 *    drm plugin is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm plugin is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with plugin; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#ifndef	__STATE_DESCRIPTOR__
#define	__STATE_DESCRIPTOR__

#include	<stdint.h>
#include	<string>
#include	<vector>

//
//	The "frame" with all derived components are maintained
//	in a structure drmFrame

class	stateDescriptor {
public:

enum	{
	SM, HM, HMI, HMIQ
};

enum	{
	AUDIO_STREAM, DATA_STREAM
};

		stateDescriptor	(uint8_t, uint8_t);
		~stateDescriptor	(void);
	void		cleanUp		(void);
	int		getAudioChannel		();
	void		activate_channel_1	();
	void		activate_channel_2	();
	bool		audio_channel_1;
	bool		audio_channel_2;
	bool		set;
	uint8_t		Mode;		// set by frameProcessor
	uint8_t		Spectrum;	// set by frameProcessor
	uint8_t		spectrumBits;	// set by facProcessor
	int16_t		frameIdentity;
	int		muxSize;
	int		mscCells;
	uint8_t		sdcMode;
	bool		RMflag;
	int16_t		reconfigurationhappens;

	int16_t		mscMode;	// gezet door FAC
	bool		currentConfig;	// gezet door FAC
	uint8_t		protLevelA;	// gezet door SDC
	uint8_t		protLevelB;	// gezet door SDC
	int16_t		QAMMode;	// gezet door FAC
	int16_t		interleaverDepth;// gezet door FAC
	int16_t		audioServices;
	int16_t		dataServices;
	int16_t		numofStreams;	// computed by SDC
	int16_t		dataLength;	// computed by SDC
	int16_t		dataHigh;	// computed by SDC
	struct {
	   bool		inUse;
	   uint8_t	shortId;	// identification
	   uint8_t	streamId;
	   int		serviceId;
	   char		serviceName [256];
	   bool		isAudio;
	   char	*	theLanguage;
	   char	*	theProgrammeType;
	   uint8_t	soort;		// AUDIO or DATA
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
	   std::vector<uint8_t> xHE_AAC;
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

