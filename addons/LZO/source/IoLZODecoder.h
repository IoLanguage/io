//metadoc LZODecoder copyright Steve Dekorte 2002

#ifndef IoLZODecoder_DEFINED
#define IoLZODecoder_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoList.h"

#define USE_MINILZO 1

#if defined USE_MINILZO
#include "minilzo.h"
#elif defined USE_LIBLZO
#include <lzo/lzo1x.h>
#endif

#include <ctype.h>

#define ISZLODECODER(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoLZODecoder_rawClone)

typedef IoObject IoLZODecoder;

typedef struct
{
	lzo_align_t __LZO_MMODEL wrkmem [ ((LZO1X_1_MEM_COMPRESS) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ];
	int isDone;
} IoLZOData;

IoTag *IoLZODecoder_newTag(void *state);
IoLZODecoder *IoLZODecoder_proto(void *state);
IoLZODecoder *IoLZODecoder_rawClone(IoLZODecoder *self);
void IoLZODecoder_free(IoLZODecoder *self);

IoObject *IoLZODecoder_beginProcessing(IoLZODecoder *self, IoObject *locals, IoMessage *m);
IoObject *IoLZODecoder_process(IoLZODecoder *self, IoObject *locals, IoMessage *m);
IoObject *IoLZODecoder_endProcessing(IoLZODecoder *self, IoObject *locals, IoMessage *m);

#endif
