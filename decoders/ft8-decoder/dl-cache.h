#
/*
 *    Copyright (C) 2010, 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of swradio
 *
 *    swradio is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    swradio is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with swradio; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef	__DL_CACHE__
#define	__DL_CACHE__

#include	<string>
#include	<vector>
typedef struct {
	float	strength;
	float	frequency;
	std::string theMessage;
} cacheElement;

class	dlCache {
private:
std::vector<cacheElement> cache;
int	p;
int	size;
#define	CACHE_SIZE	32
#define CACHE_MASK	(CACHE_SIZE - 1)
public:
	dlCache	(int size) {
	   (void)size;
	   cache. resize (CACHE_SIZE);
	   this	-> size	= CACHE_SIZE;
	   p = 0;
	}

	~dlCache	() {
	}

void	add		(const std::string &s,
	                 float strength, float frequency) {
	cache [p]. strength = strength;
	cache [p]. frequency = frequency;
	cache [p]. theMessage = s;
	p = (p + 1) & CACHE_MASK;
}

bool	update	(float strength,
	                 float frequency, const std::string &s) {
	for (uint16_t i = p; i < p + CACHE_SIZE; i ++) {
	   if (cache [i & CACHE_MASK]. theMessage == s) {
	      if (cache [i & CACHE_MASK]. strength < strength) {
	         for (uint16_t j = i; j < (p - 1) + CACHE_SIZE; j ++)
	            cache [j & CACHE_MASK] = cache [(j + 1) & CACHE_MASK];
	         cache [(p - 1 + CACHE_SIZE) & CACHE_MASK]. theMessage = s;
	         cache [(p - 1 + CACHE_SIZE) & CACHE_MASK]. strength = strength;
	         cache [(p - 1 + CACHE_SIZE) & CACHE_MASK]. frequency =
	                                                           frequency;
	      
	      }
	      return true;
	   }
	}
	cache [p]. theMessage = s;
	cache [p]. strength = strength;
	cache [p]. frequency = frequency;
	p = (p + 1) & CACHE_MASK;
	return false;
}
};
#endif
