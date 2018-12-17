#
/*
 *    Copyright (C) 2009, 2010, 2011
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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

#ifndef __PA_READER__
#define	__PA_READER__

#include	<portaudio.h>
#include	"radio-constants.h"
#include	"ringbuffer.h"
#include	<QObject>

class	QComboBox;

#define	IandQ	0100
#define	QandI	0101

class  paReader: public QObject {
Q_OBJECT
public:
	 		paReader		(int32_t, QComboBox *);
			~paReader		(void);
//	for setting up
	int16_t		numberofDevices		(void);

	int16_t		invalidDevice		(void);
	bool		isValidDevice		(int16_t);
//	operational control
	bool		selectDevice		(int16_t);
	bool		restartReader		(void);
	void		stopReader		(void);
	int32_t		Samples			(void);
	int32_t		getSamples		(std::complex <float> *,
	                                         int32_t, uint8_t);
	bool		set_StreamSelector	(int);
	bool		isRunning		(void);
	int32_t		inputRate;
private:
	bool		setupChannels		(QComboBox *);
	bool		InputrateIsSupported	(int16_t, int32_t);
const	char		*inputChannelwithRate	(int16_t, int32_t);
	
	int32_t		portAudio;
	int16_t		numofDevices;
	int		paCallbackReturn;
	int16_t		bufSize;
	PaStream	*istream;

	RingBuffer<float>	*_I_Buffer;
	PaStreamParameters	inputParameters;
	bool		readerRunning;
	int16_t		*inTable;
	QComboBox	*streamSelector;
protected:
static	int	paCallback_i	(const void *input,
	                         void	    *output,
	                         unsigned long framesperBuffer,
	                         const PaStreamCallbackTimeInfo *timeInfo,
	                         PaStreamCallbackFlags statusFlags,
	                         void	    *userData);
signals:
	void		samplesAvailable	(int);
};

#endif
