/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
docDescription("""
    Hash - Hash table with buckets
    keys and values are references (they are not copied or freed)
    key pointers are assumed unique

    Hash can store a large number of values with a reasonable
    trade off between lookup time and memory usage.

    Collisions are put in a linked list. The storage sized is increased
    and the records rehashed if the ratio of records/storage size exceeds
    a defined amount. The storage size (currently), never shrinks.

    Note: not optimized
    should probably use open addressing instead for better use of CPU cache
""")
*/


#ifndef HASH_DEFINED
#define HASH_DEFINED 1

#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HashRecord HashRecord;

struct HashRecord
{
    void *key;
    void *value;
    HashRecord *next; // next record in the bucket
    HashRecord *nextRecord; // link to next record in entire record list
    HashRecord *previousRecord; // link to previous record in entire record list
};

// --------------------------------------------------------------------- 

typedef struct 
{
    HashRecord **records;
    size_t size;
    size_t count;
    HashRecord *first;
    HashRecord *current;
} Hash;

BASEKIT_API Hash *Hash_new(void);
BASEKIT_API Hash *Hash_clone(Hash *self);
BASEKIT_API void Hash_copy_(Hash *self, Hash *other);

BASEKIT_API void Hash_free(Hash *self);
BASEKIT_API void Hash_freeRecords(Hash *self);
BASEKIT_API void Hash_clean(Hash *self);

BASEKIT_API void Hash_rehash(Hash *self);

BASEKIT_API void *Hash_firstKey(Hash *self);
BASEKIT_API void *Hash_nextKey(Hash *self);

BASEKIT_API void *Hash_firstValue(Hash *self);
BASEKIT_API void *Hash_nextValue(Hash *self);

BASEKIT_API void Hash_verify(Hash *self);
BASEKIT_API size_t Hash_count(Hash *self);

BASEKIT_API HashRecord *Hash_recordAt_(Hash *self, int index);
BASEKIT_API void *Hash_keyAt_(Hash *self, int index);
BASEKIT_API void *Hash_valueAt_(Hash *self, int index);
BASEKIT_API int Hash_indexForValue_(Hash *self, void *v);

BASEKIT_API void *Hash_at_(Hash *self, void *w);
BASEKIT_API void Hash_at_put_(Hash *self, void *w, void *v);
BASEKIT_API void Hash_removeKey_(Hash *self, void *w);
BASEKIT_API void Hash_removeValue_(Hash *self, void *value);

// enumeration

typedef void (HashDoCallback)(void *);
BASEKIT_API void Hash_do_(Hash *self, HashDoCallback *callback);
//void Hash_doOnKeyAndValue_(Hash *self, HashDoCallback *callback);

BASEKIT_API void Hash_doOnKey_(Hash *self, HashDoCallback *callback);

//void Hash_UnitTest(void);

#include "Hash_inline.h"

#ifdef __cplusplus
}
#endif
#endif
