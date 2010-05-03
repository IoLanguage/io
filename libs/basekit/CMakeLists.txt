# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Build the garbage collector library.

# Marvelous flags, likely compiler dependent.
add_definitions("-DBUILDING_BASEKIT_DLL")

# Include dirs, -I flags and whatnot
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/simd_cph/include)

# Our library sources.
set(SRCS
	source/BStream.c
	source/BStreamTag.c
	source/CHash.c
	source/Common.c
	source/Date.c
	source/Duration.c
	source/DynLib.c
	source/Hash_fnv.c
	source/Hash_murmur.c
	source/Hash_superfast.c
	source/List.c
	source/MainArgs.c
	source/PointerHash.c
	source/PortableGettimeofday.c
	source/PortableSnprintf.c
	source/PortableSorting.c
	source/PortableStrlcpy.c
	source/PortableStrptime.c
	source/PortableTruncate.c
	source/PortableUsleep.c
	source/RandomGen.c
	source/Stack.c
	source/UArray.c
	source/UArray_character.c
	source/UArray_format.c
	source/UArray_math.c
	source/UArray_path.c
	source/UArray_stream.c
	source/UArray_string.c
	source/UArray_utf.c
	source/cdecode.c
	source/cencode.c
	source/ucs2.c
	source/ucs4.c
	source/utf8.c
	source/utf_convert.c
)

# Now build the shared library
add_library(basekit SHARED ${SRCS})

# ...And the static library
add_library(basekit_static STATIC ${SRCS})

