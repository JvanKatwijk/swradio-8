#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the  SDR-J series.
 *    Many of the ideas as implemented in the SDR-J are derived from
 *    other work, made available through the (a) GNU general Public License. 
 *    All copyrights of the original authors are recognized.
 *
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
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	<QFile>
#include	<QDataStream>
#include	"program-list.h"
#include	"radio.h"

	programList::programList (RadioInterface *mr,
	                          QString saveName) {
	this	-> saveName	= saveName;
	myWidget	= new QScrollArea (NULL);
	myWidget	-> resize (240, 200);
	myWidget	-> setWidgetResizable(true);

	tableWidget 	= new QTableWidget (0, 2);
	myWidget	-> setWidget(tableWidget);
	tableWidget 	-> setHorizontalHeaderLabels (
	            QStringList () << tr ("station") << tr ("frequency"));
	connect (tableWidget, SIGNAL (cellClicked (int, int)),
	         this, SLOT (tableSelect (int, int)));
	connect (tableWidget, SIGNAL (cellDoubleClicked (int, int)),
	         this, SLOT (removeRow (int, int)));
	connect (this, SIGNAL (newFrequency (quint64)),
	         mr, SLOT (setFrequency (quint64)));
	loadTable ();
}

	programList::~programList (void) {
int16_t	i;
int16_t	rows	= tableWidget -> rowCount ();

	for (i = rows; i > 0; i --)
	   tableWidget -> removeRow (i);
	delete	tableWidget;
	delete	myWidget;
}

void	programList::show	(void) {
	myWidget	-> show ();
}

void	programList::hide	(void) {
	myWidget	-> hide ();
}

void	programList::addRow (const QString &name, const QString &freq) {
int16_t	row	= tableWidget -> rowCount ();

	tableWidget	-> insertRow (row);
	QTableWidgetItem *item0	= new QTableWidgetItem;
	item0		-> setTextAlignment (Qt::AlignRight |Qt::AlignVCenter);
	tableWidget	-> setItem (row, 0, item0);

	QTableWidgetItem *item1 = new QTableWidgetItem;
	item1		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 1, item1);

	tableWidget	-> setCurrentItem (item0);
	tableWidget	-> item (row, 0) -> setText (name);
	tableWidget	-> item (row, 1) -> setText (freq);
}
//
//	Locally we dispatch the "click" and "translate"
//	it into a frequency and a call to the main gui to change
//	the frequency

void	programList::tableSelect (int row, int column) {
QTableWidgetItem* theItem = tableWidget  -> item (row, 1);

	(void)column;
	QString theFreq	= theItem -> text ();
	int32_t	freq	= theFreq. toInt ();
	emit newFrequency (Khz (freq));
}

void	programList::removeRow (int row, int column) {
	tableWidget	-> removeRow (row);
	(void)column;
}

void	programList::saveTable (void) {
QFile	file (saveName);

	if (file. open (QIODevice::WriteOnly)) {
	   QDataStream stream (&file);
	   int32_t	n = tableWidget -> rowCount ();
	   int32_t	m = tableWidget -> columnCount ();
	   stream << n << m;

	   for (int i = 0; i < n; i ++) 
	      for (int j = 0; j < m; j ++) 
	         tableWidget -> item (i, j) -> write (stream);
	   file. close ();
	}
}

void	programList::loadTable (void) {
QFile	file (saveName);

	if (file. open (QIODevice::ReadOnly)) {
	   QDataStream stream (&file);
	   int32_t	n, m;
	   stream >> n >> m;
	   tableWidget	-> setRowCount (n);
	   tableWidget	-> setColumnCount	(m);

	   for (int i = 0; i < n; i ++) {
	      for (int j = 0; j < m; j ++) {
	         QTableWidgetItem *item = new QTableWidgetItem;
	         item -> read (stream);
	         tableWidget -> setItem (i, j, item);
	      }
	   }
	   file. close ();
	}
}

