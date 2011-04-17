# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the MD5 addon

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Output our dynamic library to the top-level _build hierarchy
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

# Generate the IoMD5Init.c file.
# Argument SHOULD ALWAYS be the exact name of the addon, case is
# important.
generate_ioinit(MD5)

# Our library sources.
set(SRCS
	"${CMAKE_CURRENT_SOURCE_DIR}/source/md5.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoMD5.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoMD5Init.c"
)

# Now build the shared library
add_library(IoMD5 SHARED ${SRCS})
add_dependencies(IoMD5 iovmall)
target_link_libraries(IoMD5 iovmall)

# Install the addon to our global addons hierarchy.
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/MD5)
