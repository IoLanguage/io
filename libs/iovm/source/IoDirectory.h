
// metadoc Directory copyright Steve Dekorte 2002

#ifndef IODIRECTORY_DEFINED
#define IODIRECTORY_DEFINED 1

#include "IoVMApi.h"

#include "IoObject.h"
#include "IoSeq.h"

#define ISDIRECTORY(self)                                                      \
    IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoDirectory_rawClone)

typedef IoObject IoDirectory;

typedef struct {
    IoSymbol *path;
} IoDirectoryData;

IOVM_API IoDirectory *IoDirectory_rawClone(IoDirectory *self);
IOVM_API IoDirectory *IoDirectory_proto(void *state);
IOVM_API IoDirectory *IoDirectory_new(void *state);
IOVM_API IoDirectory *IoDirectory_newWithPath_(void *state, IoSymbol *path);
IOVM_API IoDirectory *IoDirectory_cloneWithPath_(IoDirectory *self,
                                                 IoSymbol *path);

IOVM_API void IoDirectory_free(IoDirectory *self);
IOVM_API void IoDirectory_mark(IoDirectory *self);

// -----------------------------------------------------------

IOVM_API IO_METHOD(IoDirectory, path);
IOVM_API IO_METHOD(IoDirectory, setPath);
IOVM_API IO_METHOD(IoDirectory, name);

IOVM_API IO_METHOD(IoDirectory, at);
IOVM_API IO_METHOD(IoDirectory, size);

IOVM_API IO_METHOD(IoDirectory, exists);
IOVM_API IO_METHOD(IoDirectory, items);
IOVM_API IO_METHOD(IoDirectory, create);
IOVM_API IO_METHOD(IoDirectory, createSubdirectory);

IOVM_API UArray *IoDirectory_CurrentWorkingDirectoryAsUArray(void);
IOVM_API int IoDirectory_SetCurrentWorkingDirectory(const char *path);

IOVM_API IO_METHOD(IoDirectory, currentWorkingDirectory);
IOVM_API IO_METHOD(IoDirectory, setCurrentWorkingDirectory);

#endif
