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
cmake_minimum_required(VERSION 3.10)

# This is the default from cmake 3.0
# Mac OS X: Setting policy CMP0042 to the new behavior generates dylibs with
# RPATH-relative install name that is better suited for Mac OS X applications
# embedding Io in their bundle.
	
# Project name, this gets prefixed to a bunch of stuff under the hood. No
# spaces, or anything silly like that please.
project(IoLanguage C)

# Enable ASM language for coroutine support
IF (NOT CMAKE_SYSTEM_NAME MATCHES "Windows")
    enable_language(ASM)
ENDIF()

# Default config when building with gcc variants
IF((CMAKE_C_COMPILER_ID STREQUAL "GNU") OR (CMAKE_C_COMPILER_ID MATCHES "Clang"))
    SET(CMAKE_C_FLAGS_DEBUGFAST "-g -O")
    if (NOT DEFINED HAS_SSE2)
      cmake_host_system_information(RESULT HAS_SSE2 QUERY HAS_SSE2)
    endif()
    # Only use -msse2 flag for x86/x86_64 architectures.
    if(HAS_SSE2)
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse2")
    endif()
    
    if(NOT CMAKE_BUILD_TYPE)
            SET(CMAKE_BUILD_TYPE "DebugFast")
    endif(NOT CMAKE_BUILD_TYPE)
ENDIF()

MESSAGE(STATUS "Configuration set to: ${CMAKE_BUILD_TYPE}")

# Don't want a coloured Makefile. On some platforms, the blue that is used is
# so dark it's illegible.
set(CMAKE_COLOR_MAKEFILE off)

# Set library prefix to "lib" on Windows
# to match unix.
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(CMAKE_STATIC_LIBRARY_PREFIX "lib")
	set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
	set(CMAKE_IMPORT_LIBRARY_PREFIX "lib")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# Definitions on where we can find headers and whatnot. Convenience definitions.
set(COROUTINE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/libs/coroutine/source)
set(BASEKIT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/libs/basekit/source)
set(GARBAGECOLLECTOR_SOURCE_DIR ${PROJECT_SOURCE_DIR}/libs/garbagecollector/source)
set(IOVM_SOURCE_DIR ${PROJECT_SOURCE_DIR}/libs/iovm/source)
set(PARSON_SOURCE_DIR ${PROJECT_SOURCE_DIR}/deps/parson)

# Subdirectories. These directories should have their own CMakeLists.txt.
add_subdirectory(libs)
add_subdirectory(tools)

string(TIMESTAMP CMD_DATE_YEAR "%Y")
string(TIMESTAMP CMD_DATE_MON "%m")
string(TIMESTAMP CMD_DATE_DAY "%d")
string(TIMESTAMP CMD_DATE "%Y.%m.%d")

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

