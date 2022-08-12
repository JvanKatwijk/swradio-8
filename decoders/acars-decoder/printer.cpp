/*
 *	Copyright (c) 2015 Thierry Leconte
 *	Copyright (c) 2018
 *	Jan van Katwijk, Lazy Chair Computing
 *	This file is based on and a rewrite of acars.c and output.c
 *	of the original acars software of Thierry Leconte
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
 */
#include	"printer.h"
#include	<cstring>
#include	<sys/types.h>
#include	<sys/time.h>
#include	<sys/socket.h>
#include	<time.h>
#include	<netdb.h>
#include	"radio.h"
#include	"label.c"
#include	"acars-constants.h"
#include	"cJSON.h"

#define	JSONBUFLEN	3000

	printer::printer (int theSlots, int outtype,
	                        char * RawAddr, RadioInterface *mr):
	                                           freeSlots (theSlots) {
	this	-> outtype	= outtype;
	this	-> theSlots	= theSlots;
	this	-> rawAddr	= RawAddr;
	this	-> theRadio	= mr;
	flight_head		= nullptr;
	nextIn                  = 0;
	nextOut                 = 0;
	char sys_hostname[8];
	gethostname (sys_hostname, sizeof (sys_hostname));
        idstation = strndup(sys_hostname, 8);

	fprintf (stderr, "outtype = %o\n", outtype);
	start ();

	if ((outtype & OUTTYPE_JSON) != 0)
	   jsonbuf	= new uint8_t [3000];
	else
	   jsonbuf	= nullptr;

	if ((outtype & OUTTYPE_MONITOR) != 0) {
	   fprintf (stderr, "een monitor\n");
//	   verbose = 0;
	   cls ();
	   fflush (stdout);
        }

	if (((outtype & ANY_NETOUT) == 0) || (rawAddr == NULL)) 
	   return;

	sockfd = connectServer (rawAddr);
	if (sockfd < 0)
	   outtype &= ~ANY_NETOUT;	// kill any desire to output on the net
	else
	   fprintf (stderr, "connected\n");
}

int	printer::connectServer (char *rawAddr) {
char *addr;
char *port;
struct addrinfo hints, *servinfo, *p;
int rv, sockfd;

	fprintf (stderr, "connecting to %s\n", rawAddr);
	sockfd	= -1;		// default
	memset (&hints, 0, sizeof hints);
	if (rawAddr [0] == '[') {
	   hints.ai_family = AF_INET6;
	   addr = rawAddr + 1;
	   port = strstr (addr, "]");
	   if (port == NULL) {
	      fprintf(stderr, "Invalid IPV6 address\n");
	      return sockfd;
	   }
	   *port = 0;
	   port++;
	   if (*port != ':')
	      port =  (char *)"5555";
	   else
	      port ++;
	} else {
	   hints.ai_family = AF_UNSPEC;
	   addr = rawAddr;
	   port = strstr (addr, ":");
	   if (port == NULL)
	      port = (char *)"5555";
	   else {
	      *port = 0;
	      port++;
	   }
	}

	hints. ai_socktype = SOCK_DGRAM;
	if ((rv = getaddrinfo (addr, port, &hints, &servinfo)) != 0) {
	   fprintf (stderr, "Invalid/unknown address %s\n", addr);
	   return sockfd;
	}

	for (p = servinfo; p != NULL; p = p -> ai_next) {
	   if ((sockfd = socket (p -> ai_family,
	                         p -> ai_socktype, p->ai_protocol)) == -1) {
	      continue;
	   }

	   if (connect (sockfd, p -> ai_addr, p -> ai_addrlen) == -1) {
	      close (sockfd);
	      continue;
	   }

	   fprintf (stderr, "bingo\n");
	   break;
	}

	if (p == NULL) {
	   fprintf(stderr, "failed to connect\n");
	   sockfd = -1;
	   return sockfd;
	}

	freeaddrinfo (servinfo);
	return sockfd;
}

	printer::~printer (void) {
	if (running. load ()) {
	   running. store (false);
	   threadHandle. join ();
	}
}

void	printer::start	(void) {
        running. store (true);
        threadHandle = std::thread (&printer::run, this);
}

void	printer::stop	(void) {
	if (running. load ()) {
	   running. store (false);
	   threadHandle. join ();
	}
}

