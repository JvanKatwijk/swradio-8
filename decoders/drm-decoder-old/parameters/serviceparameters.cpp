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
#include	"serviceparameters.h"
#include	<stdio.h>

		serviceParameters::serviceParameters (void) {
	set	= false;
}
		serviceParameters::~serviceParameters (void) {
}

void	serviceParameters::ServiceIdentifier	(uint8_t *v) {
	(void)v;
}

void	serviceParameters::ShortId (uint8_t *v) {
	(void)v;
}

void	serviceParameters::AudioCA (uint8_t *v) {
	(void)v;
}
//
//	Language is a 4 bit field
void	serviceParameters::Language (uint8_t *v) {
uint16_t val = (v [0] << 3) | (v [1] << 2) | (v [2] << 1) | v [3];

	switch (val) {
	   default:		// cannot happen
	   case 0:
	      theLanguage	= "No language specified"; break;
	   case 1:
	      theLanguage	= "Arabic"; break;
	   case 2:
	      theLanguage	= "Bengali"; break;
	   case 3:
	      theLanguage	= "Chinese"; break;
	   case 4:
	      theLanguage	= "Dutch"; break;
	   case 5:
	      theLanguage	= "English"; break;
	   case 6:
	      theLanguage	= "French"; break;
	   case 7:
	      theLanguage	= "German"; break;
	   case 8:
	      theLanguage	= "Hindi"; break;
	   case 9:
	      theLanguage	= "Japanese"; break;
	   case 10:
	      theLanguage	= "Javanese"; break;
	   case 11:
	      theLanguage	= "Korean"; break;
	   case 12:
	      theLanguage	= "Portugese"; break;
	   case 13:
	      theLanguage	= "Russian"; break;
	   case 14:
	      theLanguage	= "Spanish"; break;
	   case 15:
	      theLanguage	= "Other Language"; break;
	}
//	fprintf (stderr, " Language = %s\n", theLanguage);
}

void	serviceParameters::AudioDataflag (uint8_t *v) {
	AudioService	= v [0] == 0;
}

void	serviceParameters::Servicedescriptor (uint8_t *v) {
uint8_t val	= 0;
int16_t	i;
	if (!AudioService)
	   return;		// apparently data service, skip for now

	for (i = 0; i < 5; i ++)
	   val = (val << 1) | (v [i] & 01);

//fprintf (stderr, "programmetype = %d ", val);
	switch (val) {
	   default:		// cannot happen
	   case 0:
	      theProgrammeType	= "no program type"; break;
	   case 1:
	      theProgrammeType	= "News"; break;
	   case 2:
	      theProgrammeType	= "Current Affairs"; break;
	   case 3:
	      theProgrammeType	= "Information"; break;
	   case 4:
	      theProgrammeType	= "Sport"; break;
	   case 5:
	      theProgrammeType	= "Education"; break;
	   case 6:
	      theProgrammeType	= "Drama"; break;
	   case 7:
	      theProgrammeType	= "Culture"; break;
	   case 8:
	      theProgrammeType	= "Science"; break;
	   case 9:
	      theProgrammeType	= "Varied"; break;
	   case 10:
	      theProgrammeType	= "Pop Music"; break;
	   case 11:
	      theProgrammeType	= "Rock Music"; break;
	   case 12:
	      theProgrammeType	= "Easy Listening"; break;
	   case 13:
	      theProgrammeType	= "Light Classical"; break;
	   case 14:
	      theProgrammeType	= "Serious Classical"; break;
	   case 15:
	      theProgrammeType	= "Other Music"; break;
	   case 16:
	      theProgrammeType	= "Wheather"; break;
	   case 17:
	      theProgrammeType	= "Finance/Business"; break;
	   case 18:
	      theProgrammeType	= "Children\'s programmes"; break;
	   case 19:
	      theProgrammeType	= "Social Affairs"; break;
	   case 20:
	      theProgrammeType	= "Religion"; break;
	   case 21:
	      theProgrammeType	= "Phone In"; break;
	   case 22:
	      theProgrammeType	= "Travel"; break;
	   case 23:
	      theProgrammeType	= "Leisure"; break;
	   case 24:
	      theProgrammeType	= "Jazz Music"; break;
	   case 25:
	      theProgrammeType	= "Country Music"; break;
	   case 26:
	      theProgrammeType	= "National Music"; break;
	   case 27:
	      theProgrammeType	= "Oldies Music"; break;
	   case 28:
	      theProgrammeType	= "Folk Music"; break;
	   case 29:
	      theProgrammeType	= "Documentary"; break;
	   case 30:
	   case 31:
	      theProgrammeType	= "   "; break;
	}
//	fprintf (stderr, "programme type = %s\n", theProgrammeType);
}

void	serviceParameters::DataCAindication (uint8_t *v) {
	(void)v;
}

