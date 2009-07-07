//metadoc CHash copyright Steve Dekorte 2009
//metadoc CHash license BSD revised
/*metadoc CHash description
	CHash - Cuckoo Hash
	keys and values are references (they are not copied or freed)
*/

#ifdef CHASH_C
#define IO_IN_C_FILE
#endif
#include "Common_inline.h"
#ifdef IO_DECLARE_INLINES

#define CRecords_recordAt_(records, pos) (CHashRecord *)(records + (pos * sizeof(CHashRecord)))

IOINLINE CHashRecord *CHash_record1_(CHash *self, void *k)
{
	// the ~ | 0x1 before the mask ensures an even pos
	size_t pos = self->hash1(k) & self->mask;
	//printf("pos1 %i/%i\n", pos, self->size);
	return CRecords_recordAt_(self->records, pos);
}

IOINLINE CHashRecord *CHash_record2_(CHash *self, void *k)
{
	// the | 0x1 before the mask ensures an odd pos
	size_t pos = self->hash2(k) & self->mask;
	//printf("pos2 %i/%i\n", pos, self->size);
	return CRecords_recordAt_(self->records, pos);
}

IOINLINE void *CHash_at_(CHash *self, void *k)
{
	CHashRecord *r;
	
	 r = CHash_record1_(self, k);

	if(r->k && self->equals(k, r->k))
	{
		return r->v;
	}
	
	r = CHash_record2_(self, k);
	
	if(r->k && self->equals(k, r->k))
	{
		return r->v;
	}
	
	return 0x0;
}

IOINLINE size_t CHash_count(CHash *self)
{
	return self->keyCount;
}

IOINLINE int CHashKey_hasKey_(CHash *self, void *key)
{
	return CHash_at_(self, key) != NULL;
}

IOINLINE int CHash_at_put_(CHash *self, void *k, void *v)
{
	CHashRecord *r;
	
	r = CHash_record1_(self, k);
	
	if(!r->k)
	{
		r->k = k;
		r->v = v;
		self->keyCount ++;
		return 0;
	}
	
	if(k == r->k || self->equals(k, r->k))
	{
		r->v = v;
		return 0;
	}

	r = CHash_record2_(self, k);

	if(!r->k)
	{
		r->k = k;
		r->v = v;
		self->keyCount ++;
		return 0;
	}
	
	if(k == r->k || self->equals(k, r->k))
	{
		r->v = v;
		return 0;
	}
	

	{
		CHashRecord x;
		x.k = k;
		x.v = v;
		return CHash_insert_(self, &x);
	}
}

IOINLINE void CHash_shrinkIfNeeded(CHash *self)
{
	if(self->keyCount < self->size/5)
	{
		CHash_shrink(self);
	}
}

IOINLINE void CHashRecord_swapWith_(CHashRecord *self, CHashRecord *other)
{
	CHashRecord tmp = *self;
	*self = *other;
	*other = tmp;
}

IOINLINE void CHash_clean(CHash *self)
{
	memset(self->records, 0, sizeof(CHashRecord) * self->size);
	self->keyCount = 0;
}

// --- enumeration --------------------------------------------------

#define CHASH_FOREACH(self, pkey, pvalue, code) \
{\
	CHash *_self = (self);\
	unsigned char *_records = _self->records;\
	unsigned int _i, _size = _self->size;\
	void *pkey;\
	void *pvalue;\
	\
	for (_i = 0; _i < _size; _i ++)\
	{\
		CHashRecord *_record = CRecords_recordAt_(_records, _i);\
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
