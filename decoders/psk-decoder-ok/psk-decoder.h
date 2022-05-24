#
/*
 *    Copyright (C) 2010, 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of swradio
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

#ifndef __PSK_DECODER__
#define	__PSK_DECODER__
#

#include	"radio-constants.h"
#include	"virtual-decoder.h"
#include	"ui_psk-decoder.h"
#include	"shifter.h"
#include	<vector>
#include	"fft.h"
#include	"waterfall-scope.h"

class	bandpassFIR;
class	decimatingFIR;
class	LowPassFIR;
class	viterbi;
class	QFrame;
class	QSettings;
class	shifter;
class	slidingFFT;
/*
 * PSK31  -- receiver classs
 */
class pskDecoder: public virtualDecoder, public Ui_psk_widget {
Q_OBJECT
public:
			pskDecoder		(int32_t,
	                                         RingBuffer<std::complex <float> > *,
	                                         QSettings *);
			~pskDecoder		(void);
	void		process			(std::complex<float>);
	int32_t		rateOut			(void);
	int16_t		detectorOffset		(void);
private slots:
	void		psk_setAfcon		(const QString &);
	void		psk_setReverse		(const QString &);
	void		psk_setSquelchLevel	(int);
	void		psk_setMode		(const QString &);
	void		psk_setFilterDegree	(int);
	void		handleClick		(int);
	
private:
	enum PskMode {
	   MODE_PSK31 =		0100,
	   MODE_PSK63 =		0101,
	   MODE_PSK125 =	0102,
	   MODE_QPSK31 =	0110,
	   MODE_QPSK63 =	0111,
	   MODE_QPSK125 =	0112
	};

	QFrame		myFrame;
	decimatingFIR	*theFilter;
	LowPassFIR	*newFilter;
	waterfallScope	*pskViewer;
	int		screenwidth;
	double		*x_axis;
	double		*y_values;
	int		fillP;
	common_fft	*the_fft;
	std::complex<float> *fftBuffer;
	int32_t		theRate;
	RingBuffer<std::complex<float> > *audioData;
	QSettings	*pskSettings;
	void		doDecode		(std::complex<float>);
	void		setup_pskDecoder	(int32_t);
	void		psk_setup		(void);
	float		speedofPskMode		(void);
	bool		isBinarypskMode		(void);
	bool		isQuadpskMode		(void);	
	uint8_t		decodingIsOn		(uint8_t,
	                                         double,
	                                         uint8_t, std::complex<float> *);
	void		bpsk_bit		(uint16_t);
	void		qpsk_bit		(uint16_t);
	int16_t		pskVaridecode		(uint16_t);
	double		psk_IFadjust		(double, uint16_t);
	uint8_t		getIntPhase		(std::complex<float>);
	int16_t		DecimatingCountforpskMode	(void);

	slidingFFT	*newFFT;
	int		offset			(std::complex<float> *);
	shifter		localShifter;

	int16_t		pskCycleCount;
	bandpassFIR	*BPM_Filter;
	viterbi		*viterbiDecoder;
	double		psk_IF;
	int16_t		pskDecimatorCount;
	unsigned int 	pskShiftReg;
	int16_t		pskPhaseAcc;
	uint32_t 	pskDecodeShifter;
	std::complex<float>	pskQuality;
	double		pskAfcmetrics;
	bool		pskAfcon;
	bool		pskReverse;
	int16_t		pskSquelchLevel;
	uint8_t		pskMode;
	int16_t		pskFilterDegree;
	std::complex<float>	pskOldz;
	double		psk_phasedifference;

	int16_t		pskPrev1Phase;
	int16_t		pskPrev2Phase;
	int16_t		pskCurrPhase;
	int16_t		pskSyncCounter;
	float		pskBitclk;
	std::complex<float>	*pskBuffer;

	void		restore_GUISettings	(QSettings *);
	void		mapTable	(int32_t, int32_t);
	int		resample	(std::complex<float>,
	                                 std::complex<float> *);
	std::vector<float>	theTable;
	std::vector<complex<float> >	inTable;
	int16_t		tablePointer;
//
//	former signals are now handled locally
	QString		pskText;
	void		psk_showDial		(float);
	void		psk_showMetrics		(float);
	void		psk_showIF		(float);
	void		psk_addText		(char);
	void		psk_clrText		();
};

#endif


