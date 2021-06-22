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
#include "IoVMApi.h"
#include "PortableStdint.h"

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

#include "PHash_struct.h"

PHash *PHash_new(void);
void PHash_copy_(PHash *self, const PHash *other);
PHash *PHash_clone(PHash *self);
void PHash_free(PHash *self);

// void PHash_at_put_(PHash *self, void *k, void *v);
void PHash_removeKey_(PHash *self, void *k);
IOVM_API size_t PHash_size(PHash *self); // actually the keyCount

size_t PHash_memorySize(PHash *self);
void PHash_compact(PHash *self);

// --- private methods ----------------------------------------

IOVM_API void PHash_setSize_(PHash *self, size_t size);
IOVM_API void PHash_insert_(PHash *self, PHashRecord *x);
IOVM_API void PHash_grow(PHash *self);
// void PHash_shrinkIfNeeded(PHash *self);
IOVM_API void PHash_shrink(PHash *self);
IOVM_API void PHash_show(PHash *self);
IOVM_API void PHash_updateMask(PHash *self);

#include "PHash_inline.h"

#define PHash_cleanSlots(self)
#define PHash_hasDirtyKey_(self, k) 0

#ifdef __cplusplus
}
#endif
#endif
