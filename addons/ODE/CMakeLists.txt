# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the ODE addon

# Find ODE
find_package(OpenGL)
find_package(GLUT)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find ODE? if so, set up the targets and all the support
# variables.
if(OPENGL_FOUND AND OPENGL_GLU_FOUND AND GLUT_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(
		${OPENGL_INCLUDE_DIR}
        ${GLUT_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../OpenGL/source
	)

	# Generate the IoODEInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(ODE)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoODEInit.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/geom.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/IoODEBall.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/IoODEBody.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/IoODEBox.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/IoODEContact.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/IoODEContactJoint.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/IoODEFixed.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/IoODEHinge2.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/IoODEHinge.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/IoODEJoint.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/IoODEJointGroup.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/IoODEMass.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/IoODEPlane.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/IoODESimpleSpace.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/IoODESlider.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/IoODEUniversal.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/IoODEWorld.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/IoSeq_ode.c"
	)

	# Now build the shared library
	add_library(IoODE SHARED ${SRCS})
	add_dependencies(IoODE iovmall)
	target_link_libraries(IoODE iovmall ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/ODE)
endif(OPENGL_FOUND AND OPENGL_GLU_FOUND AND GLUT_FOUND)
