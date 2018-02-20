#
/*
 *    Copyright (C) 2010, 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J
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

#ifndef __AMTOR_DECODER__
#define	__AMTOR_DECODER__
#
#include	<QFrame>
#include	<QSettings>
#include	"ui_amtor-widget.h"
#include	"radio-constants.h"
#include	"virtual-decoder.h"
#include	"ringbuffer.h"
#include	"shifter.h"

class	average;
class	LowPassFIR;

#define	AMTOR_DECODERQUEUE	10
typedef struct char_map {
	char	letter;
	char	digit;
	int	key;
} CharMap;

class amtorDecoder: public virtualDecoder, public Ui_amtorwidget {
Q_OBJECT
public:
			amtorDecoder		(int32_t,
	                                         RingBuffer<std::complex<float> > *,
	                                         QSettings *);
			~amtorDecoder		(void);
	void		process			(std::complex<float>);
private slots:
	void	set_amtorAfcon			(const QString &);
	void	set_amtorReverse		(const QString &);
	void	set_amtorFecError		(const QString &);
	void	set_amtorMessage		(const QString &);
private:
	QSettings	*amtorSettings;
	QString		amtorTextstring;
	void		amtorText		(uint8_t);
	void		setup_amtorDecoder	(void);
	QFrame		*myFrame;
	void		amtor_setAfcon		(bool);
	void		amtor_setReverse	(bool);
	void		amtor_setFecError	(bool);
	void		amtor_setMessage	(bool);

	void		addbitfragment		(int8_t);
	float		correctIF		(float);
	bool		validate		(uint16_t, int8_t);
	void		addfullBit		(int8_t);
	void		decoder			(int);
	void		initMessage		();
	void		addBytetoMessage	(int);
	void		flushBuffer		(void);
	void		HandleChar		(int16_t);

	int32_t		theRate;
	int16_t		CycleCount;
	float		amtorIF;
	average		*amtorFilter;
	int16_t		amtorDecimator;

	shifter		localShifter;
	LowPassFIR	*LPM_Filter;
	bool		amtorAfcon;
	bool		amtorReversed;
	float		amtorStrength;
	float		amtorShift;
	float		amtorBaudrate;
	uint16_t 	amtorBitLen;
	uint16_t	amtorPhaseacc;
	float		amtorBitclk;
	uint8_t		fragmentCount;
	int8_t		amtorOldFragment;
	uint16_t	amtorShiftReg;
	int16_t		amtorBitCount;
	int16_t		amtorLettershift;
	std::complex<float>	amtorOldz;
	int16_t		amtorState;
	int16_t		amtorQueue [AMTOR_DECODERQUEUE];
	int16_t		amtorQP;
	bool		amtorFecError;
	bool		showAlways;
	int16_t		messageState;

	enum messageMode {
	   NOTHING,
	   STATE_Z,
	   STATE_ZC,
	   STATE_ZCZ,
	   STATE_ZCZC,
	   STATE_N,
	   STATE_NN,
	   STATE_NNN,
	   STATE_NNNN
	};


//	former signals
	void		amtor_showStrength	(float);
	void		amtor_showCorrection	(float);
	void		amtor_clrText		();
	void		amtor_addText		(char);
};

#endif

