/*   
 *   copyright: Steve Dekorte, 2002. All rights reserved.
 *   license: See _BSDLicense.txt.
 */

#include "Array_types.h" 

/* --- at types ------------- */

#define ARRAYGETTER(type) \
type Array_ # type # At_(Array *self, size_t index) \
{ \
    return *(type *)Array_rawAt_(self, index); \
}

ARRAYGETTER(int8_t);
ARRAYGETTER(int16_t);
ARRAYGETTER(int32_t);
ARRAYGETTER(uint8_t);
ARRAYGETTER(uint16_t);
ARRAYGETTER(uint32_t);
ARRAYGETTER(float32_t);

#define ARRAYSETTER(type) \
void Array_ # type # At_put_(Array *self, size_t index, type value) \
{ \
    return *(type *)Array_rawAt_(self, index); \
}

ARRAYSETTER(int8_t);
ARRAYSETTER(int16_t);
ARRAYSETTER(int32_t);
ARRAYSETTER(uint8_t);
ARRAYSETTER(uint16_t);
ARRAYSETTER(uint32_t);
ARRAYSETTER(float32_t);

/* ----------------------------------------------------- */

size_t Array_SizeOfType_(CTYPE type)
{
    switch (type)
    {
	case CTYPE_uint8_t:  return 1;
	case CTYPE_uint16_t: return 2;
	case CTYPE_uint32_t: return 4;
	case CTYPE_uint64_t: return 8;
	
	case CTYPE_int8_t:   return 1;
	case CTYPE_int16_t:  return 2;
	case CTYPE_int32_t:  return 4;
	case CTYPE_int64_t:  return 8;
	
	case CTYPE_float32_t: return 4;
	case CTYPE_float64_t: return 8;
	case CTYPE_pointer: return sizeof(type);
    }
    return 0;
}

void Array_setType_(Array *self, CTYPE type)
{
    size_t itemSize = Array_SizeOfType_(type);
    
    if (itemSize)
    {
	self->itemSize = itemSize;
    }
}

#define HandleConvertFrom_ofType_to_ofType_(inArray, inType, outArray, outType) \
    if (inArray->itemType == CTYPE_##inType && outArray->itemType == CTYPE_##outType) \
    { \
	size_t i = inArray->size; \
	inType  *in  = (inType *)inArray->data; \
	outType *out = (outType *)outArray->data; \
	\
	while (i)  \
	{ \
	    *out = *in; \
	    in ++; \
	    out ++; \
	    i --; \
	} \
	return outArray; \
    }

#define HandleConvertFrom_ofType_to_(inArray, type1, outArray) \
    HandleConvertFrom_ofType_to_ofType_(inArray, type1, outArray, uint8_t); \
    HandleConvertFrom_ofType_to_ofType_(inArray, type1, outArray, uint16_t); \
    HandleConvertFrom_ofType_to_ofType_(inArray, type1, outArray, uint32_t); \
    HandleConvertFrom_ofType_to_ofType_(inArray, type1, outArray, int8_t); \
    HandleConvertFrom_ofType_to_ofType_(inArray, type1, outArray, int16_t); \
    HandleConvertFrom_ofType_to_ofType_(inArray, type1, outArray, int32_t); \
    HandleConvertFrom_ofType_to_ofType_(inArray, type1, outArray, float32_t); \
    HandleConvertFrom_ofType_to_ofType_(inArray, type1, outArray, float64_t);

#define HandleConvertFrom_to_(inArray, outArray) \
    HandleConvertFrom_ofType_to_(inArray, uint8_t,   outArray); \
    HandleConvertFrom_ofType_to_(inArray, uint16_t,  outArray); \
    HandleConvertFrom_ofType_to_(inArray, uint32_t,  outArray); \
    HandleConvertFrom_ofType_to_(inArray, int8_t,    outArray); \
    HandleConvertFrom_ofType_to_(inArray, int16_t,   outArray); \
    HandleConvertFrom_ofType_to_(inArray, int32_t,   outArray); \
    HandleConvertFrom_ofType_to_(inArray, float32_t, outArray); \
    HandleConvertFrom_ofType_to_(inArray, float64_t, outArray); 
    
Array *Array_newConvertedToType_(Array *self, CTYPE outType)
{	
    Array *outArray = Array_new();    
    Array_setType_(outArray, outType);
    Array_setSize_(outArray, self->size);
    HandleConvertFrom_to_(self, outArray);
    return 0x0;
}








