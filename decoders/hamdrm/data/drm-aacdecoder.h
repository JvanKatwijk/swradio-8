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
 *
 */
#
#ifndef __DRM_AACDECODER
#define __DRM_AACDECODER

#include	<neaacdec.h>
#include	<QObject>
//#include	<faad.h>
#include	<stdio.h>
#include	<stdint.h>

class	drmDecoder;

#define	AUD_DEC_TRANSFORM_LENGTH	960
//
//	It turns out that faad2 supports DRM, so the interface
//	can be kept pretty simple

class DRM_aacDecoder: public QObject {
Q_OBJECT
public:
		DRM_aacDecoder	(drmDecoder *drm);
	 	~DRM_aacDecoder (void);
	bool	checkfor 	(uint8_t, bool, uint8_t);
	bool	initDecoder	(int16_t, bool, uint8_t);
	void	decodeFrame	(uint8_t *,
	                         int16_t,
	                         bool *,
	                         int16_t	*,		// buffer
	                         int16_t	*, int32_t *);
	void	closeDecoder	(void);
protected:
	drmDecoder	*the_drmDecoder;
	NeAACDecHandle	the_aacDecoder;
	bool		SBR_flag;
	uint8_t		audioMode;
	int16_t		audioRate;
signals:
	void		aacData	(QString);
};

#endif // __DRM_AACDECODER

