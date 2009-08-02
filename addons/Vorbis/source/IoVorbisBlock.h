//metadoc copyright Chris Double 2009

#ifndef IOVorbisBlock_DEFINED
#define IOVorbisBlock_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

#define ISVORBISBLOCK(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoVorbisBlock_rawClone)

typedef IoObject IoVorbisBlock;

IoObject *IoMessage_locals_vorbisBlockArgAt_(IoMessage *self, IoObject *locals, int n);

IoTag *IoVorbisBlock_newTag(void *state);
IoVorbisBlock *IoVorbisBlock_proto(void *state);
IoVorbisBlock *IoVorbisBlock_rawClone(IoVorbisBlock *self);
void IoVorbisBlock_free(IoVorbisBlock *self);

IoObject *IoVorbisBlock_setup(IoVorbisBlock *self, IoObject *locals, IoMessage *m);
IoObject *IoVorbisBlock_synthesis(IoVorbisBlock *self, IoObject *locals, IoMessage *m);

#endif
