# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Theora addon

# Find theora
find_package(Theora)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find theora? if so, set up the targets and all the support
# variables.
if(THEORA_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(
		${THEORA_INCLUDE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/../Ogg/source
	)

	# Generate the IoTheoraInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Theora)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoTheoraComment.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoTheoraDecodeContext.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoTheoraInfo.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoTheoraSetupInfo.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoTheoraInit.c"
	)

	# Now build the shared library
	add_library(IoTheora SHARED ${SRCS})
	add_dependencies(IoTheora iovmall)
	target_link_libraries(IoTheora iovmall ${THEORA_LIBRARY} IoOgg)

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Theora)
endif(THEORA_FOUND)
