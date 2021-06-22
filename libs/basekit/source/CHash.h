//metadoc CHash copyright Steve Dekorte 2009
//metadoc CHash license BSD revised

#ifndef CHASH_DEFINED
#define CHASH_DEFINED 1

#include "Common.h"
#include <stddef.h>
#include "PortableStdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CHASH_MAXLOOP 5
	
typedef int (CHashEqualFunc)(void *, void *);
typedef intptr_t (CHashHashFunc)(void *);

typedef struct
{
	void *k;
	void *v;
} CHashRecord;
	
typedef struct
{
	unsigned char *records;
	size_t size;
	size_t keyCount;
	CHashHashFunc *hash1;
	CHashHashFunc *hash2;
	CHashEqualFunc *equals;
	intptr_t mask;
	int isResizing;
} CHash;

BASEKIT_API CHash *CHash_new(void);
BASEKIT_API void CHash_copy_(CHash *self, const CHash *other);
BASEKIT_API CHash *CHash_clone(CHash *self);
BASEKIT_API void CHash_free(CHash *self);

BASEKIT_API void CHash_setHash1Func_(CHash *self, CHashHashFunc *f);
BASEKIT_API void CHash_setHash2Func_(CHash *self, CHashHashFunc *f);
BASEKIT_API void CHash_setEqualFunc_(CHash *self, CHashEqualFunc *f);

BASEKIT_API void CHash_removeKey_(CHash *self, void *k);
BASEKIT_API void CHash_clear(CHash *self);
BASEKIT_API size_t CHash_size(CHash *self); // actually the keyCount

BASEKIT_API size_t CHash_memorySize(CHash *self);
BASEKIT_API void CHash_compact(CHash *self);

// private methods -------------------------------

BASEKIT_API void CHash_setSize_(CHash *self, size_t size); 
BASEKIT_API int CHash_insert_(CHash *self, CHashRecord *x); 
BASEKIT_API void CHash_grow(CHash *self); 
BASEKIT_API void CHash_shrink(CHash *self); 
BASEKIT_API void CHash_show(CHash *self);
BASEKIT_API void CHash_updateMask(CHash *self); 
BASEKIT_API float CHash_density(CHash *self); 

#include "CHash_inline.h"


#ifdef __cplusplus
}
#endif
#endif
