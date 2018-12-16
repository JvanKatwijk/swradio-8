#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets
INCLUDEPATH += . \
	       ../ ../.. \
	       ../../../includes \
	       ../../../includes/various
HEADERS     = ../rig-interface.h \
	      ./extio-handler.h \
	      ./card-reader.h \
	      ./reader-base.h \
	      ./common-readers.h \
	      ./simple-converter.h \
	      ../../../includes/swradio-constants.h \
	      ../../../includes/various/ringbuffer.h
SOURCES     = ./extio-handler.cpp \
	      ./card-reader.cpp \
	      ./reader-base.cpp \
	      ./common-readers.cpp \
	      ./simple-converter.cpp
TARGET      = $$qtLibraryTarget(device_extiohandler)
FORMS		+= ./extio-widget.ui

win32 {
DESTDIR     = ../../../../../../windows-bin/input-plugins
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include/qwt
LIBS	+= -lportaudio
LIBS	+= -lusb-1.0
LIBS	+= -lole32
LIBS	+= -lwinmm
LIBS	+= -lsamplerate
}

unix {
DESTDIR     = ../../../../../linux-bin/input-plugins-sw
LIBS	+= -lportaudio -lsamplerate
}

