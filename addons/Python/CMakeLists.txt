# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Python addon

# Find curses
find_package(PythonLibs)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find curses? if so, set up the targets and all the support
# variables.
if(PYTHONLIBS_FOUND)
	# On Windows, Python installed via the installer is not built with
	# the _DEBUG flag set. Thus, the pythonXY_d.lib (the debug version of
	# the pythonXY.lib) is missing.
	#
	# If the addon is needed to be built in DEBUG mode, but pythonXY_d.lib
	# is missing, changing the line:
	# 	#pragma comment(lib,"python26_d.lib")
	# to
	# 	#pragma comment(lib,"python26.lib")
	# in <python_install_dir>\include\pyconfig.h AND commenting out the
	# if-endif bellow should do the trick.
	#
	# Another solution is to build the Python package in DEBUG mode.
	if(MSVC)
		if(${PYTHON_DEBUG_LIBRARIES} MATCHES "NOTFOUND")
			MESSAGE(STATUS "Python addon will be built in RELEASE mode due to the lack of a pythonXY_d.lib")
			MESSAGE(STATUS "  see addons/Python/CMakeLists.txt for an explanation and possible workarounds")
			set(CMAKE_BUILD_TYPE "Release")
		endif(${PYTHON_DEBUG_LIBRARIES} MATCHES "NOTFOUND")
	endif(MSVC)


	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${PYTHON_INCLUDE_DIRS})

	# Generate the IoPythonInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Python)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/PythonData.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoPython.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoPythonInit.c"
	)

	# Now build the shared library
	add_library(IoPython SHARED ${SRCS})
	add_dependencies(IoPython iovmall)
	target_link_libraries(IoPython iovmall ${PYTHON_LIBRARIES})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Python)
endif(PYTHONLIBS_FOUND)
