#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DRM+ decoder
 *
 *    DRM+ decoder is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DRM+ decoder is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DRM+ decoder; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef	__WITH_FAAD__
#ifndef __DRM_AACDECODER__
#define __DRM_AACDECODER__

#include	<neaacdec.h>
#include	<QObject>
#include	<stdio.h>
#include	<stdint.h>
#include	"basics.h"
#include	"decoder-base.h"

class	drmDecoder;

#define	AUD_DEC_TRANSFORM_LENGTH	960
//
//	It turns out that faad2 supports DRM, so the interface
//	can be kept pretty simple

class DRM_aacDecoder: public decoderBase {
Q_OBJECT
public:
		DRM_aacDecoder	(drmDecoder *drm, drmParameters *);
	 	~DRM_aacDecoder (void);
	void	reinit		(std::vector<uint8_t>, int);
	void	decodeFrame	(uint8_t *,
	                         uint32_t,
	                         bool *,
	                         int16_t	*,		// buffer
	                         int16_t	*, int32_t *);
	void	closeDecoder	(void);
private:
	void	initDecoder	(int16_t       audioSampleRate,
                                  bool          SBR_used,
                                  uint8_t       audioMode);
	drmDecoder	*the_drmDecoder;
	drmParameters	*params;
	NeAACDecHandle	the_aacDecoder;
	bool		SBR_flag;
	uint8_t		audioMode;
	int16_t		audioRate;
signals:
	void		aacData	(QString);
};

#endif // __DRM_AACDECODER
#endif
