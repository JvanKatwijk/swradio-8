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
 *	1. mbed SI570 Library, for driving the SI570 programmable VCXO
 *      Copyright (c) 2010, Gerrit Polder, PA3BYA
 *	2. control a PMSDR hardware on Linux
 *	Modified to comply to firmware version 2.1.x
 *	Copyright (C) 2008,2009,2010  Andrea Montefusco IW0HDV,
 *	Martin Pernter IW3AUT
 */

#include	<QLabel>
#include	<QSettings>
#include	<QMessageBox>
#include	"pa-reader.h"
#include	"pmsdr-handler.h"
#include	"si570-handler.h"

#ifndef I2C_SLA_SI570
#define I2C_SLA_SI570     0x55
#endif
 
		pmsdrHandler::pmsdrHandler (RadioInterface *mr,
                                            int32_t        outputRate,
                                            RingBuffer<DSPCOMPLEX> *r,
                                            QSettings      *s):
                                               deviceHandler (mr) {

	this	-> myFrame	= new QFrame (NULL);
	setupUi (this -> myFrame);
	this	-> myFrame -> show ();
	this	-> dataBuffer	= r;
	this	-> pmsdrSettings	= s;
	myReader		= NULL;
	defaultFreq		= -1;	// definitely illegal
	radioOK			= false;
//
//	whether or not there is a valid device, input through the
//	card reader is possible
	inputRate	= 96000;
	myReader	= new paReader (inputRate, cardSelector);
	connect (cardSelector, SIGNAL (activated (int)),
	         this, SLOT (set_Streamselector (int)));
	connect (myReader, SIGNAL (samplesAvailable (int)),
	         this, SLOT (samplesAvailable (int)));

	setup_device ();
	if (pmsdrDevice == NULL) {	// something failed
	   statusLabel	-> setText ("No valid Device");
	   throw (21);
	}
//
//	Initial settings
	qsdBfig. QSDmute		= false;
	pmsdrDevice -> set_qsd_bias (&qsdBfig, 0);
	set_gainValue	(gainSlider	-> value ());
	setBias		(biasSlider	-> value ());

	Mode		= IandQ;
//	and the connects
	pmsdrSettings	-> beginGroup ("pmsdr");
	vfoOffset	= pmsdrSettings -> value ("pmsdr-vfoOffset",
	                                                0). toInt ();
	pmsdrSettings	-> endGroup ();
	offset_KHz	-> setValue (vfoOffset);
	connect (button_f1, SIGNAL (clicked (void)),
	         this, SLOT (set_button_1 (void)));
	connect (button_f2, SIGNAL (clicked (void)),
	         this, SLOT (set_button_2 (void)));
	connect (button_f3, SIGNAL (clicked (void)),
	         this, SLOT (set_button_3 (void)));
	connect (button_f4, SIGNAL (clicked (void)),
	         this, SLOT (set_button_4 (void)));
	connect (button_f5, SIGNAL (clicked (void)),
	         this, SLOT (set_button_5 (void)));
	connect (autoButton, SIGNAL (clicked (void)),
	         this, SLOT (set_autoButton (void)));
	connect (muteButton, SIGNAL (clicked (void)),
	         this, SLOT (setMute (void)));
	connect (gainSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_gainValue (int)));
	connect (biasSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (setBias (int)));
	connect (modeSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (set_modeSelector (const QString &)));
	connect (offset_KHz, SIGNAL (valueChanged (int)),
	         this, SLOT (set_offset_KHz (int)));
	statusLabel	-> setText ("pmsdr ready");
	radioOK		= true;
}

	pmsdrHandler::~pmsdrHandler (void) {
	if (pmsdrDevice != NULL) {
	   store_hwlo (LOfreq);
	   delete pmsdrDevice;
	   delete mySI570;
	}
	readerOwner. lock ();
	if (myReader != NULL)
	   delete myReader;
	myReader	= NULL;
	readerOwner. unlock ();
	delete	myFrame;
	pmsdrSettings	-> beginGroup ("pmsdr");
	pmsdrSettings	-> setValue ("pmsdr-vfoOffset", vfoOffset);
	pmsdrSettings	-> endGroup ();
}

