# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Creates the io_static and io binaries. These are different, only
# in so much as io_static depends on the *_static libraries, and
# io uses dynamic libraries. Use whichever floats your boat, we
# build both because we need the static binary to use our build
# system written in Io, before Io gets installed. Makes sense, eh?

# Set RPATH if the the install prefix is a non-system location
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
	SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")


# Some platform specific stuff. Do we have a sane popen? By sane
# we mean does it allow for a bidirectional pipe to be opened?
if(${CMAKE_SYSTEM_NAME} MATCHES "(Darwin|FreeBSD|NetBSD)")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSANE_POPEN")
endif(${CMAKE_SYSTEM_NAME} MATCHES "(Darwin|FreeBSD|NetBSD)")

# We want the binaries to go other places than here.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/_build/binaries)

# Include locations
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../libs/basekit/source
	${CMAKE_CURRENT_SOURCE_DIR}/../libs/coroutine/source
	${CMAKE_CURRENT_SOURCE_DIR}/../libs/garbagecollector/source
	${CMAKE_CURRENT_SOURCE_DIR}/../libs/iovm/source
)

# Only one file, main.c
set(SRCS source/main.c)

# Set up an empty LIBS so we can append to it things we may need to link in.
set(LIBS)

# Add dynamic loader library for those who need it
if(${CMAKE_SYSTEM_NAME} MATCHES "(Darwin|Linux|SunOS|syllable)")
	list(APPEND LIBS "-ldl")
endif(${CMAKE_SYSTEM_NAME} MATCHES "(Darwin|Linux|SunOS|syllable)")

# Add math library for those who need it
if(${CMAKE_SYSTEM_NAME} MATCHES "(OpenBSD|FreeBSD|Linux|NetBSD|DragonFly)")
	list(APPEND LIBS "-lm")
endif(${CMAKE_SYSTEM_NAME} MATCHES "(OpenBSD|FreeBSD|Linux|NetBSD|DragonFly)")

# Add curses library for readline needs it
if(${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
	list(APPEND LIBS "-lcurses")
endif(${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")


add_definitions("-DBUILDING_IOVMALL_DLL")

# Produce the executables!
add_executable(io_static ${SRCS})
add_dependencies(io_static iovmall_static)
target_link_libraries(io_static iovmall_static ${LIBS})


add_executable(io ${SRCS})
add_dependencies(io basekit coroutine garbagecollector iovmall)
target_link_libraries(io basekit coroutine garbagecollector iovmall ${LIBS})

# The following add the install target, so we put io and io_static in our
# install prefix.
if(WIN32)
    install(TARGETS io io_static DESTINATION ${CMAKE_INSTALL_PREFIX})
else()
    install(TARGETS io io_static DESTINATION bin)
endif(WIN32)

# Eerie

add_custom_command(TARGET io
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/eerie ${CMAKE_BINARY_DIR}
    )

IF(UNIX AND NOT APPLE)
    install(CODE "execute_process(COMMAND ldconfig)")
ENDIF(UNIX AND NOT APPLE)
