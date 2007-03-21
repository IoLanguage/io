/*   
 *   copyright: Steve Dekorte, 2002. All rights reserved.
 *   license: See _BSDLicense.txt.
 *
 *   description: A mutable array of unsigned chars
 *   supports basic operations, searching and 
 *   reading/writing to a file
 */
 

#ifndef ARRAY_TYPES_DEFINED
#define ARRAY_TYPES_DEFINED 1

#include "Array.h"

uint8_t   Array_uint8At_(Array *self,  size_t index);
uint16_t  Array_uint16At_(Array *self, size_t index);
uint32_t  Array_uint32At_(Array *self, size_t index);

int8_t    Array_int8At_(Array *self,  size_t index);
int16_t   Array_int16At_(Array *self, size_t index);
int32_t   Array_int32At_(Array *self, size_t index);

float32_t Array_float32At_(Array *self, size_t index);
float64_t Array_float64At_(Array *self, size_t index);

void Array_setType_(Array *self, CTYPE type);

Array *Array_newConvertedToType_(Array *self, CTYPE outType);

/* --- type --- */

void Array_setItemType_size_(Array *self, CTYPE type, size_t size);
CTYPE Array_itemType(Array *self);
size_t Array_itemSize(Array *self);

void Array_setBytes_type_typeSize_size_(Array *self, void *bytes, size_t typeSize, size_t size);
void *Array_at_(Array *self, size_t index);
int Array_at_put_(Array *self, size_t index, void *s);

#endif