void	printer::output_msg (int frequency,  uint8_t *blk_txt,
	                     int16_t blk_len,
	                     float blk_lvl,  struct timeval blk_tm) {
	while (!freeSlots. tryAcquire (200))
	   if (!running. load ())
	      return;

	fprintf (stderr, "adding a message\n");
	memcpy (&theData [nextIn]. blk_txt, blk_txt, blk_len);
	theData [nextIn]. blk_len = blk_len;
	theData [nextIn]. blk_tm  = blk_tm;
	theData [nextIn]. blk_lvl	= blk_lvl;
	theData [nextIn]. channel = 1;
	theData [nextIn]. frequency = 0000;
        nextIn = (nextIn + 1) % theSlots;
        usedSlots. Release ();
}

void	printer::run (void) {
	fprintf (stderr, "printer is running\n");
	running. store (true);
	while (running. load ()) {
	   while (!usedSlots. tryAcquire (200))
	      if (!running)
	         return;
           process_msg (theData [nextOut]. frequency,
	                theData [nextOut]. blk_txt,
	                theData [nextOut]. blk_len,
	                theData [nextOut]. blk_lvl,
	                theData [nextOut]. blk_tm);
	   nextOut = (nextOut + 1) % theSlots;
	   freeSlots. Release ();
        }
}

void	printer::process_msg (int	frequency,
	                      uint8_t	*blk_txt,
	                      int16_t	blk_len,
	                      float	blk_lvl,
	                      struct timeval blk_tm) {
acarsmsg_t msg;
int	i, k = 0;
bool	messageFlag	= false;

	fprintf (stderr, "message on frequency %d: ", frequency);
	msg. channel		= 1;
	msg. frequency		= frequency;
	msg. messageTime	= blk_tm;	
	msg. mode		= blk_txt [k++];
	msg. err		= 0;
	msg. lvl		= blk_lvl;

	for (i = 0; i < 7; i ++, k ++) 
	   msg. addr [i] = blk_txt [k];
	msg. addr [7] = 0;
//
//	ACK/NAK
	msg. ack	= blk_txt [k++];
//	msg. ack	= 'X';

	msg. label [0]	= blk_txt [k++];
	msg. label [1]	= blk_txt [k++];
	if (msg .label [1] == 0x7f)
	   msg. label [1] = 'd';
	msg. label [2] = '\0';

        msg. bid	= blk_txt [k++];

	/* txt start  */
        msg. bs		= blk_txt [k++];

        msg. no  [0]	= '\0';
        msg. fid [0]	= '\0';
        msg. txt [0]	= '\0';

        if ((msg. bs == 0x03 || msg. mode > 'Z') && false)
	   return;

	if (msg.bs != 0x03) {
	   if ((msg.mode <= 'Z') && (msg. bid <= '9')) {
//	message no 
	      for (i = 0; i < 4 && k < blk_len - 1; i++, k++) 
	         msg. no [i] = blk_txt [k];

	      msg. no [i] = '\0';

//	Flight id 
	      for (i = 0; i < 6 && k < blk_len - 1; i++, k++) 
	         msg. fid [i] = blk_txt [k];

	      msg. fid [i] = '\0';
	      messageFlag	= true;
	   }

//	 Message txt 
	   for (i = 0; k < blk_len - 1; i++, k++)
	      msg. txt [i] = blk_txt [k];
	   msg. txt [i] = 0;
        }
//	txt end 
	msg. be = blk_txt [blk_len - 1];

	fprintf (stderr, "outtype is hiero %d\n", outtype);
	if (((outtype & OUTTYPE_MONITOR) != 0) && messageFlag)
	   addFlight (&msg);

	if ((outtype & (ANY_OUTPUT)) != 0)
	   show_output (&msg, blk_tm);

	if ((outtype & (NETLOG_JSON |OUTTYPE_JSON)) != 0) {
	   handle_json (&msg, blk_tm);
	   return;
	}

	if ((outtype & ANY_NETOUT) != 0)
	   handle_netout (&msg, blk_tm);

}

void	printer::handle_json (acarsmsg_t *msg, struct timeval blk_tm) {
	buildJSON (msg);
//	if ((outtype & NETLOG_JSON) != 0)
//	   outjson ();
	if ((outtype & OUTTYPE_JSON) != 0)
	   fprintf (stderr, "%s\n", jsonbuf);
}

