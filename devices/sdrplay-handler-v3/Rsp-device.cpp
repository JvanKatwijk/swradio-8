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

#include	"Rsp-device.h"

#include	<stdint.h>
#include	<sdrplay_api.h>
#include	"sdrplay-handler-v3.h"

	Rsp_device::Rsp_device 	(sdrplayHandler_v3 *parent,
	                         sdrplay_api_DeviceT *chosenDevice,
	                         int sampleRate,
	                         int startFreq,
	                         bool agcMode,
	                         int lnaState,
	                         int GRdB, bool biasT) {
sdrplay_api_ErrT        err;
	this	-> parent	= parent;
	this	-> chosenDevice	=  chosenDevice;
	this	-> sampleRate	= sampleRate;
	this	-> freq		= startFreq;
	this	-> agcMode	= agcMode;
	this	-> lnaState	= lnaState;
	this	-> GRdBValue	= GRdB;
	this	-> biasT	= biasT;

	connect (this, SIGNAL (set_lnabounds_signal (int, int)),
                 parent, SLOT (set_lnabounds (int, int)));
        connect (this, SIGNAL (set_deviceName_signal (const QString &)),
                 parent, SLOT (set_deviceName (const QString &)));
        connect (this, SIGNAL (set_nrBits_signal (int)),
	         parent, SLOT (set_nrBits (int)));
	connect (this, SIGNAL (show_lnaGain (int)),
	         parent, SLOT (show_lnaGain (int)));


	err = parent -> sdrplay_api_GetDeviceParams (chosenDevice -> dev,
	                                             &deviceParams);

	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "sdrplay_api_GetDeviceParams failed %s\n",
	                         parent -> sdrplay_api_GetErrorString (err));
	   throw (21);
	}

	if (deviceParams == nullptr) {
	   fprintf (stderr, "sdrplay_api_GetDeviceParams return null as par\n");
	   throw (22);
	}

	this	-> chParams	= deviceParams -> rxChannelA;

	deviceParams    -> devParams -> fsFreq. fsHz    = sampleRate;
        chParams        -> tunerParams. bwType = sdrplay_api_BW_0_200;
        chParams        -> tunerParams. ifType = sdrplay_api_IF_Zero;
//
//      these will change:
        chParams        -> tunerParams. rfFreq. rfHz    = (float)startFreq;
        chParams        -> tunerParams. gain.gRdB       = GRdB;
	chParams        -> tunerParams. gain.LNAstate   = lnaState;
	if (this -> agcMode) {
           chParams    -> ctrlParams. agc. setPoint_dBfs = -30;
           chParams    -> ctrlParams. agc. enable =
                                             sdrplay_api_AGC_100HZ;
        }
        else
           chParams    -> ctrlParams. agc. enable =
                                             sdrplay_api_AGC_DISABLE;

	err	= parent -> sdrplay_api_Init (chosenDevice -> dev,
	                                      &parent -> cbFns, parent);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "sdrplay_api_Init failed %s\n",
                                    parent -> sdrplay_api_GetErrorString (err));
	   throw (23);
	}
}

	Rsp_device::~Rsp_device	() {}
int	Rsp_device::lnaStates	(int frequency) {
	(void)frequency;
	return 0;
}

bool	Rsp_device::restart	(int freq) {
	(void)freq;
	return false;
}

bool	Rsp_device::set_agc	(int setPoint, bool on) {
	(void)setPoint;
	(void)on;
	return false;
}

bool	Rsp_device::set_lna	(int lnaState) {
	(void)lnaState;
	return false;
}

bool	Rsp_device::set_GRdB	(int GRdBValue) {
	(void)GRdBValue;
	return false;
}

bool	Rsp_device::set_ppm	(int ppm) {
	(void)ppm;
	return false;
}

bool	Rsp_device::set_antenna	(int antenna) {
	(void)antenna;
	return false;
}

bool	Rsp_device::set_amPort 	(int amPort) {
	(void)amPort;
	return false;
}

bool	Rsp_device::set_biasT (bool  b) {
	(void)b;
	return false;
}

