#
/*
 *    Copyright (C) 2015
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
 *
 */
#ifndef	__VIRTUAL_DATAHANDLER
#define	__VIRTUAL_DATAHANDLER
#include	<stdint.h>

class virtual_dataHandler {
public:
		virtual_dataHandler	(void);
virtual		~virtual_dataHandler	(void);
virtual
void		process_mscGroup	(uint8_t *,
	                                 uint8_t,
	                                 bool,
	                                 int16_t,
	                                 uint16_t);
};


#endif

