#define COLLECTORMARKER_C
#include "CollectorMarker.h"
#undef COLLECTORMARKER_C

#include <assert.h>

void CollectorMarker_check(CollectorMarker *self) {
    CollectorMarker *v = self;

    while (v != self) {
        assert(v->next->prev == v);
        assert(v->prev->next == v);
        v = v->next;
    }
}

CollectorMarker *CollectorMarker_new(void) {
    CollectorMarker *self = io_calloc(1, sizeof(CollectorMarker));
    return self;
}

CollectorMarker *CollectorMarker_newWithColor_(unsigned int color) {
    CollectorMarker *self = CollectorMarker_new();
    self->color = color;
    return self;
}

void CollectorMarker_free(CollectorMarker *self) { io_free(self); }

void CollectorMarker_loop(CollectorMarker *self) {
    self->prev = self;
    self->next = self;
}

int CollectorMarker_count(CollectorMarker *self) {
    int count = 0;
    CollectorMarker *v = self->next;
    unsigned int c = self->color;

    while (v->color == c) {
        CollectorMarker *next = v->next;
        v = next;
        count++;
    }

    return count;
}

int CollectorMarker_colorSetIsEmpty(CollectorMarker *self) {
    return self->color != self->next->color;
}
