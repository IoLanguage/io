/*   copyright: Steve Dekorte, 2002
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IoLibSndFile_DEFINED
#define IoLibSndFile_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define __int64 int64_t

#include <sndfile.h>

typedef IoObject IoLibSndFile;

typedef struct
{
	int isRunning;
	IoSeq *outputBuffer;
	SNDFILE *sndfile;
	SF_INFO *sfinfo;
} IoLibSndFileData;

IoLibSndFile *IoLibSndFile_proto(void *state);
IoLibSndFile *IoLibSndFile_new(void *state);
IoLibSndFile *IoLibSndFile_rawClone(IoLibSndFile *self);
void IoLibSndFile_mark(IoLibSndFile *self);
void IoLibSndFile_free(IoLibSndFile *self);

// -----------------------------------------------------------

IoObject *IoLibSndFile_outputBuffer(IoLibSndFile *self, IoObject *locals, IoMessage *m);

IoObject *IoLibSndFile_openForReading(IoLibSndFile *self, IoObject *locals, IoMessage *m);
IoObject *IoLibSndFile_openForWriting(IoLibSndFile *self, IoObject *locals, IoMessage *m);
IoObject *IoLibSndFile_close(IoLibSndFile *self, IoObject *locals, IoMessage *m);

IoObject *IoLibSndFile_formatNames(IoLibSndFile *self, IoObject *locals, IoMessage *m);

IoObject *IoLibSndFile_read(IoLibSndFile *self, IoObject *locals, IoMessage *m);
IoObject *IoLibSndFile_write(IoLibSndFile *self, IoObject *locals, IoMessage *m);

#endif
