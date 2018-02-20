
swradio is  Software for Linux for listening to short wave radio.
It is a rewrite and simplification of sdr-j-sw.


             THIS IS WORK IN PROGRESS


![swradio-8 with SDRplay as device](/Screenshot-swradio-1.png?raw=true)
Example of use of the cw decoder.

------------------------------------------------------------------
Introduction
------------------------------------------------------------------

**swradio-8** is the result of rewriting and simplifying the set of sdr-j-sw programs. The software is limited in two ways:

* as input devices the SDRplay and file input is supported;
* a limited number of decoders is included.

Frequency selection is with a keypad that will appear when touching the
Frequency select key. Specification is in kHz or mHz. Fine frequency selection is with the
mousewheel (when the main widget is activated). The stepsize can be set from the main widget, default is 5Hz.

Supported decoders are:
* am
* ssb, with selection for usb or lsb;
* psk, with selection of a variety of modes and settings;
* rtty, with selection of a variety of modes and settings;
* cw, with selection of a variety of speeds and settings;
* drm, limited to 10 k bandwidth;
* amtor, with selection of options;
* weatherfax decoder, with selection of a variety of settings.

As can be seen from the pictures, the main widget has two displays, one with a
width of 96k (can be changed in the settings) showing the spectrum of the incoming data,
the other one width a width of 12k - with various options for filtering - showing the
spectrum of the data sent to the decoder.

One may select among a number of different filterings
* wide, used for e.g. DRM decoding, uses the full 12 k bandwidth;
* am, used - as the name suggests - for am decoding, uses 9 k;
* usb, used for smallband decoding in the upper side band, has a width of 2500 Hz;
* lsb, used for ssb decoding is the lower sideband, has a width of 2500 Hz

A list of preferred programs can be maintained, the list is kept between program invocations.

The input can be written to a file, that file can be processed later on.

The function of each button and slider in the widget(s) is described in the
tooltip for that button (slider)

------------------------------------------------------------------
Linux
------------------------------------------------------------------
![swradio-8](Screenshot-swradio-2.png?raw=true)
Example of use of the drm decoder.

The current version is developed under Linux (Fedora). In this version
there is no support (yet) for use under Windows.

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

The "DEST" parameter in the unix section in the ".pro" file tells where the result is to be put.

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
