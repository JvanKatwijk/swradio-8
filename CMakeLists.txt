cmake_minimum_required( VERSION 2.8.11 )
set (objectName swradio-9)
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
	add_definitions (-DHAVE_SDRPLAY_V3)
	add_definitions (-DHAVE_HACKRF)
	add_definitions (-DHAVE_RTLSDR)
	add_definitions (-DHAVE_RTTY)
	add_definitions (-DHAVE_AM_DECODER)
	add_definitions (-DHAVE_SSB_DECODER)
	add_definitions (-DHAVE_CW_DECODER)
	add_definitions (-DHAVE_AMTOR_DECODER)
	add_definitions (-DHAVE_PSK_DECODER)
	add_definitions (-DHAVE_RTTY_DECODER)
	add_definitions (-DHAVE_FT8_DECODER)
	add_definitions (-DHAVE_FAX_DECODER)
	add_definitions (-DHAVE_DRM_DECODER)
	add_definitions (-DESTIMATOR_1)
	add_definitions (-D__WITH_FDK_AAC__)
	
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
	  ${Qt5Xml_INCLUDE_DIRS}
        )
	find_package(QWT REQUIRED)

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

	set (extraLibs ${extraLibs} libfdk-aac.so)
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
	           ./fdk-aac
	           decimators 
	           filters 
	           various 
	           output 
	           scopes-qwt6 
	           devices 
	           devices/filereader 
	           devices/sdrplay-handler
	           devices/sdrplay-handler-v3
	           devices/hackrf-handler
	           devices/rtlsdr-handler
	           decoders 
	           decoders/am-decoder 
	           decoders/ssb-decoder 
	           decoders/psk-decoder 
	           decoders/cw-decoder 
	           decoders/rtty-decoder 
	           decoders/ft8-decoder 
	           decoders/ft8-decoder/fft
	           decoders/amtor-decoder 
	           decoders/fax-decoder 
	         ./decoders/drm-decoder 
	         ./decoders/drm-decoder/fac 
	         ./decoders/drm-decoder/equalizer
	         ./decoders/drm-decoder/msc 
	         ./decoders/drm-decoder/ofdm 
	         ./decoders/drm-decoder/parameters 
	         ./decoders/drm-decoder/sdc 
	         ./decoders/drm-decoder/data 
	         ./decoders/drm-decoder/support
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
           ./devices/deviceselect.h
           ./devices/filereader/filereader.h
           ./devices/filereader/filehulp.h
           ./devices/sdrplay-handler/sdrplay-handler.h
           ./devices/sdrplay-handler-v3/sdrplay-handler-v3.h
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
	   ./decoders/ft8-decoder/ft8-decoder.h
	   ./decoders/ft8-decoder/ft8-processor.h
	   ./decoders/ft8-decoder/pack-handler.h
	   ./decoders/ft8-decoder/dl-cache.h
	   ./decoders/ft8-decoder/ldpc.h
	   ./decoders/ft8-decoder/hashHandler.h
	   ./decoders/ft8-decoder/fft/kiss_fft.h
	   ./decoders/ft8-decoder/fft/kiss_fftr.h
	   ./decoders/ft8-decoder/fft/kiss_fft_guts.h
	   ./decoders/rtty-decoder/rtty-decoder.h
	   ./decoders/fax-decoder/fax-decoder.h 
	   ./decoders/fax-decoder/fax-demodulator.h 
	   ./decoders/fax-decoder/fax-image.h 
	   ./decoders/fax-decoder/fax-scroller.h 
	   ./decoders/fax-decoder/fax-filenames.h
	   ./decoders/drm-decoder/drm-decoder.h  
	   ./decoders/drm-decoder/eqdisplay.h 
	   ./decoders/drm-decoder/iqdisplay.h 
	   ./decoders/drm-decoder/basics.h 
	   ./decoders/drm-decoder/up-converter.h 
	   ./decoders/drm-decoder/rate-converter.h 
	   ./decoders/drm-decoder/data/data-processor.h 
	   ./decoders/drm-decoder/data/message-processor.h 
	   ./decoders/drm-decoder/fac/fac-tables.h 
	   ./decoders/drm-decoder/fac/fac-processor.h 
	   ./decoders/drm-decoder/sdc/sdc-include.h 
	   ./decoders/drm-decoder/sdc/sdc-processor.h 
	   ./decoders/drm-decoder/sdc/sdc-streamer.h 
	   ./decoders/drm-decoder/msc/backend-controller.h 
	   ./decoders/drm-decoder/msc/msc-handler.h 
	   ./decoders/drm-decoder/msc/msc-handler-qam16.h 
	   ./decoders/drm-decoder/msc/msc-handler-qam64.h 
	   ./decoders/drm-decoder/msc/msc-processor.h 
	   ./decoders/drm-decoder/msc/msc-streamer.h 
	   ./decoders/drm-decoder/ofdm/correlator.h 
	   ./decoders/drm-decoder/ofdm/freqsyncer.h 
	   ./decoders/drm-decoder/ofdm/reader.h 
	   ./decoders/drm-decoder/ofdm/timesync.h 
	   ./decoders/drm-decoder/ofdm/word-collector.h 
	   ./decoders/drm-decoder/equalizer/equalizer-1.h 
	   ./decoders/drm-decoder/equalizer/equalizer-base.h 
