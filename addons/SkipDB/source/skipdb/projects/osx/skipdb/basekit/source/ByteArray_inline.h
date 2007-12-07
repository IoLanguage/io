/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifdef BYTEARRAY_C 
#define IO_IN_C_FILE
#endif
#include "Common_inline.h"
#ifdef IO_DECLARE_INLINES

#include <stdlib.h>
#include <stddef.h>

IOINLINE int IsPathSeparator(const char ch)
{
#ifdef ON_WINDOWS
  return (ch == '/') || (ch == '\\');
#else
  return (ch == '/');
#endif
}

IOINLINE size_t ByteArray_size(ByteArray *self) 
{ 
	return self->size; 
}

IOINLINE unsigned char *ByteArray_bytes(ByteArray *self) 
{ 
	return self->bytes; 
}

IOINLINE int ByteArray_compareData_size_(ByteArray *self, 
								 const unsigned char *b2, 
								 unsigned int l2)
{
	unsigned int l1 = self->size;
	unsigned int min = l1 < l2 ? l1 : l2;
	int cmp = memcmp(self->bytes, b2, min);
	
	if (cmp == 0)
	{
		if (l1 == l2) return 0;
		if (l1 < l2) return -1;
		return 1;
	}
	
	return cmp;
}

IOINLINE int ByteArray_compare_(ByteArray *self, ByteArray *other)
{
	return ByteArray_compareData_size_(self, other->bytes, other->size);
}

IOINLINE int ByteArray_compareDatum_(ByteArray *self, Datum d)
{
	return ByteArray_compareData_size_(self, d.data, d.size);
}

IOINLINE char *ByteArray_asCString(ByteArray *self) 
{ 
	return (char *)self->bytes; 
}

IOINLINE unsigned char ByteArray_at_(ByteArray *self, int pos) 
{ 
	return self->bytes[ByteArray_wrapPos_(self, pos)]; 
}

// type access ----------------------- 

IOINLINE int *ByteArray_intPointerAt_(ByteArray *self, size_t i)
{
	size_t pos = (int)i * sizeof(int);
	if ((pos + sizeof(int)) > self->size) return NULL;
	return (int *)(self->bytes + pos);
}

IOINLINE float *ByteArray_floatPointerAt_(ByteArray *self, size_t i)
{
	size_t pos = (int)i * sizeof(float);
	if ((pos + sizeof(float)) > self->size) return NULL;
	return (float *)(self->bytes + pos);
}

IOINLINE double *ByteArray_doublePointerAt_(ByteArray *self, size_t i)
{
	size_t pos = (int)i * sizeof(double);
	if ((pos + sizeof(double)) > self->size) return NULL;
	return (double *)(self->bytes + pos);
}

// WARNING: no bounds check 

// 16 bit 

IOINLINE size_t ByteArray_length16(ByteArray *self)
{ 
	return self->size / 2; 
}

// 32 bit

IOINLINE size_t ByteArray_size32(ByteArray *self)
{ 
	return self->size / 4; 
}

// int32 

IOINLINE int32_t ByteArray_int32At_(ByteArray *self, size_t i)
{ 
	return *(int32_t *)(self->bytes + (4*i)); 
}

IOINLINE void ByteArray_setInt32At_(ByteArray *self, int32_t v, size_t i)
{ 
	*(int32_t *)(self->bytes + (4*i)) = v; 
}

// float32

IOINLINE float ByteArray_float32At_(ByteArray *self, size_t i)
{ 
	return *(float *)(self->bytes + (4*i)); 
}

IOINLINE void ByteArray_setFloat32_at_(ByteArray *self, float v, size_t i)
{ 
	*(float *)(self->bytes + (4*i)) = v; 
}

// uint32 

IOINLINE uint32_t ByteArray_uint32At_(ByteArray *self, size_t i)
{ 
	return *(uint32_t *)(self->bytes + (4*i)); 
}

IOINLINE void ByteArray_setUint32At_(ByteArray *self, uint32_t v, size_t i)
{ 
	*(uint32_t *)(self->bytes + (4*i)) = v; 
}

// 64 bit 

/*
 IOINLINE int ByteArray_unsignedInt64At_(ByteArray *self, size_t i)
 { 
	 return *(unsigned long *)(self->bytes + (8*i)); 
 }
 */

#undef IO_IN_C_FILE
#endif
