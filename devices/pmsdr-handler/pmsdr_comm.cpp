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
 * 	This rewrite is based on the functionality as
 *	implicitly described in the code of the original PMSDR.
 * 	All copyrights are gratefully acknowledged.
 *	Library for
 *	MSDR command line manager (experimental)
 *	control a PMSDR hardware on Linux
 *	Modified to comply to firmware version 2.1.x
 *
 *	Copyright (C) 2008,2009,2010  Andrea Montefusco IW0HDV,
 *	Martin Pernter IW3AUT
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include	<stdio.h>
#include	<string.h>
#include	<stdbool.h>
#include	<assert.h>
#include	<stdlib.h>
#include	<math.h>
#include	"pmsdr_comm.h"

//
//	Second layer, creating the object and establishing
//	a communication interface to the PMSDR hardware
//	implemented as an extension of the pmsdr_usb layer
//
		pmsdr_comm::pmsdr_comm (void):pmsdr_usb () {
bool	r;
int16_t	i;

	communications	= false;
//
//	The usb line is opened by the pmsdr_usb class
	if (!this -> statusDevice ()) {
	   fprintf (stderr, "No valid device found (%s)\n",
	                     this -> getError ());
	   return;
	}

	if (!this -> open ()) {
	   fprintf (stderr, "Could not open device (%s)\n",
	                     this -> getError ());
	   return;
	}
/*
 * 	Now we seem to have a usb gate, try a couple of times
 *	to get data
 */
	r = false;
	for (i = 0; i < 3 && !r; ++i) {
	   usbFirmwareSub = 255;
	   r = get_version_internal (&usbFirmwareMajor,
	                             &usbFirmwareMinor, 
	                             &usbFirmwareSub);
        }
//
//	modified 2703
	if (!r) {		// some error
	   fprintf (stderr, "No valid firmware or device (%s)\n",
	                     this -> getError ());
	   return;
	}
	   
	fprintf (stderr,
	         "Firmware reports version %d.%d and %d\n",
	          usbFirmwareMajor, usbFirmwareMinor, usbFirmwareSub);
//
//	Check for Lcd screen
	if (usbFirmwareSub != 255) {
	   const char *pszLcdPrompt = "* PM SDR Jan *******";
                             //   012345678901234567890
	   if ((usbFirmwareMajor >= 2 && usbFirmwareMinor >= 2) ||
               (usbFirmwareMajor >= 2 && usbFirmwareMinor >= 0 && 
	        usbFirmwareSub >= 2 ) ) {
	      fLcd = true;
	      fprintf (stderr, " lcd available .%d\n", usbFirmwareSub);
	      lcdPuts (pszLcdPrompt, strlen(pszLcdPrompt), 0, 1); // pos  row
            }
	} else 
	   fprintf (stderr, "\n");
//
//	it seems we can communicate with the pmsdr
	communications	= true;
}

	pmsdr_comm::~pmsdr_comm	(void) {
//	we did not allocate anything, so ...
}

bool	pmsdr_comm::pmsdr_isWorking (void) {
	return communications;
}

void	pmsdr_comm::set_qsd_bias (QsdBiasFilterIfGain *pQsd, uint8_t *ref_src) {
uint8_t txBuf [6];
uint8_t rxBuf [1];
bool	rc;
int16_t rxLen = sizeof rxBuf;

	if (!communications)
	   return;

// mark the buffers and set header
	memset (rxBuf, 0x55, sizeof rxBuf);
	memset (txBuf, 0x55, sizeof txBuf);
	txBuf[0] = SET_QSD_BIAS;
	txBuf[1] = pQsd -> QSDbias & 0xFF;
	txBuf[2] = (pQsd -> QSDbias & 0xFF00) >> 8;

	if (ref_src)
	   txBuf[3] = pQsd -> QSDmute + (*ref_src) * 2 ;
	else
	   txBuf[3] = pQsd -> QSDmute;

	txBuf[4] = pQsd -> IfGain;
	txBuf[5] = pQsd -> filter;

	rc =  doTransaction (txBuf, sizeof txBuf, rxBuf, &rxLen);
	if (rc && rxLen == 1 && rxBuf[0] == SET_QSD_BIAS) 
	   return;

	fprintf (stderr,
	         "Unable to set QSD bias, filtering and IF gain into PMSDR.");
	if (!rc)
	   fprintf (stderr, "usbDevice reports %s\n",
	                     this -> getError ());
}

void	pmsdr_comm::get_version (int16_t *pMajor,
	                         int16_t *pMinor, int16_t *pSub) {
	if (!communications)
	   return;
        *pMajor	= usbFirmwareMajor;
        *pMinor	= usbFirmwareMinor;
        *pSub	= usbFirmwareSub; 
}

