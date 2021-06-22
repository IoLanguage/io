#define COLLECTOR_C
#include "Collector.h"
#undef COLLECTOR_C

#include <assert.h>
#include <time.h>

#ifdef COLLECTOR_TIMER_ON
#define BEGIN_TIMER self->clocksUsed -= clock();
#define END_TIMER self->clocksUsed += clock();
#else
#define BEGIN_TIMER
#define END_TIMER
#endif

Collector *Collector_new(void) {
    Collector *self = (Collector *)io_calloc(1, sizeof(Collector));

    self->retainedValues = List_new();

    self->whites = CollectorMarker_new();
    self->grays = CollectorMarker_new();
    self->blacks = CollectorMarker_new();
    self->freed = CollectorMarker_new();

    CollectorMarker_loop(self->whites);
    CollectorMarker_removeIfNeededAndInsertAfter_(self->grays, self->whites);
    CollectorMarker_removeIfNeededAndInsertAfter_(self->blacks, self->grays);
    CollectorMarker_removeIfNeededAndInsertAfter_(self->freed, self->blacks);

    // important to set colors *after* inserts, since inserts set colors
    CollectorMarker_setColor_(self->whites, COLLECTOR_INITIAL_WHITE);
    CollectorMarker_setColor_(self->blacks, COLLECTOR_INITIAL_BLACK);
    CollectorMarker_setColor_(self->grays, COLLECTOR_GRAY);
    CollectorMarker_setColor_(self->freed, COLLECTOR_FREE);

    Collector_setSafeModeOn_(self, 1);
    self->allocated = 0;

    self->allocatedSweepLevel = 3000;
    self->allocatedStep = 1.1f;
    self->marksPerAlloc = 2;

#ifdef COLLECTOR_USE_NONINCREMENTAL_MARK_SWEEP
    self->allocsPerSweep = 10000;
#endif

    self->clocksUsed = 0;

    Collector_check(self);

    return self;
}

void Collector_check(Collector *self) {
    CollectorMarker *w = self->whites;
    CollectorMarker *g = self->grays;
    CollectorMarker *b = self->blacks;

    // colors are different
    assert(w->color != g->color);
    assert(w->color != b->color);
    assert(g->color != b->color);

    // prev color is different
    assert(w->prev->color != w->color);
    assert(g->prev->color != g->color);
    assert(b->prev->color != b->color);

    CollectorMarker_check(w);
}

size_t CollectorMarker_checkObjectPointer(CollectorMarker *marker) {
    if (marker->object == NULL) {
        printf("WARNING: Collector found a null object pointer on marker %p! "
               "Memory is likely hosed.\n",
               (void *)marker);
        exit(-1);
        return 1;
    } else {
        // read a word of memory to check for bad pointers
        uintptr_t p = *(uintptr_t *)(marker->object);
        return p - p; // so compiler doesn't complain about unused variable
    }

    return 0;
}

void Collector_checkObjectPointers(Collector *self) {
    COLLECTMARKER_FOREACH(self->blacks, v,
                          CollectorMarker_checkObjectPointer(v););
    COLLECTMARKER_FOREACH(self->grays, v,
                          CollectorMarker_checkObjectPointer(v););
    COLLECTMARKER_FOREACH(self->whites, v,
                          CollectorMarker_checkObjectPointer(v););
}

void Collector_checkObjectsWith_(Collector *self, CollectorCheckFunc *func) {
    COLLECTMARKER_FOREACH(self->blacks, v, func(v););
    COLLECTMARKER_FOREACH(self->grays, v, func(v););
    COLLECTMARKER_FOREACH(self->whites, v, func(v););
}

void Collector_free(Collector *self) {
    List_free(self->retainedValues);
    CollectorMarker_free(self->whites);
    CollectorMarker_free(self->grays);
    CollectorMarker_free(self->blacks);
    CollectorMarker_free(self->freed);
    io_free(self);
}

void Collector_setMarkBeforeSweepValue_(Collector *self, void *v) {
    self->markBeforeSweepValue = v;
}

// callbacks

void Collector_setFreeFunc_(Collector *self, CollectorFreeFunc *func) {
    self->freeFunc = func;
}

void Collector_setWillFreeFunc_(Collector *self, CollectorWillFreeFunc *func) {
    self->willFreeFunc = func;
}

void Collector_setMarkFunc_(Collector *self, CollectorMarkFunc *func) {
    self->markFunc = func;
}

// marks per alloc

void Collector_setMarksPerAlloc_(Collector *self, float n) {
    self->marksPerAlloc = n;
}

