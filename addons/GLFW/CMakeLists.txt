# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the GLFW addon

# Find GLFW
find_package(GLFW)
find_package(OpenGL)
find_package(GLUT)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find GLFW? if so, set up the targets and all the support
# variables.
if(GLFW_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND AND GLUT_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(
		${GLFW_INCLUDE_DIR}
		${OPENGL_INCLUDE_DIR}
        ${GLUT_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../OpenGL/source
	)

	add_definitions(-DGLFW_DLL)

	# Generate the IoGLFWInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(GLFW)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoGLFW.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoGLFWInit.c"
	)

	# Now build the shared library
	add_library(IoGLFW SHARED ${SRCS})
	add_dependencies(IoGLFW iovmall)
	target_link_libraries(IoGLFW iovmall ${GLFW_LIBRARY} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/GLFW)
endif(GLFW_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND AND GLUT_FOUND)
