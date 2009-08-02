//metadoc copyright Chris Double 2009

#ifndef IOTheoraComment_DEFINED
#define IOTheoraComment_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

#define ISTHEORACOMMENT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoTheoraComment_rawClone)

typedef IoObject IoTheoraComment;

IoObject *IoMessage_locals_theoraCommentArgAt_(IoMessage *self, IoObject *locals, int n);

IoTag *IoTheoraComment_newTag(void *state);
IoTheoraComment *IoTheoraComment_proto(void *state);
IoTheoraComment *IoTheoraComment_rawClone(IoTheoraComment *self);
void IoTheoraComment_free(IoTheoraComment *self);

IoObject *IoTheoraComment_count(IoTheoraComment *self, IoObject *locals, IoMessage *m);

#endif
