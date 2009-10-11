//metadoc PointerHash copyright Steve Dekorte 2002
//metadoc PointerHash license BSD revised
//metadoc PointerHash notes Suggestion to use cuckoo hash and original implementation by Marc Fauconneau 

#define POINTERHASH_C
#include "PointerHash.h"
#undef POINTERHASH_C
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

PointerHash *PointerHash_new(void)
{
	PointerHash *self = (PointerHash *)io_calloc(1, sizeof(PointerHash));
	PointerHash_setSize_(self, 8);
	return self;
}

void PointerHash_copy_(PointerHash *self, const PointerHash *other)
{
	io_free(self->records);
	memcpy(self, other, sizeof(PointerHash));
	self->records = malloc(self->size * sizeof(PointerHashRecord));
	memcpy(self->records, other->records, self->size * sizeof(PointerHashRecord));
}

PointerHash *PointerHash_clone(PointerHash *self)
{
	PointerHash *other = PointerHash_new();
	PointerHash_copy_(other, self);
	return other;
}

void PointerHash_setSize_(PointerHash *self, size_t size)
{
	self->records = realloc(self->records, size * sizeof(PointerHashRecord));
	
	if(size > self->size)
	{		
		memset(self->records + self->size * sizeof(PointerHashRecord), 
			0x0, (size - self->size) * sizeof(PointerHashRecord));
	}
	
	self->size = size;
	
	PointerHash_updateMask(self);
}

void PointerHash_updateMask(PointerHash *self)
{
	self->mask = (intptr_t)(self->size - 1);
}

void PointerHash_show(PointerHash *self)
{
	size_t i;
	
	printf("PointerHash records:\n");
	for(i = 0; i < self->size; i++)
	{
		PointerHashRecord *r = PointerHashRecords_recordAt_(self->records, i);
		printf("  %i: %p %p\n", (int)i, r->k, r->v);
	}
}

void PointerHash_free(PointerHash *self)
{
	io_free(self->records);
	io_free(self);
}

void PointerHash_insert_(PointerHash *self, PointerHashRecord *x)
{	
	int n;
	
	for (n = 0; n < POINTERHASH_MAXLOOP; n ++)
	{ 
		PointerHashRecord *r;
		
		r = PointerHash_record1_(self, x->k);
		PointerHashRecord_swapWith_(x, r);
		if(x->k == 0x0) { self->keyCount ++; return; }
		 
		r = PointerHash_record2_(self, x->k);
		PointerHashRecord_swapWith_(x, r);
		if(x->k == 0x0) { self->keyCount ++; return; }
	}
	
	PointerHash_grow(self); 
	PointerHash_at_put_(self, x->k, x->v);
}

void PointerHash_insertRecords(PointerHash *self, unsigned char *oldRecords, size_t oldSize)
{
	size_t i;
	
	for (i = 0; i < oldSize; i ++)
	{
		PointerHashRecord *r = PointerHashRecords_recordAt_(oldRecords, i);
		
		if (r->k)
		{
			PointerHash_at_put_(self, r->k, r->v);
		}
	}
}

void PointerHash_resizeTo_(PointerHash *self, size_t newSize)
{
	unsigned char *oldRecords = self->records;
	size_t oldSize = self->size;
	self->size = newSize;
	self->records = io_calloc(1, sizeof(PointerHashRecord) * self->size);
	self->keyCount = 0;
	PointerHash_updateMask(self);
	PointerHash_insertRecords(self, oldRecords, oldSize);
	io_free(oldRecords);
}

void PointerHash_grow(PointerHash *self)
{
	PointerHash_resizeTo_(self, self->size * 2);
}

void PointerHash_shrink(PointerHash *self)
{
	PointerHash_resizeTo_(self, self->size / 2);
}

void PointerHash_removeKey_(PointerHash *self, void *k)
{
	PointerHashRecord *r;
	
	r = PointerHash_record1_(self, k);	
	if(r->k == k)
	{
		r->k = 0x0;
		r->v = 0x0;
		self->keyCount --;
		PointerHash_shrinkIfNeeded(self);
		return;
	}
	
	r = PointerHash_record2_(self, k);
	if(r->k == k)
	{
		r->k = 0x0;
		r->v = 0x0;
		self->keyCount --;
		PointerHash_shrinkIfNeeded(self);
		return;
	}
}

size_t PointerHash_size(PointerHash *self) // actually the keyCount
{
	return self->keyCount;
}

// ----------------------------

size_t PointerHash_memorySize(PointerHash *self)
{
	return sizeof(PointerHash) + self->size * sizeof(PointerHashRecord);
}

void PointerHash_compact(PointerHash *self)
{
}
