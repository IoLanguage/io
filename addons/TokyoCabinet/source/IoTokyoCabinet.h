//metadoc TokyoCabinet copyright Steve Dekorte 2002
//metadoc TokyoCabinet license BSD revised

#ifndef IoTokyoCabinet_DEFINED
#define IoTokyoCabinet_DEFINED 1

#include "Common.h"
#include "IoState.h"
#include "IoObject.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISTokyoCabinet(self) \
	IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoTokyoCabinet_rawClone)

// latest versions on darwin do not use bdb infix
#if defined(__APPLE__) || defined(MACOSX)
#define tcbdbcmplexical tccmplexical 
#define tcbdbcmpdecimal tccmpdecimal
#define tcbdbcmpint32 tccmpint32
#define tcbdbcmpint64 tccmpint64
#endif 


typedef IoObject IoTokyoCabinet;

IoTokyoCabinet *IoTokyoCabinet_proto(void *state);
IoTokyoCabinet *IoTokyoCabinet_rawClone(IoObject *self);
IoTokyoCabinet *IoTokyoCabinet_new(void *state);
void IoTokyoCabinet_free(IoObject *self);
//void IoTokyoCabinet_mark(IoObject *self);

// --------------------------------------------------------

IoObject *IoTokyoCabinet_open(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinet_close(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinet_atPut(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinet_atAppend(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinet_at(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinet_sizeAt(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinet_removeAt(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinet_sync(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinet_size(IoObject *self, IoObject *locals, IoMessage *m);
//IoObject *IoTokyoCabinet_optimize(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinet_path(IoObject *self, IoObject *locals, IoMessage *m);

// transactions

IoObject *IoTokyoCabinet_begin(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinet_commit(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinet_abort(IoObject *self, IoObject *locals, IoMessage *m);

IoObject *IoTokyoCabinet_cursor(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoTokyoCabinet_prefixCursor(IoObject *self, IoObject *locals, IoMessage *m);

#ifdef __cplusplus
}
#endif
#endif
