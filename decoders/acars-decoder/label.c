#
/*
 *  Copyright (c) 2015 Thierry Leconte
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
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static
int label_q1 (char *txt, oooi_t *oooi) {
	memcpy (oooi -> sa, txt, 4);
	memcpy (oooi -> gout, &(txt [4]), 4);
	memcpy (oooi -> woff, &(txt [8]), 4);
	memcpy (oooi -> won,  &(txt [12]),4);
	memcpy (oooi -> gin,  &(txt [16]),4);
	memcpy (oooi -> da,   &(txt [24]),4);
	return 1;
}

static
int	label_q2 (char *txt, oooi_t *oooi) {

	memcpy (oooi -> sa,    txt, 4);
	memcpy (oooi -> eta, &(txt[4]), 4);
	return 1;
}

static
int	label_qa (char *txt, oooi_t *oooi) {
	memcpy(oooi -> sa,     txt,    4);
	memcpy(oooi -> gout, &(txt[4]),4);
	return 1;
}
static
int	label_qb (char *txt, oooi_t *oooi) {
	memcpy (oooi -> sa,     txt,     4);
	memcpy (oooi -> woff, &(txt[4]), 4);
	return 1;
}
static
int	label_qc (char *txt, oooi_t *oooi) {
	memcpy (oooi -> sa,     txt,     4);
	memcpy (oooi -> won,  &(txt[4]), 4);
	return 1;
}
static
int	label_qd (char *txt, oooi_t *oooi) {
	memcpy (oooi -> sa,     txt,     4);
	memcpy (oooi -> gin,  &(txt[4]), 4);
	return 1;
}
static
int	label_qe (char *txt, oooi_t *oooi) {
	memcpy (oooi -> sa,     txt,     4);
	memcpy (oooi -> gout, &(txt[4]), 4);
	memcpy (oooi -> da,   &(txt[8]), 4);
	return 1;
}
static
int	label_qf (char *txt, oooi_t *oooi) {
	memcpy (oooi -> sa,     txt,     4);
	memcpy (oooi -> woff, &(txt[4]), 4);
	memcpy (oooi -> da,   &(txt[8]), 4);
	return 1;
}
static
int	label_qg (char *txt, oooi_t *oooi) {
	memcpy (oooi -> sa,     txt,     4);
	memcpy (oooi -> gout, &(txt [4]),4);
	memcpy (oooi -> gin,  &(txt [8]),4);
	return 1;
}
static
int	label_qh (char *txt, oooi_t *oooi) {
	memcpy (oooi -> sa,     txt,     4);
	memcpy (oooi -> gout, &(txt[4]), 4);
	return 1;
}
static
int	label_qk (char *txt, oooi_t *oooi) {
	memcpy (oooi -> sa,     txt,     4);
	memcpy (oooi -> won,  &(txt[4]), 4);
	memcpy (oooi -> da,   &(txt[8]), 4);
	return 1;
}
static
int	label_ql (char *txt, oooi_t *oooi) {
	memcpy (oooi -> da,     txt,     4);
	memcpy (oooi -> gin,  &(txt[8]), 4);
	memcpy (oooi -> sa,   &(txt[13]),4);
	return 1;
}
static
int	label_qm (char *txt, oooi_t *oooi) {
	memcpy (oooi -> da,    txt,      4);
	memcpy (oooi -> sa,  &(txt[8]),  4);
	return 1;
}
static
int	label_qn (char *txt, oooi_t *oooi) {
	memcpy (oooi -> da,  &(txt[4]),  4);
	memcpy (oooi -> eta, &(txt[8]),  4);
	return 1;
}
static
int	label_qp (char *txt, oooi_t *oooi) {
	memcpy (oooi -> sa,     txt,     4);
	memcpy (oooi -> da,   &(txt[4]), 4);
	memcpy (oooi -> gout, &(txt[8]), 4);
	return 1;
}
static
int	label_qq (char *txt, oooi_t *oooi) {
	memcpy (oooi -> sa,     txt,     4);
	memcpy (oooi -> da,   &(txt[4]), 4);
	memcpy (oooi -> woff, &(txt[8]), 4);
	return 1;
}
static
int	label_qr (char *txt, oooi_t *oooi) {
	memcpy (oooi -> sa,     txt,     4);
	memcpy (oooi -> da,   &(txt[4]), 4);
	memcpy (oooi -> won,  &(txt[8]), 4);
	return 1;
}
static
int	label_qs (char *txt, oooi_t *oooi) {
	memcpy (oooi -> sa,     txt,     4);
	memcpy (oooi -> da,   &(txt[4]), 4);
	memcpy (oooi -> gin,  &(txt[8]), 4);
	return 1;
}
static
int	label_qt (char *txt, oooi_t *oooi) {
	memcpy (oooi -> sa,     txt,     4);
	memcpy (oooi -> da,   &(txt[4]), 4);
	memcpy (oooi -> gout, &(txt[8]), 4);
	memcpy (oooi -> gin,  &(txt[12]),4);
	return 1;
}
static
int	label_20(char *txt,oooi_t *oooi) {
	if(memcmp(txt,"RST",3)) return 0;
	memcpy ( oooi ->sa,&(txt[22]),4);
	memcpy ( oooi ->da,&(txt[26]),4);
	return 1;
}
static
int	label_21(char *txt,oooi_t *oooi) {
	if(txt[6]!=',') return 0;
	memcpy ( oooi ->sa,&(txt[7]),4);
	if(txt[11]!=',') return 0;
	memcpy ( oooi ->da,&(txt[12]),4);
	return 1;
}
static
int label_26 (char *txt, oooi_t *oooi) {
char *p;

	if (memcmp(txt,"VER/077",7))
	   return 0;
	p = strchr(txt,'\n');
	if (p==NULL)
	   return 0;
	p++;
	if (memcmp(p,"SCH/",4))
	   return 0;
	p=strchr(p+4,'/'); if(p==NULL) return 0;
	memcpy (oooi -> sa,p + 1,4);
	memcpy (oooi -> da,p + 6,4);
	p = strchr (p,'\n');
	if (p==NULL)
	   return 1;
	p++;
	if (memcmp(p,"ETA/",4))
	   return 0;
	memcpy (oooi -> eta,p + 4, 4);
	return 1;
}
static
int	label_2N (char *txt,oooi_t *oooi) {

	if (memcmp (txt,"TKO01", 5))
	   return 0;
	if (txt [11]!='/')
	   return 0;
	memcpy (oooi -> sa, &(txt[20]), 4);
	memcpy (oooi -> da, &(txt[24]), 4);
	return 1;
}
static
int	label_2Z (char *txt, oooi_t *oooi) {
	memcpy ( oooi ->da,txt,4);
	return 1;
}
static
int	label_33(char *txt,oooi_t *oooi)
{
	if(txt[0]!=',') return 0;
	if(txt[20]!=',') return 0;
	memcpy ( oooi ->sa,&(txt[21]),4);
	if(txt[25]!=',') return 0;
	memcpy ( oooi ->da,&(txt[26]),4);
	return 1;
}
static int label_39(char *txt,oooi_t *oooi)
{
	if(memcmp(txt,"GTA01",5)) return 0;
	if(txt[15]!='/') return 0;
	memcpy ( oooi ->sa,&(txt[24]),4);
	memcpy ( oooi ->da,&(txt[28]),4);
	return 1;
}
static int label_44(char *txt,oooi_t *oooi)
{
	if(memcmp(txt,"POS02",5)) return 0;
	if(txt[23]!=',') return 0;
	memcpy ( oooi ->da,&(txt[24]),4);
	if(txt[28]!=',') return 0;
	memcpy ( oooi ->eta,&(txt[29]),4);
	return 1;
}
static int label_45(char *txt,oooi_t *oooi)
{
	if(txt[0]!='A') return 0;
	memcpy ( oooi ->da,&(txt[1]),4);
	return 1;
}
static int label_10 (char *txt,oooi_t *oooi)
{
	if(memcmp(txt,"ARR01",5)) return 0;
	memcpy ( oooi ->da,&(txt[12]),4);
	memcpy ( oooi ->eta,&(txt[16]),4);
	return 1;
}
static int label_11(char *txt,oooi_t *oooi)
{
	if(memcmp(&(txt[13]),"/DS ",4)) return 0;
	memcpy ( oooi ->da,&(txt[17]),4);
	if(memcmp(&(txt[21]),"/ETA ",5)) return 0;
	memcpy ( oooi ->eta,&(txt[26]),4);
	return 1;
}
static int label_12 (char *txt,oooi_t *oooi)
{
	if(txt[4]!=',') return 0;
	memcpy ( oooi ->sa,txt,4);
	memcpy ( oooi ->da,&(txt[5]),4);
	return 1;
}
static int label_15 (char *txt,oooi_t *oooi)
{
	if(memcmp(txt,"FST01",5)) return 0;
	memcpy ( oooi ->sa,&(txt[5]),4);
	memcpy ( oooi ->da,&(txt[9]),4);
	return 1;
}
static int label_17 (char *txt,oooi_t *oooi)
{
	if(memcmp(txt,"ETA ",4)) return 0;
	memcpy ( oooi ->eta,&(txt[4]),4);
	if(txt[8]!=',') return 0;
	memcpy ( oooi ->sa,&(txt[9]),4);
	if(txt[13]!=',') return 0;
	memcpy ( oooi ->da,&(txt[14]),4);
	return 1;
}
static int label_1G (char *txt,oooi_t *oooi) {
	if(txt[4]!=',') return 0;
	memcpy ( oooi ->sa,txt,4);
	memcpy ( oooi ->da,&(txt[5]),4);
	return 1;
}
static int label_80 (char *txt,oooi_t *oooi)
{
	if(memcmp(&(txt[6]),"/DEST/",5)) return 0;
	memcpy ( oooi ->da,&(txt[12]),4);
	return 1;
}
static int label_83(char *txt,oooi_t *oooi)
{
	if(txt[4]!=',') return 0;
	memcpy ( oooi ->sa,txt,4);
	memcpy ( oooi ->da,&(txt[5]),4);
	return 1;
}
static int label_8D(char *txt,oooi_t *oooi)
{
	if(txt[4]!=',') return 0;
	if(txt[35]!=',') return 0;
	memcpy ( oooi ->sa,&(txt[36]),4);
	if(txt[40]!=',') return 0;
	memcpy ( oooi ->da,&(txt[41]),4);
	return 1;
}
static int label_8e(char *txt,oooi_t *oooi)
{
	if(txt[4]!=',') return 0;
	memcpy ( oooi ->da,txt,4);
	memcpy ( oooi ->eta,&(txt[5]),4);
	return 1;
}
static int label_8s(char *txt,oooi_t *oooi)
{
	if(txt[4]!=',') return 0;
	memcpy ( oooi ->da,txt,4);
	memcpy ( oooi ->eta,&(txt[5]),4);
	return 1;
}
static int label_b9(char *txt,oooi_t *oooi)
{
	if(txt[0]!='/') return 0;
	memcpy ( oooi ->da,&(txt[1]),4);
	return 1;
}


int DecodeLabel (acarsmsg_t *msg,oooi_t *oooi) {
int ov	= 0;
 
	memset (oooi, 0, sizeof (oooi_t));
	switch (msg -> label [0]) {
	   case '1':
	      if (msg -> label [1] =='0') 
	         ov = label_10 ((char *)(msg -> txt), oooi);
	      if (msg -> label [1] =='1') 
	         ov = label_11 ((char *)(msg -> txt), oooi);
	      if (msg -> label [1] =='2') 
	         ov = label_12 ((char *)(msg -> txt), oooi);
	      if (msg -> label [1] =='5') 
	         ov = label_15 ((char *)(msg -> txt), oooi);
	      if (msg -> label [1] =='7') 
	         ov = label_17 ((char *)(msg -> txt), oooi);
	      if (msg -> label [1] =='G') 
	         ov = label_1G ((char *)(msg -> txt), oooi);
	      break;

	   case '2' :
	      if (msg -> label [1] =='0') 
	         ov = label_20 ((char *)(msg -> txt), oooi);
	      if (msg -> label [1] =='1') 
	         ov = label_21 ((char *)(msg -> txt), oooi);
	      if (msg -> label [1] =='6') 
	         ov = label_26 ((char *)(msg -> txt), oooi);
	      if (msg -> label [1] =='N') 
	         ov = label_2N ((char *)(msg -> txt), oooi);
	      if (msg -> label [1] =='Z') 
	         ov = label_2Z ((char *)(msg -> txt), oooi);
	      break;

	   case '3' :
	      if (msg -> label [1]=='3') 
	         ov = label_33 ((char *)(msg -> txt), oooi);
	      if (msg -> label [1]=='9') 
	         ov = label_39 ((char *)(msg -> txt), oooi);
	      break;

	   case '4' :
	      if (msg -> label [1] =='4') 
	         ov = label_44 ((char *)(msg -> txt), oooi);
	      if (msg -> label [1] =='5') 
	         ov = label_45 ((char *)(msg -> txt), oooi);
	   break;

	   case '8' :
	      if (msg -> label [1] =='0') 
	         ov = label_80 ((char *)(msg -> txt), oooi);
	      if (msg -> label [1] =='3') 
	         ov = label_83 ((char *)(msg -> txt), oooi);
	      if (msg -> label [1] =='D') 
	         ov = label_8D ((char *)(msg -> txt), oooi);
	      if (msg -> label [1] =='E') 
	         ov = label_8e ((char *)(msg -> txt), oooi);
	      if (msg -> label [1] =='S') 
	         ov = label_8s ((char *)(msg -> txt), oooi);
	      break;

	   case 'B' :
	      if (msg -> label [1] =='9') 
	         ov = label_b9 ((char *)(msg -> txt), oooi);
	      break;

	   case 'R' :
	      if (msg -> label [1] =='B') 
	         ov = label_26 ((char *)(msg -> txt), oooi);
	      break;

	case 'Q' :
  		switch(msg->label[1]) {
			case '1':ov=label_q1((char *)(msg -> txt),oooi);break;
			case '2':ov=label_q2((char *)(msg -> txt),oooi);break;
			case 'A':ov=label_qa((char *)(msg -> txt),oooi);break;
			case 'B':ov=label_qb((char *)(msg -> txt),oooi);break;
			case 'C':ov=label_qc((char *)(msg -> txt),oooi);break;
			case 'D':ov=label_qd((char *)(msg -> txt),oooi);break;
			case 'E':ov=label_qe((char *)(msg -> txt),oooi);break;
			case 'F':ov=label_qf((char *)(msg -> txt),oooi);break;
			case 'G':ov=label_qg((char *)(msg -> txt),oooi);break;
			case 'H':ov=label_qh((char *)(msg -> txt),oooi);break;
			case 'K':ov=label_qk((char *)(msg -> txt),oooi);break;
			case 'L':ov=label_ql((char *)(msg -> txt),oooi);break;
			case 'M':ov=label_qm((char *)(msg -> txt),oooi);break;
			case 'N':ov=label_qn((char *)(msg -> txt),oooi);break;
			case 'P':ov=label_qp((char *)(msg -> txt),oooi);break;
			case 'Q':ov=label_qq((char *)(msg -> txt),oooi);break;
			case 'R':ov=label_qr((char *)(msg -> txt),oooi);break;
			case 'S':ov=label_qs((char *)(msg -> txt),oooi);break;
			case 'T':ov=label_qt((char *)(msg -> txt),oooi);break;
		}
		break;
  }

  return ov;
}
