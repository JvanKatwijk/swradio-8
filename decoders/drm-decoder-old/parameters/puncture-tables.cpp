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
#include	"puncture-tables.h"

static	uint8_t	table_12 []	= {1, 1, 0, 0, 0, 0};
static	uint8_t	table_13 []	= {1, 1, 1, 0, 0, 0};
static	uint8_t table_14 []	= {1, 1, 1, 1, 0, 0};
static	uint8_t table_15 []	= {1, 1, 1, 1, 1, 0};
static	uint8_t table_16 []	= {1, 1, 1, 1, 1, 1};

static	uint8_t	table_23 []	= {1, 1, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0};
static	uint8_t	table_25 [] 	= {1, 1, 1, 0, 0, 0,
	                           1, 1, 0, 0, 0, 0};

static	uint8_t	table_34 []	= {1, 1, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0};
static	uint8_t	table_35 []	= {1, 1, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 1, 0, 0, 0, 0};
static	uint8_t	table_310 []	= {1, 1, 1, 1, 0, 0,
	                           1, 1, 1, 0, 0, 0,
	                           1, 1, 1, 0, 0, 0};

static	uint8_t	table_411 []	= {1, 1, 1, 0, 0, 0,
	                           1, 1, 1, 0, 0, 0,
	                           1, 1, 1, 0, 0, 0,
	                           1, 1, 0, 0, 0, 0};
static	uint8_t	table_45 []	= {1, 1, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0};
static	uint8_t	table_47 []	= {1, 1, 0, 0, 0, 0,
	                           1, 0, 1, 0, 0, 0,
	                           1, 1, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0};

static	uint8_t	table_78 []	= {1, 1, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0};
static	uint8_t	table_89 []	= {1, 1, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0};

static	uint8_t	table_811 []	= {1, 1, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 1, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0,
	                           1, 1, 0, 0, 0, 0,
	                           1, 0, 0, 0, 0, 0};

	punctureTables::punctureTables (void) {
}

	punctureTables::~punctureTables (void) {
}

uint8_t	*punctureTables::getPunctureTable (uint8_t RX, uint8_t RY)  {
	switch (RX) {
	   default:
	   case 1:
	      switch (RY) {
	         default:
	         case 2:	return table_12;
	         case 3:	return table_13;
	         case 4:	return table_14;
	         case 5:	return table_15;
	         case 6:	return table_16;
	      }

	   case 2:
	      return RY == 3 ? table_23 : table_25;

	   case 3:
	      return RY == 4 ? table_34 : RY == 5 ? table_35 : table_310;

	   case 4:
	      return RY == 5 ? table_45 : RY == 7 ? table_47 : table_411;

	   case 7:
	      return table_78;

	   case 8:
	      return RY == 9 ? table_89 : table_811;
	}
}

static
uint8_t residuTables [12][36] = {
/* 0 */	{1, 1, 0, 0, 0, 0, 
	 1, 1, 0, 0, 0, 0, 
	 1, 1, 0, 0, 0, 0,
	 1, 1, 0, 0, 0, 0,  
	 1, 1, 0, 0, 0, 0, 
	 1, 1, 0, 0, 0, 0},
/* 1 */	{1, 1, 1, 0, 0, 0,  1, 1, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0,
	 1, 1, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0},
/* 2 */	{1, 1, 1, 0, 0, 0,  1, 1, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0,
	 1, 1, 0, 1, 0, 0,  1, 1, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0},
/* 3 */	{1, 1, 1, 0, 0, 0,  1, 1, 1, 0, 0, 0,  1, 1, 0, 0, 0, 0,
	 1, 1, 1, 0, 0, 0,  1, 1, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0},
/* 4 */	{1, 1, 1, 0, 0, 0,  1, 1, 1, 0, 0, 0,  1, 1, 0, 0, 0, 0,
	 1, 1, 1, 0, 0, 0,  1, 1, 1, 0, 0, 0,  1, 1, 0, 0, 0, 0},
/* 5 */	{1, 1, 1, 0, 0, 0,  1, 1, 1, 0, 0, 0,  1, 1, 1, 0, 0, 0,
	 1, 1, 1, 0, 0, 0,  1, 1, 1, 0, 0, 0,  1, 1, 0, 0, 0, 0},
/* 6 */	{1, 1, 1, 0, 0, 0,  1, 1, 1, 0, 0, 0,  1, 1, 1, 0, 0, 0,
	 1, 1, 1, 0, 0, 0,  1, 1, 1, 0, 0, 0,  1, 1, 1, 0, 0, 0},
/* 7 */	{1, 1, 1, 1, 0, 0,  1, 1, 1, 0, 0, 0,  1, 1, 1, 0, 0, 0,
	 1, 1, 1, 0, 0, 0,  1, 1, 1, 0, 0, 0,  1, 1, 1, 0, 0, 0},
/* 8 */	{1, 1, 1, 1, 0, 0,  1, 1, 1, 0, 0, 0,  1, 1, 1, 0, 0, 0,
	 1, 1, 1, 1, 0, 0,  1, 1, 1, 0, 0, 0,  1, 1, 1, 0, 0, 0},
/* 9 */	{1, 1, 1, 1, 0, 0,  1, 1, 1, 1, 0, 0,  1, 1, 1, 0, 0, 0,
	 1, 1, 1, 1, 0, 0,  1, 1, 1, 0, 0, 0,  1, 1, 1, 0, 0, 0},
/* 10*/	{1, 1, 1, 1, 0, 0,  1, 1, 1, 1, 0, 0,  1, 1, 1, 0, 0, 0,
	 1, 1, 1, 1, 0, 0,  1, 1, 1, 0, 0, 0,  1, 1, 1, 1, 0, 0},
/* 11*/	{1, 1, 1, 1, 0, 0,  1, 1, 1, 1, 0, 0,  1, 1, 1, 1, 0, 0,
	 1, 1, 1, 1, 0, 0,  1, 1, 1, 0, 0, 0,  1, 1, 1, 1, 0, 0},
};

uint8_t	*punctureTables::getResiduTable	(uint8_t RX,
	                                 uint8_t RY, int16_t nrCells) {
int16_t	residuIndex	= (2 * nrCells - 12) - RY * ((2 * nrCells - 12) / RY);
	(void)RX;
	return residuTables [residuIndex];
}

int16_t	punctureTables::getResiduBits (uint8_t RX,
	                               uint8_t RY, int16_t nrCells) {
	(void)RX;
	return 12 + (2 * nrCells - 12) - RY * ((2 * nrCells - 12) / RY);
}


//


