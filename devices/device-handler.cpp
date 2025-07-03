#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the sw radio
 *
 *    sw radio is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    sw radio is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with sw radio; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"device-handler.h"
#include	"radio.h"

	deviceHandler::deviceHandler	(RadioInterface *radio) {
	lastFrequency	= Mhz (100);
}

	deviceHandler::~deviceHandler	(void) {
}

int32_t	deviceHandler::getRate		(void) {
	return 192000;
}

void	deviceHandler::setVFOFrequency	(quint64 f) {
	lastFrequency	= f;
}

quint64	deviceHandler::getVFOFrequency	(void) {
	return lastFrequency;
}

bool	deviceHandler::restartReader	(void) {
	return true;
}

void	deviceHandler::stopReader	(void) {
}

void	deviceHandler::resetBuffer	(void) {
}

int16_t	deviceHandler::bitDepth		(void) {
	return 10;
}

