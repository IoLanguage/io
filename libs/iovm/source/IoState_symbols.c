/*
 docCopyright("Steve Dekorte", 2002)
 docLicense("BSD revised")
 */

#include "IoState.h"
#include "IoObject.h"
#include "IoSeq.h"
#include "IoNumber.h"

#define MIN_CACHED_NUMBER -10
#define MAX_CACHED_NUMBER 256

// numbers ----------------------------------

void IoState_setupCachedNumbers(IoState *self)
{
	int i;

	self->cachedNumbers = List_new();

	for (i = MIN_CACHED_NUMBER; i < MAX_CACHED_NUMBER + 1; i ++)
	{
		IoNumber *number = IoNumber_newWithDouble_(self, i);
		List_append_(self->cachedNumbers, number);
		IoState_retain_(self, number);
	}
}

IoObject *IoState_numberWithDouble_(IoState *self, double n)
{
	long i = (long)n;

	if (self->cachedNumbers && i == n && i >= MIN_CACHED_NUMBER && i <= MAX_CACHED_NUMBER)
	{
		return List_at_(self->cachedNumbers, i - MIN_CACHED_NUMBER);
	}

	return IoNumber_newWithDouble_(self, n);
}

// strings ----------------------------------


IoSymbol *IoState_symbolWithUArray_copy_(IoState *self, UArray *ba, int copy)
{
	IoSymbol *ioSymbol = SHash_at_(self->symbols, ba);

	if (!ioSymbol)
	{
		ioSymbol = IoSeq_newSymbolWithUArray_copy_(self, ba, copy);
		return IoState_addSymbol_(self, ioSymbol);
	}

	if (!copy)
	{
		UArray_free(ba);
	}

	IoState_stackRetain_(self, ioSymbol);
	return ioSymbol;
}

IoSymbol *IoState_symbolWithCString_length_(IoState *self, const char *s, size_t length)
{
	UArray *a = UArray_newWithData_type_size_copy_((char *)s, CTYPE_uint8_t, length, 1);
	return IoState_symbolWithUArray_copy_(self, a, 0);
}

IoSymbol *IoState_symbolWithCString_(IoState *self, const char *s)
{
	return IoState_symbolWithCString_length_(self, s, strlen(s));
}

IoSymbol *IoState_addSymbol_(IoState *self, IoSymbol *s)
{
	SHash_at_put_(self->symbols, IoSeq_rawUArray(s), s);
	IoObject_isSymbol_(s, 1);
	return s;
}

void IoState_removeSymbol_(IoState *self, IoSymbol *s)
{
	SHash_removeKey_(self->symbols, IoSeq_rawUArray(s));
}
