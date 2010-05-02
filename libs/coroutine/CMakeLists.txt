# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Build the coroutine library.

# Set up the _build directory hierarchy.
make_build_bundle(_build)
# Copy the headers over
copy_files(coroutine_headers "source/*.h" _build/headers/)

# Clean up after ourselves -- remove the _build dir.
get_directory_property(clean ADDITIONAL_MAKE_CLEAN_FILES)
file(GLOB loose_ends _build/*/*)
list(APPEND clean ${loose_ends})
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${clean}")

add_definitions("-DBUILDING_CORO_DLL -fPIC -falign-loops=16")

# Set up Coroutine include dirs, -I args to compiler.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/_build/headers)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../basekit/source)

# Hackery for CMake's horrible ASM support
set(ASM_SOURCES source/asm.S)
set_source_files_properties(${ASM_SOURCES} PROPERTIES LANGUAGE C)

# Copy the file to the _build dir
#add_custom_target(coroutine_move ALL COMMAND ${CMAKE_COMMAND} -E copy libcoroutine.dylib _build/dll/libcoroutine.dylib DEPENDS coroutine)

set(SRCS
	source/Coro.c
	source/context.c
	${ASM_SOURCES}
)
# Now build the shared library
add_library(coroutine SHARED ${SRCS})

# And the static library
add_library(coroutine_static STATIC ${SRCS})
