# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Volcano addon

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Generate the IoVolcanoInit.c file.
# Argument SHOULD ALWAYS be the exact name of the addon, case is
# important.
generate_ioinit(Volcano)

# Output our dynamic library to the top-level _build hierarchy
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

# Our library sources.
set(SRCS
	"${CMAKE_CURRENT_SOURCE_DIR}/source/http11_parser.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/HttpParser.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoHttpParser.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoVolcanoInit.c"
)

# Now build the shared library
add_library(IoVolcano SHARED ${SRCS})
add_dependencies(IoVolcano iovmall)
target_link_libraries(IoVolcano iovmall)

# Install the addon to our global addons hierarchy.
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Volcano)
