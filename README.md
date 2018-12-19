# swradio-8 [![Build Status](https://travis-ci.org/JvanKatwijk/swradio-8.svg?branch=master)](https://travis-ci.org/JvanKatwijk/swradio-8)

swradio is  Software for Linux and windows for listening to short wave radio.
It is a rewrite and simplification of sdr-j-sw.

![swradio-8 with SDRplay as device](/swradio-cw.png?raw=true)
Example of use of the cw decoder.

------------------------------------------------------------------
Introduction
------------------------------------------------------------------

**swradio-8** is software (both Linux and Windows) for
listening to short waves and amateur radio.
The software supports a number of decoders,
often used by radio amateurs, such as psk, cw and rtty.
swradio-8 is the result of rewriting and simplifying the set
of sdr-j-sw programs.

The program can be configured to use the SDRplay,
the HACKrf, RTLSDR-based dabsticks or the - soundcard based - pmsdr
as input device, and is able to
* dump the (decimated) input onto a file in PCM format;
* use such a file as input.

If a configured device is connected to the computer where the program runs,
the device will be connected and opened. If no device is connected,
it is assumed that file input is requested

To keep things simple, configuring the radio is with either one or more
of the "high-speed" set (SDRplay, HACKrf or RTLSDR stick) or the "low speed"
(read: sound card) singleton set, with the pmSDR as element.

Note that for use with the Hackrf device,
have to provide the hackrf lib.

--------------------------------------------------------------------------
Current development
--------------------------------------------------------------------------

The current development is to widen the scope of the radio, so that
it also can be used with other devices than the SDRplay.  
Furthermore, there are  some differences in the GUI,
most pictures need updating.

Furthermore, while these changes are being made, the CMakeLists.txt
files are outdated and no AppImage is made.

For the rtlsdr based devices, use was made of a special version of the
library, sources are part of the source tree. 
In a next version an entry will be made for the "direct sampling" option
for rtlsdr based devices.

![swradio-8](/swradio-rtlsdr-drm.png?raw=true)

I dusted off the "old" pmsdr device, unfortunately my current laptop does
not have a "line in" port, the old laptop has one, however while the
sampling rate is 96k (or 192k), some stupid internal filtering  limits
the effective bandwidth to app 60k. However, as the picture shows.the
DRM program from kuwait can be received and decoded.

![swradio-8](/swradio-pmsdr-drm.png?raw=true)

While the HACKrf seems to work well, I did not manage yet to
decode DRM programs with samples coming from it.

The SDRplay provides the full band and makes it easy to receive short wave
programs.

![swradio-8](/swradio-drm.png?raw=true)

**Preferred frequencies**
can be stored, together with a user defined label (a program name).
A list of preferred frequencies (programs) is
maintained between program invocations.
A selected frequency can be stored by pressing the save frequency button.
If touched, one is asked to specify a name to be used to label that frequency.
The pair (name, frequency) then is added to the list.

Selecting such a "preferred program" is just by clicking the mouse on 
the programname or the associated field with the frequency.

**Frequency selection**
is with a keypad, separated from the main widget,
that will appear when touching the Frequency select key.
Specification  of a frequency is either in kHz or mHz.
Fine frequency selection - up to an accuracy of 1 Hz - is with the
mousewheel (when the main widget is activated).
(The stepsize for the frequency changes can be set from the main widget, default is 5 Hz).

Of course clicking the mouse on one of the two spectra will also select the
selected frequency pointed to.

As a "quick" help, a button "middle" is present that, when pressed - will set the oscillator such
that the selected frequency is in the middle of the right half of the spectrum display.

**Waterfall displays**
Note that clicking the right mouse button on any of the two displays, that display will switch from spectrum
to waterfall display (or vice-versa).

**Decoders** are:
* am
* ssb, with selection for usb or lsb;
* psk, with selection of a variety of modes and settings and with a tuning aid,
* mfsk, with a tuning aid,
* rtty, with selection of a variety of modes and settings;
* cw, with (almost) automatic selection of speed;
* drm, limited to 10 k bandwidth;
* amtor, with selection of options;
* weatherfax decoder, with selection of a variety of settings.

