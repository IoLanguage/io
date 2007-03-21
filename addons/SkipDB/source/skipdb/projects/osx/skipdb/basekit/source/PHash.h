/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
docDescription("""
    PHash - Perfect Hash
    keys and values are references (they are not copied or freed)
    key pointers are assumed unique

    This is a perfect hash. When a new key collides with an
    existing one, it is rehashed until no collisions are found.
    
    Rehashing involves adjusting the "a" and "b" hash function 
    parameters until a perfect hash function is found. If one isn't found
    for a given hash table size, the size is doubled and the search 
    for a perfect hash function continues.

    This means lookups are very fast but the number of keys must remain 
    roughly under 100, otherwise the storage size will get too big.
      
    Since chance of collision logrithmically approaches 100% for a 
    given table size as the number of keys increase, the minimal table sizes 
    likewise goes up exponentially with the number of keys. 
    
    The parameterized hash function just manages to flatten the curve 
    far enough to make this useful for <100 keys - which is enough for 
    storing the slots of objects.
""")
*/

#ifndef PHASH_DEFINED
#define PHASH_DEFINED 1

#include "Common.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define PHASH_USE_CACHE 1
	
typedef struct 
{
    void *key;
    void *value;
} PHashRecord;

typedef struct 
{
    PHashRecord *records;
    unsigned int tableSize; // total number of records (power of 2)
    unsigned int numKeys;   // number of used records 
    unsigned int index;     // enumeration index 

    unsigned int a;         // hash mix parameter 1
    unsigned int b;         // hash mix parameter 2
    unsigned int mask;      // hash bit mask
#ifdef PHASH_USE_CACHE
    unsigned int cachedLookup;
#endif
} PHash;

BASEKIT_API PHash *PHash_new(void);
BASEKIT_API void PHash_free(PHash *self);
BASEKIT_API PHash *PHash_clone(PHash *self);
BASEKIT_API void PHash_copy_(PHash *self, PHash *other);

BASEKIT_API unsigned int PHash_size(PHash *self);
BASEKIT_API size_t PHash_memorySize(PHash *self);
BASEKIT_API void PHash_compact(PHash *self);
BASEKIT_API void PHash_collapseRecordsAndAddKey_value_(PHash *self, void *key, void *value);
BASEKIT_API void PHash_rehashWithCollapsedRecords(PHash *self);

BASEKIT_API void *PHash_firstValue(PHash *self);
BASEKIT_API void *PHash_nextValue(PHash *self);

BASEKIT_API void *PHash_firstKey(PHash *self);
BASEKIT_API void *PHash_nextKey(PHash *self);

BASEKIT_API unsigned int PHash_count(PHash *self);
BASEKIT_API unsigned int PHash_doCount(PHash *self);
/*
BASEKIT_API unsigned int PHash_countRecords_size_(unsigned char *records, unsigned int size);
*/

BASEKIT_API PHashRecord *PHash_recordAtIndex_(PHash *self, unsigned int index);
BASEKIT_API void *PHash_keyAt_(PHash *self, unsigned int index);
BASEKIT_API void *PHash_valueAt_(PHash *self, unsigned int index);
BASEKIT_API int PHash_indexForValue_(PHash *self, void *v);
BASEKIT_API void *PHash_firstKeyForValue_(PHash *self, void *v);

// --- perform -------------------------------------------------- 

typedef BASEKIT_API void (PHashDoCallback)(void *);
BASEKIT_API void PHash_do_(PHash *self, PHashDoCallback *callback);

typedef int (PHashDetectCallback)(void *);
BASEKIT_API void *PHash_detect_(PHash *self, PHashDetectCallback *callback); // returns matching key
BASEKIT_API void PHash_doOnKeys_(PHash *self, PHashDoCallback *callback);
BASEKIT_API void PHash_removeValue_(PHash *self, void *value);

// -------------------------------------------------------------- 

/* #define PHASHFUNCTION(v, size) (int)((ptrdiff_t)v)%(size - 1) */

/* a bit of a hack for transparent futures - this only works for Objects with
   unique data pointers (this won't work with Number objects, for example). 
   v is always a String except for the PHash used for protos. 
   Need to move that off of PHash...
*/

/*
#ifndef OBJECT_STRUCT_DEFINED
#include "../IoMessage.h"
#include "../IoObject_struct.h"
#endif
*/

// -------------------------------------------------------------- 

#include "PHash_inline.h"

#ifdef __cplusplus
}
#endif
#endif

