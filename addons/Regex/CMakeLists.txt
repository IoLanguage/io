# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Regex addon

# Find pcre
find_package(PCRE)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find libevent? if so, set up the targets and all the
# support variables.
if(PCRE_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${PCRE_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../Range/source)

	# Generate the IoRegexInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Regex)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/Regex.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoRegexMatch.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoRegexMatches.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoRegex.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoRegexInit.c"
	)

	# Now build the shared library
	add_library(IoRegex SHARED ${SRCS})
	add_dependencies(IoRegex iovmall IoRange)
	target_link_libraries(IoRegex iovmall ${PCRE_LIBRARIES} IoRange)

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Regex)
endif(PCRE_FOUND)
