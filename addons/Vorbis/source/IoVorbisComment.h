//metadoc copyright Chris Double 2009

#ifndef IOVorbisComment_DEFINED
#define IOVorbisComment_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

#define ISVORBISCOMMENT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoVorbisComment_rawClone)

typedef IoObject IoVorbisComment;

IoObject *IoMessage_locals_vorbisCommentArgAt_(IoMessage *self, IoObject *locals, int n);

IoTag *IoVorbisComment_newTag(void *state);
IoVorbisComment *IoVorbisComment_proto(void *state);
IoVorbisComment *IoVorbisComment_rawClone(IoVorbisComment *self);
void IoVorbisComment_free(IoVorbisComment *self);

IoObject *IoVorbisComment_count(IoVorbisComment *self, IoObject *locals, IoMessage *m);

#endif
