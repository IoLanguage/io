# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Vorbis addon

# Find vorbis
find_package(Vorbis)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find vorbis? if so, set up the targets and all the support
# variables.
if(VORBIS_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(
		${VORBIS_INCLUDE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/../Ogg/source
	)

	# Generate the IoVorbisInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Vorbis)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoVorbisBlock.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoVorbisComment.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoVorbisDspState.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoVorbisInfo.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoVorbisInit.c"
	)

	# Now build the shared library
	add_library(IoVorbis SHARED ${SRCS})
	add_dependencies(IoVorbis iovmall)
	target_link_libraries(IoVorbis iovmall ${VORBIS_LIBRARY} IoOgg)

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Vorbis)
endif(VORBIS_FOUND)
