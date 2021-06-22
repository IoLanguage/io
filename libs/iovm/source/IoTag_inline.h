// metadoc Tag copyright Steve Dekorte 2002
// metadoc Tag license BSD revised

#ifdef IOTAG_C
#define IO_IN_C_FILE
#endif
#include "Common_inline.h"
#ifdef IO_DECLARE_INLINES

// state

IOINLINE void IoTag_state_(IoTag *self, void *state) { self->state = state; }

IOINLINE void *IoTag_state(IoTag *self) { return self->state; }

// activate

IOINLINE void IoTag_activateFunc_(IoTag *self, IoTagActivateFunc *func) {
    self->activateFunc = func;
}

IOINLINE IoTagActivateFunc *IoTag_activateFunc(IoTag *self) {
    return self->activateFunc;
}

// clone

IOINLINE void IoTag_cloneFunc_(IoTag *self, IoTagCloneFunc *func) {
    self->cloneFunc = func;
}

IOINLINE IoTagCloneFunc *IoTag_cloneFunc(IoTag *self) {
    return self->cloneFunc;
}

// cleanup

IOINLINE void IoTag_cleanupFunc_(IoTag *self, IoTagFreeFunc *func) {
    self->tagCleanupFunc = func;
}

IOINLINE IoTagCleanupFunc *IoTag_cleanupFunc(IoTag *self) {
    return self->tagCleanupFunc;
}

// io_free

IOINLINE void IoTag_freeFunc_(IoTag *self, IoTagFreeFunc *func) {
    if (func == free) {
        printf("IoTag_freeFunc_ called free\n");
        exit(-1);
    }

    self->freeFunc = func;
}

IOINLINE IoTagFreeFunc *IoTag_freeFunc(IoTag *self) { return self->freeFunc; }

// mark

IOINLINE void IoTag_markFunc_(IoTag *self, IoTagMarkFunc *func) {
    self->markFunc = func;
}

IOINLINE IoTagMarkFunc *IoTag_markFunc(IoTag *self) { return self->markFunc; }

// compare

IOINLINE void IoTag_compareFunc_(IoTag *self, IoTagCompareFunc *func) {
    self->compareFunc = func;
}

IOINLINE IoTagCompareFunc *IoTag_compareFunc(IoTag *self) {
    return self->compareFunc;
}

// stream write

IOINLINE void IoTag_writeToStreamFunc_(IoTag *self,
                                       IoTagWriteToStreamFunc *func) {
    self->writeToStreamFunc = func;
}

IOINLINE IoTagWriteToStreamFunc *IoTag_writeToStreamFunc(IoTag *self) {
    return self->writeToStreamFunc;
}

// stream alloc

IOINLINE void IoTag_allocFromStreamFunc_(IoTag *self,
                                         IoTagAllocFromStreamFunc *func) {
    self->allocFromStreamFunc = func;
}

IOINLINE IoTagAllocFromStreamFunc *IoTag_allocFromStreamFunc(IoTag *self) {
    return self->allocFromStreamFunc;
}

// stream alloc

IOINLINE void IoTag_readFromStreamFunc_(IoTag *self,
                                        IoTagReadFromStreamFunc *func) {
    self->readFromStreamFunc = func;
}

IOINLINE IoTagReadFromStreamFunc *IoTag_readFromStreamFunc(IoTag *self) {
    return self->readFromStreamFunc;
}

// notification

IOINLINE void IoTag_notificationFunc_(IoTag *self,
                                      IoTagNotificationFunc *func) {
    self->notificationFunc = func;
}

IOINLINE IoTagNotificationFunc *IoTag_notificationFunc(IoTag *self) {
    return self->notificationFunc;
}

// perform

IOINLINE void IoTag_performFunc_(IoTag *self, IoTagPerformFunc *func) {
    self->performFunc = func;
}

IOINLINE IoTagPerformFunc *IoTag_performFunc(IoTag *self) {
    return self->performFunc;
}

#undef IO_IN_C_FILE
#endif