#	   ./decoders/drm-decoder/equalizer/estimator-2.h 
	   ./decoders/drm-decoder/equalizer/estimator-1.h 
	   ./decoders/drm-decoder/equalizer/matrix2.h 
	   ./decoders/drm-decoder/equalizer/referenceframe.h 
	   ./decoders/drm-decoder/parameters/msc-config.h 
	   ./decoders/drm-decoder/parameters/puncture-tables.h 
	   ./decoders/drm-decoder/parameters/state-descriptor.h 
	   ./decoders/drm-decoder/support/checkcrc.h 
	   ./decoders/drm-decoder/support/deinterleaver.h 
	   ./decoders/drm-decoder/support/drm-shifter.h 
	   ./decoders/drm-decoder/support/fft-complex.h 
	   ./decoders/drm-decoder/support/mapper.h 
	   ./decoders/drm-decoder/support/mer16-values.h 
	   ./decoders/drm-decoder/support/mer4-values.h 
	   ./decoders/drm-decoder/support/mer64-values.h 
	   ./decoders/drm-decoder/support/my-array.h 
	   ./decoders/drm-decoder/support/prbs.h 
	   ./decoders/drm-decoder/support/protlevels.h 
	   ./decoders/drm-decoder/support/qam16-metrics.h 
	   ./decoders/drm-decoder/support/qam4-metrics.h 
	   ./decoders/drm-decoder/support/qam64-metrics.h 
	   ./decoders/drm-decoder/support/viterbi-drm.h
	   ./decoders/drm-decoder/data/aac-processor-fdk.h 
	   ./decoders/drm-decoder/data/xheaac-processor.h 
	   ./decoders/drm-decoder/aac-handler.h
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
	   ./various/slidingfft.cpp
	   ./various/utilities.cpp 
	   ./various/program-list.cpp 
	   ./various/downconverter.cpp 
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
	   ./devices/deviceselect.cpp 
	   ./devices/filereader/filereader.cpp 
	   ./devices/filereader/filehulp.cpp 
	   ./devices/sdrplay-handler/sdrplay-handler.cpp 
	   ./devices/sdrplay-handler-v3/sdrplay-handler-v3.cpp 
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
	   ./decoders/ft8-decoder/ft8-decoder.cpp
           ./decoders/ft8-decoder/ft8-processor.cpp
           ./decoders/ft8-decoder/pack-handler.cpp
	   ./decoders/ft8-decoder/ldpc.cpp
           ./decoders/ft8-decoder/hashHandler.cpp
           ./decoders/ft8-decoder/fft/kiss_fft.c
           ./decoders/ft8-decoder/fft/kiss_fftr.c
	   ./decoders/fax-decoder/fax-decoder.cpp 
	   ./decoders/fax-decoder/fax-demodulator.cpp 
	   ./decoders/fax-decoder/fax-image.cpp 
	   ./decoders/fax-decoder/fax-scroller.cpp 
	   ./decoders/fax-decoder/fax-filenames.cpp
	   ./decoders/drm-decoder/drm-decoder.cpp  
	   ./decoders/drm-decoder/eqdisplay.cpp 
	   ./decoders/drm-decoder/iqdisplay.cpp 
	   ./decoders/drm-decoder/up-converter.cpp 
	   ./decoders/drm-decoder/rate-converter.cpp 
	   ./decoders/drm-decoder/basics.cpp 
	   ./decoders/drm-decoder/data/data-processor.cpp 
	   ./decoders/drm-decoder/data/message-processor.cpp 
	   ./decoders/drm-decoder/data/up-filter.cpp 
	   ./decoders/drm-decoder/fac/fac-tables.cpp 
	   ./decoders/drm-decoder/fac/fac-processor.cpp 
	   ./decoders/drm-decoder/sdc/sdc-processor.cpp 
	   ./decoders/drm-decoder/sdc/sdc-streamer.cpp 
	   ./decoders/drm-decoder/msc/backend-controller.cpp 
	   ./decoders/drm-decoder/msc/msc-handler.cpp 
	   ./decoders/drm-decoder/msc/msc-handler-qam16.cpp 
	   ./decoders/drm-decoder/msc/msc-handler-qam64.cpp 
	   ./decoders/drm-decoder/msc/msc-processor.cpp 
	   ./decoders/drm-decoder/msc/msc-streamer.cpp 
	   ./decoders/drm-decoder/ofdm/correlator.cpp 
	   ./decoders/drm-decoder/ofdm/freqsyncer.cpp 
	   ./decoders/drm-decoder/ofdm/reader.cpp 
	   ./decoders/drm-decoder/ofdm/timesync.cpp 
	   ./decoders/drm-decoder/ofdm/word-collector.cpp 
	   ./decoders/drm-decoder/equalizer/equalizer-1.cpp 
	   ./decoders/drm-decoder/equalizer/equalizer-base.cpp 
	   ./decoders/drm-decoder/equalizer/estimator-1.cpp 
	   ./decoders/drm-decoder/equalizer/matrix2.cpp 
	   ./decoders/drm-decoder/equalizer/referenceframe.cpp 
	   ./decoders/drm-decoder/parameters/msc-config.cpp 
	   ./decoders/drm-decoder/parameters/puncture-tables.cpp 
	   ./decoders/drm-decoder/parameters/state-descriptor.cpp 
	   ./decoders/drm-decoder/support/checkcrc.cpp 
	   ./decoders/drm-decoder/support/deinterleaver.cpp 
	   ./decoders/drm-decoder/support/drm-shifter.cpp 
	   ./decoders/drm-decoder/support/fft-complex.cpp 
	   ./decoders/drm-decoder/support/mapper.cpp 
	   ./decoders/drm-decoder/support/mer16-values.cpp 
	   ./decoders/drm-decoder/support/mer4-values.cpp 
	   ./decoders/drm-decoder/support/mer64-values.cpp 
	   ./decoders/drm-decoder/support/prbs.cpp 
	   ./decoders/drm-decoder/support/protlevels.cpp 
	   ./decoders/drm-decoder/support/qam16-metrics.cpp 
	   ./decoders/drm-decoder/support/qam4-metrics.cpp 
	   ./decoders/drm-decoder/support/qam64-metrics.cpp 
	   ./decoders/drm-decoder/support/viterbi-drm.cpp
	   ./decoders/drm-decoder/data/aac-processor-fdk.cpp 
	   ./decoders/drm-decoder/data/xheaac-processor.cpp  
	   ./decoders/drm-decoder/aac-handler.cpp
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
	   ./devices/deviceselect.h
	   ./devices/filereader/filereader.h 
	   ./devices/filereader/filehulp.h
	   ./devices/sdrplay-handler/sdrplay-handler.h 
	   ./devices/sdrplay-handler-v3/sdrplay-handler-v3.h 
	   ./devices/rtlsdr-handler/rtlsdr-handler.h
	   ./devices/rtlsdr-handler/dongleselect.h
	   ./devices/hackrf-handler/hackrf-handler.h
           ./decoders/virtual-decoder.h 
	   ./decoders/am-decoder/am-decoder.h 
	   ./decoders/ssb-decoder/ssb-decoder.h 
	   ./decoders/cw-decoder/cw-decoder.h 
	   ./decoders/amtor-decoder/amtor-decoder.h 
	   ./decoders/psk-decoder/psk-decoder.h 
	   ./decoders/ft8-decoder/ft8-decoder.h 
	   ./decoders/ft8-decoder/ft8-processor.h 
	   ./decoders/rtty-decoder/rtty-decoder.h 
	   ./decoders/fax-decoder/fax-decoder.h 
	   ./decoders/fax-decoder/fax-demodulator.h 
	   ./decoders/fax-decoder/fax-image.h 
	   ./decoders/fax-decoder/fax-scroller.h 
	   ./decoders/fax-decoder/fax-filenames.h 
	   ./decoders/drm-decoder/drm-decoder.h  
	   ./decoders/drm-decoder/iqdisplay.h
	   ./decoders/drm-decoder/eqdisplay.h
	   ./decoders/drm-decoder/fac/fac-processor.h 
	   ./decoders/drm-decoder/sdc/sdc-processor.h 
	   ./decoders/drm-decoder/msc/msc-processor.h 
	   ./decoders/drm-decoder/msc/msc-handler.h 
	   ./decoders/drm-decoder/msc/msc-handler.h 
	   ./decoders/drm-decoder/msc/msc-handler-qam16.h 
	   ./decoders/drm-decoder/msc/msc-handler-qam64.h 
	   ./decoders/drm-decoder/msc/backend-controller.h 
	   ./decoders/drm-decoder/parameters/state-descriptor.h
	   ./decoders/drm-decoder/ofdm/freqsyncer.h
	   ./decoders/drm-decoder/ofdm/word-collector.h
	   ./decoders/drm-decoder/equalizer/equalizer-base.h
	   ./decoders/drm-decoder/equalizer/equalizer-1.h
	   ./decoders/drm-decoder/equalizer/estimator-1.h
	   ./decoders/drm-decoder/data/aac-processor-fdk.h
	   ./decoders/drm-decoder/data/xheaac-processor.h
	   ./decoders/drm-decoder/data/data-processor.h
	   ./decoders/drm-decoder/data/message-processor.h
	)

	set (${objectName}_UIS
	        ${${objectName}_UIS}
	   ./newradio.ui 
	   ./devices/filereader/filereader-widget.ui
	   ./devices/sdrplay-handler/sdrplay-widget.ui
	   ./devices/sdrplay-handler-v3/sdrplay-widget-v3.ui
	   ./devices/rtlsdr-handler/rtlsdr-widget.ui
           ./devices/hackrf-handler/hackrf-widget.ui
	   ./decoders/am-decoder/am-decoder.ui
	   ./decoders/ssb-decoder/ssb-decoder.ui
	   ./decoders/cw-decoder/cw-decoder.ui
	   ./decoders/amtor-decoder/amtor-widget.ui
	   ./decoders/psk-decoder/psk-decoder.ui
	   ./decoders/ft8-decoder/ft8-decoder.ui
	   ./decoders/rtty-decoder/rtty-decoder.ui
	   ./decoders/fax-decoder/fax-decoder.ui
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
