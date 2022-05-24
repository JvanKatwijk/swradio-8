#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDR-J (JSDR).
 *
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
#ifndef	__AAC_PROCESSOR_FAAD_H
#define	__AAC_PROCESSOR_FAAD_H

#include	<QObject>
#include	"radio-constants.h"
#include	<cstring>
#include	<neaacdec.h>
#include	"fir-filters.h"
#include	"up-filter.h"

class	drmDecoder;
class	stateDescriptor;

typedef	struct frame {
	int16_t length, startPos;
	uint8_t	aac_crc;
	uint8_t audio [512];
} audioFrame; 
	
class	aacProcessor_faad: public QObject {
Q_OBJECT
public:
		aacProcessor_faad	(stateDescriptor *, drmDecoder *);
		~aacProcessor_faad	(void);
	void	process_aac	(uint8_t *, int16_t,
	                         int16_t, int16_t, int16_t, int16_t);
private:
	stateDescriptor	*theState;
	drmDecoder	*drmMaster;
//	DRM_aacDecoder	my_aacDecoder;
//	LowPassFIR	upFilter_24000;
//	LowPassFIR	upFilter_12000;
	upFilter	upFilter_24000;
	upFilter	upFilter_12000;

	int16_t		numFrames;
	void	handle_uep_audio	(uint8_t *, int16_t,
	                         int16_t, int16_t, int16_t, int16_t);
	void	handle_eep_audio	(uint8_t *, int16_t, int16_t, int16_t);
	void    writeOut        (int16_t *, int16_t, int32_t);
        void    toOutput	(std::complex<float> *, int16_t);
        void    playOut         (int16_t);
//	uint8_t		aac_isInit;
//	uint8_t		prev_audioSamplingRate;
//	uint8_t		prevSBR_flag;
//	uint8_t		prev_audioMode;
	bool    checkfor        (uint8_t, bool, uint8_t);
        bool    initDecoder     (int16_t, bool, uint8_t);
        void    decodeFrame     (uint8_t *,
                                 int16_t,
                                 bool *,
                                 int16_t        *,              // buffer
                                 int16_t        *, int32_t *);
        void    closeDecoder    (void);
	NeAACDecHandle  theDecoder;
        bool            SBR_flag;
        uint8_t         audioMode;
        int16_t         audioRate;

signals:
	void		show_audioMode	(QString);
	void		putSample	(float, float);
	void		faadSuccess	(bool);
	void		aacData		(QString);
};

#endif

