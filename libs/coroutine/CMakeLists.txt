# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Build the coroutine library.

# Definitions, love them.
add_definitions("-DBUILDING_CORO_DLL")

# Set up Coroutine include dirs, -I args to compiler.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../basekit/source)

# Hackery for CMake's horrible ASM support
set(ASM_SOURCES source/asm.S)
set_source_files_properties(${ASM_SOURCES} PROPERTIES LANGUAGE C)

# Sources... in all their wonderous glory!
file(GLOB SRCS "source/*.c")
list(APPEND SRCS ${ASM_SOURCES})

# Now build the shared library
add_library(coroutine SHARED ${SRCS})

# And the static library
add_library(coroutine_static STATIC ${SRCS})