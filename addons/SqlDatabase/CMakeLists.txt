# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the SqlDatabase addon

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Generate the IoSqlDatabaseInit.c file.
# Argument SHOULD ALWAYS be the exact name of the addon, case is
# important.
generate_ioinit(SqlDatabase)

# Output our dynamic library to the top-level _build hierarchy
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

# Our library sources.
set(SRCS "${CMAKE_CURRENT_SOURCE_DIR}/source/IoSqlDatabaseInit.c")

# Now build the shared library
add_library(IoSqlDatabase SHARED ${SRCS})
add_dependencies(IoSqlDatabase iovmall)
target_link_libraries(IoSqlDatabase iovmall)

# Install the addon to our global addons hierarchy.
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/io/addons)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/io/addons/SqlDatabase)

# Clean up our mess.
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/source/IoSqlDatabaseInit.c")
