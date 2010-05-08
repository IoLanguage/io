# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Socket addon

# Find libevent
find_package(Event)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find libevent? if so, set up the targets and all the
# support variables.
if(EVENT_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${EVENT_INCLUDE_DIR})

	# Generate the IoSocketInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Socket)

	# Our library sources.
	file(GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.c")

	# Now build the shared library
	add_library(IoSocket SHARED ${SRCS})
	add_dependencies(IoSocket iovmall)
	target_link_libraries(IoSocket iovmall ${EVENT_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/io/addons/Socket)
endif(EVENT_FOUND)
