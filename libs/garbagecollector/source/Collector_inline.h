// metadoc Collector copyright Steve Dekorte 2002
// metadoc Collector license BSD revised

#ifdef COLLECTOR_C
#define IO_IN_C_FILE
#endif
#include "Common_inline.h"
#ifdef IO_DECLARE_INLINES

// inspecting markers -------------------

IOINLINE int Collector_markerIsWhite_(Collector *self, CollectorMarker *m) {
    return (self->whites->color == m->color);
}

IOINLINE int Collector_markerIsGray_(Collector *self, CollectorMarker *m) {
    return (COLLECTOR_GRAY == m->color);
}

IOINLINE int Collector_markerIsBlack_(Collector *self, CollectorMarker *m) {
    return (self->blacks->color == m->color);
}

// changing marker colors -------------------

IOINLINE void Collector_makeFree_(Collector *self, CollectorMarker *v) {
#ifdef COLLECTOR_RECYCLE_FREED
    CollectorMarker_removeAndInsertAfter_((CollectorMarker *)v, self->freed);
#else
    CollectorMarker_remove(v);
    CollectorMarker_free(v);
#endif
}

IOINLINE void Collector_makeWhite_(Collector *self, CollectorMarker *v) {
    CollectorMarker_removeAndInsertAfter_((CollectorMarker *)v, self->whites);
}

IOINLINE void Collector_makeGray_(Collector *self, CollectorMarker *v) {
    CollectorMarker_removeAndInsertAfter_((CollectorMarker *)v, self->grays);
}

IOINLINE void Collector_makeBlack_(Collector *self, CollectorMarker *v) {
    CollectorMarker_removeAndInsertAfter_((CollectorMarker *)v, self->blacks);
}

IOINLINE void Collector_makeGrayIfWhite_(Collector *self, void *v) {
    if (Collector_markerIsWhite_(self, (CollectorMarker *)v)) {
        Collector_makeGray_(self, (CollectorMarker *)v);
    }
}

/*
IOINLINE void Collector_makeFreed_(Collector *self, void *v)
{
        CollectorMarker_removeAndInsertAfter_(v, self->freed);
}
*/
#ifdef COLLECTOR_USE_NONINCREMENTAL_MARK_SWEEP

#define Collector_value_addingRefTo_(self, v, ref)

#else

IOINLINE void *Collector_value_addingRefTo_(Collector *self, void *v,
                                            void *ref) {
    if (Collector_markerIsBlack_(self, (CollectorMarker *)v) &&
        Collector_markerIsWhite_(self, (CollectorMarker *)ref))
    // if (self->safeMode || (Collector_markerIsBlack_(self, (CollectorMarker
    // *)v) && Collector_markerIsWhite_(self, (CollectorMarker *)ref)))
    {
        Collector_makeGray_(self, (CollectorMarker *)ref);
    }

    return ref;
}

#endif

#undef IO_IN_C_FILE
#endif
