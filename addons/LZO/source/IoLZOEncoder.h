
//metadoc LZOEncoder copyright Steve Dekorte 2002

#ifndef IoLZOEncoder_DEFINED
#define IoLZOEncoder_DEFINED 1

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

#define ISZLOENCODER(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoLZOEncoder_rawClone)

typedef IoObject IoLZOEncoder;

typedef struct
{
	lzo_align_t __LZO_MMODEL wrkmem [ ((LZO1X_1_MEM_COMPRESS) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ];
	int isDone;
} IoLZOData;

IoTag *IoLZOEncoder_newTag(void *state);
IoLZOEncoder *IoLZOEncoder_proto(void *state);
IoLZOEncoder *IoLZOEncoder_rawClone(IoLZOEncoder *self);
void IoLZOEncoder_free(IoLZOEncoder *self);

IoObject *IoLZOEncoder_beginProcessing(IoLZOEncoder *self, IoObject *locals, IoMessage *m);
IoObject *IoLZOEncoder_process(IoLZOEncoder *self, IoObject *locals, IoMessage *m);
IoObject *IoLZOEncoder_endProcessing(IoLZOEncoder *self, IoObject *locals, IoMessage *m);

#endif
