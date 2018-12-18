#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets
INCLUDEPATH += . \
	       ../ ../.. \
	       ../../../includes \
	       ../../../includes/filters \
	       ../../../includes/various
HEADERS     = ../rig-interface.h \
	      ./dabstick.h \
	      ./dongleselect.h \
	      ../../../includes/filters/fir-filters.h \
	      ../../../includes/various/fft.h \
	      ../../../includes/swradio-constants.h
SOURCES     = ./dabstick.cpp \
	      ./dongleselect.cpp \
	      ../../../src/filters/fir-filters.cpp \
	      ../../../src/various/fft.cpp
TARGET      = $$qtLibraryTarget(device_dabstick)
FORMS	+= ./widget.ui

win32 {
DESTDIR     = ../../../../../../windows-bin/input-plugins
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include/qt5/qwt
LIBS	+= -lfftw3f
LIBS	+= -lqwt-qt5
LIBS	+= -lusb-1.0
LIBS	+= -lole32
LIBS	+= -lwinmm
}

unix{
DESTDIR     = ../../../../../linux-bin/input-plugins-sw
}

