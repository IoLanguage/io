# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the SQLite3 addon

# Find SQLite version 3
set(SQLITE_MIN_VERSION 3)
find_package(Sqlite)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find libevent? if so, set up the targets and all the
# support variables.
if(SQLITE_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${SQLITE_INCLUDE_DIRS})

	# Generate the IoSQLite3Init.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(SQLite3)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSQLite3.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSQLite3Init.c"
	)

	# Now build the shared library
	add_library(IoSQLite3 SHARED ${SRCS})
	add_dependencies(IoSQLite3 iovmall)
	target_link_libraries(IoSQLite3 iovmall ${SQLITE_LIBRARIES})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/SQLite3)
endif(SQLITE_FOUND)
