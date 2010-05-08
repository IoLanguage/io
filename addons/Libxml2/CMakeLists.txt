# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Libxml2 addon

# Find libxml2
find_package(LibXml2)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find libevent? if so, set up the targets and all the
# support variables.
if(LIBXML2_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Add LibXml2 definitions
	add_definitions(${LIBXML2_DEFINITIONS})

	# Additional include directories
	include_directories(${LIBXML2_INCLUDE_DIR})

	# Our library sources.
	file(GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.c")

	# Now build the shared library
	add_library(IoLibxml2 SHARED ${SRCS})
	add_dependencies(IoLibxml2 iovmall)
	target_link_libraries(IoLibxml2 iovmall ${LIBXML2_LIBRARIES})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/io/addons/Libxml2)
endif(LIBXML2_FOUND)
