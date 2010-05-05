# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Bitly addon

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Output our dynamic library to the top-level _build hierarchy
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

# Our library sources.
file(GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.c")

# Now build the shared library
add_library(IoBitly SHARED ${SRCS})
add_dependencies(IoBitly iovmall)
target_link_libraries(IoBitly iovmall)

# Install the addon to our global addons hierarchy.
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/io/addons)
