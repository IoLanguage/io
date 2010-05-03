# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Build the Io VM.

# Need to go in and build io2c first. We need it to build our library.
add_subdirectory(tools)

# Our Io source files to be "compiled" into a C source file.
set(IO_SRCS
	io/A0_List.io
	io/A1_OperatorTable.io
	io/A2_Object.io
	io/A3_List.io
	io/A4_Exception.io
	io/Actor.io
	io/AddonLoader.io
	io/B_List.io
	io/B_Sequence.io
	io/Block.io
	io/CFunction.io
	io/Date.io
	io/Debugger.io
	io/Directory.io
	io/DynLib.io
	io/Error.io
	io/File.io
	io/List_schwartzian.io
	io/Map.io
	io/Message.io
	io/Number.io
	io/Profiler.io
	io/Sandbox.io
	io/Serialize.io
	io/System.io
	io/UnitTest.io
	io/Vector.io
	io/Y_Path.io
	io/Z_CLI.io
	io/Z_Importer.io
)

# The custom command to generate source/IoVMInit.c which is our
# "compiled" Io to C source code.
add_custom_command(
	OUTPUT source/IoVMInit.c
	COMMAND tools/io2c VMCode IoState_doString_ ${IO_SRCS} > source/IoVMInit.c
	DEPENDS io2c
)

# Marvelous flags, likely compiler dependent.
add_definitions("-DBUILDING_IOVM_DLL")

# Include dirs, -I flags and whatnot
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../basekit/source
	${CMAKE_CURRENT_SOURCE_DIR}/../coroutine/source
	${CMAKE_CURRENT_SOURCE_DIR}/../garbagecollector/source
)

# Our library sources.
set(SRCS
	source/IoBlock.c
	source/IoCFunction.c
	source/IoCall.c
	source/IoCollector.c
	source/IoCompiler.c
	source/IoCoroutine.c
	source/IoDate.c
	source/IoDebugger.c
	source/IoDirectory.c
	source/IoDuration.c
	source/IoDynLib.c
	source/IoError.c
	source/IoFile.c
	source/IoFile_stat.c
	source/IoLexer.c
	source/IoList.c
	source/IoMap.c
	source/IoMessage.c
	source/IoMessage_opShuffle.c
	source/IoMessage_parser.c
	source/IoNumber.c
	source/IoObject.c
	source/IoObject_flow.c
	source/IoProfiler.c
	source/IoSandbox.c
	source/IoSeq.c
	source/IoSeq_immutable.c
	source/IoSeq_mutable.c
	source/IoSeq_vector.c
	source/IoState.c
	source/IoState_callbacks.c
	source/IoState_coros.c
	source/IoState_debug.c
	source/IoState_eval.c
	source/IoState_exceptions.c
	source/IoState_symbols.c
	source/IoSystem.c
	source/IoTag.c
	source/IoToken.c
	source/IoVMInit.c
	source/IoWeakLink.c
	source/PHash.c
)

# Now build the shared library
add_library(iovmall SHARED ${SRCS})
add_dependencies(iovmall io2c basekit coroutine garbagecollector)
target_link_libraries(iovmall basekit coroutine garbagecollector)

# ...And the static library
add_library(iovmall_static STATIC ${SRCS})

