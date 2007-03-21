/*
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IOBOX_DEFINED
#define IOBOX_DEFINED 1

#include "IoVectorApi.h"
#include "IoObject.h"
#include "IoSeq.h"

#define ISBOX(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoBox_rawClone)
IOVECTOR_API void *IoMessage_locals_boxArgAt_(IoMessage *self, void *locals, int n);

typedef IoObject IoBox;

typedef struct
{
    IoSeq *origin;
    IoSeq *size;
} IoBoxData;

IOVECTOR_API IoBox *IoBox_rawClone(IoBox *self);
IOVECTOR_API IoBox *IoBox_proto(void *state);
IOVECTOR_API IoBox *IoBox_new(void *state);
IOVECTOR_API void IoBox_rawCopy(IoBox *self, IoBox *other);
IOVECTOR_API void IoBox_rawSet(IoBox *self, NUM_TYPE x, NUM_TYPE y, NUM_TYPE z, NUM_TYPE w, NUM_TYPE h, NUM_TYPE d);
IOVECTOR_API IoBox *IoBox_newSet(void *state, NUM_TYPE x, NUM_TYPE y, NUM_TYPE z, NUM_TYPE w, NUM_TYPE h, NUM_TYPE d);

IOVECTOR_API IoSeq *IoBox_rawOrigin(IoBox *self);
IOVECTOR_API IoSeq *IoBox_rawSize(IoBox *self);

IOVECTOR_API void IoBox_free(IoBox *self);
IOVECTOR_API void IoBox_mark(IoBox *self);

// ----------------------------------------------------------- 

IOVECTOR_API IoObject *IoBox_origin(IoBox *self, IoObject *locals, IoMessage *m);
IOVECTOR_API IoObject *IoBox_size(IoBox *self, IoObject *locals, IoMessage *m);

IOVECTOR_API IoObject *IoBox_width(IoBox *self, IoObject *locals, IoMessage *m);
IOVECTOR_API IoObject *IoBox_height(IoBox *self, IoObject *locals, IoMessage *m);
IOVECTOR_API IoObject *IoBox_depth(IoBox *self, IoObject *locals, IoMessage *m);

IOVECTOR_API IoObject *IoBox_set(IoBox *self, IoObject *locals, IoMessage *m);
IOVECTOR_API IoObject *IoBox_setOrigin(IoBox *self, IoObject *locals, IoMessage *m);
IOVECTOR_API IoObject *IoBox_setSize(IoBox *self, IoObject *locals, IoMessage *m);

IOVECTOR_API void IoBox_rawUnion(IoBox *self, IoBox *other);
IOVECTOR_API IoObject *IoBox_Union(IoBox *self, IoObject *locals, IoMessage *m);
IOVECTOR_API IoObject *IoBox_containsPoint(IoBox *self, IoObject *locals, IoMessage *m);
IOVECTOR_API IoObject *IoBox_intersectsBox(IoBox *self, IoObject *locals, IoMessage *m);

IOVECTOR_API IoObject *IoBox_print(IoBox *self, IoObject *locals, IoMessage *m);

/*
IOVECTOR_API IoObject *IoBox_asString(IoBox *self, IoObject *locals, IoMessage *m);
IOVECTOR_API IoObject *IoBox_Min(IoBox *self, IoObject *locals, IoMessage *m);
IOVECTOR_API IoObject *IoBox_Max(IoBox *self, IoObject *locals, IoMessage *m);
*/

#endif
