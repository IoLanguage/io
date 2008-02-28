//metadoc TokyoCabinetCursor copyright Steve Dekorte 2002
//metadoc TokyoCabinetCursor license BSD revised

#ifndef IoTokyoCabinetCursor_DEFINED
#define IoTokyoCabinetCursor_DEFINED 1

#include "Common.h"
#include "IoState.h"
#include "IoObject.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISTokyoCabinetCursor(self) \
	IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoTokyoCabinetCursor_rawClone)

typedef IoObject IoTokyoCabinetCursor;

IoTokyoCabinetCursor *IoTokyoCabinetCursor_proto(void *state);
IoTokyoCabinetCursor *IoTokyoCabinetCursor_rawClone(IoObject *self);
IoTokyoCabinetCursor *IoTokyoCabinetCursor_new(void *state);
void IoTokyoCabinetCursor_free(IoObject *self);
//void IoTokyoCabinetCursor_mark(IoObject *self);

// --------------------------------------------------------

IoTokyoCabinetCursor *IoTokyoCabinetCursor_newWithDB_(void *state, void *bdb);

IoObject *IoTokyoCabinetCursor_open(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinetCursor_close(IoObject *self, IoObject *locals, IoMessage *m);

// cursor

IoObject *IoTokyoCabinetCursor_first(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinetCursor_last(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinetCursor_previous(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinetCursor_next(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinetCursor_jump(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinetCursor_key(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinetCursor_value(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinetCursor_put(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinetCursor_remove(IoObject *self, IoObject *locals, IoMessage *m);

#ifdef __cplusplus
}
#endif
#endif
