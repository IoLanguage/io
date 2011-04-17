# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the ReadLine addon

# Find readline
find_package(ReadLine)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find readline? if so, set up the targets and all the support
# variables.
if(READLINE_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${READLINE_INCLUDE_DIR})

	# Generate the IoReadLineInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(ReadLine)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoReadLine.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoReadLineInit.c"
	)

	# Now build the shared library
	add_library(IoReadLine SHARED ${SRCS})
	add_dependencies(IoReadLine iovmall)
	target_link_libraries(IoReadLine iovmall ${READLINE_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/ReadLine)
endif(READLINE_FOUND)
