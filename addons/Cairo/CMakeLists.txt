# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Cairo addon

# Find cairo
find_package(Cairo)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find cairo? if so, set up the targets and all the support
# variables.
if(CAIRO_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${CAIRO_INCLUDE_DIRS})

	# Additional link directories
	link_directories(${CAIRO_LIBRARY_DIRS})

	# Generate the IoCairoInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Cairo)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairo.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoContext.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoExtents.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoFontExtents.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoFontFace.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoFontOptions.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoGlyph.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoGradient.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoImageSurface.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoLinearGradient.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoMatrix.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoPath.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoPathElement.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoPattern.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoRadialGradient.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoRectangle.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoScaledFont.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoSolidPattern.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoSurface.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoSurfacePattern.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoTextExtents.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairo_fileSurfaces.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/_IoCairoPDFSurface.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/_IoCairoPSSurface.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/_IoCairoSVGSurface.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCairoInit.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/tools.c"
	)

	# Now build the shared library
	add_library(IoCairo SHARED ${SRCS})
	add_dependencies(IoCairo iovmall)
	target_link_libraries(IoCairo iovmall ${CAIRO_LIBRARIES})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Cairo)
endif(CAIRO_FOUND)
