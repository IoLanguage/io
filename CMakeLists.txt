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

# Mac OS X: Setting policy CMP0042 to the new behavior generates dylibs with
# RPATH-relative install name that is better suited for Mac OS X applications
# embedding Io in their bundle.
if(POLICY CMP0042)
	cmake_policy(SET CMP0042 NEW)
endif()
	
# Project name, this gets prefixed to a bunch of stuff under the hood. No
# spaces, or anything silly like that please.
project(IoLanguage C)

# Default config when building with gcc variants
IF(CMAKE_COMPILER_IS_GNUCC OR (CMAKE_C_COMPILER_ID MATCHES "Clang"))
	SET(CMAKE_BUILD_TYPE_DebugFast)
	SET(CMAKE_CXX_FLAGS_DEBUGFAST "-g -O0")
	SET(CMAKE_C_FLAGS_DEBUGFAST "-g -O0")
	SET(CMAKE_C_FLAGS "-msse2")
	if(NOT CMAKE_BUILD_TYPE)
	        SET(CMAKE_BUILD_TYPE "DebugFast")
	endif(NOT CMAKE_BUILD_TYPE)
ENDIF(CMAKE_COMPILER_IS_GNUCC OR (CMAKE_C_COMPILER_ID MATCHES "Clang"))

MESSAGE(STATUS "Configuration set to: ${CMAKE_BUILD_TYPE}")

# Don't want a coloured Makefile. On some platforms, the blue that is used is
# so dark it's illegible.
set(CMAKE_COLOR_MAKEFILE off)

# We want our binaries to go here
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/_build/binaries)

# Macro to create the _build directory hierarchy.
# Note: I'm not sure we need lib/ or objs/ in there. But I'll leave them in
# anyway, I'm just not going to do anything with them unless it breaks doing
# nothing breaks something.
macro(make_build_bundle NAME)
	file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${NAME}/binaries ${CMAKE_CURRENT_BINARY_DIR}/${NAME}/objs ${CMAKE_CURRENT_BINARY_DIR}/${NAME}/headers ${CMAKE_CURRENT_BINARY_DIR}/${NAME}/lib ${CMAKE_CURRENT_BINARY_DIR}/${NAME}/dll)
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

# Ensure the _build hierarchy is created top-level, this is where our
# binaries go.
make_build_bundle(_build)

