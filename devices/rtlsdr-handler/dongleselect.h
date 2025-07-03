#
/*
 *    Copyright (C) 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the sw radio
 *
 *    sw radio is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    sw radio is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with sw radio; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#
#pragma once
#
#include	<QDialog>
#include	<QLabel>
#include	<QListView>
#include	<QStringListModel>
#include	<QString>
#include	<QStringList>
#include	<stdint.h>

class	dongleSelect: public QDialog {
Q_OBJECT
public:
	dongleSelect	(void);
	~dongleSelect	(void);
void	addtoDongleList	(const QString &);
int16_t	getSelectedItem	(void);
private:
	QLabel		*toptext;
	QListView	*selectorDisplay;
	QStringListModel	dongleList;
	QStringList	Dongles;
	int16_t		selectedItem;
private slots:
void	selectDongle	(QModelIndex);
};