void	printer::handle_netout (acarsmsg_t *msg, struct timeval blk_tm) {
	if ((outtype & NETLOG_PLANEPLOTTER) != 0)
	   outpp (msg, blk_tm);
	else
	if ((outtype & NETLOG_NATIVE) != 0)
	   outsv (msg, blk_tm);
}

void	printer::show_output (acarsmsg_t *msg, struct timeval tv) {
	fprintf (stderr, "output ....\n");
	switch (outtype) {
	   default:		// should not happen
	      break;

	   case OUTTYPE_STD:
	      printmsg (msg);
	      break;
	
	   case OUTTYPE_ONELINE:
	      printoneline (msg);
	      break;

	   case OUTTYPE_MONITOR:
	      printmonitor (msg);
	      break;

	}
}

void	printer::outjson () {
char pkt [500];

        snprintf(pkt, sizeof(pkt), "%s\n", jsonbuf);
        write (sockfd, pkt, strlen (pkt));
}

void	printer::printmsg (acarsmsg_t *msg) {
oooi_t	oooi;
int	frequency = theRadio -> tunedFrequency ();

	fprintf (stderr, "\n[#%1d (F:%3.3f L:%+3d E:%1d) ", 1,
                         frequency / 1000000.0, msg->lvl, msg->err);

	printdate (msg -> messageTime);
	fprintf (stderr, " --------------------------------\n");
	fprintf (stderr, "Mode : %1c ", msg -> mode);
	fprintf (stderr, "Label : %2s ", msg -> label);
	if (msg -> bid) {
	   fprintf (stderr, "Id : %1c ", msg -> bid);
	   if (msg -> ack == 0x15)
	      fprintf (stderr, "Nak\n");
	   else
	      fprintf (stderr, "Ack : %1c\n", msg -> ack);
	   fprintf (stderr, "Aircraft reg: %s ", msg -> addr);
	   if (msg->mode <= 'Z') {
	      fprintf (stderr, "Flight id: %s\n", msg -> fid);
	      fprintf (stderr, "No: %4s", msg -> no);
	   }
	}

	fprintf (stderr, "\n");
        if (msg -> txt [0])
	   fprintf (stderr, "%s\n", msg->txt);
        if (msg -> be == 0x17)
	   fprintf (stderr, "ETB\n");

	if (DecodeLabel (msg, &oooi)) {
	   fprintf (stderr, "##########################\n");
	   if (oooi. da [0])
	      fprintf (stderr, "Destination Airport : %s\n", oooi. da);
	   if (oooi. sa [0])
	      fprintf (stderr, "Departure Airport : %s\n", oooi. sa);
	   if (oooi. eta [0])
	      fprintf (stderr, "Estimation Time of Arrival : %s\n", oooi.eta);
	   if (oooi. gout [0])
	      fprintf (stderr,"Gate out Time : %s\n",oooi. gout);
	   if (oooi. gin [0])
	      fprintf (stderr, "Gate in Time : %s\n", oooi. gin);
	   if (oooi. woff [0])
	      fprintf (stderr, "Wheels off Time : %s\n", oooi. woff);
	   if (oooi. won [0])
	      fprintf (stderr, "Wheels on Time : %s\n", oooi. won);
        }

	fflush(stderr);
}

void	printer::printdate (struct timeval t) {
struct tm tmp;

	if (t. tv_sec + t. tv_usec == 0)
	   return;

	gmtime_r (&(t. tv_sec), &tmp);

	fprintf (stderr, "%02d/%02d/%04d %02d:%02d:%02d",
                 tmp. tm_mday, tmp. tm_mon + 1, tmp. tm_year + 1900,
                 tmp. tm_hour, tmp. tm_min, tmp. tm_sec);
}

void	printer::printtime (struct timeval t) {
struct tm tmp;

	gmtime_r (&(t. tv_sec), &tmp);
        fprintf (stderr, "%02d:%02d:%02d",
	         tmp. tm_hour, tmp. tm_min, tmp. tm_sec);
}


void	printer::cls(void) {
	printf ("\x1b[H\x1b[2J");
}