int32_t	pmsdrHandler::getRate		(void) {
	return inputRate;
}

void	pmsdrHandler::setVFOFrequency (quint64 VFO_freq) {
int32_t	realFrequency	= VFO_freq + vfoOffset;

	if (!radioOK)
	   return;
	
	if ((realFrequency < 0) || (realFrequency > Mhz (55)))
	   return;

//	Let the SI570 do the work
	LOfreq	= mySI570 -> setVFOfrequency (realFrequency);

	fprintf (stderr, "LOfreq = %d\n", LOfreq);
	if (automaticFiltering)
	   setFilterAutomatic (realFrequency);
}

quint64	pmsdrHandler::getVFOFrequency (void) {
	LOfreq	= mySI570 -> getVFOfrequency ();
	return LOfreq - vfoOffset;
}

bool	pmsdrHandler::legalFrequency (int32_t Freq) {
	return ((Khz (100) <= Freq) && (Freq <= Mhz (55)));
}

//
bool	pmsdrHandler::restartReader	(void) {
bool b;
	if (!radioOK)
	   return false;
	readerOwner. lock ();
	b = myReader	-> restartReader ();
	readerOwner. unlock ();
	fprintf (stderr, "restart geeft %d\n", b);
	return b;
}

void	pmsdrHandler::stopReader	(void) {
	if (!radioOK)
	   return;
	readerOwner. lock ();
	myReader -> stopReader ();
	readerOwner. unlock ();
}


void	pmsdrHandler::set_Streamselector (int idx) {
	readerOwner. lock ();
	if (!myReader	-> set_StreamSelector (idx))
	   QMessageBox::warning (myFrame, tr ("sdr"),
	                               tr ("Selecting  input stream failed"));
	readerOwner. unlock ();
	restartReader ();
}
///////////////////////////////////////////////////////////////////////
void	pmsdrHandler::setup_device	(void) {
bool	fFound;

	qsdBfig			= qsdBfigDefault;
	fLcd			= false;
	LOfreq			= KHz (14070);
/*
 */
	pmsdrDevice	= new pmsdr_comm ();
	if (!pmsdrDevice -> pmsdr_isWorking ()) {
	   QMessageBox::warning (myFrame, tr ("sdr"),
	                                  tr ("No valid device"));
	   fprintf (stderr, "No valid device found (%s)\n",
	                     pmsdrDevice -> getError ());
	   delete pmsdrDevice;
	   pmsdrDevice	= NULL;
	   return;
	}

	pmsdrDevice	-> get_version (&usbFirmwareMajor,
	                                &usbFirmwareMinor,
	                                &usbFirmwareSub);
//
// Check for Si570 and CY22393 on I2C internal bus.
//
        if ( ((usbFirmwareMajor == 2) && (usbFirmwareMinor >= 2)) ||
	     ((usbFirmwareMajor >= 2) && (usbFirmwareMinor >= 1) &&
	      (usbFirmwareSub >= 7 ))) {
	   fFound = false;
			
	   if (pmsdrDevice -> ScanI2C (0x55, &fFound, ScanI2C_PMSDR))
	      fprintf (stderr, "Si570 %s\n", fFound? "detected": "not found");
           else {
	      fprintf (stderr, "Error searching for Si570 found !\n");
	   }

	   if (!fFound) {
	      fprintf (stderr, "Sorry, without Si570 we cannot do much\n");
	      return;
	   }
//
//	Now for the CY22393
	   fFound = false;
	   if (pmsdrDevice -> ScanI2C (0x69, &fFound, ScanI2C_PMSDR)) 
	      fprintf (stderr, "CY22393 %s\n", fFound? "detected": "not found");
	   else {
	      fprintf (stderr, "Error searching for CY22393 !\n");
	      return;
	   }
	}

	mySI570		= new si570Handler (pmsdrDevice,
	                                    usbFirmwareSub,
	                                    usbFirmwareMajor,
	                                    usbFirmwareMinor);
	automaticFiltering	= false;
	currentFilter	= 0;
	setFilter	(currentFilter);
	read_hwlo (&defaultFreq);
	LOfreq	= mySI570	-> setVFOfrequency (14070000);
}
//
//	Local settings and interactions

