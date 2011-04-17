# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Image addon

# Find image libraries
find_package(PNG)
find_package(TIFF)
find_package(JPEG)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find curses? if so, set up the targets and all the support
# variables.
if(PNG_FOUND AND TIFF_FOUND AND JPEG_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Add definitions
	add_definitions(${PNG_DEFINITIONS})
	add_definitions(-DBUILDING_IMAGE_ADDON)

	# Additional include directories
	include_directories(${PNG_INCLUDE_DIR} ${TIFF_INCLUDE_DIR} ${JPEG_INCLUDE_DIR})

	# Generate the IoImageInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Image)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/Image.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/JPGImage.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/PNGImage.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/TIFFImage.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoImage.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoImageInit.c"
	)

	# Now build the shared library
	add_library(IoImage SHARED ${SRCS})
	add_dependencies(IoImage iovmall)
	target_link_libraries(IoImage iovmall ${PNG_LIBRARIES} ${TIFF_LIBRARIES} ${JPEG_LIBRARIES})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Image)
endif(PNG_FOUND AND TIFF_FOUND AND JPEG_FOUND)