void	pmsdr_comm::lcdPuts (const char *Data,
	                     uint16_t Length, uint16_t pos, uint16_t row) {
uint8_t txBuf [64];
uint8_t rxBuf [1];
bool	rc;
int16_t	i;
int16_t rxLen = sizeof rxBuf;

	if (!fLcd || !communications)
	   return;

// mark the buffers and set header
	memset (txBuf, 0x55, sizeof txBuf);
	txBuf[0] = LCD_PUTS;
	txBuf[1] = Length;
	txBuf[2] = row;
	txBuf[3] = pos;
	for (i = 0; i < Length; i ++)
	   txBuf [4 + i] = Data [i] & 0xFF;

// firmware 2.1 or more
	if (usbFirmwareMajor >= 2 && usbFirmwareMinor > 0) 
	   rc = doTransaction (txBuf, Length + 4, 0, 0);
	else
// firmware 2.0
	   rc = doTransaction (txBuf, Length + 4, rxBuf, &rxLen);

	if (!rc) 
	   fprintf (stderr, "Unable to send LCD string to LCD screen (%s).",
	                     this -> getError ());
	return;
}

bool	pmsdr_comm::get_version_internal (int16_t *pMajor,
	                                  int16_t *pMinor, int16_t *pSub) {
uint8_t txBuf [2];
uint8_t rxBuf [5];
bool rc;
int16_t rxLen = sizeof rxBuf;

//
//	2703: removed the erroneous
//	if (!communications)
//	   return false;
	memset (rxBuf, 0x55, sizeof rxBuf);
	txBuf [0] = READ_VERSION;
	txBuf [1] = 0x02;

	rc =  doTransaction (txBuf, sizeof txBuf, rxBuf, &rxLen);
	if (!rc) {
	   fprintf (stderr, "get_version_internal: %s\n",
	                     this -> getError ());
	   return false;
	}

	if (pMajor != NULL)
	   *pMajor = (uint16_t) rxBuf[3] ;
	if (pMinor != NULL)
	   *pMinor = (uint16_t) rxBuf[2] ;

	if (rxLen == 5 &&
	    rxBuf[0] == READ_VERSION && rxBuf[1] == 0x03) {
	   if (pSub != NULL)
	      *pSub = (uint16_t) rxBuf[4] ;
	}
	return true;
}

bool	pmsdr_comm::ScanI2C (uint8_t slave, bool *pfFound, uint8_t i2c) {
uint8_t txBuf [2];
uint8_t rxBuf [3];
int16_t rxLen = sizeof rxBuf;
bool rc;

//	first assume device not found
	*pfFound = false;
// 	set header
//	txBuf [0] = ScanI2C_PMSDR; of
//	txBuf [0] = ScanSOFTI2C_PMSDR
	txBuf [0] = i2c;
	txBuf [1] = slave;          
	rc =  doTransaction (txBuf, sizeof txBuf, rxBuf, &rxLen);

	if (rc && rxLen >= 2 && rxBuf[0] == i2c) {
	   *pfFound = (rxBuf [2] == 0);
	   return true;
	}

	fprintf (stderr, "Unable to do I2C (soft) scan into PMSDR.");
	if (!rc)
	   fprintf (stderr, "usbDevice reports %s\n",
	                     this -> getError ());
	return false;
}