void	pmsdrHandler::set_modeSelector	(const QString &s) {
	if (s == "IandQ")
	   Mode = IandQ;
	else
	   Mode = QandI;
}

void	pmsdrHandler::set_button_1	(void) {
	if (automaticFiltering) 
	   automaticFiltering = false;
	setFilter (1);
	autoButton	-> setText ("auto");
}

void	pmsdrHandler::set_button_2	(void) {
	if (automaticFiltering) 
	   automaticFiltering = false;
	setFilter (2);
	autoButton	-> setText ("auto");
}

void	pmsdrHandler::set_button_3	(void) {
	if (automaticFiltering) 
	   automaticFiltering = false;
	setFilter (3);
	autoButton	-> setText ("auto");
}

void	pmsdrHandler::set_button_4	(void) {
	if (automaticFiltering) 
	   automaticFiltering = false;
	setFilter (4);
	autoButton	-> setText ("auto");
}

void	pmsdrHandler::set_button_5	(void) {
	if (automaticFiltering) 
	   automaticFiltering = false;
	setFilter (0);
	autoButton	-> setText ("auto");
}

void	pmsdrHandler::set_autoButton	(void) {
	if (automaticFiltering)
	   autoButton	-> setText ("manual");
	else {
	   autoButton	-> setText ("auto");
	   setFilter (0);
	}
	automaticFiltering = !automaticFiltering;
	if (automaticFiltering)
	   setFilterAutomatic (getVFOFrequency ());
}

void	pmsdrHandler::setFilter	(int16_t filter) {
	if (!radioOK || (filter < 0) || (filter > 5))
	   return;

	qsdBfig. filter	= filter;
	pmsdrDevice -> set_qsd_bias (&qsdBfig, 0);
	displayRange (filter);
}

void	pmsdrHandler::setFilterAutomatic (int32_t freq) {
	if (freq < Khz (2500)) {
	   setFilter  (4);
	}
	else
	if (freq < Khz (6000)) {
	   setFilter (1);
	}
	else
	if (freq < Khz (12000)) { 
	   setFilter (2);
	}
	else
	if (freq < Khz (30000)) {
	   setFilter (3);
	}
	else {
	   setFilter (0);
	}
}

void 	pmsdrHandler::displayRange (int16_t filterno) {
	switch (filterno) {
	   case 4:
	      filterRange	-> setText ("0 .. 2.5");
	      break;
	   case 1:
	      filterRange	-> setText ("2 .. 6");
	      break;
	   case 2:
	      filterRange	-> setText ("5 .. 12");
	      break;
	   case 3:
	      filterRange	-> setText ("11 .. 30");
	      break;
	   default:
	   case 0:
	      filterRange	-> setText ("no filter");
	}
}

void	pmsdrHandler::setMute	(void) {
	if (!radioOK)
	   return;
	qsdBfig. QSDmute	= !qsdBfig. QSDmute;
	pmsdrDevice -> set_qsd_bias (&qsdBfig, 0);
}

void	pmsdrHandler::set_gainValue	(int gain) {
	gainValue	= gain;
	gainDisplay	-> display (gain);
}
/*
 *	parameter is an integer between -100 .. 100
 */
void	pmsdrHandler::setBias		(int bias) {
	if (!radioOK)
	   return;

	qsdBfig. QSDbias	= 512 + bias * 3/2;
	pmsdrDevice -> set_qsd_bias (&qsdBfig, 0);
	biasDisplay	-> display (qsdBfig. QSDbias);
}

