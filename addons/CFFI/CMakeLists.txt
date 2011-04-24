# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Builds the CFFI addon

# Find libgmp
find_package(FFI)

# Did we find libevent? if so, set up the targets and all the
# support variables.
if(FFI_FOUND)
	# Create the _build bundle hierarchy if needed.
	make_build_bundle(_build)

	# Generate the IoCFFIInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(CFFI)

	# Output our dynamic library to the top-level _build hierarchy
	set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/_build/dll)

	# Additional include directories
	include_directories(${FFI_INCLUDE_DIRS})

	# Additional link directories
	link_directories(${FFI_LIBRARY_DIRS})

	# Generate the IoCFFIInit.c file.
	# Argument SHOULD ALWAYS be the exact name of the addon, case is
	# important.
	generate_ioinit(CFFI)

	# Our library sources.
	set(SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCFFIArray.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCFFIDataType.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCFFIFunction.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCFFILibrary.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCFFIPointer.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCFFIStructure.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/source/IoCFFIInit.c"
	)

	# Now build the shared library
	add_library(IoCFFI SHARED ${SRCS})
	add_dependencies(IoCFFI iovmall)
	target_link_libraries(IoCFFI iovmall ${FFI_LIBRARIES})

	# Install the addon to our global addons hierarchy.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION lib/io/addons)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_build DESTINATION lib/io/addons/CFFI)
endif(FFI_FOUND)
