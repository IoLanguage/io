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

# Our library!
add_library(sgml SHARED ${SRCS})

# Don't install it, we do that ourselves when the addon gets
# installed.