void	pmsdrHandler::set_offset_KHz	(int v) {
	vfoOffset	= Khz (v);
}

int16_t	pmsdrHandler::bitDepth		(void) {
	return 16;
}

void	pmsdrHandler::samplesAvailable	(int n) {
std::complex<float> b [512];
int16_t	i;

	(void)n;
	while (myReader -> Samples () > 512) {
	   myReader -> getSamples (b, 512, Mode);
	   for (i = 0; i < 512; i ++)
	      b [i] = cmul (b [i], gainValue / 10.0);
	   dataBuffer -> putDataIntoBuffer (b, 512);
	}

	if (dataBuffer -> GetRingBufferReadAvailable () > 1024)
	   dataAvailable (1024);
}
	   
//	Two functions for writing and reading (back) data
//	into and from the EEPROM.
bool	pmsdrHandler::store_hwlo (int32_t newFreq) {
int16_t	i;
// write the soft SI570 registers into the EEPROM
	for (i = 0; i < 6; i++)
	   if (!pmsdrDevice -> WriteE2P (SI570_REG_ADR+i,
	                                 mySI570 -> getReg (i)))
	      return false;
	if (!pmsdrDevice -> WriteE2P (POST_DIV_ADR,
	                                 mySI570 -> getReg (6)))
	   return false;
	if (!pmsdrDevice -> WriteE2P (DEF_FILTER_ADR, qsdBfig.filter))
	   return false;

//	write the current generated frequency into the EEPROM
//	it is useful only to display the frequency on hardware startup;
//	in this way the PMSDR micro doesn't need to do heavy computation

	if (!pmsdrDevice -> WriteE2P (DEFAULT_FRQ_ADR + 0, (newFreq & 0x000000FF)))
	   return false;
	if (!pmsdrDevice -> WriteE2P (DEFAULT_FRQ_ADR + 1, (newFreq & 0x0000FF00) >> 8))
	   return false;
	if (!pmsdrDevice -> WriteE2P (DEFAULT_FRQ_ADR + 2, (newFreq & 0x00FF0000) >> 16))
	   return false;
	if (!pmsdrDevice -> WriteE2P (DEFAULT_FRQ_ADR + 3, (newFreq & 0xFF000000) >> 24))
	   return false;

	return true;
}

bool	pmsdrHandler::read_hwlo (int32_t *pNewFreq) {
uint8_t	x;
int16_t	i;
int32_t	tmp	= 0;
uint8_t si570_reg [8];

	*pNewFreq = 0;
	// read the SI570 registers from EEPROM
	for (i = 0; i < 6; i++) 
	   if (!pmsdrDevice -> ReadE2P (SI570_REG_ADR + i, &si570_reg [i]))
	      return false;
	if (!pmsdrDevice -> ReadE2P (POST_DIV_ADR, &si570_reg [6]))
	   return false;
	mySI570	-> setRegs (si570_reg);
//	if (!WriteE2P (DEF_FILTER_ADR, qsdBfig. filter))
//	   return false;
	if (!pmsdrDevice -> ReadE2P (DEF_FILTER_ADR, &qsdBfig. filter))
	   return false;

	// read the current generated frequency into the EEPROM
	if (!pmsdrDevice -> ReadE2P (DEFAULT_FRQ_ADR + 3, &x))
	   return false;
	tmp = tmp + x ; 
	if (!pmsdrDevice -> ReadE2P (DEFAULT_FRQ_ADR + 2, &x))
	   return false;
	tmp = (tmp << 8) + x ; 
	if (!pmsdrDevice -> ReadE2P (DEFAULT_FRQ_ADR + 1, &x))
	   return false;
	tmp =  (tmp << 8) + x ; 
	if (!pmsdrDevice -> ReadE2P (DEFAULT_FRQ_ADR + 0, &x))
	   return false;
	tmp =  (tmp << 8) + x ; 
	*pNewFreq = tmp;

	return true;
}
