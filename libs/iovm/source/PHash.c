// metadoc PHash copyright Steve Dekorte 2002
// metadoc PHash license BSD revised
// metadoc PHash notes Suggestion to use cuckoo hash and original implementation
// by Marc Fauconneau

#define PHASH_C
#include "PHash.h"
#undef PHASH_C
#include "IoObject.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

PHash *PHash_new(void) {
    PHash *self = (PHash *)io_calloc(1, sizeof(PHash));
    PHash_setSize_(self, 8);
    return self;
}

void PHash_copy_(PHash *self, const PHash *other) {
    io_free(self->records);
    memcpy(self, other, sizeof(PHash));
    self->records = malloc(self->size * sizeof(PHashRecord));
    memcpy(self->records, other->records, self->size * sizeof(PHashRecord));
}

PHash *PHash_clone(PHash *self) {
    PHash *other = PHash_new();
    PHash_copy_(other, self);
    return other;
}

void PHash_setSize_(PHash *self, size_t size) {
    self->records = realloc(self->records, size * sizeof(PHashRecord));

    if (size > self->size) {
        memset(self->records + self->size * sizeof(PHashRecord), 0x0,
               (size - self->size) * sizeof(PHashRecord));
    }

    self->size = size;

    PHash_updateMask(self);
}

void PHash_updateMask(PHash *self) { self->mask = (intptr_t)(self->size - 1); }

void PHash_show(PHash *self) {
    int i;

    printf("PHash records:\n");
    for (i = 0; i < self->size; i++) {
        PHashRecord *r = Records_recordAt_(self->records, i);
        printf("  %i: %p %p\n", (int)i, r->k, r->v);
    }
}

void PHash_free(PHash *self) {
    io_free(self->records);
    io_free(self);
}

void PHash_insert_(PHash *self, PHashRecord *x) {
    int n;

    for (n = 0; n < PHASH_MAXLOOP; n++) {
        PHashRecord *r;

        r = PHash_record1_(self, x->k);
        PHashRecord_swapWith_(x, r);
        if (x->k == 0x0) {
            self->keyCount++;
            return;
        }

        r = PHash_record2_(self, x->k);
        PHashRecord_swapWith_(x, r);
        if (x->k == 0x0) {
            self->keyCount++;
            return;
        }
    }

    PHash_grow(self);
    PHash_at_put_(self, x->k, x->v);
}

void PHash_insertRecords(PHash *self, unsigned char *oldRecords,
                         size_t oldSize) {
    int i;

    for (i = 0; i < oldSize; i++) {
        PHashRecord *r = Records_recordAt_(oldRecords, i);

        if (r->k) {
            PHash_at_put_(self, r->k, r->v);
        }
    }
}

void PHash_resizeTo_(PHash *self, size_t newSize) {
    unsigned char *oldRecords = self->records;
    size_t oldSize = self->size;
    self->size = newSize;
    self->records = io_calloc(1, sizeof(PHashRecord) * self->size);
    self->keyCount = 0;
    PHash_updateMask(self);
    PHash_insertRecords(self, oldRecords, oldSize);
    io_free(oldRecords);
}

void PHash_grow(PHash *self) { PHash_resizeTo_(self, self->size * 2); }

void PHash_shrink(PHash *self) { PHash_resizeTo_(self, self->size / 2); }

void PHash_removeKey_(PHash *self, void *k) {
    PHashRecord *r;

    r = PHash_record1_(self, k);
    if (r->k == k) {
        r->k = 0x0;
        r->v = 0x0;
        self->keyCount--;
        PHash_shrinkIfNeeded(self);
        return;
    }

    r = PHash_record2_(self, k);
    if (r->k == k) {
        r->k = 0x0;
        r->v = 0x0;
        self->keyCount--;
        PHash_shrinkIfNeeded(self);
        return;
    }
}

size_t PHash_size(PHash *self) // actually the keyCount
{
    return self->keyCount;
}

// ----------------------------

size_t PHash_memorySize(PHash *self) {
    return sizeof(PHash) + self->size * sizeof(PHashRecord);
}

void PHash_compact(PHash *self) {}
