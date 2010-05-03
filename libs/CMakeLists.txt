# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# This file is a stub, here only as required.

# Our Io source files to be "compiled" into a C source file.
file(GLOB IO_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/iovm/io/*.io")

# Object files from every lib. Used to create iovmall static library.
file(GLOB COROUTINE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/coroutine/source/*.[cS]")
file(GLOB BASEKIT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/basekit/source/*.c")
file(GLOB GARBAGECOLLECTOR_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/garbagecollector/source/*.c")
file(GLOB IOVM_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/iovm/source/*.c")
set(IOVMALL_STATIC_SRCS
	${COROUTINE_SRCS}
	${BASEKIT_SRCS}
	${GARBAGECOLLECTOR_SRCS}
	${IOVM_SRCS}
)

# The custom command to generate source/IoVMInit.c which is our
# "compiled" Io to C source code.
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/source/IoVMInit.c
	COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/iovm/tools/io2c VMCode IoState_doString_ ${IO_SRCS} > ${CMAKE_CURRENT_SOURCE_DIR}/source/IoVMInit.c
	DEPENDS io2c
)

# Include dirs, -I flags and whatnot
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/iovm/source
	${CMAKE_CURRENT_SOURCE_DIR}/basekit/source
	${CMAKE_CURRENT_SOURCE_DIR}/basekit/source/simd_cph/include
	${CMAKE_CURRENT_SOURCE_DIR}/coroutine/source
	${CMAKE_CURRENT_SOURCE_DIR}/garbagecollector/source
)

# Add a file to our library sources.
list(APPEND SRCS iovm/source/IoVMInit.c)

# ...And the static library. Refer to IOVMALL_STATIC_SRCS definition
# up top.
add_library(iovmall_static STATIC ${IOVMALL_STATIC_SRCS})
add_dependencies(iovmall_static io2c basekit coroutine garabagecollector iovmall)

# Define the subdirectories we can reach from here that we want
# to go into and build stuff.
add_subdirectory(coroutine)
add_subdirectory(basekit)
add_subdirectory(garbagecollector)
add_subdirectory(iovm)
