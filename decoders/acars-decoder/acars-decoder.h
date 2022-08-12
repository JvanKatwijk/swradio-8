
/*
 *  Copyright (c) 2015 Thierry Leconte
 *
 *   
 *   This code is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2
 *   published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *	The channel decoder is a rewrite of the mask.c and (parts
 *	of) the acars.c files of Thierry Leconte. All rights 
 *	acknowledged
 */
#ifndef	__ACARS_DECODER__
#define	__ACARS_DECODER__

#include	<complex>
#include	<time.h>
#include	<sys/time.h>
#include	"virtual-decoder.h"
#include	"acars-constants.h"
#include	"printer.h"

const float PLLC	= 3.8e-3;

#define	MAXPERR	10
#define	FLEN	((WORKING_RATE / 1200) + 1)
#define SYN 0x16
#define SOH 0x01
#define STX 0x02
#define ETX 0x83
#define ETB 0x97
#define DLE 0x7f

class	printer;
class	RadioInterface;

class	acarsDecoder: public virtualDecoder {
public:
			acarsDecoder	(bool	verbose, RadioInterface *);
			~acarsDecoder	();
	void		process		(std::complex<float>);
private:
	enum {WSYN, SYN2, SOH1, TXT, CRC1,CRC2, END } Acarsstate;
	printer		myPrinter;
	void		demodMSK	(float *, int);
	void		putbit		(float v);
	void		decodeAcars	(uint8_t thebyte);
	bool		fixprerr	(uint8_t *blk_txt, int blk_len,
                                         const uint16_t crc, int *pr, int pn);
	bool		fixdberr	(uint8_t *blk_txt,
                                         int blk_len, const uint16_t crc);
	void		processBlock	(uint8_t *crc,
                                         uint8_t *blk_txt,
	                                 int blk_len, float blk_lvl);
	int		frequency;
	bool		verbose;
	double		MskPhi;
	int		MskS;
	double		MskDf;
	float		MskClk;
	float		Msklvl;
	int		idx;
	struct timeval	blk_tm;
	std::complex<float> inbuffer [FLEN];
	float		matchedFilter_h [2 * FLEN];

	int		nbits;
	uint8_t		outbits;

	int		blk_err;
	int		blk_len;
	uint8_t		blk_txt [256];
	uint8_t		blk_crc [2];
};

#endif

