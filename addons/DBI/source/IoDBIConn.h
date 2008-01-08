/*
docCopyright("Jeremy Cowgar", 2006)
docLicense("BSD revised")
*/

#ifndef IODBICONN_DEFINED
#define IODBICONN_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoList.h"
#include <dbi/dbi.h>

/*
   This *is* defined in dbi/dbi.h but I continue to get " warning:
   implicit declaration of function 'dbi_conn_quote_string'" which is driving
   me crazy, therefore I added this line in the .h.

   TODO: figure out why it is not working correctly with the dbi/dbi.h
   NOTE: see IoDBIResult.h for similar errors
*/

size_t dbi_conn_quote_string(dbi_conn Conn, char **orig);

#define ISDBICONN(self) IoObject_hasCloneFunc_(self, \
			(IoTagCloneFunc *)IoDBIConn_rawClone)

typedef IoObject IoDBIConn;

typedef struct
{
	dbi_conn conn;
} IoDBIConnData;

IoDBIConn *IoDBIConn_rawClone(IoDBIConn *self);
IoDBIConn *IoDBIConn_proto(void *state);
void IoDBIConn_free(IoDBIConn *self);
void IoDBIConn_mark(IoDBIConn *self);

/* ---------------------------------------------------------------- */

IoDBIConn *IoDBIConn_new(void *state, dbi_conn conn);
void IoDBIConn_removeResultObj(IoDBIConn *self, IoObject *r);

/* ---------------------------------------------------------------- */

IoObject *IoDBIConn_driver(IoDBIConn *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIConn_option(IoDBIConn *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIConn_optionPut(IoDBIConn *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIConn_optionClear(IoDBIConn *self, IoObject *locals,
			IoMessage *m);
IoObject *IoDBIConn_options(IoDBIConn *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIConn_optionsClear(IoDBIConn *self, IoObject *locals,
			IoMessage *m);
IoObject *IoDBIConn_quote(IoDBIConn *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIConn_connect(IoDBIConn *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIConn_close(IoDBIConn *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIConn_ping(IoDBIConn *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIConn_query(IoDBIConn *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIConn_execute(IoDBIConn *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIConn_lastSequence(
			IoDBIConn *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIConn_nextSequence(
			IoDBIConn *self, IoObject *locals, IoMessage *m);

#endif
