# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# This file is the top level CMakeLists.txt and all the items defined in this
# file are inherited by CMakeLists.txt throughout the rest of the system, if
# they can be traced back through subdirectories to this file.
#
# This is an experimental build system, and it should be treated as such. It
# is being developed on a Mac OS X system, and tested as well on a FreeBSD
# system. I have no access to other platforms to test there. If this notice
# goes away, then it can be said that this system works on at least three
# major platforms:
#   1. Mac OS X
#   2. Linux
#   3. Windows
#
# If you find a bug for a particular platform, please feel free to fix it, or
# contact the iolanguage mailing list, file a bug report, or contact Jeremy
# Tregunna directly, at the e-mail address above. Please follow that order.

# Require CMake 2.8. I know for sure that this will not work with CMake 2.6
# due to the use of the FILE command we use when creating the bundle
# hierarchy.
cmake_minimum_required(VERSION 2.8)

	
# Project name, this gets prefixed to a bunch of stuff under the hood. No
# spaces, or anything silly like that please.
project(IoLanguage)

# Default config when building with gcc variants
IF(CMAKE_COMPILER_IS_GNUCC)
	SET(CMAKE_BUILD_TYPE_DebugFast)
	SET(CMAKE_CXX_FLAGS_DEBUGFAST "-g -O0")
	SET(CMAKE_C_FLAGS_DEBUGFAST "-g -O0")

	if(NOT CMAKE_BUILD_TYPE)
	        SET(CMAKE_BUILD_TYPE "DebugFast")
	endif(NOT CMAKE_BUILD_TYPE)
ENDIF(CMAKE_COMPILER_IS_GNUCC)

MESSAGE(STATUS "Configuration set to: ${CMAKE_BUILD_TYPE}")

# Don't want a coloured Makefile. On some platforms, the blue that is used is
# so dark it's illegible.
set(CMAKE_COLOR_MAKEFILE off)

# Ensure that we find all of our support CMake scripts. These are things like
# locating required libraries for addons, etc. Store them in modules/
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules/")

# We want our binaries to go here
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/_build/binaries)

set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib)

# Macro to create the _build directory hierarchy.
# Note: I'm not sure we need lib/ or objs/ in there. But I'll leave them in
# anyway, I'm just not going to do anything with them unless it breaks doing
# nothing breaks something.
macro(make_build_bundle NAME)
	file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${NAME}/binaries ${CMAKE_CURRENT_BINARY_DIR}/${NAME}/objs ${CMAKE_CURRENT_BINARY_DIR}/${NAME}/headers ${CMAKE_CURRENT_BINARY_DIR}/${NAME}/lib ${NAME}/dll)
endmacro(make_build_bundle)

# Generic macro to copy files mattching GLOBPAT in the current source
# directory into another directory.
macro(copy_files NAME GLOBPAT DSTDIR)
	# Get a list of the filenames mattching the pattern GLOBPAT
	file(GLOB ${NAME} ${GLOBPAT})

	# Create a custom copy target and display a message
	add_custom_target(copy_${NAME} ALL COMMENT "Copying files: ${CMAKE_CURRENT_SOURCE_DIR}/${GLOBPAT} to ${DSTDIR}")

	foreach(FILENAME ${${NAME}})
		# Finally, copy the files.
		add_custom_command(
			TARGET copy_${NAME}
			COMMAND ${CMAKE_COMMAND} -E copy ${FILENAME} ${DSTDIR}
		)
	endforeach(FILENAME)
endmacro(copy_files)

# Binary suffix is used to append things like .exe to binary names, for
# windows support.
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(BINARY_SUFFIX ".exe")
	set(CMAKE_STATIC_LIBRARY_PREFIX "lib")
	set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
	set(CMAKE_IMPORT_LIBRARY_PREFIX "lib")
else()
	set(BINARY_SUFFIX "")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# Definitions on where we can find headers and whatnot. Convenience definitions.
set(COROUTINE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/libs/coroutine/source)
set(BASEKIT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/libs/basekit/source)
set(GARBAGECOLLECTOR_SOURCE_DIR ${PROJECT_SOURCE_DIR}/libs/garbagecollector/source)
set(IOVM_SOURCE_DIR ${PROJECT_SOURCE_DIR}/libs/iovm/source)

# Subdirectories. These directories should have their own CMakeLists.txt.
add_subdirectory(libs)
add_subdirectory(tools)
add_subdirectory(addons)

# Ensure the _build hierarchy is created top-level, this is where our
# binaries go.
make_build_bundle(_build)

# Next we NEED to copy all the libs headers into one single dir in the bundle.
copy_files(coroutine_headers ${PROJECT_SOURCE_DIR}/libs/coroutine/source/*.h ${CMAKE_CURRENT_BINARY_DIR}/_build/headers)
copy_files(basekit_headers ${PROJECT_SOURCE_DIR}/libs/basekit/source/*.h ${CMAKE_CURRENT_BINARY_DIR}/_build/headers)
copy_files(garbagecollector_headers ${PROJECT_SOURCE_DIR}/libs/garbagecollector/source/*.h ${CMAKE_CURRENT_BINARY_DIR}/_build/headers)
copy_files(iovm_headers ${PROJECT_SOURCE_DIR}/libs/iovm/source/*.h ${CMAKE_CURRENT_BINARY_DIR}/_build/headers)
