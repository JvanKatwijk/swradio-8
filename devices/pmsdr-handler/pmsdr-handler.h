#
/*
 *    Copyright (C) 2010, 2011, 2012
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
 *
 *	This is a rewrite of the code for
 *	Library for
 *	PMSDR command line manager (experimental)
 *	control a PMSDR hardware on Linux
 *	Modified to comply to firmware version 2.1.x
 *
 *	Copyright (C) 2008,2009,2010  Andrea Montefusco IW0HDV,
 *	Martin Pernter IW3AUT
 *	his program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef	__PMSDR_HANDLER__
#define	__PMSDR_HANDLER__

#include        <QObject>
#include        <QFrame>
#include        <QSettings>
#include	<QMutex>
#include        <atomic>
#include        "radio-constants.h"
#include        "ringbuffer.h"
#include	"device-handler.h"
#include	"pmsdr_comm.h"
#include	"ui_pmsdr-widget.h"

class	paReader;
class	si570Handler;
//	Implementing the rig interface
// Global CONSTANTS
//-----------------------------------------------------------------------------

//
const static QsdBiasFilterIfGain qsdBfigDefault = {
    512,  // bias at half scale
    0,    // mute off
    0,    // no filter
    1     // 20 dB gain
};

class	pmsdrHandler: public deviceHandler, public Ui_pmsdr {
Q_OBJECT
public:
			pmsdrHandler	(RadioInterface *,
	                                 int32_t	,
	                                 RingBuffer<std::complex<float>> *,
	                                 QSettings *);
			~pmsdrHandler		(void);
	int32_t		getRate			(void);
	void		setVFOFrequency		(quint64);
	quint64		getVFOFrequency		(void);
	bool		legalFrequency		(int32_t);
	bool		restartReader		(void);
	void		stopReader		(void);
	int16_t		bitDepth		(void);
private slots:
	void		setMute			(void);
	void		set_gainValue		(int);
	void		setBias			(int);
	void		set_autoButton		(void);
	void		set_button_1		(void);
	void		set_button_2		(void);
	void		set_button_3		(void);
	void		set_button_4		(void);
	void		set_button_5		(void);
	void		set_offset_KHz		(int);
	void		set_Streamselector	(int);
	void		set_modeSelector	(const QString &);
public slots:
	void		samplesAvailable	(int);
private:
	RingBuffer<std::complex<float>> *dataBuffer;
	int16_t		gainValue;
	int32_t		inputRate;
	uint8_t		Mode;
	QSettings	*pmsdrSettings;
	si570Handler	*mySI570;
	void		setFilter		(int16_t);
	void		setFilterAutomatic	(int32_t);
	bool		automaticFiltering;
	void		displayRange		(int16_t);
	RadioInterface	*myRadio;
	paReader	*myReader;
	QMutex		readerOwner;
	void		setup_device	(void);
	int32_t		vfoOffset;
	QFrame		*myFrame;
	QsdBiasFilterIfGain qsdBfig;
	RadioInterface	*myRadioInterface;
	bool		radioOK;
	pmsdr_comm	*pmsdrDevice;
	int16_t		usbFirmwareSub;
	int16_t		usbFirmwareMajor;
	int16_t		usbFirmwareMinor;
	int32_t		LOfreq;
	int32_t		defaultFreq;
	uint8_t		fLcd;
	
	int16_t		currentFilter;
	bool		store_hwlo		(int32_t);
	bool		read_hwlo		(int32_t *);
};
#endif

