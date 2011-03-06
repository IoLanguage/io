# Base Io build system
# Written by Alfredo Beaumont <alfredo.beaumont@gmail.com>
#
# Find libatk.

FIND_PATH(ATK_INCLUDE_DIR atk/atk.h /usr/include/atk-1.0/ /usr/local/include/atk-1.0/ /usr/local/atk-1.0/)

SET(ATK_NAMES ${ATK_NAMES} atk libatk)

FIND_LIBRARY(ATK_LIBRARY NAMES ${ATK_NAMES} PATH)

IF(ATK_INCLUDE_DIR AND ATK_LIBRARY)
	SET(ATK_FOUND TRUE)
ENDIF(ATK_INCLUDE_DIR AND ATK_LIBRARY)

IF(ATK_FOUND)
	IF(NOT Atk_FIND_QUIETLY)
		MESSAGE(STATUS "Found Atk: ${ATK_LIBRARY}")
	ENDIF (NOT Atk_FIND_QUIETLY)
ELSE(ATK_FOUND)
	IF(Atk_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find atk")
	ENDIF(Atk_FIND_REQUIRED)
ENDIF (ATK_FOUND)
