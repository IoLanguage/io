/*
	copyright: Steve Dekorte, 2006. All rights reserved.
	license: See _BSDLicense.txt.
*/

#include "UArray.h"
#include <math.h>
#include <float.h>

//#define IO_USE_SIMD 1

#ifdef IO_USE_SIMD

#ifdef _MSC_VER
#pragma message("Note: IO_USE_SIMD on, attempting to use SIMD acceleration")
#else
#warning Note: IO_USE_SIMD on, attempting to use SIMD acceleration
#endif

#include "simd_cp.h"
#else

#ifdef _MSC_VER
#pragma message("Uncomment the IO_USE_SIMD define to turn on SIMD acceleration")
#else
#warning Uncomment the IO_USE_SIMD define to turn on SIMD acceleration
#endif

#define __UNK__EMU__
#include "simd_cp_emu.h"
#endif

#define VEC_SIZE 4

#define LLVEC_DUALARG_OP(VNAME, COP, TYPE) \
void v ## TYPE ## _ ## VNAME(TYPE ##_t *aa, TYPE ##_t *bb, size_t size)\
{\
	size_t i = 0;\
	simd_m128 *a = (simd_m128 *)aa;\
	simd_m128 *b = (simd_m128 *)bb;\
	size_t max = (size / VEC_SIZE);\
	for(i = 0; i < max; i ++) { simd_4f_ ## VNAME (a[i], b[i], a[i]); }\
	i = i * VEC_SIZE;\
	while (i < size) { aa[i] COP bb[i]; i ++; }\
}

LLVEC_DUALARG_OP(add,  +=, float32);
LLVEC_DUALARG_OP(sub,  -=, float32);
LLVEC_DUALARG_OP(mult, *=, float32);
LLVEC_DUALARG_OP(div,  /=, float32);

// set

void UArray_round(UArray *self)
{
	UARRAY_FOREACHASSIGN(self, i, v, floor((double)(v+.5)));
}

void UArray_clear(UArray *self)
{
	UARRAY_FOREACHASSIGN_VALUE_UNUSED(self, i, 0);
}

void UArray_setItemsToLong_(UArray *self, long x)
{
	UARRAY_FOREACHASSIGN_VALUE_UNUSED(self, i, x);
}

void UArray_setItemsToDouble_(UArray *self, double x)
{
	UARRAY_FOREACHASSIGN_VALUE_UNUSED(self, i, x);
}

void UArray_rangeFill(UArray *self)
{
	UARRAY_FOREACHASSIGN_VALUE_UNUSED(self, i, i);
}

void UArray_negate(const UArray *self)
{
	if(UArray_isSignedType(self))
	{
		UARRAY_FOREACHASSIGN(self, i, v, -v);
	}
	else
	{
		UArray_error_(self, "UArray_negate not supported on this type");
	}
}

// basic vector math

void UArray_add_(UArray *self, const UArray *other)
{
	if (self->itemType == CTYPE_float32_t && other->itemType == CTYPE_float32_t)
	{
		vfloat32_add((float32_t *)self->data, (float32_t *)other->data, UArray_minSizeWith_(self, other));
		return;
	}

	DUARRAY_OP(UARRAY_BASICOP_TYPES, +=, self, other);
}

void UArray_subtract_(UArray *self, const UArray *other)
{
	if (self->itemType == CTYPE_float32_t && other->itemType == CTYPE_float32_t)
	{
		vfloat32_sub((float32_t *)self->data, (float32_t *)other->data, UArray_minSizeWith_(self, other));
		return;
	}

	DUARRAY_OP(UARRAY_BASICOP_TYPES, -=, self, other);
}

void UArray_multiply_(UArray *self, const UArray *other)
{
	if (self->itemType == CTYPE_float32_t && other->itemType == CTYPE_float32_t)
	{
		vfloat32_mult((float32_t *)self->data, (float32_t *)other->data, UArray_minSizeWith_(self, other));
		return;
	}

	DUARRAY_OP(UARRAY_BASICOP_TYPES, *=, self, other);
}

void UArray_divide_(UArray *self, const UArray *other)
{
	if (self->itemType == CTYPE_float32_t && other->itemType == CTYPE_float32_t)
	{
		vfloat32_div((float32_t *)self->data, (float32_t *)other->data, UArray_minSizeWith_(self, other));
		return;
	}

	DUARRAY_OP(UARRAY_BASICOP_TYPES, /=, self, other);
}

#define UARRAY_DOT(OP2, TYPE1, self, TYPE2, other)\
{\
	double p = 0;\
	size_t i, minSize = self->size < other->size ? self->size : other->size;\
	for(i = 0; i < minSize; i ++)\
	{ p += ((TYPE1 *)self->data)[i] * ((TYPE2 *)other->data)[i]; }\
	return p; \
}


double UArray_dotProduct_(const UArray *self, const UArray *other)
{
	DUARRAY_OP(UARRAY_DOT, NULL, self, other);
	return 0; // to keep compiler from annoying us
}

// basic scalar math

void UArray_addScalarDouble_(UArray *self, double value)
{
	UARRAY_FOREACHASSIGN(self, i, v, v + value);
}

void UArray_subtractScalarDouble_(UArray *self, double value)
{
	UARRAY_FOREACHASSIGN(self, i, v, v - value);
}

void UArray_multiplyScalarDouble_(UArray *self, double value)
{
	UARRAY_FOREACHASSIGN(self, i, v, v * value);
}

void UArray_divideScalarDouble_(UArray *self, double value)
{
	UARRAY_FOREACHASSIGN(self, i, v, v / value);
}

// bitwise

void UArray_bitwiseOr_(UArray *self, const UArray *other)
{
	size_t i, max = UArray_minSizeInBytesWith_(self, other);
	uint8_t *d1 = self->data;
	uint8_t *d2 = other->data;
	for (i = 0; i < max; i ++) { d1[i] |= d2[i]; }
}

void UArray_bitwiseAnd_(UArray *self, const UArray *other)
{
	size_t i, max = UArray_minSizeInBytesWith_(self, other);
	uint8_t *d1 = self->data;
	uint8_t *d2 = other->data;
	for (i = 0; i < max; i ++) { d1[i] &= d2[i]; }
}

void UArray_bitwiseXor_(UArray *self, const UArray *other)
{
	size_t i, max = UArray_minSizeInBytesWith_(self, other);
	uint8_t *d1 = self->data;
	uint8_t *d2 = other->data;
	for (i = 0; i < max; i ++) { d1[i] ^= d2[i]; }
}

void UArray_bitwiseNot(UArray *self)
{
	size_t i, max = UArray_sizeInBytes(self);
	uint8_t *data = self->data;
	for (i = 0; i < max; i ++) { data[i] = ~(data[i]); }
}

// bitwise ops

void UArray_setAllBitsTo_(UArray *self, uint8_t aBool)
{
	size_t i, max = UArray_sizeInBytes(self);
	uint8_t *data = self->data;
	uint8_t bits = aBool ? ~0 : 0;
	for (i = 0; i < max; i ++) { data[i] = bits; }
}

// adjust for endianess?

int UArray_bitAt_(UArray *self, size_t i)
{
	size_t bytePos = i / 8;
	size_t bitPos  = i % 8;
	if (bytePos >= UArray_sizeInBytes(self)) return 0;
	return (self->data[bytePos] >> bitPos) & 0x1;
}

uint8_t UArray_byteAt_(UArray *self, size_t i)
{
	if (i < UArray_sizeInBytes(self)) return self->data[i];
	return 0;
}

void UArray_setBit_at_(UArray *self, int aBool, size_t i)
{
	size_t bytePos = i / 8;
	size_t bitPos  = i % 8;
	uint8_t n = 0x1 << bitPos;
	uint8_t b;
	if (bytePos >= UArray_sizeInBytes(self)) return;
	b = self->data[bytePos];
	b ^= n;
	if (aBool) b |= (0x1 << bitPos);
	self->data[bytePos] = b;
}

UArray *UArray_asBits(const UArray *self)
{
	UArray *out = UArray_new();
	size_t i, max = UArray_sizeInBytes(self);
	uint8_t *data = self->data;

	for (i = 0; i < max; i ++)
	{
		uint8_t b = data[i];
		int j;

		for (j = 0; j < 8; j ++)
		{
			int v = (b >> j) & 0x1;
			UArray_appendCString_(out, v ? "1" : "0");
		}
	}

	return out;
}

size_t UArray_bitCount(UArray *self)
{
	const unsigned char map[] =
	{
		0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
	};

	size_t i, max = UArray_sizeInBytes(self);
	uint8_t *data = self->data;
	size_t total = 0;

	for (i = 0; i < max; i ++) { total += map[data[i]]; }

	return total;
}

// logic

#define UARRAY_LOGICOP_TYPES(OP2, TYPE1, self, TYPE2, other)\
{\
	size_t i, minSize = self->size < other->size ? self->size : other->size;\
		for(i = 0; i < minSize; i ++)\
		{\
			((TYPE1 *)self->data)[i] = ((TYPE1 *)self->data)[i] OP2 ((TYPE2 *)other->data)[i];\
		}\
}

void UArray_logicalOr_(UArray *self, const UArray *other)
{
	DUARRAY_INTOP(UARRAY_LOGICOP_TYPES, ||, self, other);
}

void UArray_logicalAnd_(UArray *self, const UArray *other)
{
	DUARRAY_INTOP(UARRAY_LOGICOP_TYPES, &&, self, other);
}

// trigonometry

#define UARRAY_DOP(OP) \
void UArray_ ## OP (UArray *self) { UARRAY_FOREACHASSIGN(self, i, v, OP((double)v)); }

UARRAY_DOP(sin);
UARRAY_DOP(cos);
UARRAY_DOP(tan);
UARRAY_DOP(asin);
UARRAY_DOP(acos);
UARRAY_DOP(atan);
UARRAY_DOP(sinh);
UARRAY_DOP(cosh);
UARRAY_DOP(tanh);
UARRAY_DOP(exp);
UARRAY_DOP(log);
UARRAY_DOP(log10);

UARRAY_DOP(sqrt);
UARRAY_DOP(ceil);
UARRAY_DOP(floor);
UARRAY_DOP(fabs);

void UArray_abs(UArray *self)
{
	UArray_fabs(self);
}

void UArray_square(UArray *self)
{
	UArray_multiply_(self, self);
}

// extras

double UArray_sumAsDouble(const UArray *self)
{
	double sum = 0;
	UARRAY_FOREACH(self, i, v, sum += v);
	return sum;
}

double UArray_productAsDouble(const UArray *self)
{
	double p = 1;
	UARRAY_FOREACH(self, i, v, p *= v);
	return p;
}

double UArray_arithmeticMeanAsDouble(const UArray *self)
{
	return UArray_sumAsDouble(self) / ((double)self->size);
}

double UArray_arithmeticMeanSquareAsDouble(const UArray *self)
{
	double r;
	UArray *s = UArray_clone(self);
	UArray_square(s);
	r = UArray_arithmeticMeanAsDouble(s);
	UArray_free(s);
	return r;
}

double UArray_maxAsDouble(const UArray *self)
{
	if(self->size > 0)
	{
		double max = DBL_MIN;
		UARRAY_FOREACH(self, i, v, if(v > max) { max = v; });
		return max;
	}

	return 0;
}

double UArray_minAsDouble(const UArray *self)
{
	if(self->size > 0)
	{
		double max = DBL_MAX;
		UARRAY_FOREACH(self, i, v, if(v < max) { max = v; });
		return max;
	}

	return 0;
}

BASEKIT_API void UArray_Max(UArray *self, const UArray *other)
{
	size_t i, minSize = self->size < other->size ? self->size : other->size;

	for(i = 0; i < minSize; i ++)
	{
		double v1 = UArray_rawDoubleAt_(self, i);
		double v2 = UArray_rawDoubleAt_(other, i);
		double m = v1 > v2 ? v1 : v2;
		UArray_at_putDouble_(self, i, m);
	}
}

BASEKIT_API void UArray_Min(UArray *self, const UArray *other)
{
	size_t i, minSize = self->size < other->size ? self->size : other->size;

	for(i = 0; i < minSize; i ++)
	{
		double v1 = UArray_rawDoubleAt_(self, i);
		double v2 = UArray_rawDoubleAt_(other, i);
		double m = v1 < v2 ? v1 : v2;
		UArray_at_putDouble_(self, i, m);
	}
}


void UArray_normalize(UArray *self)
{
	double a;
	UArray *s = UArray_clone(self);
	UArray_square(s);
	a = UArray_sumAsDouble(s);
	UArray_free(s);
	a = sqrt(a);
	//double max = UArray_maxAsDouble(self);
	UArray_divideScalarDouble_(self, a);
}

void UArray_crossProduct_(UArray *self, const UArray *other)
{
	if (self->itemType == CTYPE_float32_t &&
		other->itemType == CTYPE_float32_t &&
		self->size > 2 && other->size > 2)
	{
		float32_t *a = (float32_t *)self->data;
		float32_t *b = (float32_t *)other->data;

		float32_t i = (a[1]*b[2]) - (a[2]*b[1]);
		float32_t j = (a[2]*b[0]) - (a[0]*b[2]);
		float32_t k = (a[0]*b[1]) - (a[1]*b[0]);

		a[0] = i;
		a[1] = j;
		a[2] = k;

		UArray_changed(self);

		return;
	}
}

double UArray_distanceTo_(const UArray *self, const UArray *other)
{
	if (self->itemType == CTYPE_float32_t &&
		other->itemType == CTYPE_float32_t)
	{
		float32_t *a = (float32_t *)self->data;
		float32_t *b = (float32_t *)other->data;
		size_t max = self->size > other->size ? self->size : other->size;
		double sum = 0;

		if (self->size == other->size)
		{
			size_t i;

			for (i = 0; i < max; i ++)
			{
				float32_t d = a[i] - b[i];
				sum += d * d;
			}
		}

		return (double)sqrt((double)sum);
	}
	else if (self->itemType == CTYPE_float64_t &&
		other->itemType == CTYPE_float64_t)
	{
		float64_t *a = (float64_t *)self->data;
		float64_t *b = (float64_t *)other->data;
		size_t max = self->size > other->size ? self->size : other->size;
		double sum = 0;

		if (self->size == other->size)
		{
			size_t i;

			for (i = 0; i < max; i ++)
			{
				float32_t d = a[i] - b[i];
				sum += d * d;
			}
		}

		return (double)sqrt((double)sum);
	}

	return 0;
}

// hash

void UArray_changed(UArray *self)
{
	self->evenHash = 0;
	self->oddHash = 0;
}

#include "Hash_fnv.h"
#include "Hash_superfast.h"
#include "Hash_murmur.h"

uintptr_t UArray_superfastHash(UArray *self)
{
	return SuperFastHash((char *)(self->data), UArray_sizeInBytes(self));
}

uintptr_t UArray_fnvHash(UArray *self)
{
	return (uintptr_t)fnv_32_buf((void *)(self->data), UArray_sizeInBytes(self), FNV1_32_INIT) << 1; // ensures odd result
}

uintptr_t UArray_murmurHash(UArray *self)
{
	return (uintptr_t)MurmurHash2((const void *)(self->data), UArray_sizeInBytes(self), 0);
}

// even/odd hashes for cuckoo hashtables

uintptr_t UArray_calcOddHash(UArray *self)
{
	return UArray_superfastHash(self) | 0x1; // ensures odd result
}

uintptr_t UArray_calcEvenHash(UArray *self)
{
	return UArray_fnvHash(self) << 1; // ensures odd result
}

// Caching even/odd hashes for cuckoo hashtables

uintptr_t UArray_oddHash(UArray *self)
{
	if (!self->oddHash)
	{
		self->oddHash = UArray_calcOddHash(self);
	}

	return self->oddHash;
}

uintptr_t UArray_evenHash(UArray *self)
{
	if (!self->evenHash)
	{
		self->evenHash = UArray_calcEvenHash(self);
	}

	return self->evenHash;
}

int UArray_equalsWithHashCheck_(UArray *self, UArray *other)
{
	if (self == other)
	{
		return 1;
	}
	else
	{
		if (UArray_evenHash(self) != UArray_evenHash(other))
		{
			return 0;
		}

		if (UArray_oddHash(self) != UArray_oddHash(other))
		{
			return 0;
		}
		/*
		if(strcmp(self->data, other->data) != 0)
		{
			printf("[%s] %i == %i [%s]\n", self->data, h1, h2, other->data);
		}
		*/
	}

	return UArray_equals_(self, other);
}

// indexes

BASEKIT_API void UArray_duplicateIndexes(UArray *self)
{
	size_t size = self->size;
	int itemSize = self->itemSize;

	if (size)
	{
		size_t si = size - 1;
		size_t di = (size * 2) - 1;
		uint8_t *b;

		UArray_setSize_(self, self->size * 2);

		b = self->data;

		for (;;)
		{
			uint8_t *src  = b + si * itemSize;
			uint8_t *dest = b + di * itemSize;

			memcpy(dest, src, itemSize);
			memcpy(dest - itemSize, src, itemSize);

			if (si == 0) break;
			di = di - 2;
			si --;
		}
	}
}

void UArray_removeOddIndexes(UArray *self)
{
	size_t itemSize = self->itemSize;
	size_t di = 1;
	size_t si = 2;
	size_t max = self->size;
	uint8_t *b = self->data;

	if (max == 0)
	{
		return;
	}

	while (si < max)
	{
		uint8_t *src  = b + (si * itemSize);
		uint8_t *dest = b + (di * itemSize);
		memcpy(dest, src, itemSize);
		si = si + 2;
		di = di + 1;
	}

	UArray_setSize_(self, di);
}

void UArray_removeEvenIndexes(UArray *self)
{
	size_t itemSize = self->itemSize;
	size_t di = 0;
	size_t si = 1;
	size_t max = self->size;
	uint8_t *b = self->data;

	while (si < max)
	{
		uint8_t *src  = b + (si * itemSize);
		uint8_t *dest = b + (di * itemSize);
		memcpy(dest, src, itemSize);
		si = si + 2;
		di = di + 1;
	}

	UArray_setSize_(self, di);
}

void UArray_reverseItemByteOrders(UArray *self)
{
	size_t itemSize = self->itemSize;

	if (itemSize > 1)
	{
		size_t i, max = self->size;
		uint8_t *d = self->data;

		for(i = 0; i < max; i ++)
		{
			size_t j;

			for(j = 0; j < itemSize; j ++)
			{
				size_t i1 = i + j;
				size_t i2 = i + itemSize - j;
				uint8_t v = d[i1];
				d[i1] = d[i2];
				d[i2] = v;
			}
		}

		UArray_changed(self);
	}
}

//
void UArray_addEqualsOffsetXScaleYScale(UArray *self, UArray *other, float offset, float xscale, float yscale)
{	
	if (self->itemType == CTYPE_float32_t && other->itemType == CTYPE_float32_t)
	{
		float32_t *d1 = (float32_t *)self->data;
		float32_t *d2 = (float32_t *)other->data;
		
		long i = offset;
		long j = 0;
		
		while (i < (long)self->size)
		{			
			size_t jj = j/xscale;
			
			//printf("self->size: %i i: %i jj: %i\n", self->size, i, jj);
			if (jj > other->size - 1) break;
			
			if (i >= 0) //&& jj < other->size - 1)
			{			
				d1[i] += d2[jj] * yscale;
			}
			
			j ++;
			i ++;
		}
		
		UArray_changed(self);
	}
	else
	{
		printf("UArray_addEqualsOffsetXScaleYScale called on non float array\n");
		exit(-1);
	}
}

