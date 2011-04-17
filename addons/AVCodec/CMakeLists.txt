# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the AVCodec addon
# XXX: Please note, this is untested as I was only able to get
# the SWS related code in FFMPEG installed on My Mac OS X 10.6
# machine, and I can't locate the ffmpeg.h header at all. So,
# since I don't have it, the addon won't compile. However, this
# CMakeList should be sufficient given all that information to
# build the addon if someone has the required headers.

# Find libevent
find_package(FFMPEG)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find libevent? if so, set up the targets and all the
# support variables.
if(FFMPEG_FOUND AND FFMPEG_SWSCALE_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${FFMPEG_INCLUDE_DIR})

	# Generate the IoAVCodecInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(AVCodec)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoAVCodec.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoAVCodecInit.c"
	)

	# Now build the shared library
	add_library(IoAVCodec SHARED ${SRCS})
	add_dependencies(IoAVCodec iovmall)
	target_link_libraries(IoAVCodec iovmall ${FFMPEG_LIBRARIES} ${FFMPEG_SWS_LIBRARIES})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/AVCodec)
endif(FFMPEG_FOUND AND FFMPEG_SWSCALE_FOUND)
