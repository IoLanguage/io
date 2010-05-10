# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the SndFile addon

# Find sndfile
find_package(SndFile)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find sndfile? if so, set up the targets and all the support
# variables.
if(SNDFILE_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${SNDFILE_INCLUDE_DIR})

	# Generate the IoSndFileInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(SndFile)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSndFile.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSndFileInit.c"
	)

	# Now build the shared library
	add_library(IoSndFile SHARED ${SRCS})
	add_dependencies(IoSndFile iovmall)
	target_link_libraries(IoSndFile iovmall ${SNDFILE_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/io/addons/SndFile)
endif(SNDFILE_FOUND)
