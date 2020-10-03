# Find libfdkaac

#include(FindPkgConfig)
#pkg_check_modules(FDKAAC fdkaac)

FIND_PATH(FDKAAC_INCLUDE_DIRS
  NAMES
     aacdecoder_lib.h
  PATHS
     /usr/include/fdk-aac
     /usr/local/include/fdk-aac
)

FIND_LIBRARY(FDKAAC_LIBRARIES
   NAMES
      fdk-aac
      libfdk-aac
   PATHS
      /usr/lib64/fdk-aac
      /usr/local/lib/fdk-aac
)

IF (FDKAAC_INCLUDE_DIRS AND FDKAAC_LIBRARIES)
    SET(FDKAAC_FOUND TRUE)
ENDIF (FDKAAC_INCLUDE_DIRS AND FDKAAC_LIBRARIES)

IF (FDKAAC_FOUND)
    IF (NOT FDKAAC_FIND_QUIETLY)
        MESSAGE (STATUS "Found fdkaac: ${FDKAAC_LIBRARY}")
    ENDIF (NOT FDKAAC_FIND_QUIETLY)
ELSE (FDKAAC_FOUND)
    IF (FDKAAC_FIND_REQUIRED)
        MESSAGE (FATAL_ERROR "Could not find fdkaac")
    ENDIF (FDKAAC_FIND_REQUIRED)
ENDIF (FDKAAC_FOUND)
