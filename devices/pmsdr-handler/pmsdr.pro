#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets

INCLUDEPATH += . \
	       ../ ../.. \
	       ../../../includes \
	       ../../../includes/various
HEADERS     = ../rig-interface.h \
	      ./pmsdr.h \
	      ./pmsdr_usb.h  \
	      ./pmsdr_comm.h \
	      ./si570-handler.h \
	      ../../../includes/swradio-constants.h \
	      ../../../includes/various/ringbuffer.h \
	      ../pa-reader.h
SOURCES     = ./pmsdr.cpp \	
              ./pmsdr_usb.cpp \
	      ./pmsdr_comm.cpp \
	      ./si570-handler.cpp \
	      ../pa-reader.cpp
TARGET      = $$qtLibraryTarget(device_pmsdr)
FORMS	+= ./widget.ui

win32 {
DESTDIR     = ../../../../../../windows-bin/input-plugins
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
LIBS	+= -lportaudio
LIBS	+= -lusb-1.0
LIBS	+= -lole32
LIBS	+= -lwinmm
}

unix{
DESTDIR     = ../../../../../linux-bin/input-plugins-sw
LIBS	+= -lusb-1.0 
}

