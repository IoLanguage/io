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
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/io/addons/Python)
endif(PYTHONLIBS_FOUND)
