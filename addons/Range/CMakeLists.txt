# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Range addon

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Generate the IoRangeInit.c file.
# Argument SHOULD ALWAYS be the exact name of the addon, case is
# important.
generate_ioinit(Range)

# Output our dynamic library to the top-level _build hierarchy
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

# Our library sources.
set(SRCS
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoRange.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoRangeInit.c"
)

add_definitions(-DBUILDING_RANGE_ADDON)

# Now build the shared library
add_library(IoRange SHARED ${SRCS})
add_dependencies(IoRange iovmall)
target_link_libraries(IoRange iovmall)

# Install the addon to our global addons hierarchy.
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Range)
