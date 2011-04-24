# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the SHA1 addon

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Output our dynamic library to the top-level _build hierarchy
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

# Generate the IoSHA1Init.c file.
# Argument SHOULD ALWAYS be the exact name of the addon, case is
# important.
generate_ioinit(SHA1)

# Our library sources.
set(SRCS
	"${CMAKE_CURRENT_SOURCE_DIR}/source/sha1.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/hmac.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSHA1.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSHA1Init.c"
)

# Now build the shared library
add_library(IoSHA1 SHARED ${SRCS})
add_dependencies(IoSHA1 iovmall)
target_link_libraries(IoSHA1 iovmall)

# Install the addon to our global addons hierarchy.
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/SHA1)
