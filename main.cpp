#
/*
 *    Copyright (C) 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the swradio-8
 *
 *    swradio-8 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    swradio-8 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with swradio-8; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	Main program
 */
#include	<QApplication>
#include	<QSettings>
#include	<QDir>
#include	<unistd.h>
#include	"radio.h"
#include	"radio-constants.h"
#include	"bandplan.h"

QString fullPathfor (QString v) {
QString fileName;

	if (v == QString (""))
	   return QString ("/tmp/xxx");

	if (v. at (0) == QChar ('/'))           // full path specified
	   return v;

	fileName = QDir::homePath ();
	fileName. append ("/");
	fileName. append (v);
	fileName = QDir::toNativeSeparators (fileName);

	if (!fileName. endsWith (".ini"))
	   fileName. append (".ini");

	return fileName;
}

#define	DEFAULT_INI	".sw-radio.ini"
#define	STATION_LIST	".sw-radio-stations.bin"
#define	BAND_PLAN	".sw-bandplan.xml"

int	main (int argc, char **argv) {
int32_t		opt;
/*
 *	The default values
 */
QSettings	*ISettings;		/* .ini file	*/
RadioInterface	*MyRadioInterface;
QString iniFile		= QDir::homePath ();
QString stationList     = QDir::homePath ();
QString	bandplanFile	= QDir::homePath ();

	QCoreApplication::setOrganizationName ("Lazy Chair Computing");
        QCoreApplication::setOrganizationDomain ("Lazy Chair Computing");
        QCoreApplication::setApplicationName ("sw-radio");
        QCoreApplication::setApplicationVersion (QString (CURRENT_VERSION) + " Git: " + GITHASH);

        iniFile. append ("/");
        iniFile. append (DEFAULT_INI);
        iniFile = QDir::toNativeSeparators (iniFile);

        stationList. append ("/");
        stationList. append (STATION_LIST);
        stationList = QDir::toNativeSeparators (stationList);

	bandplanFile. append ("/");
	bandplanFile. append (BAND_PLAN);
        bandplanFile = QDir::toNativeSeparators (bandplanFile);

        while ((opt = getopt (argc, argv, "i:B:")) != -1) {
           switch (opt) {
              case 'i':
                 iniFile	= fullPathfor (QString (optarg));
                 break;

              case 'B':
                 bandplanFile       = atoi (optarg);
                 break;

             default:
                 break;
           }
        }

#if QT_VERSION >= 0x050600
        QGuiApplication::setAttribute (Qt::AA_EnableHighDpiScaling);
#endif
	QApplication a (argc, argv);

	ISettings	= new QSettings (iniFile, QSettings::IniFormat);
/*
 *	Before we connect control to the gui, we have to
 *	instantiate
 */
	int rate	= ISettings -> value ("workingRate", 96000). toInt ();
	bandPlan my_bandPlan (bandplanFile);
        MyRadioInterface = new RadioInterface (ISettings,
	                                       stationList,
	                                       &my_bandPlan,
	                                       96000, 12000);
	MyRadioInterface -> show ();
        a. exec ();
/*
 *	done:
 */
	fflush (stdout);
	fflush (stderr);
	qDebug ("It is done\n");
	ISettings	-> sync ();
	fprintf (stderr, "we gaan deleten\n");
	delete MyRadioInterface;
//	ISettings	-> ~QSettings ();
}
