//metadoc copyright Chris Double 2009

#ifndef IOVorbisInfo_DEFINED
#define IOVorbisInfo_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

#define ISVORBISINFO(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoVorbisInfo_rawClone)

typedef IoObject IoVorbisInfo;

IoObject *IoMessage_locals_vorbisInfoArgAt_(IoMessage *self, IoObject *locals, int n);

IoTag *IoVorbisInfo_newTag(void *state);
IoVorbisInfo *IoVorbisInfo_proto(void *state);
IoVorbisInfo *IoVorbisInfo_rawClone(IoVorbisInfo *self);
void IoVorbisInfo_free(IoVorbisInfo *self);

IoObject *IoVorbisInfo_version(IoVorbisInfo *self, IoObject *locals, IoMessage *m);
IoObject *IoVorbisInfo_channels(IoVorbisInfo *self, IoObject *locals, IoMessage *m);
IoObject *IoVorbisInfo_rate(IoVorbisInfo *self, IoObject *locals, IoMessage *m);

#endif
