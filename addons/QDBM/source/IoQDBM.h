/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IoQDBM_DEFINED
#define IoQDBM_DEFINED 1

#include "Common.h"
#include "IoState.h"
#include "IoObject.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISQDBM(self) \
	IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoQDBM_rawClone)

typedef IoObject IoQDBM;

IoQDBM *IoQDBM_proto(void *state);
IoQDBM *IoQDBM_rawClone(IoObject *self);
IoQDBM *IoQDBM_new(void *state);
void IoQDBM_free(IoObject *self);
//void IoQDBM_mark(IoObject *self);

// --------------------------------------------------------

IoObject *IoQDBM_open(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_close(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_atPut(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_at(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_sizeAt(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_removeAt(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_sync(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_size(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_optimize(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_name(IoObject *self, IoObject *locals, IoMessage *m);

// transactions

IoObject *IoQDBM_begin(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_commit(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_abort(IoObject *self, IoObject *locals, IoMessage *m);

// cursor

IoObject *IoQDBM_cursorFirst(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_cursorLast(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_cursorPrevious(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_cursorNext(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_cursorJumpForward(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_cursorJumpBackward(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_cursorKey(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_cursorValue(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_cursorPut(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoQDBM_cursorRemove(IoObject *self, IoObject *locals, IoMessage *m);

#ifdef __cplusplus
}
#endif
#endif
