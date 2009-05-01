//metadoc CHash copyright Steve Dekorte 2009
//metadoc CHash license BSD revised
//metadoc PHash notes Suggestion to use cuckoo hash and original implementation by Marc Fauconneau 

#define CHASH_C
#include "CHash.h"
#undef CHASH_C
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

CHash *CHash_new(void)
{
	CHash *self = (CHash *)io_calloc(1, sizeof(CHash));
	CHash_setSize_(self, 8);
	return self;
}

void CHash_copy_(CHash *self, const CHash *other)
{
	io_free(self->records);
	memcpy(self, other, sizeof(CHash));
	self->records = malloc(self->size * sizeof(CHashRecord));
	memcpy(self->records, other->records, self->size * sizeof(CHashRecord));
}

CHash *CHash_clone(CHash *self)
{
	CHash *other = CHash_new();
	CHash_copy_(other, self);
	return other;
}

void CHash_setSize_(CHash *self, size_t size)
{
	self->records = realloc(self->records, size * sizeof(CHashRecord));
	
	if(size > self->size)
	{		
		memset(self->records + self->size * sizeof(CHashRecord), 
			0x0, (size - self->size) * sizeof(CHashRecord));
	}
	
	self->size = size;
	
	CHash_updateMask(self);
	//CHash_show(self);
}

void CHash_updateMask(CHash *self)
{
	self->mask = (intptr_t)(self->size - 1);
}

void CHash_show(CHash *self)
{
	int i;
	
	printf("CHash records:\n");
	for(i = 0; i < self->size; i++)
	{
		CHashRecord *r = CRecords_recordAt_(self->records, i);
		printf("  %i: %i %i\n", i, r->k, r->v);
	}
}

void CHash_free(CHash *self)
{
	io_free(self->records);
	io_free(self);
}

void CHash_setHash1Func_(CHash *self, CHashHashFunc *f)
{
	self->hash1 = f;
}

void CHash_setHash2Func_(CHash *self, CHashHashFunc *f)
{
	self->hash2 = f;
}

void CHash_setEqualFunc_(CHash *self, CHashEqualFunc *f)
{
	self->equals = f;
}

void CHash_insert_(CHash *self, CHashRecord *x)
{	
	int n;
	
	for (n = 0; n < CHASH_MAXLOOP; n ++)
	{ 
		CHashRecord *r;
		
		r = CHash_record1_(self, x->k);
		CHashRecord_swapWith_(x, r); //x ↔ T1 [h1 (x)] 
		if(x->k == 0x0) { self->keyCount ++; return; }
		 
		r = CHash_record2_(self, x->k);
		CHashRecord_swapWith_(x, r); //x ↔ T2 [h2 (x)] 
		if(x->k == 0x0) { self->keyCount ++; return; }
	}
	
	CHash_grow(self); 
	CHash_at_put_(self, x->k, x->v);
}

void CHash_insertRecords(CHash *self, unsigned char *oldRecords, size_t oldSize)
{
	int i;
	
	for (i = 0; i < oldSize; i ++)
	{
		CHashRecord *r = CRecords_recordAt_(oldRecords, i);
		
		if (r->k)
		{
			CHash_at_put_(self, r->k, r->v);
		}
	}
}

void CHash_resizeTo_(CHash *self, size_t newSize)
{
	unsigned char *oldRecords = self->records;
	size_t oldSize = self->size;
	self->size = newSize;
	self->records = io_calloc(1, sizeof(CHashRecord) * self->size);
	self->keyCount = 0;
	CHash_updateMask(self);
	CHash_insertRecords(self, oldRecords, oldSize);
	io_free(oldRecords);
}

void CHash_grow(CHash *self)
{
	CHash_resizeTo_(self, self->size * 2);
}

void CHash_shrink(CHash *self)
{
	CHash_resizeTo_(self, self->size / 2);
}

void CHash_removeKey_(CHash *self, void *k)
{
	CHashRecord *r1 = CHash_record1_(self, k);
	CHashRecord *r2;
	
	if(r1->k && self->equals(k, r1->k))
	{
		r1->k = 0x0;
		r1->v = 0x0;
		self->keyCount --;
		CHash_shrinkIfNeeded(self);
		return;
	}
	
	r2 = CHash_record2_(self, k);
	
	if(r2->k && self->equals(k, r2->k))
	{
		r2->k = 0x0;
		r2->v = 0x0;
		self->keyCount --;
		CHash_shrinkIfNeeded(self);
		return;
	}
}

size_t CHash_size(CHash *self) // actually the keyCount
{
	return self->keyCount;
}

// ----------------------------

size_t CHash_memorySize(CHash *self)
{
	return sizeof(CHash) + self->size * sizeof(CHashRecord);
}

void CHash_compact(CHash *self)
{
}
