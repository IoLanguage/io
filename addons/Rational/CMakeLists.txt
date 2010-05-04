# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Rational addon

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Output our dynamic library to the top-level _build hierarchy
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

# Include dirs, -I flags and whatnot
include_directories(
	${COROUTINE_SOURCE_DIR}
	${BASEKIT_SOURCE_DIR}
	${GARBAGECOLLECTOR_SOURCE_DIR}
	${IOVM_SOURCE_DIR}
)

# Our library sources.
file(GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.c")

# Now build the shared library
add_library(IoRational SHARED ${SRCS})
add_dependencies(IoRational iovmall)
target_link_libraries(IoRational iovmall)

# Install rule for "install" target.
#install(TARGETS garbagecollector DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
