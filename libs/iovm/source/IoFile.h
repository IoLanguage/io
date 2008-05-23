
//metadoc File copyright Steve Dekorte 2002
//metadoc File license BSD revised

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

#define ISFILE(self) \
	IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoFile_rawClone)

typedef IoObject IoFile;

typedef struct
{
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

//IOVM_API void IoFile_writeToStream_(IoFile *self, BStream *stream);
//IOVM_API void *IoFile_readFromStream_(IoFile *self, BStream *stream);

void IoFile_justClose(IoFile *self);
int IoFile_justExists(IoFile *self);
int IoFile_create(IoFile *self);

/* ----------------------------------------------------------- */

IoObject *IoFile_descriptor(IoFile *self, IoObject *locals, IoMessage *m);

IoObject *IoFile_standardInput(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_standardOutput(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_standardError(IoFile *self, IoObject *locals, IoMessage *m);

IoObject *IoFile_setPath(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_path(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_lastPathComponent(IoFile *self, IoObject *locals, IoMessage *m);

IoObject *IoFile_mode(IoFile *self, IoObject *locals, IoMessage *m);

IoObject *IoFile_temporaryFile(IoFile *self, IoObject *locals, IoMessage *m);

IoObject *IoFile_openForReading(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_openForUpdating(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_openForAppending(IoFile *self, IoObject *locals, IoMessage *m);

IoObject *IoFile_open(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_popen(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_close(IoFile *self, IoObject *locals, IoMessage *m);

IoObject *IoFile_flush(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_rawAsString(IoFile *self);
IoObject *IoFile_contents(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_asBuffer(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_exists(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_remove(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_truncateToSize(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_moveTo_(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_write(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_readLine(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_readLines(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_readToBufferLength(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_readStringOfLength_(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_readBufferOfLength_(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_rewind(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_position_(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_position(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_positionAtEnd(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_isAtEnd(IoFile *self, IoObject *locals, IoMessage *m);

IoObject *IoFile_size(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_isOpen(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_assertOpen(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_assertWrite(IoFile *self, IoObject *locals, IoMessage *m);

IoObject *IoFile_at(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_atPut(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_foreach(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_foreachLine(IoFile *self, IoObject *locals, IoMessage *m);

//IoObject *IoFile_rawDo(IoFile *self, IoObject *context);
//IoObject *IoFile_do(IoFile *self, IoObject *locals, IoMessage *m);

/*
IoObject *IoFile_makeUnbuffered(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_makeLineBuffered(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_makeFullyBuffered(IoFile *self, IoObject *locals, IoMessage *m);
*/

#ifdef __cplusplus
}
#endif
#endif
