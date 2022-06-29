#
/*
 *    Copyright (C) 2010, 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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
#ifndef	__FT8_CONSTANTS__
#define	__FT8_CONSTANTS__

#define nrTONES 79
#define FRAMES_PER_TONE 5
#define nrBUFFERS       (3 * nrTONES * FRAMES_PER_TONE)
#define BINWIDTH (12000.0 / 3840)

#define	FT8_M_BYTES	(10)
#define	FT8_M_BITS	(77)
#define	FT8_LDPC_BITS	174
#define FTX_LDPC_N       (174) 
#define FTX_LDPC_K       (91) 
#define FTX_LDPC_M       (83)    ///< Number of LDPC checksum bits (FTX_LDPC_N - FTX_LDPC_K)
#define FTX_LDPC_N_BYTES ((FTX_LDPC_N + 7) / 8) ///< Number of whole bytes needed to store 174 bits (full message)
#define FTX_LDPC_K_BYTES ((FTX_LDPC_K + 7) / 8) ///< Number of whole bytes needed to store 91 bits (payload + CRC only)
#endif

