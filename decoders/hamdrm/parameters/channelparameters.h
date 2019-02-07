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
#ifndef	__CHANNELPARAMETERS
#define	__CHANNELPARAMETERS
#include	<stdint.h>
class	mscConfig;

class	channelParameters  {
public:
		channelParameters	(mscConfig *);
		~channelParameters	(void);
void		setBase			(uint8_t *v);
void		setIdentity 		(uint8_t *v);
void		setRMflag 		(uint8_t *v);
void		setSpectrumOccupancy	(uint8_t *v);
uint8_t		getSpectrumBits		(void);
void		setInterleaverdepthflag (uint8_t *v);
void		setMSCmode		(uint8_t *v);
void		setSDCmode 		(uint8_t *v);
void		Numberofservices 	(uint8_t *v);
void		ReconfigurationIndex 	(uint8_t *v);
void		Toggleflag 		(uint8_t *v);
bool		is_firstFACblock	(void);
bool		is_lastFACblock		(void);
int16_t		getIdentity		(void);
uint8_t		getSDCmode		(void);
uint8_t		getRMflag		(void);
void		Serviceidentifier 	(uint8_t *v);
void		ShortId			(uint8_t *v);
void		AudioCAindication 	(uint8_t *);
void		Language		(uint8_t *v);
void		AudioDataFlag		(uint8_t *v);
void		ServiceDescriptor	(uint8_t *v);
void		DataCAindication	(uint8_t *v);
void		rfa			(uint8_t *v);
private:
uint8_t		SDCMode;
bool		set;
bool		baseLayer;
bool		SDC_AFS;
bool		RMflag;
uint8_t		Spectrum;
bool		notSupported;
int16_t		interleaving;
int16_t		MSC_QAM;
int16_t		hierarchical;
int16_t		SDC_QAM;
int16_t		SDC_Coderate;
int16_t		audioServices;
int16_t		dataServices;
int16_t		reconfigurationhappens;
int16_t		FrameIdentity;
mscConfig	*msc;
uint8_t		spectrumBits;
};
#endif

