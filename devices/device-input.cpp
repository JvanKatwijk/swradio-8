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
 * 	Default (void) implementation of
 * 	virtual input class
 */
#include	"device-input.h"
#include	"radio.h"

	deviceInput::deviceInput	(RadioInterface *radio) {
	lastFrequency	= Mhz (100);
}

	deviceInput::~deviceInput	(void) {
}

int32_t	deviceInput::getRate		(void) {
	return 192000;
}

void	deviceInput::setVFOFrequency	(quint64 f) {
	lastFrequency	= f;
}

quint64	deviceInput::getVFOFrequency	(void) {
	return lastFrequency;
}

bool	deviceInput::restartReader	(void) {
	return true;
}

void	deviceInput::stopReader	(void) {
}

void	deviceInput::resetBuffer	(void) {
}

int16_t	deviceInput::bitDepth		(void) {
	return 10;
}

