
/*
 *	Copyright (c) 2015 Thierry Leconte
 *	Copyright 2018, Jan van katwijk, Lazy Chair Computing
 *
 *	The printer code is based on the output.c file
 *	of acars of Thierry Leconte. Added is a simple spool
 *	mechanism and some code is organized another way.
 *
 *   
 *   This code is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2
 *   published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *	
 */
#ifndef	__ACARS_PRINTER__
#define	__ACARS_PRINTER__

#include	<stdio.h>
#include	<stdint.h>
#include	<time.h>
#include        <stdio.h>
#include        <thread>
#include        <mutex>
#include	"semaphore.h"
#include        <atomic>
#include	"acars-constants.h"

class	RadioInterface;

typedef struct {
        unsigned char mode;
        unsigned char addr[8];
        unsigned char ack;
        unsigned char label[3];
        unsigned char bid;
        unsigned char no[5];
        unsigned char fid[7];
        unsigned char bs, be;
        unsigned char txt[250];
        int err, lvl;
	int	channel;	
	int	frequency;
	struct timeval	messageTime;
} acarsmsg_t;

typedef struct {
        char da[5];
        char sa[5];
        char eta[5];
        char gout[5];
        char gin[5];
        char woff[5];
        char won[5];
} oooi_t;

typedef struct flight_s flight_t;
struct flight_s {
        flight_t	*next;
        char		addr [8];
        char		fid [7];
        struct timeval ts, tl;
        int		chm;
        int		nbm;
        oooi_t 		oooi;
};


class	printer {
public:
		printer		(int, int, char *, RadioInterface *);
		~printer	();
	void	output_msg	(int, uint8_t *,
	                         int16_t, float, struct timeval);
	void	start		();
	void	stop		();
private:
	void	run		();
	void    process_msg	(int, uint8_t *,
	                          int16_t, float, struct timeval);
	void	printmsg	(acarsmsg_t *);
	void	printdate	(struct timeval t);
	void	printtime	(struct timeval t);
	void    cls		();
	void	handle_json	(acarsmsg_t *msg, struct timeval);
	void	handle_netout	(acarsmsg_t *msg, struct timeval);
	void	show_output	(acarsmsg_t *msg, struct timeval);
	void	printmonitor	(acarsmsg_t * msg);
	void	printoneline	(acarsmsg_t * msg);
	void	addFlight	(acarsmsg_t * msg);
	void	outpp		(acarsmsg_t * msg, struct timeval);
	void	outsv		(acarsmsg_t * msg, struct timeval);
	void	outjson		();
	void	buildJSON	(acarsmsg_t *msg);
	int	connectServer	(char *addr);
	RadioInterface		*theRadio;
	uint8_t	*jsonbuf;
	flight_t  *flight_head;
	int	outtype;
	int	sockfd;
	char	*rawAddr;
	int	channels;
	int	theSlots;
	std::atomic<bool>       running;
        std::thread		threadHandle;
	Semaphore       freeSlots;
        Semaphore       usedSlots;
	int		numberofSlots;
        int16_t         nextIn;
        int16_t         nextOut;
	char		*idstation;

	struct {
	   int	channel;
	   int	frequency;
	   uint8_t	blk_txt [255];
	   int	blk_len;
	   float	blk_lvl;
	   struct timeval blk_tm;
	} theData [20];
};

#endif
