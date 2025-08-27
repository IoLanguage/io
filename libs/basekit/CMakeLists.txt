# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Build the garbage collector library.

# Marvelous flags, likely compiler dependent.
add_definitions("-DBUILDING_BASEKIT_DLL")

# Include dirs, -I flags and whatnot
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/simd_cph/include)

set(BASEKIT_LIBS ${CMAKE_DL_LIBS})

# Add math library for those who need it
find_library(MATH_LIBRARY m)
if (MATH_LIBRARY)
  list(APPEND BASEKIT_LIBS ${MATH_LIBRARY})
endif()

# Our library sources.
file(GLOB SRCS "source/*.c")
file(GLOB HEADERS "source/*.h")

# Now build the shared library
add_library(basekit SHARED ${SRCS})
set_target_properties(basekit PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(basekit ${BASEKIT_LIBS})

# Now build the static library
add_library(basekit_static STATIC ${SRCS})
set_target_properties(basekit_static PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(basekit_static ${BASEKIT_LIBS})

# The following add the install target, so we put libbasekit.* in our
# install prefix.
  install (
  TARGETS basekit
  FILE_SET HEADERS
)

