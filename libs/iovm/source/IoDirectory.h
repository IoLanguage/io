/*
docCopyright("Steve Dekorte", 2002)
*/

#ifndef IODIRECTORY_DEFINED
#define IODIRECTORY_DEFINED 1

#include "IoVMApi.h"

#include "IoObject.h"
#include "IoSeq.h"

#define ISDIRECTORY(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoDirectory_rawClone)

typedef IoObject IoDirectory;

typedef struct
{
    IoSymbol *path;
} IoDirectoryData;

IOVM_API IoDirectory *IoDirectory_rawClone(IoDirectory *self);
IOVM_API IoDirectory *IoDirectory_proto(void *state);
IOVM_API IoDirectory *IoDirectory_new(void *state);
IOVM_API IoDirectory *IoDirectory_newWithPath_(void *state, IoSymbol *path);
IOVM_API IoDirectory *IoDirectory_cloneWithPath_(IoDirectory *self, IoSymbol *path);

IOVM_API void IoDirectory_free(IoDirectory *self);
IOVM_API void IoDirectory_mark(IoDirectory *self);

// -----------------------------------------------------------

IOVM_API IoObject *IoDirectory_path(IoDirectory *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoDirectory_setPath(IoDirectory *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoDirectory_name(IoDirectory *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoDirectory_at(IoDirectory *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoDirectory_size(IoDirectory *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoDirectory_exists(IoDirectory *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoDirectory_items(IoDirectory *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoDirectory_create(IoDirectory *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoDirectory_createSubdirectory(IoDirectory *self, IoObject *locals, IoMessage *m);

IOVM_API UArray *IoDirectory_CurrentWorkingDirectoryAsUArray(void);
IOVM_API int IoDirectory_SetCurrentWorkingDirectory(const char *path);

IOVM_API IoObject *IoDirectory_currentWorkingDirectory(IoDirectory *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoDirectory_setCurrentWorkingDirectory(IoDirectory *self, IoObject *locals, IoMessage *m);

#endif