float Collector_marksPerAlloc(Collector *self) { return self->marksPerAlloc; }

// allocated step

void Collector_setAllocatedStep_(Collector *self, float n) {
    self->allocatedStep = n;
}

float Collector_allocatedStep(Collector *self) { return self->allocatedStep; }

void Collector_setDebug_(Collector *self, int b) { self->debugOn = b ? 1 : 0; }

void Collector_setSafeModeOn_(Collector *self, int b) {
    self->safeMode = b ? 1 : 0;
}

// retain/release --------------------------------------------

void *Collector_retain_(Collector *self, void *v) {
    List_append_(self->retainedValues, v);
    CollectorMarker_removeIfNeededAndInsertAfter_(v, self->grays);
    return v;
}

void Collector_stopRetaining_(Collector *self, void *v) {
    List_removeLast_(self->retainedValues, v);
}

void Collector_removeAllRetainedValues(Collector *self) {
    List_removeAll(self->retainedValues);
}

// pausing -------------------------------------------------------------------

int Collector_isPaused(Collector *self) { return (self->pauseCount != 0); }

void Collector_pushPause(Collector *self) { self->pauseCount++; }

void Collector_popPause(Collector *self) {
    assert(self->pauseCount > 0);

    self->pauseCount--;

    // printf("collect newMarkerCount %i\n", self->newMarkerCount);
#ifdef COLLECTOR_USE_NONINCREMENTAL_MARK_SWEEP
    if (self->pauseCount == 0 && self->newMarkerCount > self->allocsPerSweep) {
        // printf("collect newMarkerCount %i\n", self->newMarkerCount);
        if (self->debugOn) {
            printf("\n  newMarkerCount %i\n", (int)self->newMarkerCount);
        }

        self->newMarkerCount = 0;
        Collector_collect(self);
    }
#else
    if (self->pauseCount == 0 && self->queuedMarks > 1.0) {
        Collector_markPhase(self);
    }
#endif
}

#ifdef COLLECTOR_USE_NONINCREMENTAL_MARK_SWEEP

void Collector_setAllocsPerSweep_(Collector *self, int n) {
    self->allocsPerSweep = n;
}

float Collector_allocsPerSweep(Collector *self) { return self->allocsPerSweep; }

#endif

// adding ------------------------------------------------

CollectorMarker *Collector_newMarker(Collector *self) {
    CollectorMarker *m;
    BEGIN_TIMER
    m = self->freed->next;

    if (m->color != self->freed->color) {
        m = CollectorMarker_new();
        // printf("new marker\n");
    } else {
        // printf("using recycled marker\n");
    }

    self->allocated++;

    Collector_addValue_(self, m);
    END_TIMER
    return m;
}

void Collector_addValue_(Collector *self, void *v) {
    CollectorMarker_removeIfNeededAndInsertAfter_(v, self->whites);

    self->queuedMarks += self->marksPerAlloc;
#ifdef COLLECTOR_USE_NONINCREMENTAL_MARK_SWEEP
    self->newMarkerCount++;
#endif
    // pauseCount is never zero here...
    /*
    if (self->pauseCount == 0)
    {
            if(self->allocated > self->allocatedSweepLevel)
            {
                    Collector_sweep(self);
            }
            else if (self->queuedMarks > 1.0)
            {
                    Collector_markPhase(self);
            }
    }
    */
}

// collection ------------------------------------------------

void Collector_markGrays(Collector *self) {
    CollectorMarkFunc *markFunc = self->markFunc;

    COLLECTMARKER_FOREACH(self->grays, v,
                          //(*markFunc)(v); Collector_makeBlack_(self, v);
                          if ((*markFunc)(v)) Collector_makeBlack_(self, v);
                          // count ++;
    );
    self->queuedMarks = 0;
}

void Collector_markGraysMax_(Collector *self, size_t max) {
    CollectorMarkFunc *markFunc = self->markFunc;

    if (!max)
        return;

    COLLECTMARKER_FOREACH(self->grays, v,
                          //(*markFunc)(v); Collector_makeBlack_(self, v);
                          if ((*markFunc)(v)) Collector_makeBlack_(self, v);
                          max--; if (0 == max) break;);

    self->queuedMarks = 0;
}

void Collector_sendWillFreeCallbacks(Collector *self) {
    CollectorWillFreeFunc *willFreeFunc = self->willFreeFunc;

    if (willFreeFunc) {
        Collector_pushPause(self); // since the callback may create new objects
        COLLECTMARKER_FOREACH(self->whites, v, (*willFreeFunc)(v));
        Collector_popPause(self);
    }
}

