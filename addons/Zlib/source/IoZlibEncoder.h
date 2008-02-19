//metadoc copyright Steve Dekorte 2002


#ifndef IOZlibEncoder_DEFINED
#define IOZlibEncoder_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include <zlib.h>

#define ISZlibEncoder(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoZlibEncoder_rawClone)

typedef IoObject IoZlibEncoder;

typedef struct
{
	z_stream *strm;
	int level;
	int isDone;
} IoZlibEncoderData;

IoTag *IoZlibEncoder_newTag(void *state);
IoZlibEncoder *IoZlibEncoder_proto(void *state);
IoZlibEncoder *IoZlibEncoder_rawClone(IoZlibEncoder *self);
void IoZlibEncoder_free(IoZlibEncoder *self);

IoObject *IoZlibEncoder_beginProcessing(IoZlibEncoder *self, IoObject *locals, IoMessage *m);
IoObject *IoZlibEncoder_process(IoZlibEncoder *self, IoObject *locals, IoMessage *m);
IoObject *IoZlibEncoder_endProcessing(IoZlibEncoder *self, IoObject *locals, IoMessage *m);

#endif
