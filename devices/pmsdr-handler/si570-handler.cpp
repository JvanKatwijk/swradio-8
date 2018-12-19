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
 *	All rights gratefully acknowledged.
 */

#include	"si570-handler.h"
#include	<stdint.h>
#include	<stdio.h>
#include	<math.h>

//these must be floating point number especially 2^28 so that
//there is enough memory to use them in the calculation
#define POW_2_16         65536.0
#define POW_2_24         16777216.0
#define POW_2_28         268435456.0
#define FOUT_START_UP	 56.320       //MHz 
#define FXTAL_DEVICE	 114.27902015
#define	PPM		 3500

#ifndef I2C_SLA_SI570
#define I2C_SLA_SI570     0x55
#endif
 
static const float FDCO_MIN 	= 4850; //MHz 
static const float FDCO_MAX 	= 5670; //MHz 
static const float fout0 	= FOUT_START_UP;
static const float SI570_FOUT_MIN = 10; //MHz

static const uint8_t HS_DIV [6] = {11, 9, 7, 6, 5, 4};

static inline
uint8_t SetBits (uint8_t value, uint8_t reset_mask, uint8_t new_val) { 
	return ((value & reset_mask ) | new_val); 
}
 
	si570Handler::si570Handler	(pmsdr_comm *pmsdrDevice,
	                                 int16_t usbFirmwareSub,
	                                 int16_t usbFirmwareMajor,
	                                 int16_t usbFirmwareMinor) {
	this	-> pmsdrDevice		= pmsdrDevice;
	this	-> usbFirmwareSub	= usbFirmwareSub;
	this	-> usbFirmwareMajor	= usbFirmwareMajor;
	this	-> usbFirmwareMinor	= usbFirmwareMinor;

        pmsdrDevice -> WriteI2C (I2C_SLA_SI570, 135, 0x01, WriteI2C_PMSDR);
	pmsdrDevice -> ReadSi570 (si570_reg, RD_SI570);
	postDivider = si570_reg [6];
	calc_si570registers (si570_reg, &si570pmsdr);

//	and calculate the fxtal
	si570pmsdr. fxtal =
	    (fout0 * si570pmsdr. n1 * si570pmsdr. hsdiv) /
	             si570pmsdr. rfreq; //MHz
	si570pmsdr. currentFreq	= fout0;

	fprintf (stderr, "Crystal frequency found in this si570 %f\n", 
	                                si570pmsdr. fxtal);

	ComputeandSetSi570 ((double) 14070000 /1000000.0);
//	read back the si570 and set the si570 soft registers
	ReadandConvertRegisters (RD_SI570);
	vfoFreq	= (int32_t) (1000000 *
	                     (si570pmsdr. rfreq	* si570pmsdr. fxtal) /
	                     (si570pmsdr. hsdiv * si570pmsdr. n1));
	vfoFreq	= postDivider == 0 ? vfoFreq : vfoFreq / (4 * postDivider);
}

	si570Handler::~si570Handler	(void) {
}

