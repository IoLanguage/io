# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Find libloudmouth.

FIND_PATH(LOUDMOUTH_INCLUDE_DIR
        loudmouth/loudmouth.h
        /usr/include/loudmouth-1.0
        /usr/local/include/loudmouth-1.0
        /opt/include/loudmouth-1.0
        /opt/local/include/loudmouth-1.0)

SET(LOUDMOUTH_NAMES ${LOUDMOUTH_NAMES} loudmouth loudmouth-1 libloudmouth libloudmouth-1)
FIND_LIBRARY(LOUDMOUTH_LIBRARY NAMES ${LOUDMOUTH_NAMES} PATH)

IF(LOUDMOUTH_INCLUDE_DIR AND LOUDMOUTH_LIBRARY)
	SET(LOUDMOUTH_FOUND TRUE)
ENDIF(LOUDMOUTH_INCLUDE_DIR AND LOUDMOUTH_LIBRARY)

IF(LOUDMOUTH_FOUND)
	IF(NOT Loudmouth_FIND_QUIETLY)
		MESSAGE(STATUS "Found Loudmouth: ${LOUDMOUTH_LIBRARY}")
	ENDIF (NOT Loudmouth_FIND_QUIETLY)
ELSE(LOUDMOUTH_FOUND)
	IF(Loudmouth_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find Loudmouth")
	ENDIF(Loudmouth_FIND_REQUIRED)
ENDIF (LOUDMOUTH_FOUND)
