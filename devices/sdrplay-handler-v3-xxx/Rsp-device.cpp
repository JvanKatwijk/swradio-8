
#include	"Rsp-device.h"

#include	<stdint.h>
#include	<sdrplay_api.h>
#include	"sdrplay-handler-v3.h"

	Rsp_device::Rsp_device 	(sdrplayHandler_v3 *parent,
	                         sdrplay_api_DeviceT *chosenDevice,
	                         int sampleRate,
	                         int startFreq,
	                         bool agcMode,
	                         int lnaState, int GRdB) {
	this	-> parent	= parent;
	this	-> chosenDevice	=  chosenDevice;
	this	-> sampleRate	= sampleRate;
	this	-> startFreq	= startFreq;
	this	-> agcMode	= agcMode;
	this	-> lnaState	= lnaState;
	this	-> GRdB		= GRdB;
	parent -> sdrplay_api_GetDeviceParams (chosenDevice -> dev,
	                                       &deviceParams);
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

void	Rsp_device::stop	() {}

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


