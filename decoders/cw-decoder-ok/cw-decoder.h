#
/*
 *    Copyright (C) 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the swradio
 *
 *    swradio is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    swradio is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with swradio; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __CW_DECODER__
#define	__CW_DECODER__
#
#include	<QWidget>
#include	"radio-constants.h"
#include	"virtual-decoder.h"
#include	"ringbuffer.h"
#include	"shifter.h"
#include	"downconverter.h"
#include	"fir-filters.h"
#include	"ui_cw-decoder.h"
#include	"waterfall-scope.h"
#include	"fft.h"

class	Oscillator;
class	average;
class	QFrame;
class	QSettings;
class	slidingFFT;

#define	CWError		1000
#define	CWNewLetter	1001
#define	CWCarrierUp	1002
#define	CWCarrierDown	1005
#define	CWStroke	1003
#define	CWDot		1004
#define	CWendofword	1006
#define	CWFailing	1007

#define	CW_RECEIVE_CAPACITY	040

class cwDecoder: public virtualDecoder, public Ui_cwDecoder {
Q_OBJECT
public:
		cwDecoder	(int32_t,
	                         RingBuffer<DSPCOMPLEX> *,
	                         QSettings *);
		~cwDecoder	(void);
	void	processBuffer	(std::complex<float> *, int32_t);
	void	process		(std::complex<float>);
private slots:
	void		cw_setWordsperMinute	(int);
	void		cw_setTracking		(void);
	void		cw_setFilterDegree	(int);
	void		cw_setSquelchValue	(int);
	void		cw_adjustFrequency	(int);
	void		handleClick             (int);

private:
	waterfallScope	*cwViewer;
	double		*x_axis;
	double		*y_values;
	common_fft	*the_fft;
	std::complex<float> *fftBuffer;
	int		screenwidth;
	int		fillP;
	decimatingFIR	*theFilter;
	int32_t		theRate;
	slidingFFT	*newFFT;
	QFrame		*myFrame;
	RingBuffer<std::complex<float>> inputBuffer;
	downConverter	inputConverter;
	shifter		localShifter;
	bandpassFIR	*cw_BandPassFilter;
	void		processSample		(std::complex<float>);
	void		setup_cwDecoder		(int32_t);
	void		cw_clrText		(void);
	void		cw_addText		(char);
	QString		cwText;
	void		cw_showdotLength	(int);
	void		cw_showspaceLength	(int);
	void		cw_showspeed		(int);
	void		cw_showagcpeak		(int);
	void		cw_shownoiseLevel	(int);
//
	void		speedAdjust		(void);
	void		printChar		(char, char);
	void		addtoStream		(double);
	void		add_cwTokens		(char *);
	int32_t		newThreshold		(int32_t, int32_t);
	int32_t		getMeanofDotDash	(int32_t, int32_t);
	void		lookupToken		(char *, char *);

	int		offset			(std::complex<float> *);
	int32_t		cwPhaseAcc;

	int32_t		CycleCount;
	int16_t		cwFilterDegree;
	average		*SmoothenSamples;
	average		*thresholdFilter;
	average		*spaceFilter;

	float		agc_peak;
	float		noiseLevel;
	float		value;
	float		metric;
	double		cw_IF;
	int32_t		cwDotLength;
	int32_t		cwSpaceLength;
	int32_t		cwDashLength;
	int16_t		SquelchValue;
	int16_t		cwState;
	int16_t		cwPreviousState;
	int32_t		cw_adaptive_threshold;
	int32_t		currentTime;
	int32_t		cwCurrent;
	int16_t		cwSpeed;
	int16_t		cwDefaultSpeed;
	char		dataBuffer [CW_RECEIVE_CAPACITY];
	int32_t		CWrange;
	int32_t		cwDefaultDotLength;
	int32_t		lowerBoundWPM;
	int32_t		upperBoundWPM;
	int32_t		cwDotLengthMin;
	int32_t		cwDotLengthMax;
	int32_t		cwDashLengthMin;
	int32_t		cwDashLengthMax;
	int32_t		lengthofPreviousTone;
	int32_t		cwNoiseSpike;
	int32_t		cwStartTimestamp;
	int32_t		cwEndTimestamp;
	bool		cwTracking;

signals:
};
#endif

