# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Zlib addon

# Find zlib
find_package(Zlib)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find zlib? if so, set up the targets and all the support
# variables.
if(ZLIB_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${ZLIB_INCLUDE_DIR})

	# Generate the IoZlibInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Zlib)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoZlibEncoder.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoZlibDecoder.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoZlibInit.c"
	)

	# Now build the shared library
	add_library(IoZlib SHARED ${SRCS})
	add_dependencies(IoZlib iovmall)
	target_link_libraries(IoZlib iovmall ${ZLIB_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Zlib)
endif(ZLIB_FOUND)
