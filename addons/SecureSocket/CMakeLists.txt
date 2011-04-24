# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the SecureSocket addon

# Find SecureSocket
find_package(OpenSSL)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find SecureSocket? if so, set up the targets and all the support
# variables.
if(SSL_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${OPENSSL_INCLUDE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/../Socket/source)

	# Generate the IoSecureSocketInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(SecureSocket)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSecureSocket.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSecureServer.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSecureClient.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCertificate.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/common.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSecureSocketInit.c"
	)

	# Now build the shared library
	add_library(IoSecureSocket SHARED ${SRCS})
	add_dependencies(IoSecureSocket IoSocket iovmall)
	target_link_libraries(IoSecureSocket iovmall ${SSL_LIBRARY} IoSocket)

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/SecureSocket)
endif(SSL_FOUND)
