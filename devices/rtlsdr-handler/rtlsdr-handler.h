#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair programming
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

#ifndef __RTLSDR_HANDLER__
#define	__RTLSDR_HANDLER__

#include	<QThread>
#include	<QObject>
#include	<QFrame>
#include	"radio-constants.h"
#include	"device-handler.h"
#include	"ringbuffer.h"
#include	"fir-filters.h"
#include	"dongleselect.h"

class		QSettings;
class		rtlsdrHandler;
//
//	create typedefs for the library functions
typedef	struct rtlsdr_dev rtlsdr_dev_t;

#ifndef	RTLSDR_API
#define	RTLSDR_API
#endif
typedef RTLSDR_API 	void (*rtlsdr_read_async_cb_t) (uint8_t *buf, uint32_t len, void *ctx);
typedef RTLSDR_API 	 int (*  pfnrtlsdr_open )(rtlsdr_dev_t **, uint32_t);
typedef RTLSDR_API 	int (*  pfnrtlsdr_close) (rtlsdr_dev_t *);
typedef RTLSDR_API	const char * (*pfnrtlsdr_get_device_name)(uint32_t);
typedef RTLSDR_API 	int (*  pfnrtlsdr_set_center_freq) (rtlsdr_dev_t *, uint32_t);
typedef RTLSDR_API	uint32_t (*  pfnrtlsdr_get_center_freq) (rtlsdr_dev_t *);
typedef RTLSDR_API 	int (*  pfnrtlsdr_get_tuner_gains) (rtlsdr_dev_t *, int *);
typedef RTLSDR_API 	int (*  pfnrtlsdr_set_tuner_gain_mode) (rtlsdr_dev_t *, int);
typedef RTLSDR_API 	int (*  pfnrtlsdr_set_sample_rate) (rtlsdr_dev_t *, uint32_t);
typedef RTLSDR_API 	int (*  pfnrtlsdr_get_sample_rate) (rtlsdr_dev_t *);
typedef RTLSDR_API 	int (*  pfnrtlsdr_set_agc_mode) (rtlsdr_dev_t *, int);
typedef RTLSDR_API 	int (*  pfnrtlsdr_set_tuner_gain) (rtlsdr_dev_t *, int);
typedef RTLSDR_API 	int (*  pfnrtlsdr_get_tuner_gain) (rtlsdr_dev_t *);
typedef RTLSDR_API	int (*  pfnrtlsdr_reset_buffer) (rtlsdr_dev_t *);
typedef RTLSDR_API 	int (*  pfnrtlsdr_read_async) (rtlsdr_dev_t *,
	                               rtlsdr_read_async_cb_t,
	                               void *,
	                               uint32_t,
	                               uint32_t);
typedef RTLSDR_API  int (*  pfnrtlsdr_cancel_async) (rtlsdr_dev_t *);
typedef RTLSDR_API  int (*  pfnrtlsdr_set_direct_sampling) (rtlsdr_dev_t *, int);
typedef RTLSDR_API  uint32_t (*  pfnrtlsdr_get_device_count) (void);
typedef RTLSDR_API 	int (* pfnrtlsdr_set_freq_correction)(rtlsdr_dev_t *, int);


#include	"ui_rtlsdr-widget.h"

class	dll_driver : public QThread {
public:
	dll_driver (rtlsdrHandler *d);
	~dll_driver (void);
//	For the callback, we do need some environment 
//	This is the user-side call back function
static
void	RTLSDRCallBack (unsigned char *buf, uint32_t len, void *ctx);

private:
virtual void	run (void);
	rtlsdrHandler	*theStick;
};

class	rtlsdrHandler: public deviceHandler, public Ui_rtlsdrWidget {
Q_OBJECT
public:
		rtlsdrHandler		(RadioInterface *mr,
                                         int32_t        outputRate,
                                         RingBuffer<DSPCOMPLEX> *r,
                                         QSettings      *s);

		~rtlsdrHandler		(void);
	int32_t	getRate			(void);
	void	setVFOFrequency		(quint64);
	quint64	getVFOFrequency		(void);
	bool	legalFrequency		(int32_t);
	bool	restartReader		(void);
	void	stopReader		(void);
	int16_t	bitDepth		(void);
//
//	I_Buffer needs to be visible for use within the callback
	RingBuffer<std::complex<float>>	*_I_Buffer;
	pfnrtlsdr_read_async rtlsdr_read_async;
	struct rtlsdr_dev	*device;
	int32_t		outputRate;
	decimatingFIR	*d_filter;
	void	newdataAvailable	(int);

private slots:
	void		setExternalGain	(int);
	void		setCorrection	(int);
	void		setAgc		(int);

private:
	dongleSelect	*dongleSelector;
	QSettings	*rtlsdrSettings;
	void		setupDevice	(int32_t, int32_t);
	QFrame		*myFrame;
	HINSTANCE	Handle;
	int32_t		deviceCount;
	dll_driver	*workerHandle;
	bool		libraryLoaded;
	int32_t		vfoFrequency;
	bool		open;
	int16_t		gainsCount;
	int		*gains;
//
//	here we need to load functions from the dll
	bool		load_rtlFunctions	(void);
	pfnrtlsdr_open	rtlsdr_open;
	pfnrtlsdr_close	rtlsdr_close;
	pfnrtlsdr_get_device_name rtlsdr_get_device_name;
	pfnrtlsdr_set_center_freq rtlsdr_set_center_freq;
	pfnrtlsdr_get_center_freq rtlsdr_get_center_freq;
	pfnrtlsdr_get_tuner_gains rtlsdr_get_tuner_gains;
	pfnrtlsdr_set_tuner_gain_mode rtlsdr_set_tuner_gain_mode;
	pfnrtlsdr_set_sample_rate rtlsdr_set_sample_rate;
	pfnrtlsdr_get_sample_rate rtlsdr_get_sample_rate;
	pfnrtlsdr_set_agc_mode rtlsdr_set_agc_mode;
	pfnrtlsdr_set_tuner_gain rtlsdr_set_tuner_gain;
	pfnrtlsdr_get_tuner_gain rtlsdr_get_tuner_gain;
	pfnrtlsdr_reset_buffer rtlsdr_reset_buffer;
	pfnrtlsdr_cancel_async rtlsdr_cancel_async;
	pfnrtlsdr_set_direct_sampling	rtlsdr_set_direct_sampling;
	pfnrtlsdr_get_device_count rtlsdr_get_device_count;
	pfnrtlsdr_set_freq_correction rtlsdr_set_freq_correction;
};
#endif

