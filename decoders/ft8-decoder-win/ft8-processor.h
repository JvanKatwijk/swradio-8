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


#ifndef	__FT8_PROCESSOR__
#define	__FT8_PROCESSOR__
#include	<QObject>
#include	<QSettings>
#include	<thread>
#include	<atomic>
#include	"semaphore.h"
#include	<QMutex>
#include	"ft8-constants.h"
#include	"pack-handler.h"
#include	"dl-cache.h"
#include	"PSKReporter.h"

class	ft8_Decoder;
class	reporterWriter;
#define	nrBlocks	100

class	ft8_processor: public QObject {
Q_OBJECT
public:
		ft8_processor	(ft8_Decoder *, int, QSettings *);
		~ft8_processor	();

	void	PassOn		(int, float, int, float *);
	void	set_maxIterations	(int);

private:
	ft8_Decoder	*theDecoder;
	QSettings	*ft8Settings;
	packHandler	unpackHandler;
	reporterWriter	*theWriter;
	void		run		();
	bool		check_crc_bits	(uint8_t *message, int nrBits);
	void		showLine	(int, int, int, const QString &);
	dlCache		theCache;
	struct {
           int lineno;
	   int value;
           int frequency;
           float log174 [174];
        } theBuffer [nrBlocks];

	std::string	homeCall;
        std::string	homeGrid;
        std::string     localAdif;

	Semaphore	freeSlots;
	Semaphore	usedSlots;
	
	std::atomic<bool> running;
	
	std::atomic<int>	maxIterations;
	int		amount;
	int		blockToRead;
	int		blockToWrite;
	std::thread	threadHandle;
        uint8_t         a91 [FT8_M_BYTES];

	void		print_statistics	();
signals:
	void		printLine	(const QString &);
	void		show_pskStatus	(bool);
};
#endif

