/*
 *  PHash.h
 *  CuckooHashTable
 *
 *  Created by Steve Dekorte on 2009 04 28.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PHASH_DEFINED
#define PHASH_DEFINED 1

#include "Common.h"
#include <stddef.h>
#include <stdint.h>

/*
#define BASEKIT_API 
#define io_calloc calloc
#define io_free free
#define io_malloc malloc
#define IOINLINE
*/

#ifdef __cplusplus
extern "C" {
#endif

#define PHASH_MAXLOOP 10
	
typedef int (PHashEqualFunc)(void *, void *);
typedef intptr_t (PHashHashFunc)(void *);

typedef struct
{
	void *k;
	void *v;
} PHashRecord;
	
typedef struct
{
	unsigned char *records;
	size_t size;
	size_t keyCount;
	intptr_t mask;
} PHash;

BASEKIT_API PHash *PHash_new(void);
BASEKIT_API void PHash_copy_(PHash *self, const PHash *other);
BASEKIT_API PHash *PHash_clone(PHash *self);
BASEKIT_API void PHash_free(PHash *self);

BASEKIT_API void PHash_setSize_(PHash *self, size_t size); // private
BASEKIT_API void PHash_insert_(PHash *self, PHashRecord *x); // private
BASEKIT_API void PHash_grow(PHash *self); // private
BASEKIT_API void PHash_shrinkIfNeeded(PHash *self); // private
BASEKIT_API void PHash_shrink(PHash *self); // private
BASEKIT_API void PHash_show(PHash *self);
BASEKIT_API void PHash_updateMask(PHash *self); // private

BASEKIT_API void PHash_at_put_(PHash *self, void *k, void *v);
BASEKIT_API void PHash_removeKey_(PHash *self, void *k);
BASEKIT_API size_t PHash_size(PHash *self); // actually the keyCount

BASEKIT_API size_t PHash_memorySize(PHash *self);
BASEKIT_API void PHash_compact(PHash *self);
//BASEKIT_API void *PHash_firstKeyForValue_(PHash *self, void *v);
//BASEKIT_API void PHash_removeValue_(PHash *self, void *value);

#include "PHash_inline.h"


#define PHash_cleanSlots(self)
#define PHash_hasDirtyKey_(self, k) 0

#ifdef __cplusplus
}
#endif
#endif
