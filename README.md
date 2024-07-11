# swradio-9 [![Build Status](https://travis-ci.org/JvanKatwijk/swradio-9.svg?branch=master)](https://travis-ci.org/JvanKatwijk/swradio-8)

--------------------------------------------------------------------
Improved support for SDRplay RSP's
---------------------------------------------------------------------

Support for the SDRplay RSP's is rewritten (and improved). For the DX version,
antennas A, B and C can be selected now, and its antenna setting is
kept between invocations.


-----------------------------------------------------------------
Introduction (Revised version 9)
------------------------------------------------------------------

swradio is software for Linux and windows for listening to short wave radio.
It is a rewrite and simplification of sdr-j-sw.
The software supports hackrf devices, rtlsdr devices
(using the rt820 tuner chip) and the SDRplay devices.
(For the rtlsdr based devices, use was made of a special version of the
library, the one by Oliver Jowett. Sources are included in the source tree of
this program.)

![swradio-8](/swradio-overview.png?raw=true)

The "main screen" shows - next to the spectrum (or, if the widget
is touched with the right mouse button a waterfall) -the selected
frequency (in Hz), the assumed signal strength at the selected
frequency, and - if installed - a brief description of the band to
which the selected frequency belongs.

Touching a screen with the right mouse button will change the view on
the spectrum from "classical" spectrum view to "waterfall" (and back).

![swradio-8](/swradio-waterfall.png?raw=true)

This feature applies to both the display showing the full spectrum as
the display showing the "decoder" spectrum.

--------------------------------------------------------------------------
New: updates to DRM decoder
--------------------------------------------------------------------------

Oe of the more comple decoders is the DRM decoder, a number of updates
is made there.

--------------------------------------------------------------------------
Device selection
--------------------------------------------------------------------------

Different from previous versions, a device selector will show upon starting
the program. The selector shows the configured devices

![swradio-8](/swradio-device-selection.png?raw=true)

Other than the previous versions, on program start up a small menu appears
from which a device can be selected. New is support for the 3.0X library
for SDRplay devices. However, other than for Qt-DAB,
it is not possible to select another device while the program is running.

-------------------------------------------------------------------------
Bandwidth selection and samplerates
-------------------------------------------------------------------------

Decoders for the various transmission modes on shortwave require
a bandwidth of a few Hz to 10 KHz (e.g. psk31 vs DRM).
The assumed inputrate for the software is therefore 96 KHz, with a
samplerate of 96 KHz. The output samplerate for devices like the
RSP's are much higher, for the RSP's 2000000 samples/second. It is
assumed that the device handlers decimate the incoming samplestream to
96000 (for the SDRplay the samplerate is set to 2112 KHz, such that
a simple integer decimation can be applied).

The "front end" of the software shows the spectrum of the incoming 
samplestream, and will apply some further filtering and decimation
to end up with a samplerate of 12 KHz. A samplestream with that 
rate is handed over to the decoders.

The front end, however, can be set to do additional filtering.
With the second combobox above the main spectrum a choice can be made
among a list of bandwidths symmetrical around 0 (200, 500, 1000, 1500, 2000,
am (9 KHz) and wide (full 12 KHz), as well as selectors for usb (upper band) and lsb (lower band), each of 2500 Hz).

It is assumed that - if needed - the decoder implementation will do further
filtering an decimation (as an example, the CW and PSK decoders use 2000 Hz
as "operating" samplerate).

Some decoders are therefore equipped with an additional "scope" widget, just
showing the signal in the contex of a few hundred Hz.

------------------------------------------------------------------------------
Implemented decoders
-----------------------------------------------------------------------------

Decoders** are:
* am
* ssb, with selection for usb or lsb;
* psk, with a wide selection of modes and settings and with a visual tuning aid,
* rtty, with a wide selection of modes and settings;
* cw, with (almost) automatic selection of speed and a visual tuning aid,
* ft8, a listener for ft8 messages,
* navtex (amtor), with a wide selection of options,
* weatherfax decoder, with selection of a variety of settings.
* drm, limited to 10 k bandwidth;

-------------------------------------------------------------------------
A note on AM/SSB detection
-------------------------------------------------------------------------

