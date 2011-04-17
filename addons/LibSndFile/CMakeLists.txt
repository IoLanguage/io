# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the LibSndFile addon

# Find sndfile
find_package(LibSndFile)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find sndfile? if so, set up the targets and all the support
# variables.
if(LIBSNDFILE_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${LIBSNDFILE_INCLUDE_DIR})

	# Generate the IoLibSndFileInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(LibSndFile)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoLibSndFile.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoLibSndFileInit.c"
	)

	# Now build the shared library
	add_library(IoLibSndFile SHARED ${SRCS})
	add_dependencies(IoLibSndFile iovmall)
	target_link_libraries(IoLibSndFile iovmall ${LIBSNDFILE_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/LibSndFile)
endif(LIBSNDFILE_FOUND)
