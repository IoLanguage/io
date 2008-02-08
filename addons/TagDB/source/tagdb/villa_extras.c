#include <villa_extras.h>
#include <stdio.h>
#include "portable_stdint.h"

VILLA *vlopen_andRepairIfNeeded(char *path, int options, VLCFUNC func)
{
	VILLA *v = vlopen(path, options, func);
	if (v) return v;
	if (vlrepair(path, func)) return vlopen(path, options, func);
	return NULL;
}

#define uintcompare(type, a, b) \
{ \
	type k1 = *(type *)a; \
	type k2 = *(type *)b; \
	if (k1 == k2) return 0; \
	return (k1 > k2) ? 1 : -1; \
}

int VL_CMPUINT(const char *a, int aSize, const char *b, int bSize)
{
	if (aSize != bSize)
	{
		printf("VL_CMPUINT error: uint size %i != uint size %i\n", aSize, bSize);
		exit(-1);	
	}
	
	switch (aSize)
	{
		case 1: uintcompare(uint8_t,  a, b);
		case 2: uintcompare(uint16_t, a, b);
		case 4: uintcompare(uint32_t, a, b);
		case 8: uintcompare(uint64_t, a, b);
	}
	
	printf("VL_CMPUINT error: unsupported uint size %i\n", aSize);
	exit(-1);
	return 0;
}

