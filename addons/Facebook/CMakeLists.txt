# Base Io build system
#
# Builds the Facebook addon

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Generate the IoFacebookInit.c file.
# Argument SHOULD ALWAYS be the exact name of the addon, case is
# important.
generate_ioinit(Facebook)

# Output our dynamic library to the top-level _build hierarchy
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

# Our library sources.
set(SRCS "${CMAKE_CURRENT_SOURCE_DIR}/source/IoFacebookInit.c")

# Now build the shared library
add_library(IoFacebook SHARED ${SRCS})
add_dependencies(IoFacebook iovmall)
target_link_libraries(IoFacebook iovmall)

# Install the addon to our global addons hierarchy.
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Facebook)