void	printer::printmonitor (acarsmsg_t * msg) {
flight_t *fl;

	cls();
	printf ("             Acarsdec monitor "); //printtime(tv);
	printf ("\n Aircraft Flight  Nb Channels     First    DEP   ARR   ETA\n");

	fl	= flight_head;
	while (fl) {
	   int i;
	   printf ("%8s %7s %3d ", fl -> addr, fl -> fid, fl -> nbm);
	   for (i = 0; i < 1; i++)
	      printf ("%c", (fl -> chm & (1 << i)) ?'x':'.');
	   for (;i < 8; i++)
	      printf(" ");
	   printf (" "); printtime (fl -> ts);
	   if (fl -> oooi. sa [0])
	      printf (" %4s ", fl -> oooi. sa);
	   else
	      printf ("      ");
	   if (fl -> oooi. da [0])
	      printf (" %4s ", fl -> oooi. da);
	   else
	      printf ("      ");
	   if (fl -> oooi. eta [0])
	      printf (" %4s ", fl -> oooi. eta);
	   else
	      printf ("      ");
	   printf ("\n");

	   fl = fl -> next;
	}
	fflush(stdout);
}

void printer::printoneline (acarsmsg_t * msg) {
char txt [30];
char *pstr;

	strncpy (txt, (char *)(msg -> txt), 29);
	txt [29] = 0;
	for (pstr = txt; *pstr != 0; pstr++)
	   if (*pstr == '\n' || *pstr == '\r')
	      *pstr = ' ';

	fprintf (stdout, "#%1d (L:%+3d E:%1d) ", 1, msg -> lvl, msg->err);

	printdate (msg -> messageTime);
	fprintf( stdout, " %7s %6s %1c %2s %4s ",
	                      msg -> addr,
	                      msg -> fid,
	                      msg -> mode,
	                      msg -> label, msg -> no);
	fprintf (stdout, "%s", txt);
	fprintf (stdout, "\n");
	fflush (stdout);
}


void	printer::addFlight (acarsmsg_t * msg) {
flight_t *fl,*flp;
oooi_t oooi;

	fl	= flight_head;
	flp	= NULL;
	while (fl != NULL) {
	   if (strcmp ((char *)(msg -> addr), (char *)(fl -> addr)) == 0)
	      break;
	   flp	= fl;
	   fl	= fl -> next;
	}

	if (fl == NULL) {
	   fl = (flight_t *)calloc (1, sizeof (flight_t));
	   strncpy (fl -> addr, (char *)(msg -> addr), 8);
	   fl -> nbm	= 0;
	   fl -> ts	= msg -> messageTime;
	   fl -> chm	= 0;
	   fl -> next	= NULL;
	}

	strncpy (fl -> fid, (char *)(msg -> fid), 7);
	fl -> tl	= msg -> messageTime;
	fl -> chm	|= (1 << 1);
	fl -> nbm	+= 1;

	if (DecodeLabel (msg, &oooi)) {
	   if (oooi. da [0])
	      memcpy (fl -> oooi. da, oooi. da, 5);
	   if (oooi. sa [0])
	      memcpy (fl -> oooi. sa, oooi. sa, 5);
	   if (oooi. eta [0])
	      memcpy (fl -> oooi. eta, oooi. eta, 5);
	   if (oooi. gout [0])
	      memcpy (fl -> oooi. gout, oooi. gout, 5);
	   if (oooi. gin [0])
	      memcpy (fl -> oooi. gin, oooi. gin, 5);
	   if (oooi. woff [0])
	      memcpy (fl -> oooi. woff, oooi. woff, 5);
	   if (oooi. won [0])
	      memcpy (fl -> oooi. won, oooi. won, 5);
	}

	if (flp != NULL) {
	   flp	-> next	= fl -> next;
	   fl	-> next	= flight_head;
	}
	flight_head	= fl;
	flp		= NULL;

	while (fl != NULL) {
	   if (fl -> tl. tv_sec < (msg -> messageTime. tv_sec)) {
	      if (flp != NULL) {
	         flp -> next	= fl -> next;
	         free (fl);
	         fl = flp -> next;
	      } else {
	         flight_head = fl -> next;
	         free (fl);
	         fl	= flight_head;
	      }
	   } else {
	      flp	= fl;
	      fl	= fl -> next;
	   }
	}
}

void printer::outpp (acarsmsg_t * msg, struct timeval blk_tm) {
char pkt[500];
char txt[250];
char *pstr;

	strcpy (txt, (char *)(msg->txt));
	for (pstr = txt; *pstr != 0; pstr++)
	   if (*pstr == '\n' || *pstr == '\r')
	      *pstr = ' ';

	sprintf (pkt, "AC%1c %7s %1c %2s %1c %4s %6s %s",
		 msg -> mode,  msg -> addr, msg -> ack,
	         msg -> label, msg -> bid,  msg -> no,
		 msg -> fid, txt);

	write (sockfd, pkt, strlen (pkt));
}

