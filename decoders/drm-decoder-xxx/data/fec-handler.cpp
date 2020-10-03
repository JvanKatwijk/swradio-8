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
#
#include	<stdio.h>
#include	"fec-handler.h"
#include	"packet-assembler.h"
//
//	
	fecHandler::fecHandler	(packetAssembler *p)
	                            :my_rsDecoder (8, 0435, 0, 1, 16) {
int16_t i;
	my_packetAssembler	= p;
	this	-> mscIndex	= 0;
//	dummies for initialization
	this	-> Rows		= 4;
	this	-> Columns	= 2;
	this	-> table_i	= new uint8_t *[Rows];
	this	-> table_o	= new uint8_t *[Rows];
	for (i = 0; i < Rows; i ++) {
	   table_i [i] = new uint8_t [Columns];
	   table_o [i] = new uint8_t [Columns];
	}
	packetCount	= 0;
	fecPackets	= 0;
}

	fecHandler::~fecHandler	(void) {
int16_t	i;
	for (i = 0; i < Rows; i ++) {
	   delete [] table_i [i];
	   delete [] table_o [i];
	}
	delete [] table_i;
	delete [] table_o;
}

void	fecHandler::checkParameters	(int16_t R, int16_t C,
	                                 int16_t l, int16_t mscIndex) {
int16_t	i;

	this	-> mscIndex	= mscIndex;
	if ((R == Rows) && (C == Columns))
	   return;

	for (i = 0; i < Rows; i ++) {
	   delete[] table_i [i];
	   delete[] table_o [i];
	}
	delete [] table_i;
	delete [] table_o;
	table_i	= new uint8_t * [R];
	table_o	= new uint8_t * [R];
	for (i = 0; i < R; i ++) {
	   table_i [i] = new uint8_t [C + 16];
	   table_o [i] = new uint8_t [C];
	}
	Rows		= R;
	Columns		= C;
	packetLength	= l;
	inCount		= R * C / packetLength;
	fecPackets	= 0;
	f_cut		= (255 - 16) - Columns;
}
//
void	fecHandler::fec_packet		(uint8_t *v, int16_t l) {
int16_t	i, j;

	(void)l;
	if (packetCount != inCount) {
	   packetCount = 0;
	   return;
	}
//
//	first, we fill the remaining slots - if any - with 0
	for (i = packetCount * packetLength;
	     i < Rows * Columns; i ++) 
	   table_i [i % Rows][ i / Rows] = 0;
//
//	the parity bytes
	uint16_t mmin = packetLength - 3 > 16 * Rows ? 16 * Rows : packetLength - 3;
	for (i = 0; i < mmin ; i ++) 
	   table_i [i % Rows][Columns + i / Rows] = v [i + 1];
//
//	Most likely not happening, but fill up the remaining bytes in the
//	parity block
	for (i = mmin; i < 16 * Rows; i ++)
	   table_i [i % Rows][Columns + i / Rows] = 0;

	packetCount	= 0;
	bool	good = true;
	for (i = 0; i < Rows; i ++) {
	   int16_t x = my_rsDecoder. dec (table_i [i], table_o [i], f_cut);
	   if (x < 0)
	      good = false;
	}
	if (!good) 
	   return;		// sorry
//
//	extracting the packets and passing them on
	for (i = 0; i < inCount; i ++) {
	   uint8_t packetBuffer [packetLength];
	   for (j = 0; j < packetLength; j ++) {
	      int16_t row	= (i * packetLength + j) % Rows;
	      int16_t column	= (i * packetLength + j) / Rows;
	      packetBuffer [j]	= table_o [row][column];
	   }
	   my_packetAssembler	-> assemble (packetBuffer,
	                                     packetLength, mscIndex);
	}
	packetCount	= 0;
}

void	fecHandler::data_packet		(uint8_t *v, int16_t l) {
int16_t	i;
	(void)l;
	fecPackets	= 0;
	if (packetCount >= inCount)
	   return;		// something is rotten

	for (i = 0; i < packetLength; i ++) {
	   int16_t c	= (packetCount * packetLength + i) / Rows;
	   int16_t r	= (packetCount * packetLength + i) % Rows;
	   table_i [r][c] = v [i];
	}
	packetCount ++;
}


