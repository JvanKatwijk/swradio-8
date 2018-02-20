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
 *	We have to create a simple virtual class here, since we
 *	want the interface with different devices (including  filehandling)
 *	to be transparent
 */
#ifndef	__VIRTUAL_INPUT__
#define	__VIRTUAL_INPUT__

#include	<stdint.h>
#include	"radio-constants.h"
#include	<QObject>
#include	<QThread>
#include	<QDialog>


/**
  *	\class virtualInput
  *	base class for devices for the fm software
  *	The class is not completely virtual, since it is
  *	used as a default in case selecting a "real" class did not work out
  */

#include	<stdint.h>

class	RadioInterface;
class	virtualInput: public QThread {
Q_OBJECT
public:
			virtualInput 	(RadioInterface *);
virtual			~virtualInput 	(void);
virtual		int32_t	getRate		(void);
virtual		void	setVFOFrequency	(int32_t);
virtual		int32_t	getVFOFrequency	(void);
virtual		bool	restartReader	(void);
virtual		void	stopReader	(void);
virtual		void	resetBuffer	(void);
virtual		int16_t	bitDepth	(void);
	        int32_t	vfoOffset;
//
protected:
		int32_t	lastFrequency;
signals:
		void	dataAvailable	(int);
};
#endif

