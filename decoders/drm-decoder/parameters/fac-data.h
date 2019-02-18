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
#ifndef	__FAC_DATA
#define	__FAC_DATA

#include	<QObject>
#include	"radio-constants.h"
#include	"serviceparameters.h"
#include	"channelparameters.h"
#include	"msc-config.h"
class	drmDecoder;

//
//	The "frame" with all derived components are maintained
//	in a structure drmFrame

class	facData: public QObject {
public:
		facData		(drmDecoder *, mscConfig *);
		~facData	(void);
void		set_FAC_crc	(bool);
bool		is_FAC_crc	(void);
void		set_SDC_crc	(bool);
bool		is_SDC_crc	(void);
void		interpretSDC	(uint8_t *, int16_t);
void		set_SDCData	(uint8_t *, uint8_t, uint8_t, 
	                         uint8_t, int8_t);
void		FAC_channelParameters	(uint8_t *);
void		FAC_serviceParameters	(uint8_t *);
mscConfig	*msc;
channelParameters	myChannelParameters;
serviceParameters	myServiceParameters;
private:
	drmDecoder	*master;
	bool		FAC_crc;
	bool		SDC_crc;
};

#endif

