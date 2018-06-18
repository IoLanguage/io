# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Build the Io VM.

# Need to go in and build io2c first. We need it to build our library.
add_subdirectory(tools)

# Output our dynamic library to the top-level _build hierarchy
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/_build/dll)

# Our Io source files to be "compiled" into a C source file.
#file(GLOB IO_SRCS "io/*.io")
set(IO_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/io/A0_List.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/A0_Object.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/A1_OperatorTable.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/A2_Object.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/A3_List.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/A4_Exception.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/Actor.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/AddonLoader.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/B_Sequence.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/Block.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/CFunction.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/Date.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/Debugger.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/Directory.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/DynLib.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/Error.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/File.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/List_schwartzian.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/Map.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/Message.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/Number.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/Profiler.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/Sandbox.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/Serialize.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/System.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/UnitTest.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/Vector.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/Y_Path.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/Z_CLI.io
    ${CMAKE_CURRENT_SOURCE_DIR}/io/Z_Importer.io
    )

# Object files from every lib. Used to create iovmall static library.
# file(GLOB COROUTINE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../coroutine/source/*.[cS]")
# file(GLOB BASEKIT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../basekit/source/*.c")
# file(GLOB GARBAGECOLLECTOR_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../garbagecollector/source/*.c")
# file(GLOB IOVM_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../source/*.c")
# set(IOVMALL_STATIC_SRCS
# ${COROUTINE_SRCS}
# ${BASEKIT_SRCS}
# ${GARBAGECOLLECTOR_SRCS}
# ${IOVM_SRCS}
# )

# Create a header file which defines our install prefix. This is
# only needed because GCC is batshit insane.
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/source/IoInstallPrefix.h "#define INSTALL_PREFIX \"${CMAKE_INSTALL_PREFIX}\"")

# The custom command to generate source/IoVMInit.c which is our
# "compiled" Io to C source code.
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/source/IoVMInit.c
    COMMAND ${PROJECT_BINARY_DIR}/_build/binaries/io2c VMCode IoState_doString_ ${IO_SRCS} > ${CMAKE_CURRENT_SOURCE_DIR}/source/IoVMInit.c
    DEPENDS io2c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/parson
    ${CMAKE_CURRENT_SOURCE_DIR}/../basekit/source
    ${CMAKE_CURRENT_SOURCE_DIR}/../coroutine/source
    ${CMAKE_CURRENT_SOURCE_DIR}/../garbagecollector/source
    )

# Our library sources.
file(GLOB SRCS "source/*.c")
list(APPEND SRCS source/IoVMInit.c)
list(APPEND SRCS ../../deps/parson/parson.c)
file(GLOB HEADERS "source/*.h")

# Now build the shared library
add_library(iovmall SHARED ${SRCS})
add_dependencies(iovmall io2c basekit coroutine garbagecollector)
set_target_properties(iovmall PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(iovmall basekit coroutine garbagecollector)

# The following add the install target, so we put libvmall.* in our
# install prefix.
if(WIN32)
    install(TARGETS iovmall 
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
        PUBLIC_HEADER DESTINATION include/io
        )
else()
    install(TARGETS iovmall 
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/io
        )
endif(WIN32)
