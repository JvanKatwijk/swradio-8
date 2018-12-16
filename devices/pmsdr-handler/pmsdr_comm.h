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
 */

#ifndef	__PMSDR_COMM__
#define	__PMSDR_COMM__

#include	<stdio.h>
#include	<string.h>
#include	<stdbool.h>
#include	<assert.h>
#include	<stdlib.h>
#include	"pmsdr_usb.h"

#define READ_VERSION        0x00
#define READ_FLASH          0x01
#define WRITE_FLASH         0x02
#define ERASE_FLASH         0x03
#define READ_EEDATA         0x04
#define WRITE_EEDATA        0x05
#define READ_CONFIG         0x06
#define WRITE_CONFIG        0x07

#define ID_BOARD            0x31
#define UPDATE_LED          0x32
#define SET_LO_FREQ         0x33
#define RD_DDS_FREQ         0x34
#define RD_PMSDR            0x35
#define SET_QSD_BIAS        0x36
#define RD_LO_FREQ          0x37
#define LCD_PUTS            0x38
#define SET_PLL             0x39
#define SET_SI570           0x40
#define SET_FILTER          0x41
#define RD_USB_STRING	    0x42
#define RD_SI570            0x43
#define RD_FILTER           0x44
#define WriteI2C_PMSDR      0x45
#define ReadI2C_PMSDR       0x46
#define WriteE2P_PMSDR      0x47
#define ReadE2P_PMSDR       0x48
#define LCD_LIGHT	    0x49
#define SET_SMOOTHSI570     0x4A
#define ScanI2C_PMSDR       0x4B
#define SetFilterboard_PMSDR    0x4C
#define ScanSOFTI2C_PMSDR  0x4D
#define SET_DOWNCONVERTER 0x4E
#define RD_DOWNCONVERTER  0x4F
#define SET_DOWNCONVERTER_SMOOTH 0x50
#define WriteI2CSOFT_PMSDR      0x51
#define ReadI2CSOFT_PMSDR       0x52
#define	SET_DOWNCONVERTER_FILTER  0x53
#define RESET               0xFF

// PIC18F4550 E2Prom mapping
#define SI570_CALIB      0x01
#define SI570_REG_ADR    0x14    // Si570 Startup registers
#define POST_DIV_ADR     0x1A    // Cy2239x Startup postdivider register
#define DEF_FILTER_ADR   0x1B    // Startup filter register
#define DEFAULT_FRQ_ADR  0x1F    // Startup default frequency (long) 4 bytes

typedef struct {
    uint16_t   QSDbias;           // 0 .. 1023, default 512
    bool       QSDmute;           // True mute the receiver
    uint8_t    filter;            // 0 no filter
                                  // 1 pass band filter 2-6   MHz
                                  // 2 pass band filter 5-12  MHz
                                  // 3 pass band filter 10-24 MHz
                                  // the following value is suitable for 2.1 HW only !
                                  // 4 low pass  filter 2 MHz
    // the following is suitable for 2.0 HW only !
    uint8_t    IfGain;            // 0 10 dB 
                                  // 1 20 dB 
                                  // 2 30 dB
                                  // 3 40 dB
} QsdBiasFilterIfGain;

class	pmsdr_comm: public pmsdr_usb {
public:
		pmsdr_comm	(void);
		~pmsdr_comm	(void);

	bool	pmsdr_isWorking	(void);
	void	get_qsd_bias	(QsdBiasFilterIfGain *);
	void	set_qsd_bias	(QsdBiasFilterIfGain *, uint8_t *);
	void	get_version	(int16_t *, int16_t *, int16_t *);
	void	lcdPuts		(const char *,
	                         uint16_t, uint16_t, uint16_t);
	bool	ScanI2C		(uint8_t, bool *, uint8_t);
	bool	WriteI2C	(uint16_t, uint8_t, uint8_t, uint8_t);
	bool	WriteE2P	(uint8_t, uint8_t);
	bool	ReadE2P		(uint8_t, uint8_t *);
	bool	ReadSi570	(uint8_t *, uint8_t);
	bool	SetSmoothSi570	(uint16_t *, uint16_t, uint8_t);
	bool	SetSi570	(uint16_t *, uint16_t, uint8_t);

private:
	bool	get_version_internal (int16_t *, int16_t *, int16_t *);
	int16_t	usbFirmwareMajor;
	int16_t	usbFirmwareMinor;
	int16_t	usbFirmwareSub;
	bool	fLcd;
	bool	communications;
	
};

#endif

