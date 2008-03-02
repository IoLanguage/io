//metadoc TokyoCabinetPrefixCursor copyright Steve Dekorte 2002
//metadoc TokyoCabinetPrefixCursor license BSD revised

#ifndef IoTokyoCabinetPrefixCursor_DEFINED
#define IoTokyoCabinetPrefixCursor_DEFINED 1

#include "Common.h"
#include "IoState.h"
#include "IoObject.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISTokyoCabinetPrefixCursor(self) \
	IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoTokyoCabinetPrefixCursor_rawClone)

typedef IoObject IoTokyoCabinetPrefixCursor;

IoTokyoCabinetPrefixCursor *IoTokyoCabinetPrefixCursor_proto(void *state);
IoTokyoCabinetPrefixCursor *IoTokyoCabinetPrefixCursor_rawClone(IoObject *self);
IoTokyoCabinetPrefixCursor *IoTokyoCabinetPrefixCursor_new(void *state);
void IoTokyoCabinetPrefixCursor_free(IoObject *self);
//void IoTokyoCabinetPrefixCursor_mark(IoObject *self);

// --------------------------------------------------------

IoTokyoCabinetPrefixCursor *IoTokyoCabinetPrefixCursor_newWithDB_(void *state, void *bdb);

IoObject *IoTokyoCabinetPrefixCursor_close(IoObject *self, IoObject *locals, IoMessage *m);

// cursor

IoObject *IoTokyoCabinetPrefixCursor_first(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinetPrefixCursor_last(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinetPrefixCursor_previous(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinetPrefixCursor_next(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinetPrefixCursor_jump(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinetPrefixCursor_key(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinetPrefixCursor_value(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinetPrefixCursor_put(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinetPrefixCursor_remove(IoObject *self, IoObject *locals, IoMessage *m);

#ifdef __cplusplus
}
#endif
#endif