# Next we NEED to copy all the libs headers into one single dir in the bundle.
copy_files(coroutine_headers ${PROJECT_SOURCE_DIR}/libs/coroutine/source/*.h ${CMAKE_CURRENT_BINARY_DIR}/_build/headers)
copy_files(basekit_headers ${PROJECT_SOURCE_DIR}/libs/basekit/source/*.h ${CMAKE_CURRENT_BINARY_DIR}/_build/headers)
copy_files(garbagecollector_headers ${PROJECT_SOURCE_DIR}/libs/garbagecollector/source/*.h ${CMAKE_CURRENT_BINARY_DIR}/_build/headers)
copy_files(iovm_headers ${PROJECT_SOURCE_DIR}/libs/iovm/source/*.h ${CMAKE_CURRENT_BINARY_DIR}/_build/headers)

# Packaging stuff

#Modified from: http://www.mail-archive.com/cmake@cmake.org/msg32916.html
MACRO (TODAY RESULT)
    IF (WIN32)
        EXECUTE_PROCESS(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE ${RESULT})
        string(REGEX REPLACE "(..)/(..)/(....).*" "\\1.\\2.\\3" ${RESULT} 
${${RESULT}})
    ELSEIF(UNIX)
        EXECUTE_PROCESS(COMMAND "date" "+%d/%m/%Y" OUTPUT_VARIABLE ${RESULT})
        string(REGEX REPLACE "(..)/(..)/(....).*" "\\1.\\2.\\3" ${RESULT} 
${${RESULT}})
    ELSE (WIN32)
        MESSAGE(SEND_ERROR "date not implemented")
        SET(${RESULT} 00.00.0000)
    ENDIF (WIN32)
ENDMACRO (TODAY)

TODAY(CMD_DATE)
STRING(SUBSTRING ${CMD_DATE} 0 2 CMD_DATE_DAY)
STRING(SUBSTRING ${CMD_DATE} 3 2 CMD_DATE_MON)
STRING(SUBSTRING ${CMD_DATE} 6 4 CMD_DATE_YEAR)
SET(CMD_DATE "${CMD_DATE_YEAR}.${CMD_DATE_MON}.${CMD_DATE_DAY}")

# If source tree is not a git repository this will not work
#IF(WIN32 AND NOT CYGWIN)
#	execute_process(COMMAND "cmd" " /C git rev-parse --short HEAD" OUTPUT_VARIABLE IO_GIT_REV)
#ELSE(WIN32 AND NOT CYGWIN)
#	execute_process(COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE IO_GIT_REV)
#ENDIF(WIN32 AND NOT CYGWIN)
#string(REGEX REPLACE "(.......)." "\\1" IO_GIT_REV ${IO_GIT_REV})

SET(CPACK_PACKAGE_NAME ${PROJECT_NAME})
SET(CPACK_PACKAGE_VENDOR "iolanguage.com")
SET(CPACK_PACKAGE_CONTACT "iolanguage@yahoogroups.com")
SET(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE.txt)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Io Language")
SET(CPACK_PACKAGE_VERSION "${CMD_DATE}")
SET(CPACK_PACKAGE_VERSION_MAJOR ${CMD_DATE_YEAR})
SET(CPACK_PACKAGE_VERSION_MINOR ${CMD_DATE_MON})
SET(CPACK_PACKAGE_VERSION_PATCH ${CMD_DATE_DAY})
SET(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})

IF(WIN32)
	# It's not very clear whether redist files for MSVC build are in the path bellow...
	# The semi-hardcoded path bellow is for Visual Studio 2008 Express.
	# Anyway, we have to distinguish between Debug and Release redist files as well.
	IF(MSVC)
		STRING(REGEX REPLACE "\\\\" "\\\\\\\\" VC_INSTALL_PATH "$ENV{VCINSTALLDIR}")
		SET(VC_RUNTIME_REDIST_FILES "File '${VC_INSTALL_PATH}\\\\redist\\\\Debug_NonRedist\\\\x86\\\\Microsoft.VC90.DebugCRT\\\\*.*'")
	ENDIF(MSVC)
	
	SET(CPACK_SET_DESTDIR "OFF")
	SET(CPACK_NSIS_URL_INFO_ABOUT "http://www.iolanguage.com")
	SET(CPACK_NSIS_HELP_LINK "http://www.iolanguage.com")
	SET(CPACK_NSIS_MODIFY_PATH "ON")
	SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
		WriteIniStr '\$INSTDIR\\\\IoLanguageWebSite.url' 'InternetShortcut' 'URL' '${CPACK_NSIS_URL_INFO_ABOUT}'
		SetOutPath '\$INSTDIR\\\\lib'
		SetOverwrite try
		${VC_RUNTIME_REDIST_FILES}
	")
	SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
		Delete '\$INSTDIR\\\\IoLanguageWebSite.url'
		Delete '\$INSTDIR\\\\lib\\\\*.*'
	")

	SET(CPACK_NSIS_CREATE_ICONS "
		CreateShortCut '\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Io CLI.lnk' '$INSTDIR\\\\bin\\\\io.exe'
		CreateShortCut '\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\IoLanguage Web Site.lnk' '\$INSTDIR\\\\IoLanguageWebSite.url'
	")
	SET(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '\$DESKTOP\\\\Io CLI.lnk' '$INSTDIR\\\\bin\\\\io.exe'")
	SET(CPACK_NSIS_DELETE_ICONS "
		Delete '\$SMPROGRAMS\\\\$MUI_TEMP\\\\Io CLI.lnk'
		Delete '\$SMPROGRAMS\\\\$MUI_TEMP\\\\IoLanguage Web Site.lnk'
	")
	SET(CPACK_NSIS_DELETE_ICONS_EXTRA "Delete '\$DESKTOP\\\\Io CLI.lnk'")
ENDIF(WIN32)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	SET(CPACK_SET_DESTDIR "ON")

	FIND_PROGRAM(DPKG_CMD dpkg)
	IF(DPKG_CMD)
		SET(CPACK_GENERATOR ${CPACK_GENERATOR} DEB)
	ELSE(DPKG_CMD)
		MESSAGE( STATUS "dpkg binary not found, not building debian package" )
	ENDIF(DPKG_CMD)

	FIND_PROGRAM(RPMBUILD_CMD rpmbuild)
	IF(RPMBUILD_CMD)
		SET( CPACK_GENERATOR ${CPACK_GENERATOR} RPM )
	ELSE(RPMBUILD_CMD)
		MESSAGE( STATUS "rpmbuild binary not found, not building rpm package" )
	ENDIF(RPMBUILD_CMD)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

include(CPack)