AM and SSB decoding are obviously the very basic modes for the software,
For AM broadcast reception, the "am" bandwidth provides some filtering for
a 9 KHz band, for SSB one may choose the usb or lsb width selectors.

-------------------------------------------------------------------------
A note on the CW decoder
-------------------------------------------------------------------------

CW is usually a small signal, and most amateur CW messages are brief.
Tuning is always an issue. The CW decoder therefore has a small
**magnifying glass** widget, showing a waterfall of the
spectrum of the selected few hundred Hz.

![swradio-8](/swradio-cw-widget.png?raw=true)

New is that the CW widget tries to tune to the strongest signal
in the window.

-------------------------------------------------------------------------
A note on the psk decoder
-------------------------------------------------------------------------

What holds for CW signals, certainly holds for PSK signals, their
spectrum is small, and usually the transmissions are brief.
As for the CW decoder, a "magnifying widget" will show a waterfall
of a small spectrum around the selected frequency.

![swradio-8](/swradio-psk-widget.png?raw=true)

There are quite some PSK modes, the decoder widget shows a
selector for choosing one.
The decoder furthermore shows the progress of the phase difference.
As known, (B)PSK is encoded using phase jumps of 180 degrees, while
(Q)PSK is encoded using jumps of 90 degrees. The "clock" gives a
reasonable idea of the stability of the phase jumps, and therefore
of the success in decoding.

(Note that while when I wrote the decoder, about 10 years ago, the band was 
full with psk signals, now one hardly sees one. That is why the decoder
picture is taken from some file input.

-------------------------------------------------------------------------
A note on the amtor (navtex) decoder
-------------------------------------------------------------------------

Navtex signals can be heard (at least here) on 518 KHz.
The signal is a small fsk signal, with some error correction.

Selectors on thr widget allow selecting "strict" and "non strict" decoding
and showing only validated messages or all text.

![swradio-8](/swradio-navtex-widget.png?raw=true)

-------------------------------------------------------------------------
A note on the RTTY decoder
-------------------------------------------------------------------------

While the use of RTTY on amateur bands seems to decrease, it is an interesting
mode. I used to listen around 14080 KHz, but do not see much activity on
RTTY there anymore.

Of course there are lots of parameters.
Amateurs use RTTY with a shift of 170 Hz and a baudrate of 45. Sometimes
one sees commercial transmissions with much larger shifts and higher
baudrates.

To aid is tuning - a 170 Hz signal remains small - there are some indicators
on the left side of the widget

 * the frequency offset, as measured between the mark and the space;
 * the estimated baudrate

![swradio-8](/swradio-rtty-widget.png?raw=true)

As with psk, while a decade ago the 14080 KHz was full (overloaded) with rtty
signals, one now only ocxasionally sees such signals.

-------------------------------------------------------------------------
A note on the ft8 decoder
-------------------------------------------------------------------------

Very small signals seem to be very popular in amateur circles.
Looking for some documentation about this types of mode, I decoded to
experiment, just to see what it was.
There was a partial implementation of Karlis Goba that provided some
insight in how a decoder could be build.
While the decoder differs significantly, it contains some parts that are
derived from his work.

Anyway, FT8 is a small signal, using tones with a predefined length
and a well defined distance between successive ones. The signal width
is 7 times 6.25 Hz, i.e. about 45 Hz, and messages have a defined
length of 79 tones, with 6 tones per second (i.e. app 12.5 seconds).

The decoder has three control elements

 * the number of iterations for the LDPC detector. Error correction is with a LDPC based algorthm;

 * the indicator for the search range, selectable to 5.5 KHz

The current version has as option sending (some parts of) the messages to
the pskreporter, i.e. using the decoder as "Monitor", the map on pskreporter.info will then show you as "monitor".

The **pskreporter** (see report.pskreporter.info) obviously wants to know
who is sending the data, so there is a possibility to fill in your
callsign (I do not have a callsign, not being a "real" amateur, but I have an official number as "listener", obtained from the VERON, the association of radio amateurs in the Netherlands), the maiddenhead grid indication of your locatio
(which helps the pskreportet to position your description on the map) and
the antenna.
 * Touching the "set identity" button will show a small form
where these data can be filled in).

 * the psk reporter can be switched on and off, if switched on and
the software could initialize the connection to the psk reporter site
the label is colored greem.

 * a file button for selecting a file where the received messages are stored.

