//metadoc Box copyright Steve Dekorte 2002
//metadoc Box license BSD revised

#ifndef IOBOX_DEFINED
#define IOBOX_DEFINED 1

#include "IoBoxApi.h"
#include "IoObject.h"
#include "IoSeq.h"

#define ISBOX(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoBox_rawClone)
IOBOX_API void *IoMessage_locals_boxArgAt_(IoMessage *self, void *locals, int n);

typedef IoObject IoBox;

typedef struct
{
	IoSeq *origin;
	IoSeq *size;
} IoBoxData;

IOBOX_API const char *IoBox_protoId(void);

IOBOX_API IoBox *IoBox_rawClone(IoBox *self);
IOBOX_API IoBox *IoBox_proto(void *state);
IOBOX_API IoBox *IoBox_new(void *state);
IOBOX_API void IoBox_rawCopy(IoBox *self, IoBox *other);
IOBOX_API void IoBox_rawSet(IoBox *self, NUM_TYPE x, NUM_TYPE y, NUM_TYPE z, NUM_TYPE w, NUM_TYPE h, NUM_TYPE d);
IOBOX_API IoBox *IoBox_newSet(void *state, NUM_TYPE x, NUM_TYPE y, NUM_TYPE z, NUM_TYPE w, NUM_TYPE h, NUM_TYPE d);

IOBOX_API IoSeq *IoBox_rawOrigin(IoBox *self);
IOBOX_API IoSeq *IoBox_rawSize(IoBox *self);

IOBOX_API void IoBox_free(IoBox *self);
IOBOX_API void IoBox_mark(IoBox *self);

// -----------------------------------------------------------

IOBOX_API IoObject *IoBox_origin(IoBox *self, IoObject *locals, IoMessage *m);
IOBOX_API IoObject *IoBox_size(IoBox *self, IoObject *locals, IoMessage *m);

IOBOX_API IoObject *IoBox_width(IoBox *self, IoObject *locals, IoMessage *m);
IOBOX_API IoObject *IoBox_height(IoBox *self, IoObject *locals, IoMessage *m);
IOBOX_API IoObject *IoBox_depth(IoBox *self, IoObject *locals, IoMessage *m);

IOBOX_API IoObject *IoBox_set(IoBox *self, IoObject *locals, IoMessage *m);
IOBOX_API IoObject *IoBox_setOrigin(IoBox *self, IoObject *locals, IoMessage *m);
IOBOX_API IoObject *IoBox_setSize(IoBox *self, IoObject *locals, IoMessage *m);

IOBOX_API void IoBox_rawUnion(IoBox *self, IoBox *other);
IOBOX_API IoObject *IoBox_Union(IoBox *self, IoObject *locals, IoMessage *m);
IOBOX_API IoObject *IoBox_containsPoint(IoBox *self, IoObject *locals, IoMessage *m);
IOBOX_API IoObject *IoBox_intersectsBox(IoBox *self, IoObject *locals, IoMessage *m);

IOBOX_API IoObject *IoBox_print(IoBox *self, IoObject *locals, IoMessage *m);

/*
IOBOX_API IoObject *IoBox_asString(IoBox *self, IoObject *locals, IoMessage *m);
IOBOX_API IoObject *IoBox_Min(IoBox *self, IoObject *locals, IoMessage *m);
IOBOX_API IoObject *IoBox_Max(IoBox *self, IoObject *locals, IoMessage *m);
*/

#endif
