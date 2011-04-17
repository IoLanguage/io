# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the QDBM addon

# Find qdbm
find_package(QDBM)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find qdbm? if so, set up the targets and all the support
# variables.
if(QDBM_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${QDBM_INCLUDE_DIR})

	# Generate the IoQDBMInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(QDBM)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoQDBM.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoQDBMInit.c"
	)

	# Now build the shared library
	add_library(IoQDBM SHARED ${SRCS})
	add_dependencies(IoQDBM iovmall)
	target_link_libraries(IoQDBM iovmall ${QDBM_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/QDBM)
endif(QDBM_FOUND)
