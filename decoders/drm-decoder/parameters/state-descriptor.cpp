#
/*
 *    Copyright (C) 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRuno plugin for drm
 *
 *    drm plugin is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm plugin is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with drm plugin; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#include	"state-descriptor.h"
#include	<stdio.h>
#include	<string.h>

	stateDescriptor::stateDescriptor (uint8_t Mode, uint8_t Spectrum) {
	this	-> Mode		= Mode;
	this	-> Spectrum	= Spectrum;
	this	-> audio_channel_1	= true;;
	this	-> audio_channel_2	= false;
	this	-> RMflag	= 0;
	this	-> sdcMode	= 0;
	this	-> numofStreams	= 0;
	this	-> set		= false;
	for (int i = 0; i < 3; i ++)
	   memset (&streams [i], 0, sizeof (streams [0]));
}

	stateDescriptor::~stateDescriptor	(void) {
}

int	stateDescriptor::getAudioChannel	() {
	return audio_channel_1 ? 0 :
	       audio_channel_2 ? 1 : 0;
}

void	stateDescriptor::cleanUp	() {
	this    -> audio_channel_1              = true;
        this    -> audio_channel_2              = false;
        this    -> RMflag       = 0;
        this    -> sdcMode      = 0;
        this    -> numofStreams = 0;
        this    -> set          = false;
        for (int i = 0; i < 3; i ++)
           memset (&streams [i], 0, sizeof (streams [0]));
}

void	stateDescriptor::activate_channel_1	() {
	audio_channel_1	= true;
	audio_channel_2	= false;
}

void	stateDescriptor::activate_channel_2	() {
	if (streams [1]. soort == AUDIO_STREAM)  {
	   audio_channel_2	= true;
	   audio_channel_1	= false;
	}
}

