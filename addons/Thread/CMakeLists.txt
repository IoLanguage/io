# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Thread addon

# Find pthreads
find_package(Threads)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find pthreads? if so, set up the targets and all the support
# variables.
if(CMAKE_USE_PTHREADS_INIT)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Generate the IoThreadInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Thread)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/Thread.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/ThreadMutex.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoThread.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoThreadInit.c"
	)

	# Now build the shared library
	add_library(IoThread SHARED ${SRCS})
	add_dependencies(IoThread iovmall)
	target_link_libraries(IoThread iovmall ${CMAKE_THREAD_LIBS_INIT} pthread)

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Thread)
endif(CMAKE_USE_PTHREADS_INIT)
