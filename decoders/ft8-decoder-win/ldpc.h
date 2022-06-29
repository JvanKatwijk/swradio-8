#
/*
 *    Copyright (C) 2022
 *    Karlis Goba
 *    YL33G
 *
 *    This file is the :.h" file for the class made for the ldpc
 *    functions imported from the ft8 linrary from Karlis Goba
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

#ifndef _INCLUDE_LDPC_H_
#define _INCLUDE_LDPC_H_

#include <stdint.h>
#include	"ft8-constants.h"

class ldpc {
public:
		ldpc	();
		~ldpc	();
		
	void	ldpc_decode (float codeword[], int max_iters,
	                                    uint8_t plain[], int* ok);
	void	bp_decode (float codeword[], int max_iters,
	                                    uint8_t plain[], int* ok);
private:

	int	ldpc_check	(uint8_t codeword []);
	float	fast_tanh	(float x);
	float	fast_atanh	(float x);
};

#endif
