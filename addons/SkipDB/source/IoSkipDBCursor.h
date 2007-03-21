/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IoSkipDBCursor_DEFINED
#define IoSkipDBCursor_DEFINED 1

#include "Common.h"
#include "IoState.h"
#include "IoObject.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISSKIPDBCURSOR(self) \
  IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSkipDBCursor_rawClone)

typedef IoObject IoSkipDBCursor;

IoSkipDBCursor *IoSkipDBCursor_proto(void *state);
IoSkipDBCursor *IoSkipDBCursor_rawClone(IoObject *self);
IoSkipDBCursor *IoSkipDBCursor_new(void *state);
IoSkipDBCursor *IoSkipDBCursor_newWithSDBCursor(void *state, SkipDBCursor *cursor);
void IoSkipDBCursor_free(IoObject *self);
void IoSkipDBCursor_mark(IoObject *self);

//void IoSkipDBCursor_writeToStream_(IoObject *self, BStream *stream);
//void IoSkipDBCursor_readFromStream_(IoObject *self, BStream *stream);

// -------------------------------------------------------- 

IoObject *IoSkipDBCursor_goto(IoObject *self, IoObject *locals, IoMessage *m);

IoObject *IoSkipDBCursor_first(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSkipDBCursor_last(IoObject *self, IoObject *locals, IoMessage *m);

IoObject *IoSkipDBCursor_next(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSkipDBCursor_previous(IoObject *self, IoObject *locals, IoMessage *m);

IoObject *IoSkipDBCursor_key(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSkipDBCursor_value(IoObject *self, IoObject *locals, IoMessage *m);


#ifdef __cplusplus
}
#endif
#endif
