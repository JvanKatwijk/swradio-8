#
/*
 *    Copyright (C) 2022
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

#ifndef	__FT8_DECODER__
#define	__FT8_DECODER__
#include	"radio-constants.h"
#include	<QFrame>
#include	<QSettings>
#include	<QStringList>
#include        <QStandardItemModel>
#include	<atomic>
#include 	"virtual-decoder.h"
#include	"ui_ft8-decoder.h"
#include	"kiss_fft.h"
#include	"ft8-constants.h"
#include	"ldpc.h"
#include	"ft8-processor.h"
#include	"PSKReporter.h"

typedef struct {
	int index;
	float	value;
} costasValue;

class	RadioInterface;

class	ft8_Decoder: public virtualDecoder, public Ui_ft8_widget {
Q_OBJECT
public:
		ft8_Decoder	(RadioInterface *,
	                         int32_t,
	                   	 RingBuffer<std::complex<float>> *,
	                         QSettings	*);
		~ft8_Decoder	(void);
	void	processBuffer	(std::complex<float> *, int32_t);
	void    process		(std::complex<float> z);
	int	getVFO		();
	bool	pskReporterReady ();

private:	
	bool		pskReady;
	QFrame		myFrame;
	ldpc		ldpcHandler;
	ft8_processor	theProcessor;
	QSettings	*ft8Settings;
	RadioInterface	*mr;
	int32_t		inputRate;
	int32_t		outputRate;
	int32_t		samplesperSymbol;
	std::atomic<FILE*> filePointer;
	void		peakFinder (float *V, int begin, int end,
                                            std::vector<costasValue> &cache);

	void		processLine	(int);
	float		testCostas	(int, int);
	float		decodeTones	(int row, int bin,
	                                                  float *log174);
	float		decodeTone	(int row, int bin, float *logl);
	void		normalize_logl	(float *);
	int		addCache	(float,
	                                 int, std::vector<costasValue> &);

	kiss_fft_cfg	plan;
	kiss_fft_cpx	*fftVector_in;
	kiss_fft_cpx	*fftVector_out;
	float		*window;

	int		toneLength;
	int		messageLength;
	int		costasLength;
	float		*theBuffer [nrBUFFERS];
	std::complex<float> *inputBuffer;
	std::complex<float> *inBuffer;
	int		fillIndex;
	int		readIndex;
	int		inPointer;
	int		lineCounter;
	std::atomic<int>	maxIterations;
	std::atomic<int>	cacheDepth;
	std::atomic<int>	spectrumWidth;
	bool		is_syncTable [nrTONES];

	QStandardItemModel	model;
	void		showText	(const QStringList &);
	QStringList	theResults;
	int		teller;
	void		print_statistics	();
public slots:
	void		printLine		(const QString &);
	void		show_pskStatus		(bool);
private slots:
	void		set_maxIterations	(int);
	void		set_spectrumWidth	(int);
	void		handle_filesaveButton	();
	void		handle_identityButton	();
	void		handle_pskReporterButton ();
};

#endif

