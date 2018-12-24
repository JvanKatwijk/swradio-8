# swradio-8 [![Build Status](https://travis-ci.org/JvanKatwijk/swradio-8.svg?branch=master)](https://travis-ci.org/JvanKatwijk/swradio-8)

swradio is  Software for Linux and windows for listening to short wave radio.
It is a rewrite and simplification of sdr-j-sw.
It is currently under development

------------------------------------------------------------------
Introduction
------------------------------------------------------------------

*swradio-8* is software (both Linux and Windows) for
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

To distinguish between "fast" and "slow" input, the version for the
pmsdr is named "swradio-pmsdr", while the regular version is
configured to be named "swradio-8". In the build for Windows there
is also an experimental version for use with extio plugins for soundcard
devices called "swradio-extio".

If a configured device is connected to the computer where the program runs,
the device will be connected and opened. If no device is detected,
it is assumed that file input is requested

--------------------------------------------------------------------------
Using rtlsdr based devices
--------------------------------------------------------------------------

For the rtlsdr based devices, use was made of a special version of the
library, the one by Oliver Jowett. Sources are included in the source tree of
this program.
This library makes it possible to use frequencies as low as 14 MHz.

![swradio-8](/swradio-rtlsdr-drm.png?raw=true)

-------------------------------------------------------------------------
Using a pmSDR device and a soundcard
-------------------------------------------------------------------------

I dusted off my old pmSDR, from the era that the data was entering the
computer through the soundcard. Now, in that time I bought an HP Pavilion
since - according to the specs - it could deliver samples with a rate
of 192K, sufficient to do experiments with FM demodulation, which is
what I wanted to do at that time.

And indeed, samples could be read with a speed of 192k, however,
some internal filtering in the stupid computer halved the signal in bandwidth,
so receiving a signal sampled at 192k, with a bandwidth of
less than 96k was completely useless for FM decoding.

Even further, when sampling on 96k, the band was halved as well,
so the effective bandwidth then would only be 48k. 
Of course a solution to get 96K was to decimate in software
a signal sampled at 192k with a factor 2, but I was not amused.

I'll not report on the discussion I had with the HP service desk
(bad for my health), I cannot remember to have met such unwilling (ignorant?)
people (and I can assure that I met lots of them).
They, the HP people, (c/w)ould not confirm that the band was halved. They
claimed that they did not have a program to verify my claim, so my claim
was ("by definition") false. Seems a little silly for a large
organization like HP. 
Of course, my programs showing the bandwidth/samplerates were not from HP, so results
I sent them (nice spectrum pictures) were "not admissable as evidence", so after
some talking they decided that there was no problem whatsoever, so no need to communicate
further. Needless to say that I'll never buy an HP laptop again.

Anyway, for using a soundcard, I had to buy an external card, an EMU-2-2.
It works well under Linux and at the time it worked on W7.
However, it does not like Windows-10, using it under W10 leads to a crash.

![swradio-8](/swradio-pmsdr-drm.png?raw=true)

--------------------------------------------------------------------------
Using extio devices with soundcard*
---------------------------------------------------------------------------

One of the windows versions of the swradio software is set
to use "extio" plugins for handling the radio device.
To keep things simple the use is limited to radio devices
with data being sent using the soundcard. 

The version is pretty experimental.

--------------------------------------------------------------------------
Using the hackrf device
-------------------------------------------------------------------------

While the HACKrf seems to work well, I did not manage yet to
decode DRM programs with samples coming from it. I do make some
error in interpreting the 8 bit input. Things depending
on amplitude work well, things depending on the phase(difference)
do not. I.e. decoding cw can be handled, decoding psk, which is
higly dependent on phase differences, not.

![swradio-8](/swradio-hackrf.png?raw=true)


-------------------------------------------------------------------------
Using the SDRplay
-------------------------------------------------------------------------

The SDRplay provides the full band and makes it easy to receive short wave
programs. Absolutely one of my favorites.

![swradio-8](/swradio-sdrplay.png?raw=true)

-----------------------------------------------------------------------------
*Preferred frequencies*
-----------------------------------------------------------------------------

Preferred frequencies
can be stored, together with a user defined label (a program name).
A table of preferred frequencies (programs) is
maintained between program invocations.
A selected frequency can be stored by pressing the save frequency button.
If touched, one is asked to specify a name to be used to label that frequency.
The pair (name, frequency) then is added to the list.

Selecting such a "preferred program" is just by clicking the mouse on 
the programname or the associated field with the frequency.

----------------------------------------------------------------------------
*Frequency selection*
----------------------------------------------------------------------------

Frequency selection is with a keypad, separated from the main widget,
that will appear when touching the Frequency select key.
Specification  of a frequency is either in kHz or mHz.
Fine frequency selection - up to an accuracy of 1 Hz - is with the
mousewheel (when the main widget is activated).
(The stepsize for the frequency changes can be set from the main widget, default is 5 Hz).

Of course clicking the mouse on one of the two spectra will also select the
selected frequency pointed to.

As a "quick" help, a button "middle" is present that, when pressed - will set the oscillator such
that the selected frequency is in the middle of the right half of the spectrum display.

----------------------------------------------------------------------------
*Waterfall displays*
-----------------------------------------------------------------------------

Note that clicking the right mouse button on any of the two displays, that display will switch from spectrum
to waterfall display (or vice-versa).

------------------------------------------------------------------------------
*Implemented decoders*
-----------------------------------------------------------------------------

*Decoders** are:
* am
* ssb, with selection for usb or lsb;
* psk, with a wide selection of modes and settings and with a visual tuning aid,
* mfsk, with a visual tuning aid,
* rtty, with a wide selection of modes and settings;
* cw, with (almost) automatic selection of speed and a visual tuning aid,
* drm, limited to 10 k bandwidth;
* amtor, with a wide selection of options;
* weatherfax decoder, with selection of a variety of settings.

As can be seen from the pictures, the main widget has two main displays,
and one smaller display to show the audio output.
The top display has a width of 96k (this width can be changed in the settings)
showing the spectrum of the incoming data,
the bottom one width a width of 12k -
showing the spectrum of the data sent to the decoder, i.e. after being
filtered and shifted.
The small display shows the spectrum of the resulting audio.

One may select among a number of different filterings:
* wide, used for e.g. DRM decoding, uses the full 12 k bandwidth;
* am, used - as the name suggests - for am decoding, uses 9 k;
* usb, used for smallband decoding in the upper side band, has a width of 2500 Hz;
* lsb, used for ssb decoding is the lower sideband, has a width of 2500 Hz

The input can be written to a file, that file can be processed later on.

------------------------------------------------------------------
*Linux
------------------------------------------------------------------

The current version is developed under Linux (Fedora). A cross compiled version
for Windows (no garantees) is available in the releases section.
This version will run in the same environment (i.e. folder, directory) as the qt-dab software.

To build a version, adapt the swradio-8.pro file.
Note that for DRM decoding a special version of the faad library,
obviously incompatible with the regular one, has to be created.

Select the device

* CONFIG	+= sdrplay
* CONFIG	+= hackrf
* CONFIG	+= rtlsdr

or

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

The releases section *of the qt-dab repository* contains a zip file with executables, among which
an executable of swradio-8 and the required dll's. Note that the library for SDRplay support is not
included, one has to obtain that from SDRplay.com

-------------------------------------------------------------------------
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