size_t Collector_freeWhites(Collector *self) {
    size_t count = 0;
    CollectorFreeFunc *freeFunc = self->freeFunc;

    COLLECTMARKER_FOREACH(self->whites, v, (*freeFunc)(v);
                          // v->object = 0x0;
                          Collector_makeFree_(self, v); count++;);

    self->allocated -= count;

    return count;
}

// ---------------------

void Collector_initPhase(Collector *self) {
    LIST_FOREACH(self->retainedValues, i, v, Collector_makeGray_(self, v));
}

void Collector_markForTimePeriod_(Collector *self, double seconds) {
    clock_t until = clock() + seconds * CLOCKS_PER_SEC;

    for (;;) {
        if (until < clock())
            return;

        if (CollectorMarker_colorSetIsEmpty(self->grays)) {
            Collector_sweep(self);
            return;
        }

        Collector_markGrays(self);
    }
}

void Collector_markPhase(Collector *self) {
    if (self->allocated > self->allocatedSweepLevel) {
        Collector_sweep(self);
    } else {
        Collector_markGraysMax_(self, self->queuedMarks);
    }

    if (CollectorMarker_isEmpty(self->grays)) {
        Collector_freeWhites(self);
        // Collector_sweepPhase(self);
    }
}

size_t Collector_sweep(Collector *self) {
    size_t freedCount = Collector_sweepPhase(self);

    /*
    if (self->debugOn)
    {
            size_t freedCount2 = Collector_sweepPhase(self);
            return freedCount + freedCount2;
    }
    */

    return freedCount;
}

size_t Collector_sweepPhase(Collector *self) {
    size_t freedCount = 0;

    self->newMarkerCount = 0;

    if (self->debugOn) {
        printf("Collector_sweepPhase()\n");
        // printf("  newMarkerCount %i\n", (int)self->newMarkerCount);
        printf("  allocsPerSweep %i\n", (int)self->allocsPerSweep);
        printf("  allocated %i\n", (int)self->allocated);
        printf("  allocatedSweepLevel %i\n", (int)self->allocatedSweepLevel);
    }

    if (self->markBeforeSweepValue) {
        Collector_makeGray_(self, self->markBeforeSweepValue);
    }

    while (!CollectorMarker_isEmpty(self->grays)) {
        do {
            Collector_markGrays(self);
        } while (!CollectorMarker_isEmpty(self->grays));

        Collector_sendWillFreeCallbacks(self);
    }

    freedCount = Collector_freeWhites(self);
    self->sweepCount++;
    // printf("whites freed %i\n", (int)freedCount);

    {
        CollectorMarker *b = self->blacks;
        self->blacks = self->whites;
        self->whites = b;
    }

    Collector_initPhase(self);

    self->allocatedSweepLevel = self->allocated * self->allocatedStep;
    // printf("allocatedSweepLevel = %i\n", self->allocatedSweepLevel);

    return freedCount;
}

size_t Collector_collect(Collector *self) {
    size_t result;

    BEGIN_TIMER
    if (self->pauseCount) {
        printf("Collector warning: attempt to force collection while pause "
               "count = %i\n",
               self->pauseCount);
        result = 0;
    } else {
        // collect twice to ensure that any now unreachable blacks get collected
        result = Collector_sweepPhase(self) + Collector_sweepPhase(self);
    }
    END_TIMER
    return result;
}

size_t Collector_freeAllValues(Collector *self) {
    size_t count = 0;
    CollectorFreeFunc *freeFunc = self->freeFunc;
    COLLECTOR_FOREACH(self, v, (*freeFunc)(v); CollectorMarker_free(v);
                      count++;);
    self->allocated -= count;
    COLLECTMARKER_FOREACH(self->freed, v, CollectorMarker_free(v); count++;);
    return count;
}

// helpers
// ----------------------------------------------------------------------------

void Collector_show(Collector *self) {
    printf("black: %i\n", (int)CollectorMarker_count(self->blacks));
    printf("gray:  %i\n", (int)CollectorMarker_count(self->grays));
    printf("white: %i\n", (int)CollectorMarker_count(self->whites));
}

char *Collector_colorNameFor_(Collector *self, void *v) {
    if (self->whites->color == MARKER(v)->color)
        return "white";
    if (self->grays->color == MARKER(v)->color)
        return "gray";
    if (self->blacks->color == MARKER(v)->color)
        return "black";
    return "off-white";
}

double Collector_timeUsed(Collector *self) {
    return (double)self->clocksUsed / (double)CLOCKS_PER_SEC;
}
