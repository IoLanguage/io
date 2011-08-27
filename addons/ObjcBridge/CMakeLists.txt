
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
# Builds the ObjcBridge addon

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)


# Output our dynamic library to the top-level _build hierarchy
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)


# Generate the IoObjcBridgeInit.c file.
# Argument SHOULD ALWAYS be the exact name of the addon, case is important.
generate_ioinit(ObjcBridge)


# Additional include directories
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../Box/source
	${CMAKE_CURRENT_SOURCE_DIR}/../Socket/source
	${CMAKE_CURRENT_SOURCE_DIR}/../SystemCall/source
)

# Our library sources.
set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/CHash_ObjcBridgeFunctions.m"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/Io2Objc.m"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoObjcBridge.m"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoObjcBridgeInit.m"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/Objc2Io.m"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/ObjcSubclass.m"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/Runtime.m"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoLanguage.m"
)

set(CMAKE_EXE_LINKER_FLAGS "-framework AppKit")

# Now build the shared library
add_library(IoObjcBridge SHARED ${SRCS})
add_dependencies(IoObjcBridge iovmall Foundation IoBox IoSocket IoSystemCall)
target_link_libraries(IoObjcBridge iovmall ${ObjcBridge_LIBRARY} IoBox IoSocket IoSystemCall /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation /System/Library/Frameworks/AppKit.framework/Versions/C/AppKit)

# Install the addon to our global addons hierarchy.
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/ObjcBridge)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

