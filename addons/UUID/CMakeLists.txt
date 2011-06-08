# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the UUID addon

# Find uuid
find_package(UUID)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find uuid? if so, set up the targets and all the support
# variables.
if(UUID_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${UUID_INCLUDE_DIR})

	# Generate the IoUUIDInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(UUID)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoUUID.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoUUIDInit.c"
	)

	# Now build the shared library
	add_library(IoUUID SHARED ${SRCS})
	add_dependencies(IoUUID iovmall)
	target_link_libraries(IoUUID iovmall ${UUID_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/UUID)
endif(UUID_FOUND)
