# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
#
# CMake file for libs subdirectory.
#

# Add a definition -- explicitly required on Windows, but shouldn't
# hurt other platforms. If it does, let me know.
add_definitions("-DBUILDING_IOVMALL_DLL")

# Hackery for CMake's horrible ASM support
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Always include asm.S for all architectures
    set(ASM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/coroutine/source/asm.S)
    set_source_files_properties(${ASM_SOURCES} PROPERTIES LANGUAGE C)
endif()

# Define the subdirectories we can reach from here that we want
# to go into and build stuff.
add_subdirectory(coroutine)
add_subdirectory(basekit)
add_subdirectory(garbagecollector)
add_subdirectory(iovm)
