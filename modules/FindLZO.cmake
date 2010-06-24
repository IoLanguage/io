# Find liblzo2
# LZO_FOUND - system has the LZO library
# LZO_INCLUDE_DIR - the LZO include directory
# LZO_LIBRARIES - The libraries needed to use LZO

if (LZO_INCLUDE_DIR AND LZO_LIBRARIES)
	# in cache already
	SET(LZO_FOUND TRUE)
else (LZO_INCLUDE_DIR AND LZO_LIBRARIES)
	FIND_PATH(LZO_INCLUDE_DIR lzo/lzo1x.h
		 ${LZO_ROOT}/include/
		 /usr/include/
		 /usr/local/include/
		 /sw/lib/
		 /sw/local/lib/
	)

	if(WIN32 AND MSVC)
	else(WIN32 AND MSVC)
		FIND_LIBRARY(LZO_LIBRARIES NAMES lzo2
			PATHS
			${LZO_ROOT}/lib
			/usr/lib
			/usr/local/lib
			/sw/lib
			/sw/local/lib
		)
	endif(WIN32 AND MSVC)

	if (LZO_INCLUDE_DIR AND LZO_LIBRARIES)
		 set(LZO_FOUND TRUE)
	endif (LZO_INCLUDE_DIR AND LZO_LIBRARIES)

	if (LZO_FOUND)
		 if (NOT LZO_FIND_QUIETLY)
				message(STATUS "Found LZO: ${LZO_LIBRARIES}")
		 endif (NOT LZO_FIND_QUIETLY)
	else (LZO_FOUND)
		 if (LZO_FIND_REQUIRED)
				message(FATAL_ERROR "Could NOT find LZO")
		 endif (LZO_FIND_REQUIRED)
	endif (LZO_FOUND)

	MARK_AS_ADVANCED(LZO_INCLUDE_DIR LZO_LIBRARIES)
endif (LZO_INCLUDE_DIR AND LZO_LIBRARIES)
