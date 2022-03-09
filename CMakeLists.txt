cmake_minimum_required( VERSION 2.8.11 )
set (objectName swradio-8.1)
set (CMAKE_CXX_FLAGS "${CMAKE_XCC_FLAGS} -Wall -std=c++11 -g")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g")

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "/usr/local/bin" CACHE PATH "default install path" FORCE )
endif()
#set (CMAKE_INSTALL_PREFIX /usr/local/bin)
#	modify if you want

if(MINGW)
    add_definitions ( -municode)
endif()
IF(EXISTS (".git") AND IS_DIRECTORY (".git"))
   execute_process (
      COMMAND git rev-parse --short HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_COMMIT_HASH
      OUTPUT_STRIP_TRAILING_WHITESPACE
   )
endif ()

if(GIT_COMMIT_HASH)
   add_definitions("-DGITHASH=\"${GIT_COMMIT_HASH}\"")
else ()
   add_definitions ("-DGITHASH=\"       \"")
endif()

	add_definitions (-DHAVE_SDRPLAY)
	add_definitions (-DHAVE_AM_DECODER)
	add_definitions (-DHAVE_SSB_DECODER)
	add_definitions (-DHAVE_CW_DECODER)
	add_definitions (-DHAVE_AMTOR_DECODER)
	add_definitions (-DHAVE_PSK_DECODER)
	add_definitions (-DHAVE_FAX_DECODER)
	add_definitions (-DHAVE_RTTY_DECODER)
	add_definitions (-DHAVE_MFSK_DECODER)
	add_definitions (-DHAVE_DRM_DECODER)
	add_definitions (-DESTIMATOR_1)

	add_definitions	(-D__WITH_FDK_AAC__)
	add_definitions	(-DESTIMATOR_1)
########################################################################
# select the release build type by default to get optimization flags
########################################################################
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

### make sure our local CMake Modules path comes first
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/Modules)

######################################################################
#
#####################################################################

        find_package (PkgConfig)
        find_package (LSBId)

        find_package (Qt5Core REQUIRED)
        find_package (Qt5Widgets REQUIRED)
        find_package (Qt5Network REQUIRED)
	find_package (Qt5Xml REQUIRED)
        include_directories (
          ${Qt5Network_INCLUDE_DIRS}
        )
	find_package(QWT REQUIRED)

	find_package (FDKAAC)
	if (NOT FDKAAC_FOUND)
	   message (FATAL "please install FDKAAC")
	endif ()
	include_directories ( ${FDKAAC_INCLUDE_DIRS})
	list (APPEND extraLibs ${FDKAAC_LIBRARIES})

        find_package(FFTW3f)
        if (NOT FFTW3F_FOUND)
            message(FATAL_ERROR "please install FFTW3")
        endif ()

	find_package(Qwt)
        if (NOT QWT_FOUND)
            message(FATAL_ERROR "please install qwt library")
        endif ()
	include_directories ( ${QWT_INCLUDE_DIRS})
        list(APPEND extraLibs ${QWT_LIBRARIES})

        find_package(Portaudio)
        if (NOT PORTAUDIO_FOUND)
            message(FATAL_ERROR "please install portaudio V19")
        endif ()
        list(APPEND extraLibs ${PORTAUDIO_LIBRARIES})

        find_package(zlib)
	if (NOT ZLIB_FOUND)
            message(FATAL_ERROR "please install libz")
        endif ()
	list(APPEND extraLibs ${ZLIB_LIBRARY})

        find_package(LibSndFile)
        if (NOT LIBSNDFILE_FOUND)
            message(FATAL_ERROR "please install libsndfile")
        endif ()
        list(APPEND extraLibs ${LIBSNDFILE_LIBRARY})

        find_package(LibSampleRate)
        if (NOT LIBSAMPLERATE_FOUND)
            message(FATAL_ERROR "please install libsamplerate")
        endif ()
        list(APPEND extraLibs ${LIBSAMPLERATE_LIBRARY})

        find_library (PTHREADS pthread)
        if (NOT(PTHREADS))
           message (FATAL_ERROR "please install libpthread")
        else (NOT(PTHREADS))
           set (extraLibs ${extraLibs} ${PTHREADS})
        endif (NOT(PTHREADS))

	find_package(FaadDRM)
	if (NOT FAAD_FOUND )
           message(FATAL_ERROR "please install libfaad_drm")
	endif ()
	list(APPEND extraLibs ${FAAD_LIBRARY})

