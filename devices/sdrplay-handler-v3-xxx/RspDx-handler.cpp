
#include	"RspDx-handler.h"
#include	"sdrplay-handler-v3.h"

	RspDx_handler::RspDx_handler (sdrplayHandler_v3 *parent,
	                              sdrplay_api_DeviceT *chosenDevice,
	                              int	sampleRate,
	                              int	freq,
	                              bool	agcMode,
	                              int	lnaState,
	                              int 	GRdB) :
	                              Rsp_device (parent,
	                                          chosenDevice, 
	                                         sampleRate,
	                                         freq,
	                                         agcMode,
	                                         lnaState,
	                                         GRdB) {

	this	-> lna_upperBound	= 26;
	this	-> deviceModel		= "RSP-Dx";
	this	-> nrBits		= 14;
	this	-> antennaSelect	= true;
	set_lnabounds_signal	(0, lna_upperBound);
	set_deviceName_signal	(deviceModel);
	set_antennaSelect_signal (antennaSelect);
	set_nrBits	(nrBits);
}

	RspDx_handler::~RspDx_handler	() {}

static
int	RSPdx_Table [6][15] = {
	{14, 0, 3,  6,  9, 12, 15, 24, 27, 30, 33, 36, 39, 42, 45},
	{14, 0, 3,  6,  9, 12, 15, 18, 24, 27, 30, 33, 36, 39, 42},
	{14, 0, 3,  6,  9, 12, 15, 24, 27, 30, 33, 36, 39, 42, 45},
	{14, 0, 3,  6,  8, 12, 15, 18, 24, 27, 30, 33, 36, 39, 42},
	{14, 0, 7, 10, 13, 16, 19, 22, 25, 31, 34, 37, 40, 43, 46},
	{14, 0, 5,  8, 11, 14, 17, 20, 32, 35, 38, 41, 44, 47, 50}};


int16_t RspDx_handler::bankFor_rspdx (int freq) {
	if (freq < MHz (12))
	   return 0;
	if (freq < MHz (60))
	   return 1;
	if (freq < MHz (250))
	   return 2;
	if (freq < MHz (420))
	   return 3;
	if (freq < MHz (1000))
	   return 4;
	return 5;
}

int	RspDx_handler::lnaStates (int frequency) {
int band	= bankFor_rspdx (frequency);
	return RSPdx_Table [band][0];
}

bool	RspDx_handler::restart (int freq) {
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
	return true;
}

void	RspDx_handler::stop	() {
sdrplay_api_ErrT        err;
}

bool	RspDx_handler::set_agc	(int setPoint, bool on) {
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

	return true;
}

bool	RspDx_handler::set_GRdB	(int GRdBValue) {
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
	return true;
}

bool	RspDx_handler::set_ppm	(int ppmValue) {
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

bool	RspDx_handler::set_lna	(int lnaState) {
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
	return true;
}

bool	RspDx_handler::set_antenna (int antenna) {
sdrplay_api_ErrT        err;

	deviceParams    -> devParams -> rspDxParams. antennaSel =
	                           antenna == 'A' ?
                                             sdrplay_api_RspDx_ANTENNA_A:
	                           antenna == 'B' ?
                                             sdrplay_api_RspDx_ANTENNA_B :
	                                        sdrplay_api_RspDx_ANTENNA_C;
	    
	err = parent ->  sdrplay_api_Update (chosenDevice -> dev, 
	                                     chosenDevice -> tuner,
	                                     sdrplay_api_Update_None,
	                                     sdrplay_api_Update_RspDx_AntennaControl);
	if (err != sdrplay_api_Success)
	   return false;

	return true;
}

bool	RspDx_handler::set_amPort (int amPort) {
sdrplay_api_ErrT        err;

	deviceParams    -> devParams -> rspDxParams. hdrEnable = amPort;
	err = parent -> sdrplay_api_Update (chosenDevice -> dev,
	                                    chosenDevice -> tuner,
	                                    sdrplay_api_Update_None,
	                                    sdrplay_api_Update_RspDx_HdrEnable);
	if (err != sdrplay_api_Success)
	   return false;

	return true;
}


