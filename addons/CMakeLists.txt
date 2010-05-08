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
	)

	# Clean up our mess.
	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/source/Io${NAME}Init.c")
endmacro(generate_ioinit)

# Addons listed below, and not commented out, are addons which not
# only exist on the system, but can be built. Addons in a state of
# flux should be commented out here until such time as they are in
# a stable state.
#add_subdirectory(AVCodec) # XXX: Broken
#add_subdirectory(AppleExtras)
add_subdirectory(AsyncRequest)
#add_subdirectory(Beanstalk)
add_subdirectory(BigNum)
add_subdirectory(Bitly)
add_subdirectory(Blowfish)
#add_subdirectory(Box)
add_subdirectory(CFFI)
add_subdirectory(CGI)
#add_subdirectory(Cairo)
#add_subdirectory(Clutter)
add_subdirectory(ContinuedFraction)
add_subdirectory(Curses)
add_subdirectory(DBI)
add_subdirectory(DistributedObjects)
add_subdirectory(EditLine)
#add_subdirectory(Flux)
add_subdirectory(Fnmatch)
#add_subdirectory(Font)
#add_subdirectory(GLFW)
#add_subdirectory(GoogleSearch)
add_subdirectory(HttpClient)
#add_subdirectory(Image)
add_subdirectory(LZO)
#add_subdirectory(LibSndFile)
add_subdirectory(Libxml2)
add_subdirectory(Loki)
#add_subdirectory(Loudmouth)
add_subdirectory(MD5)
add_subdirectory(Memcached)
#add_subdirectory(MySQL)
#add_subdirectory(NetworkAdapter)
add_subdirectory(NotificationCenter)
#add_subdirectory(NullAddon)
#add_subdirectory(ODE)
#add_subdirectory(ObjcBridge)
add_subdirectory(Obsidian)
#add_subdirectory(Ogg)
#add_subdirectory(OpenGL)
#add_subdirectory(PortAudio)
#add_subdirectory(PostgreSQL)
#add_subdirectory(Postgres)
#add_subdirectory(Python)
#add_subdirectory(QDBM)
add_subdirectory(Random)
add_subdirectory(Range)
add_subdirectory(Rational)
#add_subdirectory(ReadLine)
add_subdirectory(Regex)
#add_subdirectory(SGML)
add_subdirectory(SHA1)
#add_subdirectory(SQLite)
add_subdirectory(SQLite3)
#add_subdirectory(SampleRateConverter)
#add_subdirectory(SecureSocket)
#add_subdirectory(SkipDB)
add_subdirectory(Socket)
#add_subdirectory(SoundTouch)
add_subdirectory(SqlDatabase)
add_subdirectory(Syslog)
add_subdirectory(SystemCall)
#add_subdirectory(TagDB)
#add_subdirectory(TagLib)
#add_subdirectory(Theora)
#add_subdirectory(Thread)
#add_subdirectory(TokyoCabinet)
add_subdirectory(Twitter)
#add_subdirectory(UUID)
add_subdirectory(User)
#add_subdirectory(VertexDB)
#add_subdirectory(Volcano)
#add_subdirectory(Vorbis)
add_subdirectory(Yajl)
#add_subdirectory(Zlib)
