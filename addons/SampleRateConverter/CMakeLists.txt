# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the SampleRateConverter addon

# Find curses
find_package(SampleRate)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find curses? if so, set up the targets and all the support
# variables.
if(SAMPLERATE_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${SAMPLERATE_INCLUDE_DIR})

	# Generate the IoSampleRateConverterInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(SampleRateConverter)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSampleRateConverter.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSampleRateConverterInit.c"
	)

	# Now build the shared library
	add_library(IoSampleRateConverter SHARED ${SRCS})
	add_dependencies(IoSampleRateConverter iovmall)
	target_link_libraries(IoSampleRateConverter iovmall ${SAMPLERATE_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/SampleRateConverter)
endif(SAMPLERATE_FOUND)
