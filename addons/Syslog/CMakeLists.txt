# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Syslog addon

# Find syslog.h
find_package(Syslog)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

if(SYSLOG_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Generate the IoSyslogInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Syslog)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSyslog.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSyslogInit.c"
	)

	# Now build the shared library
	add_library(IoSyslog SHARED ${SRCS})
	add_dependencies(IoSyslog iovmall)
	target_link_libraries(IoSyslog iovmall)

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Syslog)
endif(SYSLOG_FOUND)
