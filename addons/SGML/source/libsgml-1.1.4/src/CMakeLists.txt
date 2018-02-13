# SGML library.
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the libsgml library.

# Not sure if we should be outputing the dynamic lib to the addon,
# but whatever.
set(LIBRARY_OUTPUT_PATH ${BUILD_DIR}/dll)

# Set up the include dir
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include/sgml)

# Our sources
set(SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/DomComment.c
	${CMAKE_CURRENT_SOURCE_DIR}/DomDocument.c
	${CMAKE_CURRENT_SOURCE_DIR}/DomElement.c
	${CMAKE_CURRENT_SOURCE_DIR}/DomNode.c
	${CMAKE_CURRENT_SOURCE_DIR}/DomNodeList.c
	${CMAKE_CURRENT_SOURCE_DIR}/DomText.c
	${CMAKE_CURRENT_SOURCE_DIR}/SgmlExtensionHtml.c
	${CMAKE_CURRENT_SOURCE_DIR}/SgmlExtensionXml.c
	${CMAKE_CURRENT_SOURCE_DIR}/SgmlParser.c
	${CMAKE_CURRENT_SOURCE_DIR}/Variant.c
)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

# Our library!
add_library(sgml STATIC ${SRCS})

# Libsgml issues some warnings under clang. Since it's a third-party library, just
# silence the warnings instead of modifying the source code.
target_compile_options(sgml PUBLIC -Wno-pointer-sign)

if(NOT WIN32)
	target_compile_options(sgml PUBLIC -fPIC)
endif(NOT WIN32)

# Don't install it, we do that ourselves when the addon gets
# installed.
