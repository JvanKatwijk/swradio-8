
#include	"Rsp1A_handler.h"

	Rsp1A_handler::Rsp1AHandler (sdrplay_api_DeviceT *chosenDevice,
	                             int sampleRate,
	                             quint64_t freq,
	                             bool agcMode,
	                             int lnaState, int GRdB) {
	this	-> Device	= chosenDevice -> dev;
	this	-> tuner	= chosenDevice -> tuner;
	this	-> chParams	= chosenDevice -> rxChannelA;
	sdrplay_api_GetDeviceParams (chosenDevice, &deviceParams);
//
//	Initialize
	deviceParams	-> devParams -> fsFreq. fsHz	= 2112000.0;
	chParams	-> tunerParams. bwType = sdrplay_api_BW_0_200;
	chParams	-> tunerParams. ifType = sdrplay_api_IF_Zero;
//
//	these will change:
	chParams	-> tunerParams. rfFreq. rfHz    = 14070000.0;
	chParams	-> tunerParams. gain.gRdB	= GRdB;
	chParams	-> tunerParams. gain.LNAstate	= 3;
//	chParams	-> tunerParams. gain.LNAstate	= lnaState;
	chParams	-> ctrlParams.agc.enable = sdrplay_api_AGC_DISABLE;
	if (agcMode) {
	   chParams    -> ctrlParams. agc. setPoint_dBfs = -30;
	   chParams    -> ctrlParams. agc. enable =
                                             sdrplay_api_AGC_100HZ;
	}
	else
	   chParams    -> ctrlParams. agc. enable =
                                             sdrplay_api_AGC_DISABLE;
}

static
int	Rsp1A_Table [6][15] = {
	{14, 0, 3,  6,  9, 12, 15, 24, 27, 30, 33, 36, 39, 42, 45},
	{14, 0, 3,  6,  9, 12, 15, 18, 24, 27, 30, 33, 36, 39, 42},
	{14, 0, 3,  6,  9, 12, 15, 24, 27, 30, 33, 36, 39, 42, 45},
	{14, 0, 3,  6,  8, 12, 15, 18, 24, 27, 30, 33, 36, 39, 42},
	{14, 0, 7, 10, 13, 16, 19, 22, 25, 31, 34, 37, 40, 43, 46},
	{14, 0, 5,  8, 11, 14, 17, 20, 32, 35, 38, 41, 44, 47, 50}};


int16_t Rsp1A_handler::bankFor_rspdx (int freq) {
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

int	Rsp1A_handler::lnaStates (quint64_t frequency) {
int band	= bankfor_rsdpx (frequency)'
	return RSPdx_Table [band][0];
}

bool	Rsp1A_handler::restart (quint64_t freq) {
sdrplay_api_ErrT        err;

	chParams -> tunerParams. rfFreq. rfHz = (float)freq;
	err = sdrplay_api_Update (Device, tuner,
                                  sdrplay_api_Update_Tuner_Frf,
                                  sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "restart: error %s\n",
	                          sdrplay_api_GetErrorString (err));
	   return false;
	}
	return true;
}

void	Rsp1A_handler::stop	() {
sdrplay_api_ErrT        err;
}

bool	Rsp1A_handler::set_agc	(int setPoint, bool on) {
sdrplay_api_ErrT        err;

	if (on) {
	   chParams    -> ctrlParams. agc. setPoint_dBfs = - p -> setPoint;
	   chParams    -> ctrlParams. agc. enable = sdrplay_api_AGC_100HZ;
	}
	else
	   chParams    -> ctrlParams. agc. enable =
                                             sdrplay_api_AGC_DISABLE;

	err = sdrplay_api_Update (Device, tuner,
                                  sdrplay_api_Update_Ctrl_Agc,
                                  sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "agc: error %s\n",
	                              sdrplay_api_GetErrorString (err));
	   return false;
	}

	return true;
}

bool	Rsp1A_handler::set_GRdB	(int GRdBValue) {
sdrplay_api_ErrT        err;

	chParams -> tunerParams. gain. gRdB = GRdBValue;
	err = sdrplay_api_Update (Device, tuner,
	                          sdrplay_api_Update_Tuner_Gr,
	                          sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "grdb: error %s\n",
                                      sdrplay_api_GetErrorString (err));
	   return false;
	}

	return true;
}

bool	Rsp1A_handler::set_ppm	(int ppm) {
sdrplay_api_ErrT        err;

	deviceParams    -> devParams -> ppm = p -> ppmValue;
	err = sdrplay_api_Update (Device, tuner,
	                          sdrplay_api_Update_Dev_Ppm,
	                          sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "lna: error %s\n",
	                              sdrplay_api_GetErrorString (err));
	   return false;
	}

	return true;
}

bool	Rsp1A_handler::set_lna	(int lnaState) {
sdrplay_api_ErrT        err;

	chParams -> tunerParams. gain. LNAstate =lnaState;
	err = sdrplay_api_Update (Device, tuner,
	                          sdrplay_api_Update_Tuner_Gr,
	                          sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "grdb: error %s\n",
	                            drplay_api_GetErrorString (err));
	   return false;
	}
	return true;
}

bool	RSp1A_handler::set_antenna (int antenna) {
	return false;
}

bool	Rsp1A_handler::set_amPort (int amPort) {
	return false;
}