bool	si570Handler::ReadandConvertRegisters (int8_t choice) {
//	for now
	(void)choice;

	if (!pmsdrDevice -> ReadSi570 (si570_reg, RD_SI570)) 
	   return false;

	postDivider = si570_reg [6];
	calc_si570registers (si570_reg, &si570pmsdr);
	return true;
}
//
void	si570Handler::calc_si570registers (uint8_t* reg,
	                                   si570data* si570struct) {
	// HS_DIV conversion 
	si570struct -> hsdiv =
	               ((reg [0] & 0xE0) >> 5) + 4; // get reg 7 bits 5, 6, 7
	// hsdiv's value could be verified here to ensure that it is one
	// of the valid HS_DIV values from the datasheet. 
	// n1 conversion 
	si570struct -> n1 =
	               ((reg [0] & 0x1F ) << 2) + // get reg 7 bits 0 to 4 
	               ((reg [1] & 0xC0 ) >> 6);  // add with reg 8 bits 7 and 8 

	if (si570struct -> n1 == 0)
	   si570struct -> n1 = 1;
        else
	if ((si570struct -> n1 & 1) != 0) // add one to an odd number
	   si570struct -> n1 += 1;

	si570struct -> frac_bits = (reg[2] & 0xF) * POW_2_24; 
	si570struct -> frac_bits += reg [3] * POW_2_16;
	si570struct -> frac_bits += reg [4] * 256; 
	si570struct -> frac_bits += reg[5]; 
	
	si570struct -> rfreq = si570struct -> frac_bits; 
	si570struct -> rfreq /= POW_2_28;
	si570struct -> rfreq += ((reg [1] & 0x3F) << 4) +
	                                        ((reg [2] & 0xF0) >> 4);
}
//  
//	compute and set the frequency for the regular Si570
//
void	si570Handler::ComputeandSetSi570 (double theFrequency) { 
bool	smooth_tune;
uint16_t reg [8];

	smooth_tune	= false;	// until shown otherwise

	if ((usbFirmwareMajor >= 2 &&
             usbFirmwareMinor >= 1 && usbFirmwareSub >= 4) ||
	    (usbFirmwareMajor == 2 && usbFirmwareMinor >= 2) ) {
	   float diff = abs (theFrequency -  si570pmsdr. currentFreq) /
	                                           si570pmsdr. currentFreq;
	   smooth_tune = diff * 1000000 < PPM;
	}
	   
	reg [6] = 1;   // default set the CY2239X Postdivider = 1

	theFrequency *= 4.0; // QSD Clock = 4 * frx

	// find first Frequency above SI570_FOUT_MIN
	while (theFrequency < SI570_FOUT_MIN) {
	   reg [6] *= 2;       // set the CY2239X Postdivider
	   theFrequency *= 2.0;
	}

	postDivider = reg [6];
	if (smooth_tune) { 
	   setFrequencySmallChange (&si570pmsdr, theFrequency, reg);
//	and update registers in hardware
	   pmsdrDevice -> SetSmoothSi570 (reg, 8, SET_SMOOTHSI570);
	}
	else {
	   setFrequencyLargeChange (&si570pmsdr, theFrequency, reg);
//	and update the hardware registers
	   pmsdrDevice -> SetSi570 (reg, 8, SET_SI570);
	}

	si570pmsdr. currentFreq = theFrequency;
}

//	Find dividers (get the max and min divider range for
//	the HS_DIV and N1 combo)
bool	si570Handler::computeDividers (double theFrequency,
	                               uint8_t *hsdiv, uint8_t *n1) {
int32_t	divider_max;
int32_t	curr_div;
int16_t	counter;
int16_t	curr_hsdiv;
double	curr_n1;
double	n1_tmp;
uint8_t	local_n1;

	divider_max	= floor (FDCO_MAX / theFrequency); // floorf for SDCC 
	curr_div	= ceil (FDCO_MIN / theFrequency);  //ceilf for SDCC 

	while (curr_div <= divider_max) { 
	   // check all HS_DIV values with the next curr_div 
	   for (counter = 0; counter < 6; counter++) { 
// get the next possible n1 value 
	      curr_hsdiv = HS_DIV [counter]; 
	      curr_n1 = double (curr_div) / double (curr_hsdiv); 
	      // determine if curr_n1 is an integer and an even number or one
	      // then curr_div % (curr_n1 * hsdiv) == 0
	      n1_tmp = curr_n1 - floor (curr_n1); 
	      if (n1_tmp == 0.0) { 
	         // then curr_n1 has zero as fraction and equals an integer 
	         local_n1 = (uint8_t) curr_n1; 
	         if ((local_n1 == 1) ||
	             ((local_n1 & 1) == 0))  {
//	then the calculated N1 is either 1 or an even number and we are through
	             *n1 = local_n1;
	             *hsdiv = curr_hsdiv;
	             return true;
	         }
	      }
	   }	// we end the forloop 

	   // increment curr_div to find the next divider 
	   // since the current one was not valid 
	   curr_div = curr_div + 1; 
	} 
	return false;
}

