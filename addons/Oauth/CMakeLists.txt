
# Find image libraries
find_package(OPENSSL)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Output our dynamic library to the top-level _build hierarchy
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	add_definitions(${OPENSSL_DEFINITIONS})
	include_directories(${OPENSSL_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/source/")

# Generate the IoOauthInit.c file.
# Argument SHOULD ALWAYS be the exact name of the addon, case is
# important.
generate_ioinit(Oauth)

# Our library sources.
set(SRCS
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoOauth.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoOauthInit.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/Oauth/aux.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/Oauth/crypto.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/Oauth/http.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/Oauth/Oauth.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/Oauth/tls.c"
)

# -lssl -lcrypto 

# Now build the shared library
add_library(IoOauth SHARED ${SRCS})
add_dependencies(IoOauth iovmall)
target_link_libraries(IoOauth iovmall ${OPENSSL_LIBRARIES})

# Install the addon to our global addons hierarchy.
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/io/addons)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/io/addons/Oauth)
