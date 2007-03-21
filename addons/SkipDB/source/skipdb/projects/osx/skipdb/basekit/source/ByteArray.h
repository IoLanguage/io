/*#io
docCopyright("Steve Dekorte, 2002. All rights reserved.")
docLicense("BSD revised") 
docDescription("A mutable array of unsigned chars supports basic operations, searching and reading/writing to a file")
*/
 

#ifndef BYTEARRAY_DEFINED
#define BYTEARRAY_DEFINED 1

#include "Common.h"
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include "List.h"

// internally, Io always uses a forward slash "/" for path separators,
// but on Windows, back slashes are also tolerated as path separators.
#if defined(DOS) || defined(ON_WINDOWS)
#define OS_PATH_SEPARATOR "\\"
#define IO_PATH_SEPARATORS "\\/"
#else
#define OS_PATH_SEPARATOR "/"
#define IO_PATH_SEPARATORS "/"
#endif

#define IO_PATH_SEPARATOR   "/"
#define IO_PATH_SEPARATOR_DOT   IO_PATH_SEPARATOR "." IO_PATH_SEPARATOR

#ifdef __cplusplus
extern "C" {
#endif

#define BYTEARRAY_BYTES_PER_CHARACTER 1

typedef struct
{
    unsigned char *bytes;
    size_t size;
    size_t memSize;
    uint8_t encoding;
} ByteArray ;

BASEKIT_API size_t sane_fread(void *buffer, size_t size, size_t nmemb, FILE *stream);

BASEKIT_API ByteArray *ByteArray_new(void);
BASEKIT_API ByteArray *ByteArray_newWithData_size_(const unsigned char *buf, size_t size);
BASEKIT_API ByteArray *ByteArray_newWithCString_size_(const char *s, int size);
BASEKIT_API ByteArray *ByteArray_newWithCString_(const char *s);
BASEKIT_API ByteArray *ByteArray_newWithSize_(int size);
BASEKIT_API ByteArray *ByteArray_newWithBytesFrom_to_(ByteArray *self, int startpos, int endpos);
BASEKIT_API ByteArray *ByteArray_clone(ByteArray *self);

#include "Datum.h"
BASEKIT_API Datum ByteArray_asDatum(ByteArray *self);
BASEKIT_API Datum *ByteArray_asNewDatum(ByteArray *self);

BASEKIT_API void ByteArray_free(ByteArray *self);
BASEKIT_API size_t ByteArray_memorySize(ByteArray *self);
BASEKIT_API void ByteArray_compact(ByteArray *self);

BASEKIT_API void ByteArray_clear(ByteArray *self);
BASEKIT_API void ByteArray_setAllBytesTo_(ByteArray *self, unsigned char c);
BASEKIT_API void ByteArray_sizeTo_(ByteArray *self, size_t size);
BASEKIT_API void ByteArray_setSize_(ByteArray *self, size_t size);
BASEKIT_API void ByteArray_copy_(ByteArray *self, ByteArray *other);
BASEKIT_API void ByteArray_setData_size_(ByteArray *self, const unsigned char *data, size_t size);
BASEKIT_API void ByteArray_setCString_(ByteArray *self, const char *s);

/*
BASEKIT_API int ByteArray_compare_(ByteArray *self, ByteArray *other);
BASEKIT_API int ByteArray_compareData_size_(ByteArray *self, unsigned char *b2, unsigned int l2);
BASEKIT_API int ByteArray_compareDatum_(ByteArray *self, Datum d);
*/

// character ops 

BASEKIT_API int ByteArray_hasDigit(ByteArray *self);
BASEKIT_API unsigned long ByteArray_at_bytesCount_(ByteArray *self, int i, int l);
BASEKIT_API int ByteArray_at_put_(ByteArray *self, int pos, unsigned char c);
BASEKIT_API unsigned char ByteArray_dropLastByte(ByteArray *self);
BASEKIT_API void ByteArray_removeByteAt_(ByteArray *self, int pos);
BASEKIT_API void ByteArray_removeCharAt_(ByteArray *self, int pos);
BASEKIT_API void ByteArray_removeSlice(ByteArray *self, int from, int to);

// escape & quote 

BASEKIT_API void ByteArray_escape(ByteArray *self);
BASEKIT_API void ByteArray_unescape(ByteArray *self);

BASEKIT_API void ByteArray_quote(ByteArray *self);
BASEKIT_API void ByteArray_unquote(ByteArray *self);

// modification ops 

BASEKIT_API void ByteArray_appendChar_(ByteArray *self, char c);
BASEKIT_API void ByteArray_appendByte_(ByteArray *self, unsigned char c);
BASEKIT_API void ByteArray_append_(ByteArray *self, ByteArray *other);
BASEKIT_API void ByteArray_appendCString_(ByteArray *self, const char *s);
BASEKIT_API void ByteArray_appendAndEscapeCString_(ByteArray *self, const char *s);
BASEKIT_API void ByteArray_appendBytes_size_(ByteArray *self, const unsigned char *bytes, size_t length);

BASEKIT_API void ByteArray_prepend_(ByteArray *self, ByteArray *other);
BASEKIT_API void ByteArray_prependCString_(ByteArray *self, const char *s);
BASEKIT_API void ByteArray_prependBytes_size_(ByteArray *self, const unsigned char *bytes, size_t length);

BASEKIT_API void ByteArray_insert_at_(ByteArray *self, ByteArray *other, size_t pos);
BASEKIT_API void ByteArray_insertCString_at_(ByteArray *self, const char *s, size_t pos);
BASEKIT_API void ByteArray_insertBytes_size_at_(ByteArray *self, const unsigned char *bytes, size_t size, size_t pos);

// clipping 

BASEKIT_API char ByteArray_clipBefore_(ByteArray *self, ByteArray *other);
BASEKIT_API char ByteArray_clipBeforeEndOf_(ByteArray *self, ByteArray *other);
BASEKIT_API char ByteArray_clipAfter_(ByteArray *self, ByteArray *other);
BASEKIT_API char ByteArray_clipAfterStartOf_(ByteArray *self, ByteArray *other);

// strip 

BASEKIT_API int ByteArray_containsByte_(ByteArray *self, unsigned char b);
BASEKIT_API void ByteArray_strip_(ByteArray *self, ByteArray *other);
BASEKIT_API void ByteArray_lstrip_(ByteArray *self, ByteArray *other);
BASEKIT_API void ByteArray_rstrip_(ByteArray *self, ByteArray *other);

// enumeration 

// case

BASEKIT_API int ByteArray_isLowercase(ByteArray *self);
BASEKIT_API int ByteArray_isUppercase(ByteArray *self);
BASEKIT_API void ByteArray_Lowercase(ByteArray *self);
BASEKIT_API void ByteArray_uppercase(ByteArray *self);

// comparision 

BASEKIT_API int ByteArray_equals_(ByteArray *self, ByteArray *other);
BASEKIT_API int ByteArray_equalsAnyCase_(ByteArray *self, ByteArray *other);

// search 

BASEKIT_API int ByteArray_contains_(ByteArray *self, ByteArray *other);
BASEKIT_API int ByteArray_containsAnyCase_(ByteArray *self, ByteArray *other);
BASEKIT_API int ByteArray_find_(ByteArray *self, ByteArray *other);
BASEKIT_API int ByteArray_beginsWith_(ByteArray *self, ByteArray *other);
BASEKIT_API int ByteArray_endsWith_(ByteArray *self, ByteArray *other);

BASEKIT_API int ByteArray_findAnyCase_(ByteArray *self, ByteArray *other);
BASEKIT_API int ByteArray_find_from_(ByteArray *self, ByteArray *other, int from);
BASEKIT_API int ByteArray_findCString_from_(ByteArray *self, const char *other, int from);
BASEKIT_API int ByteArray_rFind_from_(ByteArray *self, ByteArray *other, int from);
BASEKIT_API int ByteArray_rFindCString_from_(ByteArray *self, const char *other, int from);
BASEKIT_API int ByteArray_rFindCharacters_from_(ByteArray *self, const char *chars, int from);
BASEKIT_API int ByteArray_findAnyCase_from_(ByteArray *self, ByteArray *other, int from);
BASEKIT_API size_t ByteArray_count_(ByteArray *self, ByteArray *other);

BASEKIT_API int ByteArray_findByteWithValue_from_(ByteArray *self, unsigned char b, int from);
BASEKIT_API int ByteArray_findByteWithoutValue_from_(ByteArray *self, unsigned char b, int from);

BASEKIT_API void ByteArray_setByteWithValue_from_to_(ByteArray *self, unsigned char b, size_t from, size_t to);

// replace

BASEKIT_API void ByteArray_replaceCString_withCString_(ByteArray *self, const char *s1, const char *s2);
BASEKIT_API void ByteArray_replaceFrom_size_with_(ByteArray *self, 
    size_t index, 
    size_t substringSize, 
    ByteArray *other);
BASEKIT_API size_t ByteArray_replace_with_(ByteArray *self, ByteArray *substring, ByteArray *other);
BASEKIT_API size_t ByteArray_replaceFirst_from_with_(ByteArray *self, ByteArray *substring, size_t start, ByteArray *other);
/* for effiency, replacement is done without allocating a seperate byte array */
BASEKIT_API void ByteArray_replace_with_output_(ByteArray *self, ByteArray *substring, ByteArray *other, ByteArray *output);

// input/output

BASEKIT_API void ByteArray_print(ByteArray *self);
BASEKIT_API size_t ByteArray_writeToCStream_(ByteArray *self, FILE *stream);
BASEKIT_API int ByteArray_writeToFilePath_(ByteArray *self, const char *path); /* returns 0 on success */

BASEKIT_API int ByteArray_readFromFilePath_(ByteArray *self, const char *path); /* returns 1 on success */
BASEKIT_API unsigned char ByteArray_readLineFromCStream_(ByteArray *self, FILE *stream);
BASEKIT_API int ByteArray_readFromCStream_(ByteArray *self, FILE *stream);
BASEKIT_API size_t ByteArray_readNumberOfBytes_fromCStream_(ByteArray *self, size_t length, FILE *stream);

// private utility functions 

BASEKIT_API int ByteArray_wrapPos_(ByteArray *self, int pos);

BASEKIT_API ByteArray *ByteArray_newWithFormat_(const char *format, ...);
BASEKIT_API ByteArray *ByteArray_newWithVargs_(const char *format, va_list ap);
BASEKIT_API ByteArray *ByteArray_fromFormat_(ByteArray *self, const char *format, ...);
BASEKIT_API void ByteArray_fromVargs_(ByteArray *self, const char *format, va_list ap);
 
// hex conversion

BASEKIT_API ByteArray *ByteArray_asNewHexStringByteArray(ByteArray *self);

// file paths

BASEKIT_API void ByteArray_appendPathCString_(ByteArray *self, const char *path);
BASEKIT_API void ByteArray_removeLastPathComponent(ByteArray *self);
BASEKIT_API void ByteArray_clipBeforeLastPathComponent(ByteArray *self);
BASEKIT_API ByteArray *ByteArray_lastPathComponent(ByteArray *self);
BASEKIT_API char *ByteArray_lastPathComponentAsCString(ByteArray *self);
BASEKIT_API void ByteArray_removePathExtension(ByteArray *self);
BASEKIT_API ByteArray *ByteArray_pathExtension(ByteArray *self);
BASEKIT_API ByteArray *ByteArray_fileName(ByteArray *self);

// bitwise ops

BASEKIT_API void ByteArray_and_(ByteArray *self, ByteArray *other);
BASEKIT_API void ByteArray_or_(ByteArray *self, ByteArray *other);
BASEKIT_API void ByteArray_xor_(ByteArray *self, ByteArray *other);
BASEKIT_API void ByteArray_compliment(ByteArray *self);
BASEKIT_API void ByteArray_byteShiftLeft_(ByteArray *self, int s);
BASEKIT_API void ByteArray_byteShiftRight_(ByteArray *self, int s);
BASEKIT_API void ByteArray_bitShiftLeft_(ByteArray *self, int s);

BASEKIT_API unsigned char ByteArray_byteAt_(ByteArray *self, size_t i);
BASEKIT_API int ByteArray_bitAt_(ByteArray *self, size_t i);
BASEKIT_API void ByteArray_setBit_at_(ByteArray *self, int b, size_t i);
BASEKIT_API void ByteArray_printBits(ByteArray *self);
BASEKIT_API unsigned int ByteArray_bitCount(ByteArray *self);

// typed array ops

BASEKIT_API float ByteArray_aveAbsFloat32From_to_(ByteArray *self, size_t from, size_t to);
BASEKIT_API int ByteArray_aveAbsSignedInt32From_to_(ByteArray *self, size_t from, size_t to);
BASEKIT_API void ByteArray_convertFloat32ArrayToInt32(ByteArray *self);
BASEKIT_API void ByteArray_convertFloat32ArrayToInt16(ByteArray *self);
BASEKIT_API void ByteArray_convertInt16ArrayToFloat32(ByteArray *self);
BASEKIT_API void ByteArray_float32ArrayAdd_(ByteArray *self, ByteArray *other);
BASEKIT_API void ByteArray_float32ArrayMultiplyScalar_(ByteArray *self, float s);
BASEKIT_API void ByteArray_zero(ByteArray *self);

// split

BASEKIT_API int ByteArray_splitCount_(ByteArray *self, List *others);
/*int ByteArray_nextChunkUsingStopList_from_(ByteArray *self, List *others, size_t startIndex);*/
BASEKIT_API List *ByteArray_split_(ByteArray *self, List *others);

/*
unsigned int ByteArray_HashData_size_(const unsigned char *bytes, unsigned int length);
uint32_t ByteArray_orderedHash32(ByteArray *self);
*/

// indexwise ops

BASEKIT_API void ByteArray_removeOddIndexesOfSize_(ByteArray *self, size_t typeSize);
BASEKIT_API void ByteArray_removeEvenIndexesOfSize_(ByteArray *self, size_t typeSize);
BASEKIT_API void ByteArray_duplicateIndexesOfSize_(ByteArray *self, size_t typeSize);

BASEKIT_API int ByteArray_endsWithCString_(ByteArray *self, const char *suffix);

BASEKIT_API size_t ByteArray_matchingPrefixSizeWith_(ByteArray *self, ByteArray *other);

#include "ByteArray_inline.h"

#ifdef __cplusplus
}
#endif
#endif