void printer::outsv (acarsmsg_t * msg, struct timeval tv) {
char pkt [500];
struct tm tmp;

	gmtime_r (& (tv.tv_sec), &tmp);

	sprintf(pkt,
		"%8s %1d %02d/%02d/%04d %02d:%02d:%02d %1d %03d %1c %7s %1c %2s %1c %4s %6s %s",
		idstation, 1 + 1, tmp.tm_mday, tmp.tm_mon + 1,
		tmp.tm_year + 1900, tmp.tm_hour, tmp.tm_min, tmp.tm_sec,
		msg->err, msg->lvl, msg->mode, msg->addr, msg->ack, msg->label,
		msg->bid, msg->no, msg->fid, msg->txt);

	write (sockfd, pkt, strlen (pkt));
}

void	printer::buildJSON (acarsmsg_t *msg) {
cJSON *json_obj;
oooi_t	oooi;
char convert_tmp[8];
double t	= (double)msg -> messageTime.tv_sec +
	              ((double)msg -> messageTime. tv_usec)/1e6;
json_obj	= cJSON_CreateObject ();

        if (json_obj == NULL)
                return;

        cJSON_AddNumberToObject (json_obj, "timestamp", t);
        cJSON_AddNumberToObject (json_obj, "channel", 1);
//	snprintf (convert_tmp, sizeof(convert_tmp), "%3.3f", msg -> frequency / 1000);
        cJSON_AddNumberToObject (json_obj, "freq", msg -> frequency / 1000);

        cJSON_AddNumberToObject(json_obj, "level", msg->lvl);
        cJSON_AddNumberToObject(json_obj, "error", msg->err);
        snprintf (convert_tmp, sizeof (convert_tmp), "%c", msg -> mode);
        cJSON_AddStringToObject(json_obj, "mode", convert_tmp);
        cJSON_AddStringToObject(json_obj, "label", (char *)(msg -> label));

	if (msg -> bid) {
	   snprintf (convert_tmp, sizeof (convert_tmp), "%c", msg -> bid);
	   cJSON_AddStringToObject(json_obj, "block_id", convert_tmp);

	   if (msg -> ack == 0x15) {
	      cJSON_AddFalseToObject (json_obj, "ack");
	   } else {
	      snprintf (convert_tmp, sizeof(convert_tmp), "%c", msg -> ack);
	      cJSON_AddStringToObject (json_obj, "ack", convert_tmp);
	   }

	   cJSON_AddStringToObject (json_obj, "tail", (char *)(msg -> addr));
	   if (msg -> mode <= 'Z') {
	      cJSON_AddStringToObject (json_obj, "flight", (char *)(msg -> fid));
	      cJSON_AddStringToObject (json_obj, "msgno",  (char *)(msg -> no));
	   }
        }

	if (msg -> txt [0])
	   cJSON_AddStringToObject (json_obj, "text", (char *)(msg -> txt));

	if (msg -> be == 0x17)
	   cJSON_AddTrueToObject (json_obj, "end");

	if (DecodeLabel (msg, &oooi)) {
	   if (oooi. sa [0])
	      cJSON_AddStringToObject (json_obj, "depa", oooi.sa);
	   if (oooi. da [0])
	      cJSON_AddStringToObject (json_obj, "dsta", oooi.da);
	   if (oooi. eta [0])
	      cJSON_AddStringToObject (json_obj, "eta", oooi.eta);
	   if (oooi. gout [0])
	      cJSON_AddStringToObject (json_obj, "gtout", oooi.gout);
	   if (oooi. gin [0])
	      cJSON_AddStringToObject (json_obj, "gtin", oooi.gin);
	   if (oooi. woff [0])
	      cJSON_AddStringToObject (json_obj, "wloff", oooi.woff);
	   if (oooi. won[0])
	      cJSON_AddStringToObject (json_obj, "wlin", oooi.won);
        }

        cJSON_AddStringToObject (json_obj, "station_id", idstation);
        (void)cJSON_PrintPreallocated (json_obj, (char *)jsonbuf, JSONBUFLEN, 0);
        cJSON_Delete (json_obj);
}