//	easy: computing the parameters for the Si570
//	in case of a small change
//
bool	si570Handler::setFrequencySmallChange (si570data *si570,
	                                      double Frequency,
	                                      uint16_t *reg) {
//	New Rfreq is easy to compute:
	si570 -> rfreq = si570 -> rfreq * Frequency /si570 -> currentFreq;
//
//	convert new RFREQ to the binary representation
//	in the software registerset
//	The algorithm to do so can be found on
//	data sheet.
	memset (reg, 0, 6 * sizeof (uint16_t));
	convertFloattoHex (si570, reg);
//
	return true;
}

bool	si570Handler::setFrequencyLargeChange (si570data *si570,
	                                      double Frequency,
	                                      uint16_t *reg) {
uint8_t	local_hsdiv, local_n1;

	if (!computeDividers (Frequency, &local_hsdiv, &local_n1)) {
	   fprintf (stderr, "FreqProgSi570: error in calculation !!! \n");
	   return false;
	}
//
//	we have a new n1 and hsdiv, so we can compute a new rfreq
	si570 -> rfreq =
	   (Frequency * local_hsdiv * local_n1) / si570 -> fxtal; 
//
//	clear registers
	memset (reg, 0, 6 * sizeof (uint16_t));

//	new HS_DIV conversion 
	local_hsdiv =  local_hsdiv - 4; 
//	reset this memory 
	reg [0] = 0; 

//	set the top 3 bits of reg 13 
	reg [0] = local_hsdiv << 5; 
//	convert new N1 to the binary representation 
	if (local_n1 == 1)
	   local_n1 = 0;
	else
	if ((local_n1 & 1) == 0)
	   local_n1 = local_n1 - 1;
//	set reg 7 bits 0 to 4 
	reg [0] = SetBits (reg [0], 0xE0, local_n1 >> 2); 

//	set reg 8 bits 6 and 7 
	reg [1] = (local_n1 & 3) << 6; 
//	convert new RFREQ to the binary representation 

	convertFloattoHex (si570, reg);
	return true;
}

int32_t	si570Handler::setVFOfrequency	(int32_t f) {
	ComputeandSetSi570 ((double) f / 1000000.0);
	ReadandConvertRegisters (RD_SI570);
	vfoFreq  = (int32_t) (1000000 *
                             (si570pmsdr. rfreq * si570pmsdr. fxtal) /
                             (si570pmsdr. hsdiv * si570pmsdr. n1));

	vfoFreq	= postDivider == 0 ? vfoFreq : vfoFreq / (4 * postDivider);
	return vfoFreq;
}

int32_t	si570Handler::getVFOfrequency (void) {
	return vfoFreq;
}

void	si570Handler::convertFloattoHex (si570data *si570, uint16_t *reg) {
int16_t		counter;
uint32_t	whole;
uint32_t	fraction;

//	first separate fraction and integer part
	whole = floor (si570 -> rfreq);

// get the binary representation of the fractional part
	fraction = floor ((si570 -> rfreq - whole) * POW_2_28);
//
//	12 - 8 =  5 - 1
//	set reg 12 to 10 (i.e. the top 24 bits),
	for (counter = 5; counter >= 3; counter--){
	   reg [counter] = fraction & 0xFF;
	   fraction = fraction >> 8;
	}

// set the last 4 bits of the fractional portion  are set in reg 9
	reg [2] = SetBits (reg [2], 0xF0, (fraction & 0xF));

// set the integer portion of RFREQ across reg 8 and 9
	reg [2] = SetBits (reg [2], 0x0F, (whole & 0xF) << 4);
	reg [1] = SetBits (reg [1], 0xC0, (whole >> 4) & 0x3F);
}

uint8_t	si570Handler::getReg (int16_t i) {
	return si570_reg [i];
}

void	si570Handler::setRegs (uint8_t *v) {
int16_t	i;
	for (i = 0; i <= 6; i ++)
	   si570_reg [i] = v [i];
}

