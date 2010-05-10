# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Clutter addon

# Find clutter
find_package(Clutter)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find clutter? if so, set up the targets and all the support
# variables.
if(CLUTTER_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${CLUTTER_INCLUDE_DIR})

	# Generate the IoClutterInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Clutter)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoClutter.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoClutterInit.c"
	)

	# Now build the shared library
	add_library(IoClutter SHARED ${SRCS})
	add_dependencies(IoClutter iovmall)
	target_link_libraries(IoClutter iovmall ${CLUTTER_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/io/addons/Clutter)
endif(CLUTTER_FOUND)
