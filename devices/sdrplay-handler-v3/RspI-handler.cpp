#
/*
 *    Copyright (C) 2016 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the sw radio
 *
 *    sw radio is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    sw radio is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with sw radio; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	"RspI-handler.h"
#include	"sdrplay-handler-v3.h"

	Rsp1_handler::Rsp1_handler   (sdrplayHandler_v3 *parent,
	                              sdrplay_api_DeviceT *chosenDevice,
	                              int	sampleRate,
	                              int	freq,
	                              bool	agcMode,
	                              int	lnaState,
	                              int 	GRdB,
	                              bool	biasT) :
	                              Rsp_device (parent,
	                                          chosenDevice, 
	                                         sampleRate,
	                                         freq,
	                                         agcMode,
	                                         lnaState,
	                                         GRdB, biasT) {

	this	-> lna_upperBound	= 3;
	this	-> deviceModel		= "RSP-1";
	this	-> nrBits		= 12;
	set_lnabounds_signal	(0, lna_upperBound);
	set_deviceName_signal	(deviceModel);
	set_nrBits_signal	(nrBits);
	show_lnaGain (get_lnaGain (lnaState, freq));

	if (biasT)
	   set_biasT (true);
}

	Rsp1_handler::~Rsp1_handler	() {}

static
int     RSPI_Table [3] [5] = {
	{4,  0, 24, 19, 43},
	{4,  0,  7, 19, 26},
	{4,  0,  5, 19, 24}};

int16_t Rsp1_handler::bankFor_rsp1 (int freq) {
	if (freq < MHz (60))
	   return 0;
	if (freq < MHz (420))
	   return 1;
	if (freq < MHz (1000))
	   return 2;
	return 3;
}

int	Rsp1_handler::lnaStates (int frequency) {
int band	= bankFor_rsp1 (frequency);
	return RSPI_Table [band][0];
}

int	Rsp1_handler::get_lnaGain (int lnaState, int freq) {
int	band	= bankFor_rsp1 (freq);
	return RSPI_Table [band][lnaState + 1];
}

bool	Rsp1_handler::restart (int freq) {
sdrplay_api_ErrT        err;

	chParams -> tunerParams. rfFreq. rfHz = (float)freq;
	err =parent ->  sdrplay_api_Update (chosenDevice -> dev,
	                                    chosenDevice -> tuner,
                                            sdrplay_api_Update_Tuner_Frf,
                                            sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "restart: error %s\n",
	                         parent -> sdrplay_api_GetErrorString (err));
	   return false;
	}

	this -> freq	= freq;
	set_lnabounds_signal	(0, lna_upperBound);
	show_lnaGain (get_lnaGain (lnaState, freq));
	return true;
}

bool	Rsp1_handler::set_agc	(int setPoint, bool on) {
sdrplay_api_ErrT        err;

	if (on) {
	   chParams    -> ctrlParams. agc. setPoint_dBfs = - setPoint;
	   chParams    -> ctrlParams. agc. enable = sdrplay_api_AGC_100HZ;
	}
	else
	   chParams    -> ctrlParams. agc. enable =
                                             sdrplay_api_AGC_DISABLE;

	err = parent ->  sdrplay_api_Update (chosenDevice -> dev,
	                                     chosenDevice -> tuner,
                                             sdrplay_api_Update_Ctrl_Agc,
                                             sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "agc: error %s\n",
	                          parent -> sdrplay_api_GetErrorString (err));
	   return false;
	}

	this	-> agcMode = on;
	return true;
}

bool	Rsp1_handler::set_GRdB	(int GRdBValue) {
sdrplay_api_ErrT        err;

	chParams -> tunerParams. gain. gRdB = GRdBValue;
	err = parent ->  sdrplay_api_Update (chosenDevice -> dev,
	                                     chosenDevice -> tuner,
	                                     sdrplay_api_Update_Tuner_Gr,
	                                     sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "grdb: error %s\n",
                                   parent -> sdrplay_api_GetErrorString (err));
	   return false;
	}
	this	-> GRdBValue = GRdBValue;
	return true;
}

bool	Rsp1_handler::set_ppm	(int ppmValue) {
sdrplay_api_ErrT        err;

	deviceParams    -> devParams -> ppm = ppmValue;
	err = parent -> sdrplay_api_Update (chosenDevice -> dev,
	                                    chosenDevice -> tuner,
	                                    sdrplay_api_Update_Dev_Ppm,
	                                    sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "lna: error %s\n",
	                          parent -> sdrplay_api_GetErrorString (err));
	   return false;
	}
	return true;
}

bool	Rsp1_handler::set_lna	(int lnaState) {
sdrplay_api_ErrT        err;

	chParams -> tunerParams. gain. LNAstate = lnaState;
	err = parent -> sdrplay_api_Update (chosenDevice -> dev,
	                                    chosenDevice -> tuner,
	                                    sdrplay_api_Update_Tuner_Gr,
	                                    sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "grdb: error %s\n",
	                         parent -> sdrplay_api_GetErrorString (err));
	   return false;
	}
	this	-> lnaState	= lnaState;
	show_lnaGain (get_lnaGain (lnaState, freq));
	return true;
}

bool	Rsp1_handler::set_biasT	(bool biasT_value) {
	(void)biasT_value;
	return true;
}