Note: the decoder is experimental and will definitely not catch all transmitted messages.

![swradio-8](/swradio-ft8-widget.png?raw=true)

-------------------------------------------------------------------------
a note on the weatherfax decoder
-------------------------------------------------------------------------

Even in this time of internet and satelites, weatherfaxes still can be 
received on a variety of frequencies. In my region, I can receive
faxes on 3588, 4610, 7880 and 8040 KHz.

![swradio-8](/swradio-wfax-widget.png?raw=true)

Receiving a fax takes quite some time, and tuning into the frequency will
show a running transmission. To " pop-in", the decoder has a "cheat"
button, a button with which a synchronization can be faked and the
data is decoded. Of course, the picture will be out of sync, the white
margin will show somewhere in the picture.

-------------------------------------------------------------------------
a note on the DRM decoder
-------------------------------------------------------------------------

DRM, Digital Radio Mondiale, is digital radio over shortwave. 
There are not many DRM transmissions these days, but it is interesting
to compare DRM with DAB. The latter of course on much higher frequencies
with a much higher bandwidth (and with much higher payload).

The DRM signal is transmitted - an OFDM signal - with app 300 carriers,
close to each other (carrier distance is app 45 Hz).
Decoding is coherent, so the signal as transmitted has to be restored.
To aid in "undoing" the channel effects, a number of carriers, pilots,
are transmitted, carriers with predefined amplitude and phase.

![swradio-8](/drm-decoder-1.png?raw=true)

The decoder widget gives quite some information. 
The scopes show some visible data. The "black" one
shows the constellation of the received MSC signal, either
QAM16 or QAM64.
The larger scope shows either
 * the required modification on the pilot symbols are they are receiver,
 * the channel in the time domain
 * the residual error on the corrected pilots,

The picture shows  the reception of Radio Romenia, which gives
where I live a pretty strong signal.

The decodrr widget shows the detected offset in the frequency, i.e.
the correction applied.

The decoder shows 4 green indicators, telling that all kinds synchronization
succeeded and the audio could be decodeed. 
The black "scope" on the bottom right shows the measured constellation
of the decoded signal, here QAM 16. The 16 dots are clearly visible.

![swradio-8](/drm-decoder-2.png?raw=true)

The second picture shows the state of the reception a few minutes later,
and shows the "channel",

In general, DRM is pretty difficult to decode, due to channel conditions.

Note that the drm decoder in the "drm-receiver" and here are the same.

--------------------------------------------------------------------------
Using the swradio
---------------------------------------------------------------------------

Frequency presets
can be stored, together with a user defined label (a program name).
A table of preferred frequencies (programs) is
maintained between program invocations.
A selected frequency can be stored by pressing the save frequency button.
If touched, one is asked to specify a name to be used to label that frequency.
The pair (name, frequency) then is added to the list.

Selecting such a **preferred program** is just by clicking the mouse on 
the programname or the associated field with the frequency.

Buttons and slider are equipped with a *tooltip*, touching the button or
slider will show a brief description of the function.

----------------------------------------------------------------------------
A bandplan
----------------------------------------------------------------------------

On program startup the program reads in a file ".sw-bandplan.xml"
with an xml encoded bandplan from the home directory (folder).
An example bandplan file "sw-bandplan.xml" is part of the source
distribution.

-------------------------------------------------------------------------------
Windows
--------------------------------------------------------------------------------

For Windows there is an installer, to be found in the releases section, setup-swradio.exe, for the swradio.
The installer will install an executable as well as the required dll's. The installer
will call the official installer for the dll implementing the api to get access to the
SDRplay device.

-------------------------------------------------------------------------------
Linux
-------------------------------------------------------------------------------

For Linux an "appImage" can be found in the releases section.
It is of course possible to create your own executable, the ".pro" file
for use with qmake and the CMakeList.txt file for use with cmake
can be used to generate a makefile.

-------------------------------------------------------------------------
--------------------------------------------------------------------------

# Copyright

	Copyright (C)  2013, 2014, 2015, 2016, 2017, 2018, 2020, 2022
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Computing

	The swradio software is made available under the GPL-2.0.
	The SDR-J software, of which the sw-radio software is a part, 
	is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

# swradio-8
