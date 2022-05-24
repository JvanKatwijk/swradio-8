#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DRM+ Decoder
 *    DRM+ Decoder is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DRM+ Decoder is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DRM+ Decoder; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#

#ifdef	__WITH_FDK_AAC__
#ifndef	__FDK_AAC__
#define	__FDK_AAC__

#include	<aacdecoder_lib.h>
#include	<stdio.h>
#include	<vector>
#include	"basics.h"
#include	"decoder-base.h"

class	drmDecoder;

class	fdkAAC : public decoderBase {
Q_OBJECT
public:
		fdkAAC		(drmDecoder *, drmParameters *);
		~fdkAAC		();
	void	reinit		(std::vector<uint8_t>, int);
	void	decodeFrame 	(uint8_t	*audioFrame,
	             	         uint32_t	frameSize,
	             	         bool		*conversionOK,
	             	         int16_t	*buffer,
	             	         int16_t	*samples,
	             	         int32_t	*pcmRate);
private:
	void		init		();
	HANDLE_AACDECODER	handle;
	UCHAR		*theBuffer;
	uint32_t	bufferP;
	std::vector<uint8_t>	currentConfig;
	std::vector<uint8_t>
			getAudioInformation (drmParameters *drm, int streamId);

};

#endif
#endif
