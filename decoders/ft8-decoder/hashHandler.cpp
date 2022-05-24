#
/*
 *    Copyright (C) 2010, 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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

#include	"hashHandler.h"
#include	<stdio.h>

		hashHandler::hashHandler	(const std::string &f) {
	this -> saveFile = f;
	reader (f);
}

		hashHandler::~hashHandler	() {
	writer (this -> saveFile);
}

void	hashHandler::add_hash		(uint32_t key,
	                                             const QString &s) {
	for (int i = 0; i < hashTable. size (); i++)
	   if (hashTable.at (i). key == key)
	      return;
	hashElement h;
	h. key = key;
	h. value = s;
	hashTable. push_back (h);
	fprintf (stderr, "adding %X %s\n",
	                  key, s. toLatin1 (). data ());
}

QString	hashHandler::lookup		(uint32_t key) {
	for (int i = 0; i < hashTable. size (); i ++)
	   if (hashTable. at (i). key == key)
	      return hashTable. at (i). value;
	fprintf (stderr, "key %X not found\n", key);
	return "<....>";
}

void	hashHandler::reader	(const std::string &s) {
char line [256];
hashElement he;
FILE	*f	= fopen (s. c_str (), "r");
	if (f == nullptr)
	   return;

	while (!feof (f)) {
	   char s [255];
	   char * t =fgets (line, 256, f);
	   if  (t == nullptr)
	      break;
	   sscanf (line, "<%X:%s>", &he. key, s);
	   he. value = QString (s);
	   hashTable. push_back (he);
	}
	fclose (f);
}

void	hashHandler::writer	(const std::string &s) {
FILE	*f	= fopen (s. c_str (), "w");
	if (f == nullptr)
	   return;

	for (int i = 0; i < hashTable. size (); i ++)
	   fprintf (stderr, "<%X:%s>\n", hashTable. at (i). key,
	                                 hashTable. at (i). value. toLatin1 (). data ());
	fclose (f);
}

