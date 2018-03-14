# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# This is the top level for our addons.

# Common include directories. Add these regardless of what the
# addon itself wants to add.
include_directories(
	${COROUTINE_SOURCE_DIR}
	${BASEKIT_SOURCE_DIR}
	${GARBAGECOLLECTOR_SOURCE_DIR}
	${IOVM_SOURCE_DIR}
)

# Need to flatten the namespace so we can find symbols on Darwin
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -flat_namespace")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# Packages are installed under /usr/local on OpenBSD
if(${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
	set(CMAKE_C_FLAGS  "-I/usr/local/include ${CMAKE_C_FLAGS}")
	set(CMAKE_SHARED_LINKER_FLAGS "-L/usr/local/lib ${CMAKE_SHARED_LINKER_FLAGS}")
endif(${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")

# Get the filename component, and generate the IoFooInit.c file.
macro(generate_ioinit NAME)
	# Ensure the source dir exists.
	file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/source")

	# Call io_static on generate.io and supply the name of the
	# addon. The result will be source/IoFooInit.c if the name
	# was Foo.
	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/source/Io${NAME}Init.c
		COMMAND ${PROJECT_BINARY_DIR}/_build/binaries/io_static ${CMAKE_SOURCE_DIR}/addons/generate.io ${CMAKE_SOURCE_DIR}/addons ${NAME}
		DEPENDS io_static
	)

	# Clean up our mess.
	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/source/Io${NAME}Init.c")
endmacro(generate_ioinit)

add_subdirectory(User)
add_subdirectory(Yajl)
