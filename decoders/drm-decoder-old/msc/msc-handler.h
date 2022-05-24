#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDR-J (JSDR).
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
#
#ifndef	__MSC_HANDLERS__
#define	__MSC_HANDLERS__

#include	<QObject>
#include	"radio-constants.h"
#include	"basics.h"
#include	<cstring>

class	drmDecoder;
class	stateDescriptor;
class	prbs;
//
//	empty base class for the different msc Handlers

class	mscHandler : public QObject {
Q_OBJECT
public:
		mscHandler 	(drmDecoder *, stateDescriptor *);
virtual		~mscHandler	();
virtual	void	process		(theSignal *, uint8_t *);
protected:
	drmDecoder	*master;
	stateDescriptor	*theState;
	int16_t		muxLength;
	prbs		*thePRBS;
signals:
	void		show_mer	(float);
};
#endif

