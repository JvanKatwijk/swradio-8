#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SW software
 *
 *    SW software is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    SW software is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with SW software; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __SDRPLAY_HANDLER_V3__
#define	__SDRPLAY_HANDLER_V3__

#include	<QObject>
#include	<QFrame>
#include	<QSettings>
#include	<QSemaphore>
#include	<atomic>
#include	<stdio.h>
#include	<queue>
#include	"radio-constants.h"
#include	"ringbuffer.h"
#include	"fir-filters.h"
#include	"device-handler.h"
#include	"ui_sdrplay-widget-v3.h"
#include	<sdrplay_api.h>

class	generalCommand;

#ifdef __MINGW32__
#define GETPROCADDRESS  GetProcAddress
#else
#define GETPROCADDRESS  dlsym
#endif

class	sdrplayHandler_v3: public deviceHandler, public Ui_sdrplayWidget_v3 {
Q_OBJECT
public:
			sdrplayHandler_v3	(RadioInterface *mr,
	                                         int32_t	outputRate,
	                                         RingBuffer<std::complex<float>> *,
	                                         QSettings *);
			~sdrplayHandler_v3	();
	int32_t		getRate		();
	void		setVFOFrequency	(quint64);
	quint64		getVFOFrequency	();

	bool		restartReader		();
	void		stopReader		();
	void		resetBuffer	();
	int16_t		bitDepth	();
        void            update_PowerOverload (
                                         sdrplay_api_EventParamsT *params);
        std::atomic<bool>	receiverRuns;
//	This buffer is used in the callback, it therefore
//      should be visible
        RingBuffer<std::complex<float>>  *_I_Buffer;
        int32_t         sampleCnt;
        uint32_t        inputRate;
        int32_t         outputRate;
        void            report_dataAvailable    (void);
        decimatingFIR   *filter;
        int32_t         localShift;
        int32_t         oscillatorPhase;
        std::complex<float> *oscillatorTable;
        int             denominator;
	int		theGain;
	std::atomic<int>status;
private:
	QFrame				myFrame;
	sdrplay_api_Open_t              sdrplay_api_Open;
        sdrplay_api_Close_t             sdrplay_api_Close;
        sdrplay_api_ApiVersion_t        sdrplay_api_ApiVersion;
        sdrplay_api_LockDeviceApi_t     sdrplay_api_LockDeviceApi;
        sdrplay_api_UnlockDeviceApi_t   sdrplay_api_UnlockDeviceApi;
        sdrplay_api_GetDevices_t        sdrplay_api_GetDevices;
        sdrplay_api_SelectDevice_t      sdrplay_api_SelectDevice;
        sdrplay_api_ReleaseDevice_t     sdrplay_api_ReleaseDevice;
        sdrplay_api_GetErrorString_t    sdrplay_api_GetErrorString;
	sdrplay_api_GetLastError_t      sdrplay_api_GetLastError;
        sdrplay_api_DebugEnable_t       sdrplay_api_DebugEnable;
        sdrplay_api_GetDeviceParams_t   sdrplay_api_GetDeviceParams;
        sdrplay_api_Init_t              sdrplay_api_Init;
        sdrplay_api_Uninit_t            sdrplay_api_Uninit;
        sdrplay_api_Update_t            sdrplay_api_Update;

	sdrplay_api_DeviceT             *chosenDevice;
	sdrplay_api_DeviceParamsT       *deviceParams;
	sdrplay_api_CallbackFnsT        cbFns;
	sdrplay_api_RxChannelParamsT    *chParams;

        std::atomic<bool>       threadRuns;
	void			run			();
	bool			messageHandler		(generalCommand *);

	QString			recorderVersion;
	
	int32_t			vfoFrequency;
	int16_t			hwVersion;
	QSettings		*sdrplaySettings;
	bool			agcMode;
	int16_t			nrBits;
	int			lna_upperBound;
	float			apiVersion;
	QString			serial;
	bool			has_antennaSelect;
	QString			deviceModel;
	int			lnaState;
	int			ppmValue;
	HINSTANCE		Handle;
	std::queue<generalCommand *>	server_queue;
	QSemaphore		serverjobs;
	HINSTANCE		fetchLibrary		();
        void			releaseLibrary		();
        bool			loadFunctions		();

private slots:
	void			set_ifgainReduction	(int);
	void			set_lnagainReduction	(int);
	void			set_agcControl		(int);
	void			set_ppmControl		(int);
	void			set_antennaSelect	(const QString &);
public slots:
	void			set_lnabounds		(int, int);
	void			set_deviceName		(const QString &);
	void			set_serial		(const QString &);
	void			set_apiVersion		(float);
	void			set_antennaSelect	(bool);
	void			show_tunerSelector	(bool);
signals:
	void			set_lnabounds_signal	(int, int);
	void			set_deviceName_signal	(const QString &);
	void			set_serial_signal	(const QString &);
	void			set_apiVersion_signal	(float);
	void			set_antennaSelect_signal	(bool);
};
#endif

