#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets

INCLUDEPATH += . \
	       ../ ../.. \
	       /usr/local/include \
	       ../../../includes \
	       ../../../includes/scopes-qwt6 \
	       ../../../includes/various \
	       ../../../includes/filters \
	       ../decoder-utils \
	       ./signal-handling \
	       ./signal-handling/eq-1 \
	       ./utilities \
	       ./parameters \
	       ./msc ./sdc ./fac ./data ./input
DEPENDPATH += . \
	       ../ ../.. \
	       ../../../includes \
	       ../../radio-utils \
	       ../decoder-utils \
	       ./signal-handling \
	       ./utilities \
	       ./parameters \
	       ./msc ./sdc ./fac ./data ./input
HEADERS     = ../decoder-interface.h \
	       ./drm-decoder.h  \
	      ./basics.h \
	      ./ofdm-processor.h \
	      ./frame-processor.h \
	      ./input/syncer.h \
	      ./input/simplebuf.h \
	      ./signal-handling/referenceframe.h \
	      ./signal-handling/equalizer-base.h \
	      ./signal-handling/matrix2.h \
	      ./signal-handling/eq-1/equalizer-1.h \
	      ./signal-handling/eq-1/estimator-base.h \
	      ./utilities/viterbi-drm.h \
	      ./utilities/mapper.h \
	      ./utilities/prbs.h \
	      ./utilities/checkcrc.h \
	      ./utilities/qam64-metrics.h \
	      ./utilities/qam16-metrics.h \
	      ./utilities/qam4-metrics.h \
	      ./utilities/deinterleaver.h \
	      ./parameters/msc-config.h \
	      ./parameters/fac-data.h \
	      ./parameters/channelparameters.h \
	      ./parameters/serviceparameters.h \
	      ./parameters/puncture-tables.h \
	      ./fac/fac-processor.h \
	      ./sdc/sdc-streamer.h \
	      ./sdc/sdc-processor.h \
	      ./msc/msc-processor.h \
	      ./msc/msc-handler-qam16.h \
	      ./msc/msc-handler-qam64.h \
	      ./msc/msc-streamer.h \
	      ./msc/msc-handler.h \
	      ./data/data-processor.h \
	      ./data/fec-handler.h \
	      ./data/galois.h \
	      ./data/reed-solomon.h \
	      ./data/message-processor.h \
	      ./data/packet-assembler.h \
	      ./data/virtual-datahandler.h \
	      ./data/mot-data.h \
	      ./data/drm-aacdecoder.h \
	      ./data/neaacdec_dll.h \
	      ../../../includes/filters/fir-filters.h  \
	      ../../../includes/various/oscillator.h \
#	      ../../../includes/scopes-qwt6/iqdisplay.h \
	      ../../../includes/various/utilities.h \
	      ../../../includes/various/ringbuffer.h \
	      ../../../includes/various/fft.h \
#	      ../../../includes/scopes-qwt6/scope.h \
	      ../../../includes/swradio-constants.h
SOURCES     = ./drm-decoder.cpp  \
	      ./main.cpp \
	      ./basics.cpp \
	      ./input/syncer.cpp \
	      ./input/simplebuf.cpp \
	      ./ofdm-processor.cpp \
	      ./frame-processor.cpp \
	      ./signal-handling/referenceframe.cpp \
	      ./signal-handling/equalizer-base.cpp \
	      ./signal-handling/matrix2.cpp \
	      ./signal-handling/eq-1/estimator-base.cpp \
	      ./signal-handling/eq-1/equalizer-1.cpp \
	      ./utilities/viterbi-drm.cpp \
	      ./utilities/mapper.cpp \
	      ./utilities/prbs.cpp \
	      ./utilities/checkcrc.cpp \
	      ./utilities/qam64-metrics.cpp \
	      ./utilities/qam16-metrics.cpp \
	      ./utilities/qam4-metrics.cpp \
	      ./utilities/deinterleaver.cpp \
	      ./parameters/msc-config.cpp \
	      ./parameters/fac-data.cpp \
	      ./parameters/channelparameters.cpp \
	      ./parameters/serviceparameters.cpp \
	      ./parameters/puncture-tables.cpp \
	      ./fac/fac-processor.cpp \
	      ./sdc/sdc-streamer.cpp \
	      ./sdc/sdc-processor.cpp \
	      ./msc/msc-processor.cpp \
	      ./msc/msc-handler-qam16.cpp \
	      ./msc/msc-handler-qam64.cpp \
	      ./msc/msc-streamer.cpp \
	      ./msc/msc-handler.cpp \
	      ./data/data-processor.cpp \
	      ./data/fec-handler.cpp \
	      ./data/galois.cpp \
	      ./data/reed-solomon.cpp \
	      ./data/message-processor.cpp \
	      ./data/packet-assembler.cpp \
	      ./data/virtual-datahandler.cpp \
	      ./data/mot-data.cpp \
	      ./data/drm-aacdecoder.cpp \
	      ../../../src/filters/fir-filters.cpp  \
	      ../../../src/various/oscillator.cpp \
	      ../../../src/various/fft.cpp \
#	      ../../../src/scopes-qwt6/iqdisplay.cpp \
#	      ../../../src/scopes-qwt6/scope.cpp \
	      ../../../src/various/utilities.cpp 
FORMS		+= widget.ui

unix {	
CONFIG		+= estimator-1
DESTDIR    	 = ../../../../../linux-bin/decoder-plugins
INCLUDEPATH	+= /usr/include/qt5/qwt
INCLUDEPATH	+= /usr/local/include
LIBS		+= -L/usr/local/lib
#LIBS		+= -lqwt			# ubuntu 15.04
LIBS		+= -lqwt-qt5			# fedora 21
LIBS		+= -lfaad
LIBS		+= -lsndfile
LIBS		+= -lm
}

win32 {
CONFIG	+= estimator-1
DESTDIR     = ../../../../../../windows-bin/decoder-plugins
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include/qt5/qwt
LIBS	+= -lfftw3f
LIBS	+= -lqwt-qt5
LIBS	+= -lfaad
LIBS	+= -lole32
LIBS	+= -lwinmm
}

estimator-1 {
HEADERS	+= ./signal-handling/eq-1/estimator-1.h 
SOURCES	+= ./signal-handling/eq-1/estimator-1.cpp
DEFINES	+= ESTIMATOR_1
TARGET		= $$qtLibraryTarget(drmdecoder-1)
}

estimator-2 {
HEADERS	+= ./signal-handling/eq-1/estimator-2.h 
SOURCES	+= ./signal-handling/eq-1/estimator-2.cpp
LIBS	+= -larmadillo
DEFINES	+= ESTIMATOR_2
TARGET		= $$qtLibraryTarget(drmdecoder-2)
}

estimator-3 {
HEADERS	+= ./signal-handling/eq-1/estimator-3.h 
SOURCES	+= ./signal-handling/eq-1/estimator-3.cpp
LIBS	+= -larmadillo
DEFINES	+= ESTIMATOR_3
}

