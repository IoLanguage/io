//metadoc copyright Steve Dekorte 2002

#ifndef IOZlibDecoder_DEFINED
#define IOZlibDecoder_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include <zlib.h>

#define ISZlibDecoder(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoZlibDecoder_rawClone)

typedef IoObject IoZlibDecoder;

typedef struct
{
	z_stream *strm;
	int isDone;
} IoZlibDecoderData;

IoTag *IoZlibDecoder_newTag(void *state);
IoZlibDecoder *IoZlibDecoder_proto(void *state);
IoZlibDecoder *IoZlibDecoder_rawClone(IoZlibDecoder *self);
void IoZlibDecoder_free(IoZlibDecoder *self);

IoObject *IoZlibDecoder_beginProcessing(IoZlibDecoder *self, IoObject *locals, IoMessage *m);
IoObject *IoZlibDecoder_process(IoZlibDecoder *self, IoObject *locals, IoMessage *m);
IoObject *IoZlibDecoder_endProcessing(IoZlibDecoder *self, IoObject *locals, IoMessage *m);

#endif
