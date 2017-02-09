# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Font addon

# Find Font
find_package(OpenGL)
find_package(GLUT)
find_package(Freetype)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find Font? if so, set up the targets and all the support
# variables.
if(OPENGL_FOUND AND GLUT_FOUND AND FREETYPE_FOUND AND ((GLUT_Xmu_LIBRARY AND GLUT_Xi_LIBRARY) OR NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux"))
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(
		${GLUT_INCLUDE_DIR}
		${FREETYPE_INCLUDE_DIRS}
	)

	# Generate the IoFontInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Font)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/FreeTypeErrorCodes.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/GLFont.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoFont.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoFontInit.c"
	)

	# Now build the shared library
	add_library(IoFont SHARED ${SRCS})
	add_dependencies(IoFont iovmall)
	target_link_libraries(IoFont iovmall ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${FREETYPE_LIBRARIES})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Font)
endif(OPENGL_FOUND AND GLUT_FOUND AND FREETYPE_FOUND AND ((GLUT_Xmu_LIBRARY AND GLUT_Xi_LIBRARY) OR NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux"))
