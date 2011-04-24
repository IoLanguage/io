# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Yajl addon

# Find yajl
find_package(Yajl)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find yajl? if so, set up the targets and all the support
# variables.
if(YAJL_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${YAJL_INCLUDE_DIR})

	# Generate the IoYajlInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Yajl)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoYajl.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoYajlGen.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoYajlInit.c"
	)

	# Now build the shared library
	add_library(IoYajl SHARED ${SRCS})
	add_dependencies(IoYajl iovmall)
	target_link_libraries(IoYajl iovmall ${YAJL_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Yajl)
endif(YAJL_FOUND)
