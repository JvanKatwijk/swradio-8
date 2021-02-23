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
 */

#ifndef	__FEC_HANDLER
#define	__FEC_HANDLER

#include	<stdint.h>
#include	"reed-solomon.h"
class	packetAssembler;

class	fecHandler {
public:
			fecHandler	(packetAssembler *);
			~fecHandler	(void);
	void		checkParameters	(int16_t R, int16_t C, int16_t l, int16_t);
	void		fec_packet	(uint8_t *v, int16_t l);
	void		data_packet	(uint8_t *v, int16_t l);
private:
	packetAssembler	*my_packetAssembler;
	int16_t		mscIndex;
	int16_t		old_CI;
	uint8_t		**table_i;
	uint8_t		**table_o;
	reedSolomon	my_rsDecoder;
	int16_t		f_cut;
	int16_t		Rows;
	int16_t		Columns;
	int16_t		packetLength;
	int16_t		inCount;
	int16_t		packetCount;
	int16_t		fecPackets;
};


#endif
