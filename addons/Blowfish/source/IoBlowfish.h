//metadoc Blowfish copyright Steve Dekorte 2002

#ifndef IoBlowfish_DEFINED
#define IoBlowfish_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoList.h"
#include "blowfish.h"

#define ISBLOWFISH(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoBlowfish_rawClone)

typedef IoObject IoBlowfish;

typedef struct
{
	blowfish_ctx context;
	int isEncrypting;
	int isDone;
} IoBlowfishData;

IoTag *IoBlowfish_newTag(void *state);
IoBlowfish *IoBlowfish_proto(void *state);
IoBlowfish *IoBlowfish_rawClone(IoBlowfish *self);
void IoBlowfish_free(IoBlowfish *self);

IoObject *IoBlowfish_setIsEncrypting(IoBlowfish *self, IoObject *locals, IoMessage *m);
IoObject *IoBlowfish_beginProcessing(IoBlowfish *self, IoObject *locals, IoMessage *m);
IoObject *IoBlowfish_process(IoBlowfish *self, IoObject *locals, IoMessage *m);
IoObject *IoBlowfish_endProcessing(IoBlowfish *self, IoObject *locals, IoMessage *m);

#endif
