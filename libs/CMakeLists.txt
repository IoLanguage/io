# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# This file is a stub, here only as required.

# Add a definition -- explicitly required on Windows, but shouldn't
# hurt other platforms. If it does, let me know.
add_definitions("-DBUILDING_IOVMALL_DLL")

# Output our static library to the top-level _build hierarchy
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/_build/lib)

# Our Io source files to be "compiled" into a C source file.
#file(GLOB IO_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/iovm/io/*.io")
set(IO_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/io/A0_List.io
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

# Hackery for CMake's horrible ASM support
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(ASM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/coroutine/source/asm.S)
	set_source_files_properties(${ASM_SOURCES} PROPERTIES LANGUAGE C)
endif(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# Object files from every lib. Used to create iovmall static library.
file(GLOB COROUTINE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/coroutine/source/*.c")
file(GLOB BASEKIT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/basekit/source/*.c")
file(GLOB GARBAGECOLLECTOR_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/garbagecollector/source/*.c")
file(GLOB IOVM_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/iovm/source/*.c")
list(APPEND IOVM_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/../deps/parson/parson.c)

# Marvelous flags, likely compiler dependent.
#add_definitions(-DBUILDING_IOVM_DLL)# -DINSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")

set(IOVMALL_STATIC_SRCS
	${COROUTINE_SRCS}
	${ASM_SOURCES}
	${BASEKIT_SRCS}
	${GARBAGECOLLECTOR_SRCS}
	${IOVM_SRCS}
)

# The custom command to generate source/IoVMInit.c which is our
# "compiled" Io to C source code.
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/iovm/source/IoVMInit.c
	COMMAND ${PROJECT_BINARY_DIR}/_build/binaries/io2c VMCode IoState_doString_ ${IO_SRCS} > ${CMAKE_CURRENT_SOURCE_DIR}/iovm/source/IoVMInit.c
	DEPENDS io2c
)

# Include dirs, -I flags and whatnot
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/iovm/source
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/parson
	${CMAKE_CURRENT_SOURCE_DIR}/basekit/source
	${CMAKE_CURRENT_SOURCE_DIR}/basekit/source/simd_cph/include
	${CMAKE_CURRENT_SOURCE_DIR}/coroutine/source
	${CMAKE_CURRENT_SOURCE_DIR}/garbagecollector/source
)

# Add a file to our library sources.
list(APPEND IOVMALL_STATIC_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/iovm/source/IoVMInit.c) # Because we generate it

# ...And the static library. Refer to IOVMALL_STATIC_SRCS definition
# up top.
add_library(iovmall_static STATIC ${IOVMALL_STATIC_SRCS})
add_dependencies(iovmall_static io2c basekit coroutine garbagecollector iovmall)

# Define the subdirectories we can reach from here that we want
# to go into and build stuff.
add_subdirectory(coroutine)
add_subdirectory(basekit)
add_subdirectory(garbagecollector)
add_subdirectory(iovm)
