# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the DBI addon

# Find curses
find_package(DBI)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find curses? if so, set up the targets and all the support
# variables.
if(DBI_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${DBI_INCLUDE_DIR})

	# Generate the IoDBIInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(DBI)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoDBI.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoDBIConn.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoDBIResult.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoDBIInit.c"
	)

	# Now build the shared library
	add_library(IoDBI SHARED ${SRCS})
	add_dependencies(IoDBI iovmall)
	target_link_libraries(IoDBI iovmall ${DBI_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/DBI)
endif(DBI_FOUND)
