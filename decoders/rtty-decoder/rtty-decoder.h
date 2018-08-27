#
/*
 *    Copyright (C) 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
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

#ifndef __RTTY_DECODER__
#define	__RTTY_DECODER__
#
#include	<QFrame>
#include	"radio-constants.h"
#include	"virtual-decoder.h"
#include	"ui_rtty-decoder.h"
#include	"shifter.h"

class	bandpassFIR;
class	QSettings;
class	average;
/*
 *	defines, local for the rttyDetector stuff
 */
#define	RTTY_RX_STATE_IDLE 	0
#define	RTTY_RX_STATE_START	1
#define	RTTY_RX_STATE_DATA	2
#define	RTTY_RX_STATE_PARITY	3
#define	RTTY_RX_STATE_STOP	4
#define	RTTY_RX_STATE_STOP2	5

#define	BAUDOT_LETS		0
#define	BAUDOT_FIGS		1

class rttyDecoder: public virtualDecoder, public Ui_rtty_widget {
Q_OBJECT
public:
			rttyDecoder		(int32_t,
	                                         RingBuffer<std::complex<float>> *,
	                                         QSettings *);
			~rttyDecoder		(void);
	void		process			(std::complex<float>);
private:
	QFrame	*myFrame;

	enum Parities {
	   RTTY_PARITY_NONE =	0,
	   RTTY_PARITY_EVEN =	1,
	   RTTY_PARITY_ODD =	2,
	   RTTY_PARITY_ZERO =	3,
	   RTTY_PARITY_ONE =	4
	};

private slots:
	void	set_rttyAfcon		(const QString &);
	void	set_rttyReverse		(const QString &);
	void	set_rttyShift		(const QString &);
	void	set_rttyBaudrate	(const QString &);
	void	set_rttyParity		(const QString &);
	void	set_rttyMsb		(const QString &);
	void	set_rttyNbits		(const QString &);
	void	set_rttyStopbits	(const QString &);
private:
	int32_t		theRate;
	QSettings	*rttySettings;
	void		setup_rttyDecoder	(void);
	void		rtty_setStopbits	(int16_t);
	void		rtty_setBaudrate	(int16_t);
	void		rtty_setShift		(int16_t);
	void		rtty_setParity		(int16_t);
	void		rtty_setReverse		(bool);
	void		rtty_setNbits		(int16_t);
	void		rtty_setAfcon		(bool);
	void		rtty_setMsb		(bool);

private:
	void		restore_GUISettings	(QSettings *);
	void		rtty_setup		(void);
	uint8_t		addrttyBitFragment	(void);
	uint8_t		pickup_char		(void);
	uint32_t	reverseBits		(uint32_t, uint32_t);
	uint8_t		BaudottoASCII		(uint8_t);
	void		printChar		(char, char);
	uint8_t		check_parity		(uint32_t,
	                                         uint32_t, uint8_t);
	uint8_t		odd			(uint32_t);
	void		check_baudrate		(int16_t, int16_t);
	double		adjust_IF		(double);
	double		rttyS2N			(void);
	average		*rttyAverager;
	shifter		localShifter;
	int16_t		rttyCycleCount;
	uint8_t		rttyBitFragment;
	int16_t		rttyPoscnt;
	int16_t		rttyNegcnt;
	double		rttyPosFreq;
	double		rttyNegFreq;
	double		rttyFreqError;
	std::complex<float>	rttyPrevsample;
	bandpassFIR	*BPM_Filter;
	uint8_t		rttyAfcon;
	double		rttyShift;
	double		rttyIF;
	int16_t		rttySymbolLength;
	int16_t		rttyNbits;
	uint8_t		rttyParity;
	int16_t		rttyStopbits;
	bool		rttyMsb;
	int16_t		rttyStoplen;
	bool		rttyReversed;
	int16_t		rttyFilterDegree;
	int16_t		rttyPhaseacc;
	int16_t		rttyState;
	int16_t		rttyCounter;
	int16_t		rttyBitcntr;
	int16_t		rttyRxdata;
	uint8_t		rttyRxmode;
	double		rttyBaudrate;
	int16_t		rttyPluscnt;
	int16_t		rttyMincnt;
	int16_t		rttyPrevfragment;
	QString		rttyText;
	double		rttySignalPower;
	double		rttyNoisePower;
//
//	former signals, now handled locally
	void		rtty_showStrength	(float);
	void		rtty_showFreqCorrection	(float);
	void		rtty_showBaudRate	(int);
	void		rtty_showGuess		(int);
	void		rtty_clrText		();
	void		rtty_addText		(char);
	void		doDisplay		(double *, double *, double *, int);
};

#endif

