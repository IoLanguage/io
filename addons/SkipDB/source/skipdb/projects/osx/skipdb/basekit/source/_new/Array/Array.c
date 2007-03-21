/*   
 *   copyright: Steve Dekorte, 2002. All rights reserved.
 *   license: See _BSDLicense.txt.
 */

#include "Base.h" 

#define Array_C
#include "Array.h"
#undef Array_C

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

Array *Array_new(void)
{
    Array *self = (Array *)calloc(1, sizeof(Array));
    self->data = Data_new();
    self->itemType = CTYPE_uint8_t;
    self->itemSize = 1;
    return self;
}

Array *Array_newWithData_size_copy_(uint8_t *bytes, size_t byteCount, int copy)
{
    Array *self = Array_new();
    
    Data_release(self->data);
    self->data = Data_newWithBytes_size_copy_(self->data, byteCount, copy);
    
    return self;
}

inline uint8_t *Array_bytes(Array *self)
{
    uint8_t *bytes = Data_bytesAt_size_(self->data, self->start, self->size);
}

void Array_setBytes_type_itemSize_size_(Array *self, void *bytes, size_t itemSize, size_t size)
{
    size_t byteCount = itemSize * size;
    
    self->data = realloc(self->bytes, byteCount);
    memmove(self->data, data, byteCount);
    self->itemSize = itemSize;
    self->size = size;
}

Array *Array_clone(Array *self)
{
    Array *newArray = (Array *)malloc(sizeof(Array));
    Array_setData_type_itemSize_size_(newArray, self->data, self->itemSize, self->size);
    return newArray;
}

void Array_setSize_(Array *self, size_t size)
{
    size_t newMemSize = self->itemSize * size;
    Data_setSize_(self->data, newMemSize);
    self->size = size;
}

size_t Array_size(Array *self)
{
    return self->size;
}

size_t Array_byteCount(Array *self)
{
    return self->size * self->itemSize;
}

/* --- type --- */

void Array_setItemType_size_(Array *self, CTYPE type, size_t size)
{
    self->itemType = type;
    self->itemSize = size;
}

CTYPE Array_itemType(Array *self)
{
    return self->itemType;
}

size_t Array_itemSize(Array *self)
{
    return self->itemSize;
}

/* --- at, put ------------- */

void *Array_rawAt_(Array *self, size_t index)
{
    size_t pos = self->itemSize * index;
    return (void *)(self->data + pos);
}

void *Array_at_(Array *self, size_t index)
{    
    if (index < self->size)
    {
	return Array_rawAt_(self, index);
    }
    
    return 0x0;
}

int Array_at_put_(Array *self, size_t index, void *s)
{
    void *v = Array_at_(self, index);
    
    if (v) 
    {
	memcpy(v, s, self->itemSize);
	return 1;
    }
    
    return 0;
}

/* ----------------------------------------------------- */

Array *Array_newFrom_to_(Array *self, size_t startIndex, size_t endIndex)
{

}

#include "Datum.h"
Datum Array_asDatum(Array *self);

void Array_free(Array *self)
{
    free(self->data);
    free(self);
}

size_t Array_memorySize(Array *self)
{
    return sizeof(Array) + self->memSize;
}

void Array_compact(Array *self)
{
}

void Array_clear(Array *self)
{
    self->size = 0;
}


void Array_setAllDataTo_(Array *self, uint8_t c)
{

}

void Array_copy_(Array *self, Array *other)
{

}

int Array_compare_(Array *self, Array *other)
{
    return Array_compareData_length_(self, other->data, other->size);
}

int Array_compareData_length_(Array *self, uint8_t *b2, size_t l2)
{
    size_t l1 = self->size;
    size_t min = l1 < l2 ? l1 : l2;
    int cmp = memcmp(self->data, b2, min);
    
    if (cmp == 0)
    {
	if (l1 == l2) 
	{
	    return 0;
	}
	
	if (l1 < l2) 
	{
	    return -1;
	}
	
	return 1;
    }
    
    return cmp;
}

int Array_compareDatum_(Array *self, Datum d);



