/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifdef HASH_C 
#define IO_IN_C_FILE
#endif
#include "Common_inline.h"
#ifdef IO_DECLARE_INLINES

#include <stdlib.h>
#include <stddef.h>

IOINLINE HashRecord *HashRecord_newWithKey_value_(void *key, void *value)
{
	HashRecord *self = (HashRecord *)calloc(1, sizeof(HashRecord));
	self->key = key;
	self->value = value;
	return self;
}

IOINLINE void HashRecord_free(HashRecord *self) 
{ 
	free(self); 
}

// next --- 

IOINLINE void HashRecord_next_(HashRecord *self, HashRecord *r)
{
	self->next = r;
}

IOINLINE HashRecord *HashRecord_next(HashRecord *self)
{
	return self->next;
}

// nextRecord --- 

IOINLINE void HashRecord_nextRecord_(HashRecord *self, HashRecord *r)
{
	self->nextRecord = r;
}

IOINLINE HashRecord *HashRecord_nextRecord(HashRecord *self)
{
	return self->nextRecord;
}

// previousRecord

IOINLINE void HashRecord_previousRecord_(HashRecord *self, HashRecord *r)
{
	self->previousRecord = r;
}

IOINLINE HashRecord *HashRecord_previousRecord(HashRecord *self)
{
	return self->previousRecord;
}

// key

IOINLINE void HashRecord_key_(HashRecord *self, void *k)
{
	self->key = k;
}

IOINLINE void *HashRecord_key(HashRecord *self)
{
	return self->key;
}

// value

IOINLINE void HashRecord_value_(HashRecord *self, void *v)
{
	self->value = v;
}

IOINLINE void *HashRecord_value(HashRecord *self)
{
	return self->value;
}

IOINLINE void Hash_doOnKeyAndValue_(Hash *self, HashDoCallback *callback)
{
	HashRecord *record = self->first;
	int count = 0;
	
	while ( record )
	{
		(*callback)(record->key);
		(*callback)(record->value);
		record = record->nextRecord;
		count ++;
	}
}

#undef IO_IN_C_FILE
#endif
