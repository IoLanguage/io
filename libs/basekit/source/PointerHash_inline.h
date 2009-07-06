//metadoc PointerHash copyright Steve Dekorte 2002, Marc Fauconneau 2007
//metadoc PointerHash license BSD revised
/*metadoc PointerHash description
	PointerHash - Cuckoo Hash
	keys and values are references (they are not copied or freed)
	key pointers are assumed unique
*/

#ifdef POINTERHASH_C
#define IO_IN_C_FILE
#endif
#include "Common_inline.h"
#ifdef IO_DECLARE_INLINES

#define PointerHashRecords_recordAt_(records, pos) (PointerHashRecord *)(records + (pos * sizeof(PointerHashRecord)))

/*
IOINLINE unsigned int PointerHash_hash(PointerHash *self, void *key)
{
	intptr_t k = (intptr_t)PointerHashKey_value(key);
	return k^(k>>4);
}

IOINLINE unsigned int PointerHash_hash_more(PointerHash *self, unsigned int hash)
{
	return hash ^ (hash >> self->log2tableSize);
}
*/

// -----------------------------------

IOINLINE PointerHashRecord *PointerHash_record1_(PointerHash *self, void *k)
{
	// the ~| 0x1 before the mask ensures an odd pos
	intptr_t kk = (intptr_t)k;
	size_t pos = ((kk^(kk>>4)) | 0x1) & self->mask;
	return PointerHashRecords_recordAt_(self->records, pos);
}

IOINLINE PointerHashRecord *PointerHash_record2_(PointerHash *self, void *k)
{
	// the | 0x1 before the mask ensures an even pos
	intptr_t kk = (intptr_t)k;
	//size_t pos = (((kk^(kk/33)) << 1)) & self->mask;
	size_t pos = (kk << 1) & self->mask;
	return PointerHashRecords_recordAt_(self->records, pos);
}

IOINLINE void *PointerHash_at_(PointerHash *self, void *k)
{
	PointerHashRecord *r;
	
	r = PointerHash_record1_(self, k);
	if(k == r->k) return r->v;
	
	r = PointerHash_record2_(self, k);	
	if(k == r->k) return r->v;
	
	return 0x0;
}

IOINLINE size_t PointerHash_count(PointerHash *self)
{
	return self->keyCount;
}

IOINLINE int PointerHashKey_hasKey_(PointerHash *self, void *key)
{
	return PointerHash_at_(self, key) != NULL;
}

IOINLINE void PointerHash_at_put_(PointerHash *self, void *k, void *v)
{
	PointerHashRecord *r;
	
	r = PointerHash_record1_(self, k);
	
	if(!r->k)
	{
		r->k = k;
		r->v = v;
		self->keyCount ++;
		return;
	}
	
	if(r->k == k)
	{
		r->v = v;
		return;
	}

	r = PointerHash_record2_(self, k);

	if(!r->k)
	{
		r->k = k;
		r->v = v;
		self->keyCount ++;
		return;
	}
	
	if(r->k == k)
	{
		r->v = v;
		return;
	}
	
	{
	PointerHashRecord x;
	x.k = k;
	x.v = v;
	PointerHash_insert_(self, &x);
	}
}

IOINLINE void PointerHash_shrinkIfNeeded(PointerHash *self)
{
	if(self->keyCount < self->size/8)
	{
		PointerHash_shrink(self);
	}
}

IOINLINE void PointerHashRecord_swapWith_(PointerHashRecord *self, PointerHashRecord *other)
{
	PointerHashRecord tmp = *self;
	*self = *other;
	*other = tmp;
}

IOINLINE void PointerHash_clean(PointerHash *self)
{
	memset(self->records, 0, sizeof(PointerHashRecord) * self->size);
	self->keyCount = 0;
}

// --- enumeration --------------------------------------------------

#define POINTERHASH_FOREACH(self, pkey, pvalue, code) \
{\
	PointerHash *_self = (self);\
	unsigned char *_records = _self->records;\
	unsigned int _i, _size = _self->size;\
	void *pkey;\
	void *pvalue;\
	\
	for (_i = 0; _i < _size; _i ++)\
	{\
		PointerHashRecord *_record = PointerHashRecords_recordAt_(_records, _i);\
		if (_record->k)\
		{\
			pkey = _record->k;\
			pvalue = _record->v;\
			code;\
		}\
	}\
}

#undef IO_IN_C_FILE
#endif