#######################################################################
#
#	Here we really start

	include_directories (
	           ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	           ${QT5Widgets_INCLUDES}
	           ${QT_QTCORE_INCLUDE_DIR}
	           ${QT_QTGUI_INCLUDE_DIR}
	           .
	           ./
	           decimators 
	           filters 
	           various 
	           output 
	           scopes-qwt6 
	           devices 
	           devices/filereader 
	           devices/sdrplay-handler
	           devices/hackrf-handler
	           devices/rtlsdr-handler
	           decoders 
	           decoders/am-decoder 
	           decoders/ssb-decoder 
	           decoders/psk-decoder 
	           decoders/cw-decoder 
	           decoders/rtty-decoder 
	           decoders/amtor-decoder 
	           decoders/fax-decoder 
	           decoders/mfsk-decoder
	         ./decoders/drm-decoder 
	         ./decoders/drm-decoder/ofdm
	         ./decoders/drm-decoder/support
	         ./decoders/drm-decoder/signal-handling 
	         ./decoders/drm-decoder/signal-handling/eq-1 
	         ./decoders/drm-decoder/utilities 
	         ./decoders/drm-decoder/parameters 
	         ./decoders/drm-decoder/msc 
	         ./decoders/drm-decoder/sdc 
	         ./decoders/drm-decoder/fac 
	         ./decoders/drm-decoder/data 
	         ./decoders/drm-decoder/input
	)

	set (${objectName}_HDRS
	   ./radio-constants.h 
	   ./radio.h 
	   ./bandplan.h
	   ./output/upconverter.h 
	   ./output/audiosink.h 
	   ./various/agchandler.h 
	   ./various/ringbuffer.h 
	   ./various/fft.h 
	   ./various/shifter.h 
	   ./various/utilities.h 
	   ./various/slidingfft.h 
	   ./various/program-list.h 
	   ./various/popup-keypad.h 
	   ./various/s-meter.h 
	   ./various/downconverter.h 
	   ./various/rate-converter.h
	   ./decimators/newconverter.h 
	   ./decimators/decimator.h 
	   ./filters/fft-filters.h 
	   ./filters/fir-filters.h 
	   ./filters/iir-filters.h 
	   ./scopes-qwt6/virtual-scope.h 
	   ./scopes-qwt6/spectrogramdata.h 
	   ./scopes-qwt6/waterfall-scope.h 
	   ./scopes-qwt6/spectrum-scope.h 
	   ./scopes-qwt6/audio-scope.h
	   ./scopes-qwt6/fft-scope.h
           ./devices/device-handler.h
           ./devices/filereader/filereader.h
           ./devices/filereader/filehulp.h
           ./devices/sdrplay-handler/sdrplay-handler.h
           ./devices/sdrplay-handler/sdrplayselect.h
	   ./devices/rtlsdr-handler/rtlsdr-handler.h
	   ./devices/rtlsdr-handler/dongleselect.h
	   ./devices/hackrf-handler/hackrf-handler.h
	   ./decoders/virtual-decoder.h 
	   ./decoders/am-decoder/am-decoder.h
	   ./decoders/ssb-decoder/ssb-decoder.h
	   ./decoders/cw-decoder/cw-decoder.h
	   ./decoders/amtor-decoder/amtor-decoder.h
	   ./decoders/psk-decoder/psk-decoder.h 
	   ./decoders/psk-decoder/viterbi.h
	   ./decoders/rtty-decoder/rtty-decoder.h
	   ./decoders/fax-decoder/fax-decoder.h 
	   ./decoders/fax-decoder/fax-demodulator.h 
	   ./decoders/fax-decoder/fax-image.h 
	   ./decoders/fax-decoder/fax-scroller.h 
	   ./decoders/fax-decoder/fax-filenames.h
	   ./decoders/mfsk-decoder/mfsk-decoder.h 
	   ./decoders/mfsk-decoder/goertzel-filter.h 
#	   ./decoders/mfsk-decoder/slidingfft.h 
	   ./decoders/mfsk-decoder/interleave.h
	   ./decoders/drm-decoder/drm-decoder.h  
	   ./decoders/drm-decoder/iqdisplay.h 
	   ./decoders/drm-decoder/eqdisplay.h 
	   ./decoders/drm-decoder/basics.h 
	   ./decoders/drm-decoder/frame-processor.h 
	   ./decoders/drm-decoder/ofdm/reader.h 
	   ./decoders/drm-decoder/ofdm/correlator.h 
	   ./decoders/drm-decoder/ofdm/timesync.h 
	   ./decoders/drm-decoder/ofdm/freqsyncer.h 
	   ./decoders/drm-decoder/ofdm/word-collector.h 
	   ./decoders/drm-decoder/signal-handling/referenceframe.h 
	   ./decoders/drm-decoder/signal-handling/equalizer-base.h 
	   ./decoders/drm-decoder/signal-handling/eq-1/equalizer-1.h 
	   ./decoders/drm-decoder/signal-handling/eq-1/estimator-base.h 
	   ./decoders/drm-decoder/signal-handling/eq-1/estimator-1.h 
	   ./decoders/drm-decoder/support/my-array.h 
	   ./decoders/drm-decoder/support/viterbi-drm.h 
	   ./decoders/drm-decoder/support/mapper.h 
	   ./decoders/drm-decoder/support/prbs.h 
	   ./decoders/drm-decoder/support/checkcrc.h 
	   ./decoders/drm-decoder/support/qam64-metrics.h 
	   ./decoders/drm-decoder/support/qam16-metrics.h 
	   ./decoders/drm-decoder/support/qam4-metrics.h 
	   ./decoders/drm-decoder/support/deinterleaver.h 
	   ./decoders/drm-decoder/support/matrix2.h 
	   ./decoders/drm-decoder/support/protlevels.h 
	   ./decoders/drm-decoder/support/mer4-values.h 
	   ./decoders/drm-decoder/support/mer16-values.h 
	   ./decoders/drm-decoder/support/mer64-values.h 
	   ./decoders/drm-decoder/parameters/state-descriptor.h 
	   ./decoders/drm-decoder/parameters/puncture-tables.h 
	   ./decoders/drm-decoder/fac/fac-tables.h 
	   ./decoders/drm-decoder/fac/fac-processor.h 
	   ./decoders/drm-decoder/sdc/sdc-streamer.h 
	   ./decoders/drm-decoder/sdc/sdc-processor.h 
	   ./decoders/drm-decoder/msc/backend-controller.h 
	   ./decoders/drm-decoder/msc/msc-processor.h 
	   ./decoders/drm-decoder/msc/msc-handler-qam16.h 
	   ./decoders/drm-decoder/msc/msc-handler-qam64.h 
	   ./decoders/drm-decoder/msc/msc-streamer.h 
	   ./decoders/drm-decoder/msc/msc-handler.h 
	   ./decoders/drm-decoder/data/data-processor.h 
#	   ./decoders/drm-decoder/data/aac-processor-faad.h 
	   ./decoders/drm-decoder/data/aac-processor-fdk.h 
	   ./decoders/drm-decoder/data/xheaac-processor.h 
#	   ./decoders/drm-decoder/data/drm-aacdecoder.h 
	   ./decoders/drm-decoder/data/fec-handler.h 
	   ./decoders/drm-decoder/data/galois.h 
	   ./decoders/drm-decoder/data/reed-solomon.h 
	   ./decoders/drm-decoder/data/message-processor.h 
	   ./decoders/drm-decoder/data/packet-assembler.h 
	   ./decoders/drm-decoder/data/virtual-datahandler.h 
	   ./decoders/drm-decoder/data/mot-data.h 
	)

	set (${objectName}_SRCS
	     ${${objectName}_SRCS}
	   ./main.cpp 
	   ./radio.cpp 
	   ./bandplan.cpp
	   ./output/upconverter.cpp 
	   ./output/audiosink.cpp 
	   ./various/agchandler.cpp 
	   ./various/fft.cpp 
	   ./various/popup-keypad.cpp 
	   ./various/s-meter.cpp 
	   ./various/shifter.cpp 
	   ./various/utilities.cpp 
	   ./various/slidingfft.cpp
	   ./various/program-list.cpp 
	   ./various/downconverter.cpp 
	   ./various/rate-converter.cpp 
	   ./filters/fft-filters.cpp 
	   ./filters/fir-filters.cpp 
	   ./filters/iir-filters.cpp 
	   ./scopes-qwt6/virtual-scope.cpp 
	   ./scopes-qwt6/waterfall-scope.cpp 
	   ./scopes-qwt6/spectrum-scope.cpp 
	   ./scopes-qwt6/audio-scope.cpp
	   ./scopes-qwt6/fft-scope.cpp 
	   ./decimators/decimator.cpp 
	   ./devices/device-handler.cpp 
	   ./devices/filereader/filereader.cpp 
	   ./devices/filereader/filehulp.cpp 
	   ./devices/sdrplay-handler/sdrplay-handler.cpp 
	   ./devices/sdrplay-handler/sdrplayselect.cpp 
	   ./devices/rtlsdr-handler/rtlsdr-handler.cpp
	   ./devices/rtlsdr-handler/dongleselect.cpp
	   ./devices/hackrf-handler/hackrf-handler.cpp
	   ./decoders/virtual-decoder.cpp
	   ./decoders/am-decoder/am-decoder.cpp 
	   ./decoders/ssb-decoder/ssb-decoder.cpp 
	   ./decoders/cw-decoder/cw-decoder.cpp 
	   ./decoders/amtor-decoder/amtor-decoder.cpp 
	   ./decoders/psk-decoder/psk-decoder.cpp 
	   ./decoders/psk-decoder/viterbi.cpp 
	   ./decoders/rtty-decoder/rtty-decoder.cpp 
	   ./decoders/fax-decoder/fax-decoder.cpp 
	   ./decoders/fax-decoder/fax-demodulator.cpp 
	   ./decoders/fax-decoder/fax-image.cpp 
	   ./decoders/fax-decoder/fax-scroller.cpp 
	   ./decoders/fax-decoder/fax-filenames.cpp
	   ./decoders/mfsk-decoder/mfsk-decoder.cpp 
	   ./decoders/mfsk-decoder/goertzel-filter.cpp 
#	   ./decoders/mfsk-decoder/slidingfft.cpp 
	   ./decoders/mfsk-decoder/interleave.cpp
	   ./decoders/drm-decoder/drm-decoder.cpp  
	   ./decoders/drm-decoder/eqdisplay.cpp 
	   ./decoders/drm-decoder/iqdisplay.cpp 
	   ./decoders/drm-decoder/basics.cpp 
	   ./decoders/drm-decoder/frame-processor.cpp 
	   ./decoders/drm-decoder/ofdm/reader.cpp 
	   ./decoders/drm-decoder/ofdm/correlator.cpp 
	   ./decoders/drm-decoder/ofdm/timesync.cpp 
	   ./decoders/drm-decoder/ofdm/freqsyncer.cpp 
	   ./decoders/drm-decoder/ofdm/word-collector.cpp 
	   ./decoders/drm-decoder/signal-handling/referenceframe.cpp 
	   ./decoders/drm-decoder/signal-handling/equalizer-base.cpp 
	   ./decoders/drm-decoder/signal-handling/eq-1/equalizer-1.cpp 
	   ./decoders/drm-decoder/signal-handling/eq-1/estimator-base.cpp 
	   ./decoders/drm-decoder/signal-handling/eq-1/estimator-1.cpp 
	   ./decoders/drm-decoder/support/viterbi-drm.cpp 
	   ./decoders/drm-decoder/support/mapper.cpp 
	   ./decoders/drm-decoder/support/prbs.cpp 
	   ./decoders/drm-decoder/support/checkcrc.cpp 
	   ./decoders/drm-decoder/support/qam64-metrics.cpp 
	   ./decoders/drm-decoder/support/qam16-metrics.cpp 
	   ./decoders/drm-decoder/support/qam4-metrics.cpp 
 	   ./decoders/drm-decoder/support/deinterleaver.cpp 
	   ./decoders/drm-decoder/support/matrix2.cpp 
	   ./decoders/drm-decoder/support/protlevels.cpp 
	   ./decoders/drm-decoder/support/mer4-values.cpp 
	   ./decoders/drm-decoder/support/mer16-values.cpp 
	   ./decoders/drm-decoder/support/mer64-values.cpp 
	   ./decoders/drm-decoder/parameters/state-descriptor.cpp 
	   ./decoders/drm-decoder/parameters/puncture-tables.cpp 
	   ./decoders/drm-decoder/fac/fac-tables.cpp 
	   ./decoders/drm-decoder/fac/fac-processor.cpp 
	   ./decoders/drm-decoder/sdc/sdc-streamer.cpp 
	   ./decoders/drm-decoder/sdc/sdc-processor.cpp 
	   ./decoders/drm-decoder/msc/backend-controller.cpp 
	   ./decoders/drm-decoder/msc/msc-processor.cpp 
	   ./decoders/drm-decoder/msc/msc-handler-qam16.cpp 
	   ./decoders/drm-decoder/msc/msc-handler-qam64.cpp 
	   ./decoders/drm-decoder/msc/msc-streamer.cpp 
	   ./decoders/drm-decoder/msc/msc-handler.cpp 
	   ./decoders/drm-decoder/data/data-processor.cpp 
#	   ./decoders/drm-decoder/data/aac-processor-faad.cpp 
	   ./decoders/drm-decoder/data/aac-processor-fdk.cpp 
	   ./decoders/drm-decoder/data/xheaac-processor.cpp 
#	   ./decoders/drm-decoder/data/drm-aacdecoder.cpp 
	   ./decoders/drm-decoder/data/fec-handler.cpp 
	   ./decoders/drm-decoder/data/galois.cpp 
	   ./decoders/drm-decoder/data/reed-solomon.cpp 
	   ./decoders/drm-decoder/data/message-processor.cpp 
	   ./decoders/drm-decoder/data/packet-assembler.cpp 
	   ./decoders/drm-decoder/data/virtual-datahandler.cpp 
	   ./decoders/drm-decoder/data/mot-data.cpp 
	)

	set (${objectName}_MOCS
	   ./radio.h 
           ./output/audiosink.h 
           ./various/program-list.h 
           ./various/popup-keypad.h 
           ./scopes-qwt6/virtual-scope.h 
           ./scopes-qwt6/spectrogramdata.h 
           ./scopes-qwt6/waterfall-scope.h 
           ./scopes-qwt6/spectrum-scope.h 
	   ./scopes-qwt6/audio-scope.h
           ./scopes-qwt6/fft-scope.h 
	   ./devices/device-handler.h
	   ./devices/filereader/filereader.h 
	   ./devices/filereader/filehulp.h
	   ./devices/sdrplay-handler/sdrplay-handler.h 
	   ./devices/sdrplay-handler/sdrplayselect.h 
	   ./devices/rtlsdr-handler/rtlsdr-handler.h
	   ./devices/rtlsdr-handler/dongleselect.h
	   ./devices/hackrf-handler/hackrf-handler.h
           ./decoders/virtual-decoder.h 
	   ./decoders/am-decoder/am-decoder.h 
	   ./decoders/ssb-decoder/ssb-decoder.h 
	   ./decoders/cw-decoder/cw-decoder.h 
	   ./decoders/amtor-decoder/amtor-decoder.h 
	   ./decoders/psk-decoder/psk-decoder.h 
	   ./decoders/rtty-decoder/rtty-decoder.h 
	   ./decoders/fax-decoder/fax-decoder.h 
	   ./decoders/fax-decoder/fax-demodulator.h 
	   ./decoders/fax-decoder/fax-image.h 
	   ./decoders/fax-decoder/fax-scroller.h 
	   ./decoders/fax-decoder/fax-filenames.h 
	   ./decoders/mfsk-decoder/mfsk-decoder.h 
	   ./decoders/drm-decoder/drm-decoder.h  
	   ./decoders/drm-decoder/iqdisplay.h 
	   ./decoders/drm-decoder/eqdisplay.h 
	   ./decoders/drm-decoder/frame-processor.h 
	   ./decoders/drm-decoder/fac/fac-processor.h
	   ./decoders/drm-decoder/sdc/sdc-processor.h
	   ./decoders/drm-decoder/ofdm/reader.h
	   ./decoders/drm-decoder/ofdm/freqsyncer.h
	   ./decoders/drm-decoder/ofdm/word-collector.h
	   ./decoders/drm-decoder/signal-handling/equalizer-base.h
	   ./decoders/drm-decoder/signal-handling/eq-1/equalizer-1.h
	   ./decoders/drm-decoder/signal-handling/eq-1/estimator-base.h
	   ./decoders/drm-decoder/signal-handling/eq-1/estimator-1.h
	   ./decoders/drm-decoder/sdc/sdc-processor.h
	   ./decoders/drm-decoder/msc/msc-handler.h
	   ./decoders/drm-decoder/msc/backend-controller.h
	   ./decoders/drm-decoder/data/mot-data.h
	   ./decoders/drm-decoder/data/data-processor.h
	   ./decoders/drm-decoder/data/aac-processor-fdk.h
	   ./decoders/drm-decoder/data/xheaac-processor.h
	   ./decoders/drm-decoder/data/message-processor.h
	)

	set (${objectName}_UIS
	        ${${objectName}_UIS}
	   ./newradio.ui 
	   ./devices/filereader/filereader-widget.ui
	   ./devices/sdrplay-handler/sdrplay-widget.ui
	   ./devices/rtlsdr-handler/rtlsdr-widget.ui
           ./devices/hackrf-handler/hackrf-widget.ui
	   ./decoders/am-decoder/am-decoder.ui
	   ./decoders/ssb-decoder/ssb-decoder.ui
	   ./decoders/cw-decoder/cw-decoder.ui
	   ./decoders/amtor-decoder/amtor-widget.ui
	   ./decoders/psk-decoder/psk-decoder.ui
	   ./decoders/rtty-decoder/rtty-decoder.ui
	   ./decoders/fax-decoder/fax-decoder.ui
	   ./decoders/mfsk-decoder/mfsk-decoder.ui
	   ./decoders/drm-decoder/drmdecoder.ui
	)


        QT5_WRAP_UI (UIS ${${objectName}_UIS})

        include_directories (
                  ${SDRPLAY_INCLUDES}
                  ${QT5Widgets_INCLUDES}
                  ${QT_QTCORE_INCLUDE_DIR}
                  ${QT_QTGUI_INCLUDE_DIR}
                  ${FFTW_INCLUDE_DIRS}
                  ${PORTAUDIO_INCLUDE_DIRS}
                  ${FAAD_INCLUDE_DIRS}
                  ${SNDFILES_INCLUDE_DIRS}
        )

        QT5_WRAP_CPP (MOCS ${${objectName}_MOCS})

	add_executable (${objectName}
	                ${${objectName}_SRCS}
	                ${UIS}
	                ${RSCS}
	                ${TRS}
	                ${MOCS}
	)

	target_link_libraries (${objectName}
                               Qt5::Widgets
	                       Qt5::Xml
                               ${FFTW3F_LIBRARIES}
                               ${extraLibs}
                               ${CMAKE_DL_LIBS}
        )


	INSTALL (TARGETS ${objectName} DESTINATION  ${CMAKE_INSTALL_PREFIX})
#	INSTALL (TARGETS ${objectName} DESTINATION ./linux-bin)

########################################################################
# Create uninstall target
########################################################################

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
