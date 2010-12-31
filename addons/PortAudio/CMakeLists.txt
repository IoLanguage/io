# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the PortAudio addon


find_package(PkgConfig)
pkg_check_modules(PORTAUDIO portaudio-2.0)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

if(PORTAUDIO_FOUND)

	# Generate the IoBoxInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(PortAudio)

	include_directories(${PORTAUDIO_INCLUDE_DIR})

	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/AudioDevice.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoAudioDevice.c"

		)

	add_definitions(-DBUILDING_NULLADDON_ADDON)

	# Now build the shared library
	add_library(IoPortAudio SHARED ${SRCS})
	add_dependencies(IoPortAudio iovmall)
	target_link_libraries(IoPortAudio iovmall ${PORTAUDIO_LIBRARIES})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_INSTALL_LIBDIR}/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION ${CMAKE_INSTALL_LIBDIR}/io/addons/PortAudio)

endif(PORTAUDIO_FOUND)
