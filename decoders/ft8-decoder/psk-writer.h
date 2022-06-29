
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
#ifndef	__REPORTER_WRITER__
#define	__REPORTER_WRITER__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include	<string>
#include	<vector>
#include	<mutex>
#ifndef	__MINGW32__
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#else
#include        <winsock2.h>
#include        <windows.h>
#endif
#include	<QSettings>

typedef struct {
        std::string     call;
        std::string     grid;
        int             freq;   // in KHz
        int             snr;
	int		seconds;
} pskMessage;


class reporterWriter {
public:
		reporterWriter (QSettings *settings);
		~reporterWriter	();
	bool	reporterReady	();
	void	addMessage	(std::string call,
	                         std::string grid,
	                         int Frequency,
	                         int snr);

	int	sendMessages	();
private:
	std::string		homeCall;
	std::string		homeGrid;
	std::string		programName;
	std::string		antenna;
	bool			reporterOK;
	std::mutex		locker;
	std::vector<pskMessage> messageStack;
	int	copy_char	(uint8_t *pointer, const char *value);
	int	copy_int1	(uint8_t *pointer, int8_t value);
	int	copy_int2	(uint8_t *pointer, int16_t value);
	int	copy_int4	(uint8_t *pointer, int32_t value);

	int	counter;
	int	sequence;
	int	offset_for_size;
	int	offset_for_transmission_time;
	int	offset_for_sequence_number;
	int	offset_for_random;
	uint8_t buffer [1024];
	int	sock;
	struct sockaddr_in addr;
	int	data_offset;
	int	start_of_transmitters;
};

#endif

