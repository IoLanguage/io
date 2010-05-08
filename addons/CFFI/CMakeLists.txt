# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the CFFI addon

# Find libgmp
find_package(FFI)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find libevent? if so, set up the targets and all the
# support variables.
if(FFI_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${FFI_INCLUDE_DIR})

	# Our library sources.
	file(GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.c")

	# Now build the shared library
	add_library(IoCFFI SHARED ${SRCS})
	add_dependencies(IoCFFI iovmall)
	target_link_libraries(IoCFFI iovmall ${FFI_LIBRARIES})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/io/addons/CFFI)
endif(FFI_FOUND)
