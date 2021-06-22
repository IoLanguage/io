// metadoc CollectorMarker copyright Steve Dekorte 2002
// metadoc CollectorMarker license BSD revised

#ifdef COLLECTORMARKER_C
#define IO_IN_C_FILE
#endif
#include "Common_inline.h"
#ifdef IO_DECLARE_INLINES

IOINLINE void CollectorMarker_clear(CollectorMarker *self) {
    self->prev = NULL;
    self->next = NULL;
}

IOINLINE void CollectorMarker_insertAfter_(CollectorMarker *self,
                                           CollectorMarker *other) {
    self->color = other->color;

    self->prev = other;
    self->next = other->next;

    other->next->prev = self;
    other->next = self;
}

/*
IOINLINE void CollectorMarker_insertBefore_(CollectorMarker *self,
CollectorMarker *other)
{
        self->color = other->color;

        self->prev = other->prev;
        self->next = other;

        other->prev->next = self;
        other->prev = self;
}
*/

IOINLINE void CollectorMarker_remove(CollectorMarker *self) {
    self->prev->next = self->next;
    self->next->prev = self->prev;
    // self->next = NULL; // temp
    // self->prev = NULL; // temp
}

IOINLINE void CollectorMarker_removeAndInsertAfter_(CollectorMarker *self,
                                                    CollectorMarker *other) {
    CollectorMarker_remove(self);
    CollectorMarker_insertAfter_(self, other);
}

/*
IOINLINE void CollectorMarker_removeAndInsertBefore_(CollectorMarker *self,
CollectorMarker *other)
{
        CollectorMarker_remove(self);
        CollectorMarker_insertBefore_(self, other);
}
*/

IOINLINE void
CollectorMarker_removeIfNeededAndInsertAfter_(CollectorMarker *self,
                                              CollectorMarker *other) {
    if (self->prev) {
        CollectorMarker_remove(self);
    }

    CollectorMarker_insertAfter_(self, other);
}

IOINLINE int CollectorMarker_isEmpty(CollectorMarker *self) {
    return (self->color != self->next->color);
}

#undef IO_IN_C_FILE
#endif
