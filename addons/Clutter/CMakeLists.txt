# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Clutter addon

# Find clutter
find_package(Clutter)
find_package(Atk)
find_package(GLIB2)
find_package(Pango)
find_package(Cairo)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find clutter? if so, set up the targets and all the support
# variables.
if(CLUTTER_FOUND AND ATK_FOUND AND GLIB2_FOUND AND Pango_1_0_FOUND AND CAIRO_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${CLUTTER_INCLUDE_DIR})
	include_directories(${CLUTTER_ATK_DIR})
	include_directories(${GLIB2_INCLUDE_DIRS})
	include_directories(${Pango_1_0_INCLUDE_DIR})
	include_directories(${CAIRO_INCLUDE_DIRS})

	# Generate the IoClutterInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Clutter)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/utils.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoClutter.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoClutterActor.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoClutterActorBox.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoClutterColor.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoClutterEvent.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoClutterInputDevice.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoClutterShader.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoClutterStage.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoClutterUnits.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoClutterVertex.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoClutterInit.c"
	)

	# Now build the shared library
	add_library(IoClutter SHARED ${SRCS})
	add_dependencies(IoClutter iovmall)
	target_link_libraries(IoClutter iovmall
        ${CLUTTER_LIBRARY}
        ${ATK_LIBRARY}
        ${GLIB2_LIBRARIES}
        ${Pango_1_0_LIBRARIES}
        ${CAIRO_LIBRARIES})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Clutter)
endif(CLUTTER_FOUND AND ATK_FOUND AND GLIB2_FOUND AND Pango_1_0_FOUND AND CAIRO_FOUND)
