# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Build the garbage collector library.

# Marvelous flags, likely compiler dependent.
add_definitions("-DBUILDING_COLLECTOR_DLL")

# Include dirs, -I flags and whatnot
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../basekit/source)

# Our library sources.
file(GLOB SRCS "source/*.c")
file(GLOB HEADERS "source/*.h")

# Now build the shared library
add_library(garbagecollector SHARED ${SRCS})
add_dependencies(garbagecollector basekit)
set_target_properties(garbagecollector PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(garbagecollector basekit)

# Now build the static library
add_library(garbagecollector_static STATIC ${SRCS})
add_dependencies(garbagecollector_static basekit_static)
set_target_properties(garbagecollector_static PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(garbagecollector_static basekit_static)

# The following add the install target, so we put libgarbagecollector.*
# in our install prefix.
install(
  TARGETS garbagecollector
  FILE_SET HEADERS
)
