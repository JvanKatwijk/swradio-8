#
/*
 *    Copyright (C) 2008, 2009, 2010
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J.
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

#ifndef __FILEHULP__
#define	__FILEHULP__

#include	<QThread>
#include	<QString>
#include	<sndfile.h>
#include	<atomic>
#include	"radio-constants.h"
#include	"ringbuffer.h"
//
//
class	fileHulp: public QThread {
Q_OBJECT
public:
	fileHulp	(QString, int32_t, RingBuffer<std::complex<float> > *);
	~fileHulp	(void);
//	
//	functions really for this rig
	bool		restartReader	(void);
	void		stopReader	(void);
	int32_t		Samples		(void);
	int32_t		getSamples	(std::complex<float> *, int32_t, uint8_t);
	void		reset		(void);
	int32_t		setFileat	(int32_t);
protected:
virtual void		run		(void);
	int32_t		theRate;
	QString		f;
	RingBuffer<std::complex<float> >	*_I_Buffer;
	QString		fileName;
	int32_t		readBuffer	(float *data, int32_t length);
	SNDFILE		*filePointer;
	bool		readerOK;
	int32_t		sampleRate;
	int16_t		bitsperSample;
	int32_t		samplesinFile;
	int32_t		totalTime;
	int32_t		fileLength;
	int32_t		currPos;
	int16_t		numofChannels;
	std::atomic<bool> running;
	bool		resetRequest;
signals:
	void		dataAvailable	(int);
	void		setProgress	(int, float, float);
};
#endif

