#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets

INCLUDEPATH += . \
	       ../ ../.. \
	       ../decoder-utils \
	       ../../../includes \
	       ../../../includes/filters \
	       ../../../includes/various
	
HEADERS     = ../decoder-interface.h \
	      ./fax-decoder.h \
	      ./fax-demodulator.h \
	      ./fax-image.h \
	      ./fax-scroller.h \
	      ./fax-filenames.h \
	      ../../../includes/filters/iir-filters.h \
	      ../../../includes/filters/fir-filters.h \
	      ../../../includes/various/oscillator.h \
	      ../../../includes/various/utilities.h \
	      ../../../includes/various/fft.h \
	      ../../../includes/swradio-constants.h
SOURCES     =  ./fax-decoder.cpp \
	       ./fax-demodulator.cpp \
	       ./fax-image.cpp \
	       ./fax-scroller.cpp \
	       ./fax-filenames.cpp \
	      ../../../src/filters/iir-filters.cpp \
	      ../../../src/filters/fir-filters.cpp \
	      ../../../src/various/oscillator.cpp \
	      ../../../src/various/utilities.cpp \
	      ../../../src/various/fft.cpp \
	      ./main.cpp 
TARGET      = $$qtLibraryTarget(faxdecoder)
FORMS	+= widget.ui

win32 {
DESTDIR	= ../../../../../../windows-bin/decoder-plugins
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include/qt5/qwt
LIBS	+= -lfftw3f
LIBS	+= -lqwt-qt5
LIBS	+= -lole32
LIBS	+= -lwinmm
}

unix {
DESTDIR	= ../../../../../linux-bin/decoder-plugins
INCLUDEPATH	+= /usr/include/qt5/qwt
}

