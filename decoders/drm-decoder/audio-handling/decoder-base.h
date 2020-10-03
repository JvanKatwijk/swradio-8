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

#ifndef	__DECODER_BASE_X__
#define	__DECODER_BASE_X__

#include	<QObject>
#include	<stdio.h>
#include	<stdint.h>
#include	<vector>

class	decoderBase : public QObject {
public:
	decoderBase	();
virtual	~decoderBase	();
virtual
void	reinit		(std::vector<uint8_t>, int);
virtual
void	decodeFrame (uint8_t    *audioFrame,
	             uint32_t	frameSize,
	             bool       *conversionOK,
	             int16_t	*buffer,
	             int16_t    *samples,
	             int32_t    *pcmRate);
};

#endif

