
// metadoc File copyright Steve Dekorte 2002
// metadoc File license BSD revised

#ifndef IOFILE_DEFINED
#define IOFILE_DEFINED 1

#include "Common.h"
#include "IoObject.h"
#include "UArray.h"
#include "IoNumber.h"
#include "IoSeq.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISFILE(self)                                                           \
    IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoFile_rawClone)

typedef IoObject IoFile;

typedef struct {
    FILE *stream;
    IoSymbol *path;
    IoSymbol *mode;
    unsigned char flags;
    void *info; /* reserved for use in OS specific extensions */
} IoFileData;

#define IOFILE_FLAGS_NONE 0x0
#define IOFILE_FLAGS_PIPE 0x1
#define IOFILE_FLAGS_NONBLOCKING 0x2

IOVM_API IoFile *IoFile_proto(void *state);
IOVM_API IoFile *IoFile_rawClone(IoFile *self);
IOVM_API IoFile *IoFile_new(void *state);
IOVM_API IoFile *IoFile_newWithPath_(void *state, IoSymbol *path);
IOVM_API IoFile *IoFile_cloneWithPath_(IoFile *self, IoSymbol *path);
IOVM_API IoFile *IoFile_newWithStream_(void *state, FILE *stream);

void IoFile_free(IoFile *self);
void IoFile_mark(IoFile *self);

void IoFile_justClose(IoFile *self);
int IoFile_justExists(IoFile *self);
int IoFile_create(IoFile *self);

/* ----------------------------------------------------------- */

IO_METHOD(IoFile, descriptor);

IO_METHOD(IoFile, standardInput);
IO_METHOD(IoFile, standardOutput);
IO_METHOD(IoFile, standardError);

IO_METHOD(IoFile, setPath);
IO_METHOD(IoFile, path);
IO_METHOD(IoFile, lastPathComponent);

IO_METHOD(IoFile, mode);

IO_METHOD(IoFile, temporaryFile);

IO_METHOD(IoFile, openForReading);
IO_METHOD(IoFile, openForUpdating);
IO_METHOD(IoFile, openForAppending);

IO_METHOD(IoFile, open);
IO_METHOD(IoFile, reopen);
IO_METHOD(IoFile, popen);
IO_METHOD(IoFile, close);

IO_METHOD(IoFile, flush);
IoObject *IoFile_rawAsString(IoFile *self);
IO_METHOD(IoFile, contents);
IO_METHOD(IoFile, asBuffer);
IO_METHOD(IoFile, exists);
IO_METHOD(IoFile, remove);
IO_METHOD(IoFile, truncateToSize);
IO_METHOD(IoFile, moveTo_);
IO_METHOD(IoFile, write);
IO_METHOD(IoFile, readLine);
IO_METHOD(IoFile, readLines);
IO_METHOD(IoFile, readToBufferLength);
IO_METHOD(IoFile, readStringOfLength_);
IO_METHOD(IoFile, readBufferOfLength_);
IO_METHOD(IoFile, rewind);
IO_METHOD(IoFile, position_);
IO_METHOD(IoFile, position);
IO_METHOD(IoFile, positionAtEnd);
IO_METHOD(IoFile, isAtEnd);

IO_METHOD(IoFile, size);
IO_METHOD(IoFile, isOpen);
IO_METHOD(IoFile, assertOpen);
IO_METHOD(IoFile, assertWrite);

IO_METHOD(IoFile, at);
IO_METHOD(IoFile, atPut);
IO_METHOD(IoFile, foreach);
IO_METHOD(IoFile, foreachLine);

// IoObject *IoFile_rawDo(IoFile *self, IoObject *context);
// IO_METHOD(IoFile, do);

/*
IO_METHOD(IoFile, makeUnbuffered);
IO_METHOD(IoFile, makeLineBuffered);
IO_METHOD(IoFile, makeFullyBuffered);
*/

#ifdef __cplusplus
}
#endif
#endif
