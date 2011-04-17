# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the TagLib addon

# Find taglib
find_package(TagLib)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find taglib? if so, set up the targets and all the support
# variables.
if(TAGLIB_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${TAGLIB_INCLUDE_DIR})

	# Generate the IoTagLibInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(TagLib)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoTagLib.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoTagLibInit.c"
	)

	# Now build the shared library
	add_library(IoTagLib SHARED ${SRCS})
	add_dependencies(IoTagLib iovmall)
	target_link_libraries(IoTagLib iovmall ${TAGLIB_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/TagLib)
endif(TAGLIB_FOUND)
