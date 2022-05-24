#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRunoPlugin_drm
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
#ifndef	__DEINTERLEAVER__
#define	__DEINTERLEAVER__

#include	<stdint.h>
#include	"basics.h"
#include	"mapper.h"
//
//	Since we have a "short" and a "long"
//	interleaver, we create an "mother" class
//	from which the two are derived
//
class deInterleaver {
public:
		deInterleaver	(int16_t);
virtual		~deInterleaver	(void);
virtual	void	deInterleave	(theSignal *, theSignal *);
private:
protected:
	int16_t	Length;
	Mapper	mapper;
};
//

class	deInterleaver_long: public deInterleaver {
public:
		deInterleaver_long	(int16_t length, int16_t depth);
		~deInterleaver_long	(void);
void		deInterleave		(theSignal *in, theSignal *out);
private:
	int16_t		depth;
	theSignal	**buffer;
};
#endif


