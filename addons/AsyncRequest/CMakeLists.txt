# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the AsyncRequest addon

# Find aio.h
find_package(AsyncRequest)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

if(ASYNCREQUEST_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Generate the IoAsyncRequestInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(AsyncRequest)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoAsyncRequest.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoAsyncRequestInit.c"
	)

	if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	        set(LINK_RT_LIB "rt")
	endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


	# Now build the shared library
	add_library(IoAsyncRequest SHARED ${SRCS})
	add_dependencies(IoAsyncRequest iovmall)
	target_link_libraries(IoAsyncRequest iovmall ${LINK_RT_LIB})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/AsyncRequest)
endif(ASYNCREQUEST_FOUND)
