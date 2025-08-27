# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# CMake file for libs/iovm subdirectory.
#
# Build the Io VM.

# Need to go in and build io2c first. We need it to build our library.
add_subdirectory(tools)

# Our Io source files to be "compiled" into a C source file.
set(IO_SRCS
    A0_List.io
    A0_Object.io
    A1_OperatorTable.io
    A2_Object.io
    A3_List.io
    A4_Exception.io
    Actor.io
    AddonLoader.io
    B_Sequence.io
    Block.io
    CFunction.io
    Date.io
    Debugger.io
    Directory.io
    DynLib.io
    Error.io
    File.io
    List_schwartzian.io
    Map.io
    Message.io
    Number.io
    Profiler.io
    Sandbox.io
    Serialize.io
    System.io
    UnitTest.io
    Vector.io
    Y_Path.io
    Z_CLI.io
    Z_Importer.io
    )
# Prepend the source location to every file
list(TRANSFORM IO_SRCS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/io/)

# Create a header file which defines our install prefix. This is
# only needed because GCC is batshit insane.
configure_file(IoInstallPrefix.h.in IoInstallPrefix.h)

# The custom command to generate source/IoVMInit.c which is our
# "compiled" Io to C source code.
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/source)
add_custom_command(
    OUTPUT IoVMInit.c
    COMMAND io2c VMCode IoState_doString_ ${IO_SRCS} > IoVMInit.c
    DEPENDS io2c ${IO_SRCS}
    )

# Handle annoying NAN
include(CheckCSourceRuns)
check_c_source_runs("
#include <stdio.h>
#include <string.h>
#include <math.h>
#define _GNU_SOURCE

int main(){
	char buf[5];
	sprintf(buf, \"%f\", NAN);
	return strcmp(buf, \"nan\");
}
" HAVE_POSITIVE_NAN)
if (NOT HAVE_POSITIVE_NAN)
	add_definitions(-DUSE_BUILTIN_NAN)
endif()

# Marvelous flags, likely compiler dependent.
add_definitions("-DBUILDING_IOVM_DLL")# -DINSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")

# Include dirs, -I flags and whatnot
include_directories(
    ${COROUTINE_SOURCE_DIR}
    ${BASEKIT_SOURCE_DIR}
    ${BASEKIT_SOURCE_DIR}/simd_cph/include
    ${GARBAGECOLLECTOR_SOURCE_DIR}
    ${IOVM_SOURCE_DIR}
    ${PARSON_SOURCE_DIR})

# Our library sources.
file(GLOB IOVM_SRCS "${IOVM_SOURCE_DIR}/*.c")
list(APPEND IOVM_SRCS 
    ${CMAKE_CURRENT_BINARY_DIR}/IoVMInit.c
    ${PARSON_SOURCE_DIR}/parson.c)
set(SRCS ${IOVM_SRCS})
file(GLOB HEADERS "source/*.h")

# Now build the shared library
add_library(iovmall SHARED ${SRCS})
set_target_properties(iovmall PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(iovmall basekit coroutine garbagecollector)
target_include_directories(iovmall PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Now build the static library
add_library(iovmall_static STATIC ${IOVM_SRCS})
target_link_libraries(iovmall_static coroutine_static)
target_link_libraries(iovmall_static basekit_static)
target_link_libraries(iovmall_static garbagecollector_static)
target_include_directories(iovmall_static PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# The following add the install target, so we put libvmall.* in our
# install prefix.
install (
  TARGETS iovmall iovmall_static
  FILE_SET HEADERS
)
