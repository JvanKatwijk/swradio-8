#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the swradio-8
 *
 *    swradio-8 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    swradio-8 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with sw-radio-8; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	We have to create a simple virtual class here, since we
 *	want the interface with different devices (including  filehandling)
 *	to be transparent
 */

#pragma once

#include	<stdint.h>
#include	"radio-constants.h"
#include	<QObject>
#include	<QThread>
#include	<QDialog>


/**
  *	\class deviceHandler
  *	base class for devices for the fm software
  *	The class is not completely virtual, since it is
  *	used as a default in case selecting a "real" class did not work out
  */

#include	<stdint.h>

class	RadioInterface;
class	deviceHandler: public QThread {
Q_OBJECT
public:
			deviceHandler 	(RadioInterface *);
virtual			~deviceHandler 	();
virtual		int32_t	getRate		();
virtual		void	setVFOFrequency	(quint64);
virtual		quint64	getVFOFrequency	();
virtual		bool	restartReader	();
virtual		void	stopReader	();
virtual		void	resetBuffer	();
virtual		int16_t	bitDepth	();
	        int32_t	vfoOffset;
//
protected:
		uint64_t	lastFrequency;
signals:
		void	dataAvailable	(int);
//	The following signals originate from the Winrad Extio interface
//	and are only used by the extio handler
	void	set_ExtFrequency	(int);
	void	set_ExtLO		(int);
	void	set_lockLO		(void);
	void	set_unlockLO		(void);
	void	set_stopHW		(void);
	void	set_startHW		(void);
};

