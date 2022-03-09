# swradio-8 [![Build Status](https://travis-ci.org/JvanKatwijk/swradio-8.svg?branch=master)](https://travis-ci.org/JvanKatwijk/swradio-8)

------------------------------------------------------------------
Introduction
------------------------------------------------------------------

swradio is software for Linux and windows for listening to short wave radio.
It is a rewrite and simplification of sdr-j-sw.
The software supports hackrf devices, rtlsdr devices
(using the rt820 tuner chip) and the SDRplay devices.
(For the rtlsdr based devices, use was made of a special version of the
library, the one by Oliver Jowett. Sources are included in the source tree of
this program.)


![swradio-8](/swradio-picture-1.png?raw=true)

One of the less common decoders is a *drm-decoder*, the picture shows
the reception of Nigeria. There are not that many drm transmissions,
but Kuwait, Tiganesti (i.e. Romenia), and Nigeria are received very well here.
The current version is limited to drm transmissions with a spectrum
equal to or less than 10 KHz.

Classical decoders are - obviously - available as well. Early evening there
is always - at least here - the "Nederlandstalig amateurnet" is well received.

![swradio-8](/swradio-picture-2.png?raw=true)

------------------------------------------------------------------------------
Implemented decoders
-----------------------------------------------------------------------------

*Decoders** are:
* am
* ssb, with selection for usb or lsb;
* psk, with a wide selection of modes and settings and with a visual tuning aid,
* mfsk, with a visual tuning aid,
* rtty, with a wide selection of modes and settings;
* cw, with (almost) automatic selection of speed and a visual tuning aid,
* drm, limited to 10 k bandwidth;
* navtex (amtor), with a wide selection of options;
* weatherfax decoder, with selection of a variety of settings.

![swradio-8](/swradio-picture-3.png?raw=true)

The "main screen" shows - next to the spectrum (or, if the widget
is touched with the right mouse button a waterfall) -the selected
frequency (in Hz), the assumed signal strength at the selected
frequency, and - if installed - a brief description of the band to
which the selected frequency belongs.

Touching a screen with the right mouse button will change the view on
the spectrum from "classical" spectrum view to "waterfall" (and back).

![swradio-8](/swradio-picture-4.png?raw=true)

This feature applies to both the display showing the full spectrum as
the display showing the "decoder" spectrum.

-------------------------------------------------------------------------
bandwidth selection and decoders
-------------------------------------------------------------------------

Since in this version the "IF" for decoding most of the implemented
modes is 0 Hz, some additional bandwidth selections are added
These selections are all centered around 0 Hz,
so, a selection of 200 Hz, means from -100 .. 100.

This is different from the selection of the usb or lsb band, since these
activate filtering from 0 .. 2500 Hz resp. -2500 .. 0 Hz.

Many typical amateur signals have a small bandwidth, making tuning harder.
Take as example Bpsk32, the width of the signal is just a couple of tens of Hz.

Some decoders are therefore equipped with an additional "scope" widget, just
showing the signal in the contex of a few hundred Hz.

![swradio-8](/swradio-loupe.png?raw=true)

Clicking with the mouse on a location on such a window will adapt
the frequency.
The picture shows the "magnifying glass" for psk. It shows that the
actual width of the received signal is less than 50 Hz.

-------------------------------------------------------------------------
a note on drm reception
-------------------------------------------------------------------------

![swradio-8](/swradio-picture-drm.png?raw=true)

DRM, Digital Radio Mondiale, is digital radio over shortwave. 
There are not many drm transmissions these days, but it is interesting
to compare drm with DAB. The latter of course on much higher frequencies
with a much higher bandwidth.

DRM is in two ways more complex than DAB. First of all, the
DAB frames are preceded by a null period, i.e. a period with a
low-amplitude signal, making it easy to detect the start of a frame
and synchronize in time.
DRM on the other hand does not. It is therefore more resource intensive
to detect the start of a frame. The fact that there are different DRM modes,
with different profiles, does not make identification easier.

Since there are differences - whatever small -
in clocks between transmitter and receiver, it is inevitable to continuously
apply some computing to find the correct start of a frame.

The second issue making DRM harder to detect that DAB is the demodulation,
while DAB uses a form of Differential PSK, DRM uses coherent demodulation,
with up to QAM64 signals. This means that the receiver has to restore
the received signal as it was at the transmitter.

The process to do that is equalizing, a DRM frame contains some special
signals helping the equalization, i.e. signals with predefined values,
but equalization takes some effort.
The picture shows (red line) the correction to be applied to the amplitude
of the values for a frame, and (blue line) the correciton to
be applied to the phase of the values for a frame.

