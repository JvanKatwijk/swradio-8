#
/*
 *    Copyright (C) 2022
 *    J van Katwijk
 *    :azy Chair Computing
 *
 *    Parts of the algorithms are copied or derived from
 *    Karlis Goba
 *    YL33G
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

#ifndef	__PACK_HANDLER__
#define	__PACK_HANDLER__
#include	<stdint.h>
#include	<string.h>
#include	<QObject>
#include	<QString>
#include	<QStringList>
#include	"hashHandler.h"

class	packHandler: public QObject {
Q_OBJECT
public:
		packHandler	();
		~packHandler	();

	QString unpackMessage	(const uint8_t* m_in);
	QStringList	extractCall	(const uint8_t* m_in);
private:
	hashHandler		the_hashHandler;
	bool			pskReporterReady;
	QString	handle_type0	(const uint8_t *m_in, int n3);
	QString	handle_type1	(const uint8_t *m_in, uint8_t i3);
	QString	handle_type3	(const uint8_t *m_in);
	QString	handle_type4	(const uint8_t* m_in);
	QString	handle_type5	(const uint8_t* m_in);
	QString	handle_type00	(const uint8_t *m_in);
	QString	handle_type01	(const uint8_t *m_in);
	QString	handle_type03	(const uint8_t *m_in);
	QString	handle_type04	(const uint8_t *m_in);
	QString	handle_type05	(const uint8_t *m_in);

	QString	get_CQcode	(uint32_t data, int size);
	QString number_2	(int number);
	QString number_3	(int number);
	void	pack_bits	(const uint8_t bit_array [],
	                         int num_bits, uint8_t packed []);
	QString	getCallsign	(const uint32_t n28);
	QStringList extract_call_type_0 (const uint8_t *m_in);
	QStringList extract_call_type_1 (const uint8_t *m_in, int type);
	QStringList extract_call_type_3 (const uint8_t *m_in);


	std::vector<uint32_t>	gehad;
signals:
	void	show_pskStatus	(bool);
};
#endif
