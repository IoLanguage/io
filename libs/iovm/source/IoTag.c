
// metadoc Tag copyright Steve Dekorte 2002
// metadoc Tag license BSD revised

/*cmetadoc Tag description
Per-type vtable shared by every IoObject of a given primitive class.
An IoTag carries the type's name, a set of function pointers (clone,
free, mark, activate, compare, perform, write/read, notification,
cleanup) and is reference-counted across the proto and all its clones.
Each IoObject_new... constructor attaches exactly one tag (installed
by the corresponding IoXxx_newTag helper) and the collector calls
markFunc / freeFunc through the tag rather than switching on type.
Keeping the dispatch here lets IoObject stay a fixed-size marker
regardless of how many primitive subtypes the VM defines.
*/

#define IOTAG_C 1
#include "IoTag.h"
#undef IOTAG_C

#include "IoObject.h"
#include "IoState.h"
#include <string.h>

/*cdoc Tag IoTag_new()
Allocates a zeroed tag with referenceCount 1 and a default performFunc.
When IOMESSAGE_INLINE_PERFORM is defined the performFunc is left NULL
so IoMessage can inline the dispatch; otherwise it points at
IoObject_perform. All other slots (clone/free/mark/activate/compare/
write/read/notification/cleanup) start NULL and must be filled in by
the caller before the tag is attached to any object.
*/
IoTag *IoTag_new(void) {
    IoTag *self = (IoTag *)io_calloc(1, sizeof(IoTag));
#ifdef IOMESSAGE_INLINE_PERFORM
    self->performFunc = NULL;
#else
    self->performFunc = (IoTagPerformFunc *)IoObject_perform;
#endif

    self->referenceCount = 1;
    // self->recyclableInstances = Stack_new();
    // self->maxRecyclableInstances = 10000;
    return self;
}

/*cdoc Tag IoTag_newWithName_(name)
Convenience constructor used by every IoXxx_newTag helper: allocate
a fresh tag, then set its name. The name is duplicated into the tag's
own heap buffer (see IoTag_name_) so the caller's string literal need
not remain live.
*/
IoTag *IoTag_newWithName_(const char *name) {
    IoTag *self = IoTag_new();
    IoTag_name_(self, name);
    return self;
}

/*cdoc Tag IoTag_free(self)
Reference-counted release. Decrements referenceCount and returns early
while any object still holds the tag; on the final release it invokes
the optional tagCleanupFunc (for per-type bookkeeping like freeing
interned pools), frees the duplicated name, and io_frees the tag
itself. Called by IoObject's free path through each primitive's tag
registration.
*/
void IoTag_free(IoTag *self) {
    // printf("io_free tag %p\n", (void *)self);
    // printf("%s\n", self->name ? self->name : "NULL");
    if (--self->referenceCount > 0) {
        return;
    }

    if (self->tagCleanupFunc) {
        (self->tagCleanupFunc)(self);
    }

    if (self->name) {
        io_free(self->name);
        self->name = NULL;
    }

    // Stack_free(self->recyclableInstances);
    io_free(self);
}

/*cdoc Tag IoTag_reference(self)
Bumps referenceCount. Used when the same tag is shared across multiple
owners (e.g. CFunction instances of a particular class) so that
IoTag_free only actually releases on the last drop.
*/
int IoTag_reference(IoTag *self) { return ++self->referenceCount; }

/*cdoc Tag IoTag_name_(self, name)
Copies name into the tag's own heap buffer via io_realloc, so the
caller's string need not remain live. Called by IoTag_newWithName_
and whenever a tag is renamed (rare — mostly during bootstrap).
*/
void IoTag_name_(IoTag *self, const char *name) {
    self->name = strcpy((char *)io_realloc(self->name, strlen(name) + 1), name);
}

const char *IoTag_name(IoTag *self) { return self->name; }

/*cdoc Tag IoTag_mark(self)
Collector hook for per-tag GC roots. Currently a no-op because the
recyclableInstances stack is disabled; kept as a seam so future
per-type caches (e.g. instance freelists) can participate in marking
without touching the collector core.
*/
void IoTag_mark(IoTag *self) {
    /*
    if (Stack_count(self->recyclableInstances))
    {
            Stack_do_(self->recyclableInstances, (StackDoCallback
    *)IoObject_shouldMark);
    }
    */
}
