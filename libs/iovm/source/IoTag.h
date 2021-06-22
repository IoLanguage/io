
// metadoc Tag copyright Steve Dekorte 2002
// metadoc Tag license BSD revised

#ifndef IOTAG_DEFINED
#define IOTAG_DEFINED 1

#include "Common.h"
//#include "Stack.h"
#include "BStream.h"

#include "IoVMApi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *(IoTagCloneFunc)(void *);              // self
typedef void(IoTagFreeFunc)(void *);                 // self
typedef void(IoTagMarkFunc)(void *);                 // self
typedef void(IoTagNotificationFunc)(void *, void *); // self, notification
typedef void(IoTagCleanupFunc)(void *);              // self

/*
typedef struct
{
        IoTagCursorFirstFunc *nextFunc;
        IoTagCursorNextFunc *nextFunc;
        IoTagCursorPreviousFunc *nextFunc;
        IoTagCursorJumpFunc *nextFunc;
        IoTagCursorLastFunc *nextFunc;
}
*/

/*
typedef void * (IoTagGetFunc)(void *, void *); // self, symbol -> object or 0x0
typedef void   (IoTagSetFunc)(void *, void *, void *); // self, symbol, object
typedef void   (IoTagGetAfterFunc)(void *, void *); // self, symbol -> object or
0x0

typedef void * (IoTagGetMetaFunc)(void *, void *); // self, symbol -> object or
0x0 typedef void   (IoTagSetMetaFunc)(void *, void *, void *); // self, symbol,
object
*/

typedef void *(IoTagPerformFunc)(void *, void *,
                                 void *); // self, locals, message
typedef void *(
    IoTagActivateFunc)(void *, void *, void *, void *,
                       void *); // self, target, locals, message, slotContext
typedef int(IoTagCompareFunc)(void *, void *); // self and another IoObject

typedef void(IoTagWriteToStreamFunc)(void *, BStream *); // self, store, stream
typedef void *(IoTagAllocFromStreamFunc)(void *,
                                         BStream *);      // self, store, stream
typedef void(IoTagReadFromStreamFunc)(void *, BStream *); // self, store, stream

typedef struct {
    void *state;
    char *name;

    // memory management

    IoTagCloneFunc *cloneFunc;
    IoTagFreeFunc *freeFunc;
    IoTagCleanupFunc *tagCleanupFunc;
    IoTagMarkFunc *markFunc;
    IoTagNotificationFunc *notificationFunc;

    // actions

    // IoTagTouchFunc *touchFunc; // if present, call before type check
    IoTagPerformFunc *performFunc; // lookup and activate, return result
    IoTagActivateFunc
        *activateFunc; // return the receiver or compute and return a value
    IoTagCompareFunc *compareFunc;

    /*
    IoTagSetFunc *setFunc
    IoTagGetFunc *getFunc
    IoTagCursorFunc *cursorFunc
    */

    // persistence

    IoTagWriteToStreamFunc *writeToStreamFunc;
    IoTagAllocFromStreamFunc *allocFromStreamFunc;
    IoTagReadFromStreamFunc *readFromStreamFunc;

    // Stack *recyclableInstances;
    // int maxRecyclableInstances;
    int referenceCount;
} IoTag;

IOVM_API IoTag *IoTag_new(void);
IOVM_API IoTag *IoTag_newWithName_(const char *name);
IOVM_API void IoTag_free(IoTag *self);
IOVM_API int IoTag_reference(IoTag *self);

IOVM_API void IoTag_name_(IoTag *self, const char *name);
IOVM_API const char *IoTag_name(IoTag *self);

IOVM_API void IoTag_mark(IoTag *self);

#include "IoTag_inline.h"

#ifdef __cplusplus
}
#endif
#endif
