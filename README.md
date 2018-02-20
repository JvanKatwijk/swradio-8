
swradio is  Software for Linux for listening to short wave radio.
It is a  rewrite and simplification of sdr-j-sw.


THIS IS WORK IN PROGRESS
![swradio-8 with SDRplay as device](/Screenshot-swradio-1.png?raw=true)
Example of use of the cw decoder.

------------------------------------------------------------------
Introduction
------------------------------------------------------------------

**swradio-8** is the result of rewriting and simplifying the set of sdr-j-sw programs. The software is limited in two ways:

* as input devices the SDRplay and file input is supported;
* a limited number of decoders is included.


Supported decoders are:
* am
* ssb, with selection for usb or lsb;
* psk, with selection of a variety of modes and settings;
* rtty, with selection of a variety of modes and settings;
* cw, with selection of a variety of speeds and settings;
* drm, limited to 10 k bandwidth;
* amtor, with selection of options;
* weatherfax decoder, with selection of a variety of settings.

------------------------------------------------------------------
Linux
------------------------------------------------------------------
![swradio-8](Screenshot-swradio-2.png?raw=true)
Example of use of the drm decoder.

The current version is developed under Linux (Fedora).
To build a version, adapt the swradio-8.pro file.
Note that for drm a special version of the faad library,
obviosuly incompatible with the regular one has to be created.

Select - or deselect - decoders:

* CONFIG          += am-decoder
* CONFIG          += ssb-decoder
* CONFIG          += cw-decoder
* CONFIG          += amtor-decoder
* CONFIG          += psk-decoder
* CONFIG          += rtty-decoder
* CONFIG          += fax-decoder
* CONFIG          += drm-decoder

The "DEST" parameter in the setting tells where the result is to be put.

-------------------------------------------------------------------------
-------------------------------------------------------------------------

![swradio-8](/Screenshot-swradio-3.png?raw=true)
Example of use of the psk decoder.
--------------------------------------------------------------------------
--------------------------------------------------------------------------

![swradio-8](/Screenshot-swradio-4.png?raw=true)
Example of use of the ssb decoder (lsb selected with an lsb bandfilter).

--------------------------------------------------------------------------
--------------------------------------------------------------------------

![swradio-8](/Screenshot-swradio-5.png?raw=true)
Example of listening to the dutch coastguard with amtor.

--------------------------------------------------------------------------
--------------------------------------------------------------------------

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