The resulting values - here QAM64 - are given. With some imagination
one can see the 64 dots, the possible values of the signals.

Values as given here are good enough to be decoded further.

![swradio-8](/swradio-picture-drm-2.png?raw=true)

The second picture shows a reception of Radio Nigeria, with a mode
where QAM16 is used as encoding for the content. The picture shows the
16 possible signal values clearly.

The current version now was tested on decoding xHE-AAC

--------------------------------------------------------------------------
Using the swradio
---------------------------------------------------------------------------

If a configured device is connected to the computer where the program runs,
the device will be connected and opened. If no device is detected,
it is assumed that file input is requested and a file selection
menu will appear (input files in PCM format, with a 2 channel,
96000 samples/second configuration will be accepted.)

Most controls are on the main widget. Touching the frequency select
button will cause a keypad to be shown where the frequency can be
types in (in KHz or MHz).

One may select among a number of different filterings:
* wide, used for e.g. DRM decoding, uses the full 12 k bandwidth;
* am, used - as the name suggests - for am decoding, uses 9 k;
* usb, used for smallband decoding in the upper side band, has a width of 2500 Hz;
* lsb, used for ssb decoding is the lower sideband, has a width of 2500 Hz

The input can be written to a file, that file can be processed later on.

Frequency presets
can be stored, together with a user defined label (a program name).
A table of preferred frequencies (programs) is
maintained between program invocations.
A selected frequency can be stored by pressing the save frequency button.
If touched, one is asked to specify a name to be used to label that frequency.
The pair (name, frequency) then is added to the list.

Selecting such a "preferred program" is just by clicking the mouse on 
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
For Linux there is a description of how to create an executable, it is written
for Ubuntu, it is, however, simply to translate to scripts to be used with other
distros. 

Furthermore there is an "appImage", to be found in the releases section
(while the indicator at the top mentions "failure", the appImage is correct).
The appImage was created - using the travis service - in an Ubuntu 14 environment,
it contains the required libraries and should run on any more or less recent Linux
environment. (Note that a passwd is asked form since the software tries to install
the udev rules for the devices).


-------------------------------------------------------------------------
Using a pmSDR device and a soundcard
-------------------------------------------------------------------------

I dusted off my old pmSDR, dating from the era that the data was entering the
computer through the soundcard. Now, in that time I bought an HP Pavilion
since - according to the specs - it could deliver samples with a rate
of 192K, sufficient to do experiments with FM demodulation, which is
what I wanted to do at that time.

And indeed, samples could be read with a speed of 192k, however,
some internal filtering in the stupid computer halved the signal in bandwidth,
so receiving a signal sampled at 192k gave me a signal with a bandwidth of
less than 96k, completely useless for FM decoding (it is a time ago
but still pretty frustrated about it).

(*frustration section:*)
Even further, when sampling on 96k, the band was halved as well,
so the effective bandwidth then would only be 48k. 
Of course a solution to get 96K was to decimate (in software)
a signal sampled at 192k with a factor 2, but it was not why I bought the
stupid thing.

I'll not report on the discussion I had with the HP service desk
(bad for my health), I cannot remember to have met such unwilling (ignorant?)
people (and I can assure you that I met lots of them).
They, the HP people, (c/w)ould not confirm or falsify that the band was halved. They
claimed that they did not have a program to verify my claim, so my claim
was ("by definition") false. Seems a little silly for a large
organization like HP. 
Of course, the programs that I wrote to show the bandwidth/samplerates were mine, not from HP, so results
I sent them (nice spectrum pictures) were "not admissable as evidence", so after
some talking they decided that there was no problem whatsoever, so no need to communicate
further. Needless to say that I'll never buy an HP laptop again. (*end of frustration section*)

Anyway, for using a soundcard, I had to buy an external card, an EMU-202 with
which I did all kinds of FM decoding at the time in combination with the pmSDR.
Here we need "only" 96k, it works well under Linux and at the time it worked on W7.
However, it does not like Windows-10, using it under W10 leads to a crash.

![swradio-8](/swradio-pmsdr-drm.png?raw=true)

--------------------------------------------------------------------
Windows
--------------------------------------------------------------------

An installer for windows is available. 
The supported devices are the good old SDRplay (with support for the 2.13
library), the hackRF and - limited - the RTLSDR devices

------------------------------------------------------------------
Ubuntu Linux
------------------------------------------------------------------

For generating an executable under Ubuntu (16.04 or newer) one may take the
following steps.

