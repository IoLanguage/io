# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the NullAddon addon

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Generate the IoBoxInit.c file.
# Argument SHOULD ALWAYS be the exact name of the addon, case is
# important.
generate_ioinit(NullAddon)

# Output our dynamic library to the top-level _build hierarchy
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

# Our library sources.
set(SRCS
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoNullAddon.c"
)

add_definitions(-DBUILDING_NULLADDON_ADDON)

# Now build the shared library
add_library(IoNullAddon SHARED ${SRCS})
add_dependencies(IoNullAddon iovmall)
target_link_libraries(IoNullAddon iovmall)

# Install the addon to our global addons hierarchy.
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_INSTALL_LIBDIR}/io/addons)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION ${CMAKE_INSTALL_LIBDIR}/io/addons/NullAddon)
