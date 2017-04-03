# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Loudmouth addon

# Find loudmouth
find_package(Loudmouth)
find_package(GLIB2)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find loudmouth? if so, set up the targets and all the support
# variables.
if(LOUDMOUTH_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${LOUDMOUTH_INCLUDE_DIR}
                        ${GLIB2_INCLUDE_DIRS})

	# Generate the IoLoudmouthInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Loudmouth)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoLoudmouth.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoLoudmouthInit.c"
	)

	# Now build the shared library
	add_library(IoLoudmouth SHARED ${SRCS})
	add_dependencies(IoLoudmouth iovmall)
	target_link_libraries(IoLoudmouth iovmall ${LOUDMOUTH_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Loudmouth)
endif(LOUDMOUTH_FOUND)
