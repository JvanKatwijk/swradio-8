#
/*
 *    Copyright (C)  2018
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of SDR-J (JSDR).
 *    Many of the ideas as implemented in SDR-J are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are acknowledged and recognized.
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
 */

#ifndef	__SW_RADIO__
#define	__SW_RADIO__

#include        <QMainWindow>
#include        <QTimer>
#include        <QWheelEvent>
#include        <QLineEdit>
#include	<QTimer>
#include	<sndfile.h>
#include        "ui_newradio.h"
#include        "radio-constants.h"
#include        "ringbuffer.h"
#include	"decimator.h"
#include	"fft-filters.h"
#include	"shifter.h"
#include	"s-meter.h"
#include        "agchandler.h"

class		deviceHandler;
class           virtualDecoder;
class           downConverter;
class           upConverter;
class           QSettings;
class           fftScope;
class           fft_scope;
class		audioScope;
class           audioSink;
class           keyPad;
class           programList;
class		fftFilter;
class		bandPlan;

class	RadioInterface:public QMainWindow,
	               private Ui_MainWindow {
Q_OBJECT
public:
		RadioInterface (QSettings	*sI,
	                        QString		stationList,
	                        bandPlan	*my_bandPlan,
	                        int		inputRate,
	                        int		decoderRate,
	                        QWidget		*parent = NULL);
		~RadioInterface	();
	int32_t	tunedFrequency	();
private:
	struct band {
	   int32_t	lowF;
	   int32_t	highF;
	   int32_t	currentOffset;
	}theBand;
	
	QSettings       *settings;
	bandPlan	*my_bandPlan;
        int32_t         inputRate;
	int32_t		decoderRate;
	int32_t		scopeWidth;
        int32_t         workingRate;
        int32_t         audioRate;
        int16_t         displaySize;
        int16_t         spectrumSize;
        double          *displayBuffer;
        audioSink       *audioHandler;
        int16_t         *outTable;
        deviceHandler	*theDevice;
        virtualDecoder	*theDecoder;
        RingBuffer<std::complex<float> > *inputData;
        RingBuffer<std::complex<float> > *audioData;
        fftScope        *hfScope;
	fftScope	*lfScope;
	audioScope	*showAudio;
	shifter		hfShifter;
	fftFilter	hfFilter;
	fftFilter	lfFilter;
	agcHandler	agc;
	decimator	theDecimator;
	upConverter	*theUpConverter;
	int16_t		mouseIncrement;
	int16_t		delayCount;
//
	SNDFILE		*dumpfilePointer;
	QTimer		secondsTimer;
	void            setupSoundOut   (QComboBox        *streamOutSelector,
                                         audioSink        *our_audioSink,
                                         int32_t          cardRate,
                                         int16_t          *table);

        keyPad          *mykeyPad;
        programList     *myList;
        QLineEdit       *myLine;
        void            adjust          (int32_t);

	int32_t		theFrequency;
private slots:
        deviceHandler	*setDevice		(
	                                      RingBuffer<std::complex<float>> *);
        void            adjustFrequency_khz	(int);
        void            handle_myLine		();
        void            set_hfscopeLevel	(int);
        void            set_lfscopeLevel	(int);
	virtualDecoder	*selectDecoder		(const QString &);
        void            setStreamOutSelector	(int idx);
        void            handle_freqButton	();
        void            wheelEvent              (QWheelEvent *);
	void		set_mouseIncrement	(int);
	void		setBand			(const QString &);
	void		set_inMiddle		(void);
	void		set_freqSave		(void);
	void		handle_quitButton	(void);
	void		set_agcThresholdSlider  (int);
	void		set_AGCMode             (const QString &);
	void		switch_hfViewMode	(int);
	void		switch_lfViewMode	(int);
	void		updateTime		(void);
	void		set_dumpButton		(void);
	void		closeEvent		(QCloseEvent *event);
public slots:
	void		sampleHandler		(int amount);
        void            processAudio		(int, int);
	void		setDetectorMarker	(int);
	void		setFrequency		(quint64);
        void            adjustFrequency_hz	(int);

#ifdef	HAVE_EXTIO
//      and for the extio handling
        void    set_ExtFrequency        (int);
        void    set_ExtLO               (int);
        void    set_lockLO              (void);
        void    set_unlockLO            (void);
        void    set_stopHW              (void);
        void    set_startHW             (void);
#endif
};

#endif
