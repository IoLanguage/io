/*   
 *   copyright: Steve Dekorte, 2002. All rights reserved.
 *   license: See _BSDLicense.txt.
 *
 *   description: A mutable array of unsigned chars
 *   supports basic operations, searching and 
 *   reading/writing to a file
 */
 

#ifndef ARRAY_DEFINED
#define ARRAY_DEFINED 1

#include "Common.h"
#include "BStream.h"
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#if !defined(float32_t)
    typedef float  float32_t;
    typedef double float64_t;
#endif

enum CTYPE 
{ 
    CTYPE_generic,
    CTYPE_pointer,
    
    CTYPE_uint8_t,
    CTYPE_uint16_t,
    CTYPE_uint32_t,
    CTYPE_uint64_t,
    
    CTYPE_int8_t, 
    CTYPE_int16_t,
    CTYPE_int32_t,
    CTYPE_int64_t,
    
    CTYPE_float32_t, 
    CTYPE_float64_t
};

typedef int CTYPE;

typedef struct
{
    uint8_t *data;
    size_t size;    /* number of items */
    CTYPE itemType;
    size_t itemSize;
    int encoding;
} Array;

Array *Array_new(void);

Array *Array_newWithData_size_copy_(uint8_t *data, size_t byteCount, int copy);

Array *Array_clone(Array *self);

// size ---------------------------------

void Array_setSize_(Array *self, size_t size);
size_t Array_size(Array *self);

size_t Array_byteCount(Array *self);

#include "Array_types.h"

/* ----------------------------------------------------- */

Array *Array_newFrom_to_(Array *self, size_t startIndex, size_t endIndex);

#include "Datum.h"
Datum Array_asDatum(Array *self);

void Array_free(Array *self);
size_t Array_memorySize(Array *self);

void Array_compact(Array *self);
void Array_clear(Array *self);
void Array_setAllDataTo_(Array *self, uint8_t c);

void Array_copy_(Array *self, Array *other);

// compare -----------------------------------------------

int Array_compare_(Array *self, Array *other);
int Array_compareData_length_(Array *self, uint8_t *b2, size_t l2);
int Array_compareDatum_(Array *self, Datum d);

int Array_equals_(Array *self, Array *other);
int Array_contains_(Array *self, Array *other);

int Array_beginsWith_(Array *self, Array *other);
int Array_endsWith_(Array *self, Array *other);

#include "Array_deletion.h"

// insert -------------------------------------------------

void Array_insert_at_(Array *self, Array *other, size_t pos);
void Array_insertData_length_at_(Array *self, unsigned char *data, size_t length, size_t pos);

// append -------------------------------------------------

void Array_appendData_length_(Array *self, unsigned char *data, size_t length);
void Array_append_(Array *self, void *s);
void Array_appendAll_(Array *self, Array *other);

// prepend -------------------------------------------------

void Array_prependData_length_(Array *self, unsigned char *data, size_t length);
void Array_prepend_(Array *self, void *s);
void Array_prependAll_(Array *self, Array *other);

/* --- clip --- */

char Array_clipBefore_(Array *self, Array *other);
char Array_clipBeforeEndOf_(Array *self, Array *other);
char Array_clipAfter_(Array *self, Array *other);
char Array_clipAfterStartOf_(Array *self, Array *other);

/* --- strip --- */

void Array_strip_(Array *self, Array *other);
void Array_lstrip_(Array *self, Array *other);
void Array_rstrip_(Array *self, Array *other);

/* --- search --- */

int Array_find_from_(Array *self, Array *other, int from);
int Array_rFind_from_(Array *self, Array *other, int from);
int Array_find_(Array *self, Array *other);
int Array_count_(Array *self, Array *other);

/* --- replace --- */

void Array_copyItem_from_to_(Array *self, void *s, size_t from, size_t to);
void Array_replace_with_(Array *self, Array *substring, Array *other);
void Array_replace_with_output_(Array *self, Array *substring, Array *other, Array *output);

/* --- streams --- */

void Array_writeToStream_(Array *self, BStream *s);
void Array_readFromStream_(Array *self, BStream *s);
void Array_readFromStream_count_(Array *self, BStream *s, size_t count);
void Array_readLineFromStream_(Array *self, BStream *stream);

Array *Array_split_(Array *self, Array *items);

/*
#include "Array_inline.h"
*/

#ifdef __cplusplus
}
#endif
#endif

