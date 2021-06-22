
// metadoc Tag copyright Steve Dekorte 2002
// metadoc Tag license BSD revised

#define IOTAG_C 1
#include "IoTag.h"
#undef IOTAG_C

#include "IoObject.h"
#include "IoState.h"
#include <string.h>

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

IoTag *IoTag_newWithName_(const char *name) {
    IoTag *self = IoTag_new();
    IoTag_name_(self, name);
    return self;
}

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

int IoTag_reference(IoTag *self) { return ++self->referenceCount; }

void IoTag_name_(IoTag *self, const char *name) {
    self->name = strcpy((char *)io_realloc(self->name, strlen(name) + 1), name);
}

const char *IoTag_name(IoTag *self) { return self->name; }

void IoTag_mark(IoTag *self) {
    /*
    if (Stack_count(self->recyclableInstances))
    {
            Stack_do_(self->recyclableInstances, (StackDoCallback
    *)IoObject_shouldMark);
    }
    */
}
