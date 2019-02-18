#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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
#ifndef	__QAM16_METRICS
#define	__QAM16_METRICS

#include	"radio-constants.h"
#include	"basics.h"

class	qam16_metrics	{
public:
		qam16_metrics		(void);
		~qam16_metrics		(void);
	void	computemetrics		(theSignal *,
	                                 int32_t,	// amount 
	                                 uint8_t,	// level
	                                 metrics	*, // output
	                                 bool,		// first time or retry
	                                 uint8_t	*, // corrected level 0
	                                 uint8_t	* // corrected level 1
	                                );
private:

};

#endif

