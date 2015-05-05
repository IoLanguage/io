# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the AppleSensors addon

# Create the _build bundle hierarchy if needed.
make_build_bundle(_build)

# Generate the IoAppleSensorsInit.c file.
# Argument SHOULD ALWAYS be the exact name of the addon, case is
# important.
generate_ioinit(AppleSensors)

# Output our dynamic library to the top-level _build hierarchy
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

# Our library sources.
set(SRCS
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoAppleSensors.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/AppleSMC.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/AppleLMU.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/unimotion.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/IoAppleSensorsInit.c"
)

find_library(IOKIT IOKit)
if (IOKIT)
  #add_executable(IoAppleSensors ${program_SOURCES})

  # Now build the shared library
  add_library(IoAppleSensors SHARED ${SRCS})
  add_dependencies(IoAppleSensors iovmall)
  target_link_libraries(IoAppleSensors iovmall ${IOKIT})

  # Install the addon to our global addons hierarchy.
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/AppleSensors)
else()
  message(STATUS "Not building AppleSensors because IOKit was not found")
endif()
