# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the LZO addon

# Find curses
find_package(LZO)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find curses? if so, set up the targets and all the support
# variables.
if(LZO_FOUND)
	add_definitions("-DUSE_LIBLZO")

	# Additional include directories
	include_directories(${LZO_INCLUDE_DIR})
else()
	add_definitions("-DUSE_MINILZO")
endif(LZO_FOUND)

# Output our dynamic library to the top-level _build hierarchy
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

# Generate the IoLZOInit.c file.
# Argument SHOULD ALWAYS be the exact name of the addon, case is
# important.
generate_ioinit(LZO)

# Our library sources.
set(SRCS
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoLZODecoder.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoLZOEncoder.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/minilzo.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoLZOInit.c"
)

# Now build the shared library
add_library(IoLZO SHARED ${SRCS})
add_dependencies(IoLZO iovmall)
if(LZO_FOUND)
	target_link_libraries(IoLZO iovmall ${LZO_LIBRARIES})
else()
	target_link_libraries(IoLZO iovmall)
endif(LZO_FOUND)

# Install the addon to our global addons hierarchy.
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/LZO)
