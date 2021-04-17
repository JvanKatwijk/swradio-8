#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the sw radio
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
#
//
//	the real msc work is to be done by descendants of the mscHandler
//
#include	"msc-handler.h"
#include	"state-descriptor.h"
#include	"drm-decoder.h"

	mscHandler::mscHandler	(drmDecoder *m, stateDescriptor *theState) {
	this	-> master	= m;
	this	-> theState	= theState;
	muxLength		= theState	-> mscCells / 3;
	connect (this, SIGNAL (show_mer (float)),
	         master, SLOT (show_mer_msc (float)));
}

	mscHandler::~mscHandler	(void) {
}

void	mscHandler::process	(theSignal *v, uint8_t * o) {
	(void)v; (void)o;
}