bool	pmsdr_comm::WriteI2C (uint16_t slave,
	                 uint8_t reg, uint8_t data, uint8_t i2c) {
uint8_t	txBuf [4];
uint8_t	rxBuf [1];
bool	rc;
int16_t rxLen = sizeof rxBuf;

//	set header and data byte
//	txBuf [0] = WriteI2C_PMSDR; 
//	txBuf [0] = WriteI2CSOFT_PMSDR; 
	txBuf [0] = i2c;
	txBuf [1] = slave;          
	txBuf [2] = reg;            
	txBuf [3] = data;           
	rc =  doTransaction (txBuf, sizeof txBuf, rxBuf, &rxLen);

	if (rc && rxLen == 1 && rxBuf[0] == WriteI2C_PMSDR) 
	   return true;

	fprintf (stderr, "Unable to do I2C (soft)write into PMSDR.");
	if (!rc)
	   fprintf (stderr, "usbDevice reports %s\n",
	                     this -> getError ());
	return false;
}
//
//	write the eeprom register reg
bool	pmsdr_comm::WriteE2P (uint8_t reg, uint8_t data) {
uint8_t txBuf [3];
uint8_t rxBuf [1];
bool	rc;
int16_t rxLen = sizeof rxBuf;

	if (!communications)
	   return false;

//	set header and databyte
	txBuf[0] = WriteE2P_PMSDR;
	txBuf[1] = reg;
	txBuf[2] = data;
	rc = doTransaction (txBuf, sizeof txBuf, rxBuf, &rxLen);

	if (rc && rxLen == 1 && rxBuf[0] == WriteE2P_PMSDR ) 
	   return true;

	fprintf (stderr, "Unable to write into PMSDR's EEPROM");
	if (!rc)
	   fprintf (stderr, "usbDevice reports %s\n",
	                     this -> getError ());
	return false;
}
//
//	read from the EEPROM
bool	pmsdr_comm::ReadE2P (uint8_t reg, uint8_t *data) {
uint8_t txBuf [2];
uint8_t rxBuf [3];
bool	rc;
int16_t rxLen = sizeof rxBuf;

	if (!communications)
	   return false;

// 	set header
	txBuf[0] = ReadE2P_PMSDR;
  	txBuf[1] = reg;
	rc = doTransaction (txBuf, sizeof txBuf, rxBuf, &rxLen);

	if (rc && rxLen == 3 &&
	        rxBuf[0] == ReadE2P_PMSDR && rxBuf[1] == reg) {
	   *data = rxBuf[2];
	   return true;
	}

	fprintf (stderr, "Unable to do read EEPROM from PMSDR.");
	if (!rc)
	   fprintf (stderr, "usbDevice reports %s\n", 
	                     this -> getError ());
	return false;
}

//	read the registers (7 .. 12) from the Si570 of choice
bool	pmsdr_comm::ReadSi570 (uint8_t *Data, uint8_t Choice) {
uint8_t	txBuf [1];
uint8_t	rxBuf [10];
bool	rc;
int16_t rxLen = sizeof rxBuf;

	if (!communications)
	   return false;

	// mark the buffer and set header
	memset (rxBuf, 0x55, sizeof rxBuf);
	txBuf [0] = Choice;
	rc = doTransaction (txBuf, sizeof txBuf, rxBuf, &rxLen);

	if (rc && rxLen > 2 && rxBuf[0] == Choice) {
	   memcpy (Data, &(rxBuf[2]), rxBuf[1]);
	   return true;
	}

	fprintf (stderr, "Unable to read from Si570\n");
	if (!rc)
	   fprintf (stderr, "usbDevice reports %s\n",
	                     this -> getError ());
	return false;
}
//
//	set smooth change in frequency on the Si570 of choice
bool	pmsdr_comm::SetSmoothSi570 (uint16_t *Data,
	                            uint16_t Length, uint8_t Choice) {
uint8_t txBuf [64];
uint8_t rxBuf [1];
bool	rc;
int16_t rxLen = sizeof rxBuf;
int16_t	i;

	if (!communications)
	   return false;

// mark the buffers, set header and fill data buffer
	memset (rxBuf, 0x55, sizeof rxBuf);
	memset (txBuf, 0x55, sizeof txBuf);
	txBuf [0] = Choice;
	txBuf [1] = Length;
	for (i = 0; i < Length; i ++)
	   txBuf [2 + i] = Data [i];

	rc = doTransaction (txBuf, Length + 2, rxBuf, &rxLen);

	if (rc && rxLen == 1 && rxBuf[0] == Choice) 
	   return true;

	fprintf (stderr, "Unable to set smooth frequency into Si570\n");
	if (!rc)
	   fprintf (stderr, "usbDevice reports %s",
	                     this -> getError ());
	return false;
}
//
//	set the full registers (7 .. 12) for the Si570 of choice
bool	pmsdr_comm::SetSi570 (uint16_t *Data, uint16_t Length, uint8_t Choice) {
uint8_t txBuf [64];
uint8_t rxBuf [1];
bool	rc;
int16_t rxLen = sizeof rxBuf;
int16_t	i;

// mark the buffers, set headers and fill data buffer
	memset (rxBuf, 0x55, sizeof rxBuf);
	memset (txBuf, 0x55, sizeof txBuf);
	txBuf [0] = Choice;
	txBuf [1] = Length;
	for (i = 0; i < Length; i ++)
	   txBuf [2 + i] = Data [i];

	rc = doTransaction (txBuf, Length + 2, rxBuf, &rxLen);
	if (rc && rxLen == 1 && rxBuf[0] == Choice)
	   return true;

	fprintf (stderr, "Unable to set frequency into Si570.");
	if (!rc)
	   fprintf (stderr, "usbDevice reports %s\n",
	                     this -> getError ());
	return false;
}

