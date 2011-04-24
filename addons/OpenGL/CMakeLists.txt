# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the OpenGL addon

# Find OpenGL
find_package(OpenGL)
find_package(GLUT)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find OpenGL? if so, set up the targets and all the support
# variables.
if(OPENGL_FOUND AND OPENGL_GLU_FOUND AND GLUT_FOUND AND ((GLUT_Xmu_LIBRARY AND GLUT_Xi_LIBRARY) OR NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux"))
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(
		${OPENGL_INCLUDE_DIR}
        ${GLUT_INCLUDE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/../Box/source
		${CMAKE_CURRENT_SOURCE_DIR}/../Image/source
	)

	# Generate the IoOpenGLInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(OpenGL)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoBox_gl.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoGLScissor.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoGLU.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoGLUQuadric.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoGLUT.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoVector_gl.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoOpenGL.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoOpenGLInit.c"
	)

	# Now build the shared library
	add_library(IoOpenGL SHARED ${SRCS})
	add_dependencies(IoOpenGL iovmall IoBox)
	target_link_libraries(IoOpenGL iovmall IoBox ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/OpenGL)
endif(OPENGL_FOUND AND OPENGL_GLU_FOUND AND GLUT_FOUND AND ((GLUT_Xmu_LIBRARY AND GLUT_Xi_LIBRARY) OR NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux"))
