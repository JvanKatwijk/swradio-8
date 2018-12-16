#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J.
 *    Many of the ideas as implemented in SDR-J are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
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
 *
 * 	Elements of this code are adapted from:
 *	mbed SI570 Library, for driving the SI570 programmable VCXO
 *      Copyright (c) 2010, Gerrit Polder, PA3BYA
 *	All rights acknowledged
 */

#ifndef	__SI570_HANDLER__
#define	__SI570_HANDLER__

#include	"pmsdr_comm.h"

typedef
struct si570data_ {
        unsigned char	n1;
        unsigned char	hsdiv;
        uint32_t	frac_bits;
	double		currentFreq;
        double		rfreq;
        double		fxtal;
} si570data;

class	si570Handler {
public:
	  	si570Handler	(pmsdr_comm *, int16_t, int16_t, int16_t); 
		~si570Handler	(void);
	int32_t	setVFOfrequency	(int32_t);
	int32_t	getVFOfrequency	(void);
	uint8_t	getReg		(int16_t);
	void	setRegs		(uint8_t *);
private:
	pmsdr_comm	*pmsdrDevice;
	int16_t		usbFirmwareSub;
	int16_t		usbFirmwareMajor;
	int16_t		usbFirmwareMinor;
	uint8_t		si570_reg [8];
	si570data	si570pmsdr;
	uint8_t		postDivider;
	int32_t		vfoFreq;

	bool		ReadandConvertRegisters (int8_t);
	void		calc_si570registers 	(uint8_t* reg,
	                                         si570data* si570struct);
	void		ComputeandSetSi570 	(double);
	bool		computeDividers 	(double,
	                                         uint8_t *, uint8_t *);
	void		convertFloattoHex 	(si570data *, uint16_t *);
	bool		setFrequencySmallChange (si570data *,
	                                         double,
	                                         uint16_t *);
	bool		setFrequencyLargeChange (si570data *,
	                                         double,
	                                         uint16_t *);
};

#endif
