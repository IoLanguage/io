# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Ogg addon

# Find libogg
find_package(Ogg)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find libogg? if so, set up the targets and all the support
# variables.
if(OGG_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${OGG_INCLUDE_DIR})

	# Generate the IoOggInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Ogg)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoOggPacket.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoOggPage.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoOggStreamState.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoOggSyncState.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoOggInit.c"
	)

	# Now build the shared library
	add_library(IoOgg SHARED ${SRCS})
	add_dependencies(IoOgg iovmall)
	target_link_libraries(IoOgg iovmall ${OGG_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Ogg)
endif(OGG_FOUND)
