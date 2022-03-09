#
/*
 *    Copyright (C) 2013, 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the drm receiver
 *
 *    Many of the ideas as implemented in SDR-J are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
 *
 *    drm receiver is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm receiver is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with drm receiver; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __DRM_DECODER__
#define	__DRM_DECODER__
#

#include	<QObject>
#include	<complex>
#include	<vector>
#include	"radio-constants.h"
#include	"virtual-decoder.h"
#include	"fir-filters.h"
#include	"ui_drmdecoder.h"
#include	"ringbuffer.h"
#include	<QLabel>

class	frameProcessor;
class	RadioInterface;
class	QSettings;
class	IQDisplay;
class	EQDisplay;

class drmDecoder:public virtualDecoder, private Ui_drmdecoder {
Q_OBJECT
public:
			drmDecoder		(int32_t,
	                                         RingBuffer<std::complex<float> > *,
	                                         QSettings *);
			~drmDecoder		(void);
	void		processBuffer		(std::complex<float> *, int);
	void		process			(std::complex<float>);
//
//	For internal use:
	void		show			(std::complex<float>);
	bool		haltSignal		(void);
	int16_t		getAudioChannel		(void);
	int16_t		getDataChannel		(void);
	bool		isSelectedDataChannel	(int16_t);
private:
	QFrame		myFrame;
	RingBuffer<std::complex<float>> iqBuffer;
	RingBuffer<std::complex<float>> eqBuffer;
	RingBuffer<std::complex<float>> buffer;
	LowPassFIR	Filter_10;
	Basic_FIR	*theFilter;
	frameProcessor	*my_frameProcessor;
	IQDisplay	*my_iqDisplay;
	EQDisplay	*my_eqDisplay;
	bool		running;
	bool		decimatorFlag;
	int16_t		Length;

	int16_t		bufferLength;
	int32_t		theRate;
	std::vector<std::complex<float> > localOscillator;
	int		currentPhase;
	int		phaseOffset;
	int32_t		workingRate;
	RingBuffer<std::complex<float>> *audioOut;
public slots:
	void		show_stationLabel	(const QString &, int);
	void		show_timeLabel		(const QString &);
	void		showIQ			(int);
	void		show_eqsymbol		(int);
	void		show_audioMode		(QString);
	void		sampleOut		(float, float);
	void		samplesAvailable	();
	void		show_coarseOffset	(float);
	void		show_fineOffset		(float);
	void		show_timeDelay		(float);
	void		show_timeOffset		(float);
	void		show_clockOffset	(float);
	void		show_channels		(int, int);
	void		show_angle		(float);
	void		faadSuccess		(bool);
	void		showMOT			(QByteArray, int);
	void		aacData			(QString);
	void		show_country		(QString);
	void		show_programType	(QString);
	void		show_time		(QString);
	void		show_datacoding		(QString);
	void		show_mer_sdc		(float);
	void		show_mer_msc		(float);

private slots:
	void		executeTimeSync		(bool);
	void		executeFACSync		(bool);
	void		executeSDCSync		(bool);
	void		execute_showMode	(int);
	void		execute_showSpectrum	(int);
	void		showSNR			(float);
	void		showMessage		(QString);
//
	void		selectChannel_1		(void);
	void		selectChannel_2		(void);
	void		selectChannel_3		(void);
	void		selectChannel_4		(void);

	void		set_phaseOffset		(int);


signals:
	void		audioAvailable		(int, int);
	void		setDetectorMarker	(int);
	void		adjustFrequency		(int);
};

typedef	struct distances {
	float	distance_to_0;
	float	distance_to_1;
} distance;


#endif


