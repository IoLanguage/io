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

# Our install prefix. Defaults to /usr/local
set(INSTALL_PREFIX "/usr/local")

# Macro to create the _build directory hierarchy.
# Note: I'm not sure we need lib/ or objs/ in there. But I'll leave them in
# anyway, I'm just not going to do anything with them unless it breaks doing
# nothing breaks something.
macro(make_build_bundle NAME)
	file(MAKE_DIRECTORY ${NAME}/objs ${NAME}/headers ${NAME}/lib ${NAME}/dll)
endmacro(make_build_bundle)

# Generic macro to copy files mattching GLOBPAT in the current source
# directory into another directory.
macro(copy_files NAME GLOBPAT DSTDIR)
	# Get a list of the filenames mattching the pattern GLOBPAT
	file(GLOB ${NAME} ${GLOBPAT})

	# Create a custom copy target and display a message
	add_custom_target(copy_${NAME} ALL COMMENT "Copying files: ${CMAKE_CURRENT_SOURCE_DIR}/${GLOBPAT} to ${DSTDIR}")

	foreach(FILENAME ${${NAME}})
		set(SRC "${FILENAME}")
		set(DST "${DESTINATION}/${FILENAME}")

		add_custom_command(
			TARGET copy_${NAME}
			COMMAND ${CMAKE_COMMAND} -E copy ${SRC} ${DSTDIR}
		)
	endforeach(FILENAME)

	# Finally, copy the files.
	#file(COPY ${${NAME}} DESTINATION ${DSTDIR})
endmacro(copy_files)

# Subdirectories. These directories should have their own CMakeLists.txt.
add_subdirectory(libs)
add_subdirectory(tools)
