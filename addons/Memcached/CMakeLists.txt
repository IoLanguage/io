# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Memcached addon

# Find libmemcached
find_package(LibMemcached)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find libmemcached? if so, set up the targets and all the support
# variables.
if(LIBMEMCACHED_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${LIBMEMCACHED_INCLUDE_DIR})

	# Generate the IoMemcachedInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Memcached)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoMemcached.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoMemcachedInit.c"
	)

	# Now build the shared library
	add_library(IoMemcached SHARED ${SRCS})
	add_dependencies(IoMemcached iovmall)
	target_link_libraries(IoMemcached iovmall ${LIBMEMCACHED_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Memcached)
endif(LIBMEMCACHED_FOUND)
