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
#include	"channelparameters.h"
#include	"msc-config.h"
#include	<stdio.h>

	channelParameters::channelParameters (mscConfig *msc) {
	set		= false;
	this	-> msc	= msc;
	RMflag		= 0;
	SDCMode		= 0;
}

	channelParameters::~channelParameters	(void) {
}

void	channelParameters::setBase (uint8_t *v) {
	   baseLayer = v [0];
}

void	channelParameters::setIdentity (uint8_t *v) {
	FrameIdentity	= (v [0] << 1) | v [1];
}

int16_t	channelParameters::getIdentity (void) {
	return FrameIdentity;
}

void	channelParameters::setRMflag (uint8_t *v) {
	RMflag	= v [0]& 01;
}

uint8_t	channelParameters::getRMflag	(void) {
	return RMflag;
}

void	channelParameters::setSpectrumOccupancy (uint8_t *v) {
	spectrumBits	= (v [0] << 2) | (v [1] << 1) | v [2];
}

uint8_t	channelParameters::getSpectrumBits	(void) {
	return spectrumBits;
}

void	channelParameters::setInterleaverdepthflag (uint8_t *v) {
	if (RMflag == 0) {
	   if (v [0] == 0)
	      msc	-> interleaverDepth	= 5;
	   else
	      msc	-> interleaverDepth	= 1;
	}
	else
	   if (v [0] == 0)
	      interleaving = 600;
	   else
	      interleaving = -1;		// reserved
}

void	channelParameters::setMSCmode (uint8_t *v) {
uint8_t val = (v [0] << 1) | v [1];

	if (RMflag == 0) {
	   switch (val) {
	      default:		// cannot happen
	      case 0: msc	-> QAMMode	= mscConfig::QAM64;
	              msc	-> mscMode	= mscConfig::SM;
	              break;
	      case 1: msc	-> QAMMode	= mscConfig::QAM64;
	              msc	-> mscMode	= mscConfig::HMI;
	              break;
	      case 2: msc	-> QAMMode	= mscConfig::QAM64;
	              msc	-> mscMode	= mscConfig::HMIQ;
	              break;
	      case 3: msc	-> QAMMode	= mscConfig::QAM16;
	              msc	-> mscMode	= mscConfig::SM;
	   }
	}
	else {		// RMflag == 1
	   switch (val) {
	      default:		// cannot happen
	      case 0: msc	-> QAMMode	= mscConfig::QAM16;
	              msc	-> mscMode	= mscConfig::SM;
	      case 1: ;
	   }
	}
}

void	channelParameters::setSDCmode (uint8_t *v) {
	SDCMode		= v [0] & 01;
}

uint8_t	channelParameters::getSDCmode (void) {
	return SDCMode;
}


void	channelParameters::Numberofservices (uint8_t *v) {
uint8_t val	= (v [0] << 3) | (v [1] << 2) | (v [2] << 1) | v [3];

	switch (val) {
	   case 0000:
	      audioServices = 4; dataServices = 0; break;
	   case 0001:
	      audioServices = 0; dataServices = 1; break;
	   case 0002:
	      audioServices = 0; dataServices = 2; break;
	   case 0003:
	      audioServices = 0; dataServices = 3; break;
	   case 0004:
	      audioServices = 1; dataServices = 0; break;
	   case 0005:
	      audioServices = 1; dataServices = 1; break;
	   case 0006:
	      audioServices = 1; dataServices = 2; break;
	   case 0007:
	      audioServices = 1; dataServices = 3; break;
	   case 0010:
	      audioServices = 2; dataServices = 0; break;
	   case 0011:
	      audioServices = 2; dataServices = 1; break;
	   case 0012:
	      audioServices = 2; dataServices = 2; break;
	   case 0013:	// cannot happen
	   case 0014:
	      audioServices = 3; dataServices = 0; break;
	   case 0015:
	      audioServices = 3; dataServices = 1; break;
	   case 0016:		// cannot happen
	   case 0017:
	      audioServices = 0; dataServices = 4; break;
	}
//	fprintf (stderr, "audioservices = %d, dataservices = %d\n",
//	         audioServices, dataServices);
}

void	channelParameters::ReconfigurationIndex (uint8_t *v) {
	reconfigurationhappens = (v [0] << 2) | (v [1] << 1) | v [2];
}

void	channelParameters::Toggleflag (uint8_t *v) {
//	just ignore for now
	(void)v;
}

//	starting with bit 20 (starting with 0)
void	channelParameters::Serviceidentifier (uint8_t *v) {
	(void)v;
}

//	at bit 44
void	channelParameters::ShortId	(uint8_t *v) {
	(void)v;
}

//	at bit 46
void	channelParameters::AudioCAindication (uint8_t *v) {
	(void)v;
}

//	at bit 47
void	channelParameters::Language	(uint8_t *v) {
	(void)v;
}

//	at bit 51
void	channelParameters::AudioDataFlag	(uint8_t *v) {
	(void)v;
}

//	at bit 52
void	channelParameters::ServiceDescriptor	(uint8_t *v) {
	(void)v;
}

//	at bit 57
void	channelParameters::DataCAindication	(uint8_t *v) {
	(void)v;
}

//	at bit 58
void	channelParameters::rfa		(uint8_t *v) {
	(void)v;
}