1. Download the source tree (it is assumed that you have a git client and cmake installed.
   ```
   git clone https://github.com/JvanKatwijk/swradio-8
   ```

2. Fetch needed components
   ```
   sudo apt-get update
   sudo apt-get install qt5-qmake build-essential g++
   sudo apt-get install libsndfile1-dev qt5-default libfftw3-dev portaudio19-dev 
   sudo apt-get install zlib1g-dev libusb-1.0-0-dev mesa-common-dev
   sudo apt-get install libgl1-mesa-dev libqt5opengl5-dev libsamplerate0-dev libqwt-qt5-dev
   sudo apt-get install qtbase5-dev

   ```

4. Create the faad_drm library if you want to use the drm decoder.
   To make life easy, the sources for the faad library are included
   in the source tree (packed).

   ```
   cd ./swradio-8
   tar zxvf faad2-2.8.8.tar.gz
   cd faad2-2.8.8
   ./configure
   make
   sudo make install
   cd ..
   rm -rf faad2-2.8.8
   ```

4. Device support

  a) if you have an SDRplay device, I assume you already have installed
the library, otherwise visit "www.sdrplay.com"  and follow the instructions.
Make sure to uncomment in swradio-8.pro the line

	CONFIG += sdrplay

  b) the sources for using the pmSDR device are part of the sourcetree. Note
that for pmSDR the cardread functions are installed. The idea is to use
either the pmSDR or the "fast" devices, reflected in the name. A configuration
with (only) pmSDR will be named "swradio-pmsdr", a configuration with (only)
fast input devices will be named "swradio-8".

For selecting the pmSDR, make sure to uncomment in swradio-8.pro the line

	CONFIG += pmsdr

and to comment out the lines

	#CONFIG += sdrplay
	#CONFIG += rtlsdr
	#CONFIG += hackrf

   Create a file /etc/udev/rules.d/96-pmsdr.rules with as content

	#
	# udev rules file for Microchip 18F4455 USB Micro (PMSDR)
	#
	SUBSYSTEM=="usb", ATTRS{idVendor}=="04d8", ATTRS{idProduct}=="000c", MODE:="0666"

   to ensure non-root access to the device through usb.

   c) To make life easy, the sources for the required -non-standard - rtlsdr library used are included in the source tree,
   again as a packed file.

  ```
   tar zxvf rtl-sdr.tgz
   cd rtl-sdr/
   mkdir build
   cd build
   cmake ../ -DINSTALL_UDEV_RULES=ON -DDETACH_KERNEL_DRIVER=ON
   make
   sudo make install
   rm -rf rtl-sdr
   sudo ldconfig
   cd ..
   rm -rf build
   cd ..
  ```

   Make sure that a file exists in the `/etc/udev/rules.d` directory
   describing the device, allowing "ordinary" users to access the device.

   d) Create a library for the hackrf device by downloading the sources and compiling

   ```
   git clone https://github.com/mossmann/hackrf
   cd hackrf
   cd host
   mkdir build
   cd build
   cmake .. -DINSTALL_UDEV_RULES=ON
   sudo make install
  ```

   Make sure that a file exists in the `/etc/udev/rules.d` directory
   describing the device, allowing "ordinary" users to access the device. I.e.
   add yourself to the "plugdev" group.
   
5. Edit the `swradio-8.pro` file for configuring the supported devices and decoders.
   For the devices

	* CONFIG	+= sdrplay
	* CONFIG	+= hackrf
	* CONFIG	+= rtlsdr
   or
	* CONFIG	+= pmsdr

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

Note that the "faad_drm" library is (only) needed for the drm decoder.

The "DESTDIR" parameter in the unix section in the ".pro" file tells where the result is to be put.

6. Check the installation path to qwt. If you were downloading it from http://qwt.sourceforge.net/qwtinstall.html please mention the correct path in `qt-dab.pro` file (for other installation change it accordingly): 
  ```
  INCLUDEPATH += /usr/local/include  /usr/local/qwt-6.1.3
  ````

7. Build and make
  ```
  qmake swradio-8.pro
  make
  ```
Alternatively, you could use the "cmake" route. The file CMakeLists.txt-qt5 can be used for qt-5,
the file CMakeLists.txt-qt4 is merely used for the construction of the appImage.
The configurations here include the three mentioned "fast" devices.

-------------------------------------------------------------------------
--------------------------------------------------------------------------

# Copyright

	Copyright (C)  2013, 2014, 2015, 2016, 2017, 2018
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Computing

	The swradio software is made available under the GPL-2.0.
	The SDR-J software, of which the sw-radio software is a part, 
	is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

# swradio-8
