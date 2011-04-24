# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the Socket addon

# Find libevent
find_package(Event)

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Did we find libevent? if so, set up the targets and all the
# support variables.
if(EVENT_FOUND)
	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${EVENT_INCLUDE_DIR})

	# Generate the IoSocketInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(Socket)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IPAddress.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/LocalNameServers.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/Socket.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoDNS.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoEvConnection.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoEvHttpServer.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoEvOutRequest.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoEvOutResponse.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoEvent.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoEventManager.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoIPAddress.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSocket.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoSocketInit.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/Address.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoUnixPath.c"
	)

	IF(NOT MSVC AND NOT MINGW)
		SET(SRCS ${SRCS}
			"${CMAKE_CURRENT_SOURCE_DIR}/source/UnixPath.c"
		)
	ENDIF(NOT MSVC AND NOT MINGW)

	set(LIBS "")
	
	if(MSVC)
		set(LIBS "Ws2_32.lib Iphlpapi.lib")
	elseif(MINGW)
		add_definitions(-DIO_WINSOCK_COMPAT)
		set(LIBS "-lws2_32 -liphlpapi")
	endif()
	
	# Now build the shared library
	add_library(IoSocket SHARED ${SRCS})
	add_dependencies(IoSocket iovmall)
	target_link_libraries(IoSocket iovmall ${EVENT_LIBRARY} ${LIBS})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/Socket)
endif(EVENT_FOUND)