As can be seen from the pictures, the main widget has two main displays,
and one smaller display to show the audio output.
The top display has a width of 96k (this width can be changed in the settings)
showing the spectrum of the incoming data,
the bottom one width a width of 12k -
showing the spectrum of the data sent to the decoder, i.e. after being
filtered and shifted.

One may select among a number of different filterings:
* wide, used for e.g. DRM decoding, uses the full 12 k bandwidth;
* am, used - as the name suggests - for am decoding, uses 9 k;
* usb, used for smallband decoding in the upper side band, has a width of 2500 Hz;
* lsb, used for ssb decoding is the lower sideband, has a width of 2500 Hz

The input can be written to a file, that file can be processed later on.

** Frequencies**

The frequency to which the oscillator of the selected device will be set will be
the one in the middle of the "large" display. Any offset of a selected
frequency to this oscillator frequency will be handled in software.

The SDRplay and HACKrf devices support samplerates of 2M and up.
These rates are decimated to 96k. The soundcard, used for the pmSDR
device will be set to 96k.

The function of each button and slider in the widget(s) is described in the
tooltip for that button (slider)

------------------------------------------------------------------
Linux
------------------------------------------------------------------

The current version is developed under Linux (Fedora). A cross compiled version
for Windows (no garantees) is available in the releases section.
This version will
run in the same environment (i.e. folder, directory) as the qt-dab software.

To build a version, adapt the swradio-8.pro file.
Note that for DRM decoding a special version of the faad library,
obviously incompatible with the regular one, has to be created.

Select the device

* CONFIG	+= sdrplay
* CONFIG	+= hackrf
* CONFIG	+= pmsdr

It is possible to select more than one device, in which case the
software will try to open configured devices, until one found, otherwise
file input will be selected.

Select - or deselect - decoders:
* CONFIG          += am-decoder
* CONFIG          += ssb-decoder
* CONFIG          += cw-decoder
* CONFIG          += amtor-decoder
* CONFIG          += psk-decoder
* CONFIG          += rtty-decoder
* CONFIG          += fax-decoder
* CONFIG          += drm-decoder
* CONFIG          += mfsk-decoder

The "DESTDIR" parameter in the unix section in the ".pro" file tells where the result is to be put.

-------------------------------------------------------------------------

Windows

-------------------------------------------------------------------------

The releases section contains a zip file with an executable of swradio-8 
and the required dll's. Note that the library for SDRplay support is not
included, one has to obtain that from SDRplay.com

-------------------------------------------------------------------------
--------------------------------------------------------------------------

![swradio-8](/swradio-psk.png?raw=true)
Example of use of the psk decoder. The psk decoder now is equipped with
a small "scope" as an aid in tuning. Clicking on a positionin the
psk window will shift the frequency such that that point is in the middle.

--------------------------------------------------------------------------
--------------------------------------------------------------------------

![swradio-8](/swradio-mfsk.png?raw=true)
Example of use of the mfsk decoder. The psk decoder now is equipped with
a small "scope" as an aid in tuning. Clicking on a positionin the
psk window will shift the frequency such that that point is on the mark.

--------------------------------------------------------------------------
--------------------------------------------------------------------------

![swradio-8](/Screenshot-swradio-4.png?raw=true)
Example of use of the ssb decoder (old picture) (lsb selected with an lsb bandfilter).

--------------------------------------------------------------------------
--------------------------------------------------------------------------

![swradio-8](/screenshot-amtor.png?raw=true)
Example of "listening" to the dutch coastguard with amtor (old picture)

--------------------------------------------------------------------------
----------------------------------------------------------------------

![swradio-8](/swradio-fax.png?raw=true)
Example of "listening" to a transmission of a weatherfax on shortwave.

--------------------------------------------------------------------------
--------------------------------------------------------------------------

# Copyright


	Copyright (C)  2013, 2014, 2015, 2016, 2017
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Computing

	The swradio software is made available under the GPL-2.0.
	The SDR-J software, of which the sw-radio software is a part, 
	is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

# swradio-8
