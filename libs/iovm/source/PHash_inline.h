// metadoc PHash copyright Steve Dekorte 2002, Marc Fauconneau 2007
// metadoc PHash license BSD revised
/*metadoc PHash description
        PHash - Cuckoo Hash
        keys and values are references (they are not copied or freed)
        key pointers are assumed unique
*/

#ifdef PHASH_C
#define IO_IN_C_FILE
#endif
#include "Common_inline.h"
#ifdef IO_DECLARE_INLINES

#define Records_recordAt_(records, pos)                                        \
    (PHashRecord *)(records + (pos * sizeof(PHashRecord)))

/*
IOINLINE unsigned int PHash_hash(PHash *self, void *key)
{
        intptr_t k = (intptr_t)PHashKey_value(key);
        return k^(k>>4);
}

IOINLINE unsigned int PHash_hash_more(PHash *self, unsigned int hash)
{
        return hash ^ (hash >> self->log2tableSize);
}
*/

// -----------------------------------

/*
IOINLINE PHashRecord *PHash_record1_(PHash *self, void *k)
{
        // the ~| 0x1 before the mask ensures an odd pos
        intptr_t kk = (intptr_t)k;
        size_t pos = ((kk^(kk>>4)) | 0x1) & self->mask;
        return Records_recordAt_(self->records, pos);
}

IOINLINE PHashRecord *PHash_record2_(PHash *self, void *k)
{
        // the | 0x1 before the mask ensures an even pos
        intptr_t kk = (intptr_t)k;
        //size_t pos = (((kk^(kk/33)) << 1)) & self->mask;
        size_t pos = (kk << 1) & self->mask;
        return Records_recordAt_(self->records, pos);
}
*/

/*
IOINLINE void *PHash_at_(PHash *self, void *k)
{
        PHashRecord *r;

        r = PHash_record1_(self, k);
        if(k == r->k) return r->v;

        r = PHash_record2_(self, k);
        if(k == r->k) return r->v;

        return 0x0;
}

IOINLINE unsigned int PHash_count(PHash *self)
{
        return self->keyCount;
}

IOINLINE int PHashKey_hasKey_(PHash *self, void *key)
{
        return PHash_at_(self, key) != NULL;
}

IOINLINE void PHash_at_put_(PHash *self, void *k, void *v)
{
        PHashRecord *r;

        r = PHash_record1_(self, k);

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

        r = PHash_record2_(self, k);

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
        PHashRecord x;
        x.k = k;
        x.v = v;
        PHash_insert_(self, &x);
        }
}

IOINLINE void PHash_shrinkIfNeeded(PHash *self)
{
        if(self->keyCount < self->size/8)
        {
                PHash_shrink(self);
        }
}

IOINLINE void PHashRecord_swapWith_(PHashRecord *self, PHashRecord *other)
{
        PHashRecord tmp = *self;
        *self = *other;
        *other = tmp;
}

IOINLINE void PHash_clean(PHash *self)
{
        memset(self->records, 0, sizeof(PHashRecord) * self->size);
        self->keyCount = 0;
}

// --- enumeration --------------------------------------------------

#define PHASH_FOREACH(self, pkey, pvalue, code) \
{\
        PHash *_self = (self);\
        unsigned char *_records = _self->records;\
        unsigned int _i, _size = _self->size;\
        void *pkey;\
        void *pvalue;\
        \
        for (_i = 0; _i < _size; _i ++)\
        {\
                PHashRecord *_record = Records_recordAt_(_records, _i);\
                if (_record->k)\
                {\
                        pkey = _record->k;\
                        pvalue = _record->v;\
                        code;\
                }\
        }\
}
*/

#undef IO_IN_C_FILE
#endif
