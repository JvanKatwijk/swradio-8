#
/*
 *    Copyright (C) 2022
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the swradio
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

#include	"ft8-constants.h"
#include	"ft8-decoder.h"
#include	"ldpc.h"
#include	"pack-handler.h"
#include	"ft8-processor.h"

	ft8_processor::ft8_processor	(ft8_Decoder *theDecoder,
	                                 int maxIterations):
	                                        freeSlots (nrBlocks) {
	this	-> theDecoder	= theDecoder;
	this	-> maxIterations = maxIterations;
	this	-> blockToRead	= 0;
	this	-> blockToWrite	= 0;

	running. store (false);
	connect (this,
	         SIGNAL (showLine (const QString &, float, int, const QString &)),
	         theDecoder,
	         SLOT (showLine (const QString &, float, int, const QString &)));
	threadHandle    = std::thread (&ft8_processor::run, this);
}

	ft8_processor::~ft8_processor	() {
	if (running. load ()) {
           running. store (false);
           sleep (1);
           threadHandle. join ();
        }
}
//
//	entry for processing
void	ft8_processor::PassOn (int lineno,
	                       float refVal, int frequency, float *log174) {
        while (!freeSlots. tryAcquire (200)) 
	   if (!running. load ())
	      return;
	theBuffer [blockToWrite]. lineno	= lineno;
	theBuffer [blockToWrite]. value 	= refVal;
	theBuffer [blockToWrite]. frequency	= frequency;
	for (int i = 0; i < FT8_LDPC_BITS; i ++)
	   theBuffer [blockToWrite]. log174 [i] = log174 [i];
	blockToWrite = (blockToWrite + 1 ) % nrBlocks;;
	usedSlots. Release ();
}

void	ft8_processor::set_maxIterations	(int n) {
	maxIterations. store (n);
}

void	ft8_processor::run () {
uint8_t ldpcBits [FT8_LDPC_BITS];
ldpc ldpcHandler;
int	errors;
	running. store (true);
        while (running. load()) {
	   while (!usedSlots. tryAcquire (200)) 
	      if (!running. load ())
	         return;

//	   ldpcHandler. bp_decode (theBuffer [blockToRead]. log174,
//	                           maxIterations. load (),
//	                           ldpcBits, &errors);
	   ldpcHandler. ldpc_decode (theBuffer [blockToRead]. log174,
	                             maxIterations. load (),
	   	                     ldpcBits, &errors);

//	   Release the buffer now
	   freeSlots. Release ();
           blockToRead = (blockToRead + 1) % (nrBlocks);
//
	   if (errors != 0) 
	      continue;

//	the check bits are to be moved from 77 up to 82 up
//	and 77 .. 82 should be 0
	   for (int i = 96; i > 82; i --)
	      ldpcBits [i] = ldpcBits [i - 14];

	   for (int i = 77; i < 82; i ++)
	      ldpcBits [i] = 0;

	   if (check_crc_bits (ldpcBits, 96)) {
//	crc is correct, unpack  the message
	      QString res = unpackHandler. unpackMessage (ldpcBits);
	      if (res != "") {
	         time_t rawTime;
	         struct tm *timeinfo;
	         char buffer [100];
	         time (&rawTime);
	         timeinfo = localtime (&rawTime);
	         strftime (buffer, 80, "%I:%M:%S%p", timeinfo);
	         QString s = QString (buffer);
	         showLine (s,
	                   theBuffer [blockToRead]. value,
	                   theBuffer [blockToRead]. frequency, res);
	      }
	   }
        }
}

static
uint8_t crcPolynome [] = 
	{1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1};	// MSB .. LSB
bool	ft8_processor::check_crc_bits (uint8_t *in, int nrBits) {
uint8_t	b [14];
int16_t	Sum	= 0;

	memset (b, 0, 14);

	for (int i = nrBits - 14; i < nrBits; i ++)
	   in [i] = ~in [i];

	for (int i = 0; i < nrBits; i++) {
	   if ((b [0] ^ in [i]) == 1) {
	      for (int f = 0; f < 13; f++) 
	         b [f] = b [f + 1]; 
	      b [13] = 0;
	      for (int f = 0; f < 14; f ++)
	         b [f] ^= crcPolynome [f];
	   }
	   else {
	      memmove (&b [0], &b[1], sizeof (uint8_t ) * 13); // Shift
	      b [13] = 0;
	   }
	}
	for (int i = 0; i < 14; i ++)
	   Sum += b [i];
	return Sum == 0;
}

