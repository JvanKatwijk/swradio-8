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
#ifndef	__MSCCONFIG
#define	__MSCCONFIG
#
#include	<stdint.h>
//
//	mscConfig is a class whose instance contains the 
//	relevant values for decoding the msc superframes
//	The instance is filled by both the 
//	FAC and SDC, its values used by the mscHandlers

class	mscConfig {
public:
			mscConfig	(uint8_t Mode, uint8_t Spectrum);
			~mscConfig	(void);
	int16_t		muxSize		(void);
	void		protLevel	(uint8_t protLevel, int16_t nr,
	                                 int16_t *, int16_t *);
	int16_t		getRYlcm_64	(int16_t);
	int16_t		getRYlcm_16	(int16_t);
	float		getRp		(uint8_t, int16_t);
	int16_t		muxDepth	(void);
	int16_t		mscCells	(void);
enum	{
	QAM4, QAM16, QAM64
};

enum	{
	SM, HM, HMI, HMIQ
};

enum	{
	AUDIO_STREAM, DATA_STREAM
};

	uint8_t		Mode;		// set by constructor
	uint8_t		Spectrum;	// set by constructor
	int16_t		mscMode;	// gezet door FAC
	bool		currentConfig;	// gezet door FAC
	uint8_t		protLevelA;	// gezet door SDC
	uint8_t		protLevelB;	// gezet door SDC
	int16_t		QAMMode;	// gezet door FAC
	int16_t		numofStreams;	// computed by SDC
	int16_t		dataLength;	// computed by SDC
	int16_t		dataHigh;	// computed by SDC
	int16_t		interleaverDepth;// gezet door FAC
	struct {
	   uint8_t	soort;		// AUDIO or DATA
	   uint8_t	shortId;	// identification
	   bool		selected;
	   uint8_t	streamId;
	   uint8_t	language [3];
	   uint8_t	country  [2];
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
	int16_t		getnrAudio	(void);
	int16_t		getnrData	(void);
};

#endif

