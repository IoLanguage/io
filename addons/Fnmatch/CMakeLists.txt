# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Fnmatch addon

# Find fnmatch.h
find_package(Fnmatch)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

if(FNMATCH_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Generate the IoFnmatchInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Fnmatch)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoFnmatch.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoFnmatchInit.c"
	)

	# Now build the shared library
	add_library(IoFnmatch SHARED ${SRCS})
	add_dependencies(IoFnmatch iovmall)
	target_link_libraries(IoFnmatch iovmall)

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Fnmatch)
endif(FNMATCH_FOUND)
