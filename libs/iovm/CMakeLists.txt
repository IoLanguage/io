# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Build the Io VM.

# Need to go in and build io2c first. We need it to build our library.
add_subdirectory(tools)

# Our Io source files to be "compiled" into a C source file.
file(GLOB IO_SRCS "io/*.io")

# The custom command to generate source/IoVMInit.c which is our
# "compiled" Io to C source code.
add_custom_command(
	OUTPUT source/IoVMInit.c
	COMMAND tools/io2c VMCode IoState_doString_ ${IO_SRCS} > source/IoVMInit.c
	DEPENDS io2c
)

# Marvelous flags, likely compiler dependent.
add_definitions("-DBUILDING_IOVM_DLL")

# Include dirs, -I flags and whatnot
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../basekit/source
	${CMAKE_CURRENT_SOURCE_DIR}/../coroutine/source
	${CMAKE_CURRENT_SOURCE_DIR}/../garbagecollector/source
)

# Our library sources.
file(GLOB SRCS "source/*.c")
list(APPEND SRCS source/IoVMInit.c)

# Now build the shared library
add_library(iovmall SHARED ${SRCS})
add_dependencies(iovmall io2c basekit coroutine garbagecollector)
target_link_libraries(iovmall basekit coroutine garbagecollector)

# ...And the static library
add_library(iovmall_static STATIC ${SRCS})
