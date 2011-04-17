# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the SystemCall addon

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Output our dynamic library to the top-level _build hierarchy
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

# Generate the IoSystemCallInit.c file.
# Argument SHOULD ALWAYS be the exact name of the addon, case is
# important.
generate_ioinit(SystemCall)

# Our library sources.
set(SRCS
	"${CMAKE_CURRENT_SOURCE_DIR}/source/callsystem.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSystemCall.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSystemCallInit.c"
)

# Now build the shared library
add_library(IoSystemCall SHARED ${SRCS})
add_dependencies(IoSystemCall iovmall)
target_link_libraries(IoSystemCall iovmall)

# Install the addon to our global addons hierarchy.
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/SystemCall)
