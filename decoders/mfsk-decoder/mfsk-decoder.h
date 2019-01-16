#
/*
 *    Copyright (C) 2010, 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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

#ifndef __MFSK_DECODER__
#define	__MFSK_DECODER__
#
/*		
 *	Mfsk  -- receiver classs
 */
#include	<QObject>
#include	<QFrame>
#include	<QString>
#include	"radio-constants.h"
#include	"virtual-decoder.h"
#include	"ui_mfsk-decoder.h"
#include	"slidingfft.h"
#include	"viterbi.h"
#include	"interleave.h"
#include	"shifter.h"
#include	"fft.h"
#include	"fft-filters.h"
#include	"utilities.h"
#include	"waterfall-scope.h"

class	QSettings;
class	Cache;

class mfskDecoder: public virtualDecoder, public Ui_mfsk_widget {
Q_OBJECT
public:
			mfskDecoder		(int32_t,
	                                         RingBuffer<std::complex<float>> *,
	                                         QSettings *);
			~mfskDecoder		(void);
	void		process			(std::complex<float>);
	bool		initforRate		(int32_t);
	int32_t		rateOut			(void);
	int16_t		detectorOffset		(void);
private:
	int32_t		theRate;
	QSettings	*mfskSettings;
	shifter		localShifter;
	fftFilter	mfskbandFilter;
	viterbi		ViterbiDecodera;

	QFrame		*myFrame;
	void		AdjustParameters	(void);

	waterfallScope		*mfskViewer;
	QString		mfskText;
	enum decoders {
	   MODE_MFSK_4 =	0100,
	   MODE_MFSK_8 =	0101,
	   MODE_MFSK_11	=	0102,
	   MODE_MFSK_16 =	0103,
	   MODE_MFSK_22	=	0104,
	   MODE_MFSK_31	=	0105,
	   MODE_MFSK_32	=	0106,
	   MODE_MFSK_64 =	0107
	};
	void		MapTonetoBits 		(std::complex<float> *,
	                                         int16_t, unsigned char *);
	void		setupInterfaceforMode	(uint8_t);
	int16_t		IndextoFullestBin	(std::complex<float> *);
	void		handle_mfsk_bits	(uint8_t);
	void		shiftBit		(uint8_t);
	int16_t		vari_Decode		(uint16_t symbol);

	int32_t		_WorkingRate;

	double		mfskIF;
	bool		mfsk_afcon;
	uint8_t		mfsk_mode;
	bool		mfsk_reverse;
	int16_t		mfsk_squelch;
	int16_t		mfskSamplesperSymbol;
	int16_t		mfskNumBits;
	int16_t		mfskBaseTone;	/* 1500 Hz		*/
	int16_t		mfskNumTones;
	int16_t		mfskBorder;
	double		mfskBaudRate;
	double		mfskToneSpacing;

	float		mfskBw;

	unsigned char	mfskDibitpair [2];

	viterbi		*ViterbiDecoderb;
	Interleaver	*mfskInterleaver;
	Cache		*mfskSpectrumCache;
	uint16_t	mfskCacheLineP;

	slidingFFT	*mfskSlidingfft;
	uint16_t	mfskDatashreg;

	std::complex<float>	mfskPrev1Vector;
	std::complex<float>	mfskPrev2Vector;

	uint16_t	mfskPrev1Index;
	uint16_t	mfskPrev2Index;
	int16_t		showCount;

	float		mfskMet1;
	float		mfskMet2;
	float		mfskSignalMetric;
	uint8_t		mfskbitCounter;
	int		mfskSyncCounter;

	float		mfskbaudrateEstimate;


private slots:
	void		mfsk_setAfcon		(const QString &);
	void		mfsk_setReverse		(const QString &);
	void		mfsk_setMode		(const QString &);
	void		mfsk_setSquelch		(int);
//	void		handle_amplitude	(int);
	void		handleClick		(int);
private:
	void		mfsk_clrText	();
	void		mfsk_addText	(char);
	void		mfsk_showLocalScope	(int);
	void		mfsk_addLocalScope	(int, double);
	void		mfsk_showCF	(float);
	void		mfsk_showS2N	(int);
	void		mfsk_showTuning	(int);
	void		mfsk_showEstimate	(float);
};

class Cache {
public:
		Cache		(int16_t, int16_t);
		~Cache		(void);
std::complex<float>	*CacheLine	(int16_t a);
private:
	int16_t		rowBound;
	int16_t		columnBound;
	std::complex<float>	**data;
};

#endif
