# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the MySQL addon

# Find MYSQL
find_package(MySQL)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find MYSQL? if so, set up the targets and all the support
# variables.
if(MYSQL_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${MYSQL_INCLUDE_DIR})

	# Generate the IoMySQLInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(MySQL)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoMySQL.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoMySQLInit.c"
	)

	# Now build the shared library
	add_library(IoMySQL SHARED ${SRCS})
	add_dependencies(IoMySQL iovmall)
	target_link_libraries(IoMySQL iovmall ${MYSQL_LIBRARY})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/MySQL)
endif(MYSQL_FOUND)
