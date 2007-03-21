/*   
 *   copyright: Steve Dekorte, 2002. All rights reserved.
 *   license: See _BSDLicense.txt.
 *
 *   description: String operations for Array.
 */
 

#ifndef ARRAY_STRING_DEFINED
#define ARRAY_STRING_DEFINED 1

#include "Array.h"
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>

Array *Array_newWithFormat_(const char *format, ...);
Array *Array_newWithVargs_(const char *format, va_list ap);
Array *Array_fromFormat_(Array *self, const char *format, ...);
void Array_fromVargs_(Array *self, const char *format, va_list ap);

void Array_escape(Array *self);
void Array_unescape(Array *self);

void Array_quote(Array *self);
void Array_unquote(Array *self);

void Array_appendCString_(Array *self, char *s);
void Array_prependCString_(Array *self, char *s);
void Array_appendAndEscapeCString_(Array *self, char *s);

void Array_appenddata_length_(Array *self, unsigned char *data, size_t length);
void Array_prependdata_length_(Array *self, unsigned char *data, size_t length);

int Array_containsByte_(Array *self, unsigned char b);

int Array_isLowerCase(Array *self);
int Array_isUpperCase(Array *self);
void Array_lower(Array *self);
void Array_upper(Array *self);

int Array_equalsAnyCase_(Array *self, Array *other);
int Array_containsAnyCase_(Array *self, Array *other);

void Array_setByteWithValue_from_to_(Array *self, unsigned char b, size_t from, size_t to);
void Array_replaceCString_withCString_(Array *self, char *s1, char *s2);

/* --- search operations ------------------- */
int Array_findAnyCase_(Array *self, Array *other);
int Array_findCString_from_(Array *self, char *other, int from);
int Array_findAnyCase_from_(Array *self, Array *other, int from);
int Array_findByteWithValue_from_(Array *self, unsigned char b, int from);
int Array_findByteWithoutValue_from_(Array *self, unsigned char b, int from);


#endif

