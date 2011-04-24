# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the TokyoCabinet addon

# Find tokyo-cabinet
find_package(TokyoCabinet)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find tokyo-cabinet? if so, set up the targets and all the support
# variables.
if(TOKYOCABINET_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${TOKYOCABINET_INCLUDE_DIR})

	# Generate the IoTokyoCabinetInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(TokyoCabinet)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoTokyoCabinet.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoTokyoCabinetCursor.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoTokyoCabinetPrefixCursor.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoTokyoCabinetInit.c"
	)

	# Now build the shared library
	add_library(IoTokyoCabinet SHARED ${SRCS})
	add_dependencies(IoTokyoCabinet iovmall)
	target_link_libraries(IoTokyoCabinet iovmall ${TOKYOCABINET_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/TokyoCabinet)
endif(TOKYOCABINET_FOUND)
