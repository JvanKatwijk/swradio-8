
swradio is  Software for Linux for listening to short wave radio.
It is a  rewrite and simplification of sdr-j-sw.


THIS IS WORK IN PROGRESS
------------------------------------------------------------------
Introduction
------------------------------------------------------------------

![swradio-8 with SDRplay as device](/screenshot-swradio-1.png?raw=true)

**swradio-8** is the result of rewriting and simplifying the set of sdr-j-sw programs. A number of decoders is omitted and - for the time being - the only
device that is supported is the SDRplay.

Supported decoders are
* am
* ssb, with selection for usb or lsb;
* psk, with selection of a variety of modes and settings;
* rtty, with selection of a variety of modes and settings;
* cw, with selection of a variety of speeds and settings;
* drm, limited to 10 k bandwidth.

------------------------------------------------------------------
Linux
------------------------------------------------------------------
![swradio-8](/screenshot-swradio-2.png?raw=true)

The current version is developed under Linux (Fedora).
To build a version, adapt the swradio-8.pro file: select the decoders
you want in and deselect the others (Note that for drm a special
version of the faad library, obviosuly incompatible with the regular one
has to be created).

The "DEST" parameter in the setting tells where the result is to be put.

-------------------------------------------------------------------------
-------------------------------------------------------------------------
![swradio-8](/screenshot-swradio-1.png?raw=true)
# Copyright


	Copyright (C)  2013, 2014, 2015, 2016, 2017
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Computing

	The Qt-DAB software is made available under the GPL-2.0.
	The SDR-J software, of which the Qt-DAB software is a part, 
	is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

# swradio-8
