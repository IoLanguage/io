# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the SGML addon

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)
set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/_build)

# Ensure we build libsgml before we continue
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/libsgml-1.1.4/src)

# Generate the IoSGMLInit.c file.
# Argument SHOULD ALWAYS be the exact name of the addon, case is
# important.
generate_ioinit(SGML)

# Output our dynamic library to the top-level _build hierarchy
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

# Set up the include dir
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/libsgml-1.1.4/include)

# Our library sources.
set(SRCS
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSGMLParser.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSGMLInit.c"
)

# Now build the shared library
add_library(IoSGML SHARED ${SRCS})
add_dependencies(IoSGML iovmall sgml)
target_link_libraries(IoSGML iovmall sgml)

# Install the addon to our global addons hierarchy.
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/SGML)
