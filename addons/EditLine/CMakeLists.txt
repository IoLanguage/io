# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the EditLine addon

# Find curses
find_package(Edit)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find curses? if so, set up the targets and all the support
# variables.
if(EDIT_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${EDIT_INCLUDE_DIR})

	# Generate the IoEditLineInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(EditLine)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoEditLine.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoEditLineInit.c"
	)

	# Now build the shared library
	add_library(IoEditLine SHARED ${SRCS})
	add_dependencies(IoEditLine iovmall)
	target_link_libraries(IoEditLine iovmall ${EDIT_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/EditLine)
endif(EDIT_FOUND)
