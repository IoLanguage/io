/*
docCopyright("Jeremy Cowgar", 2006)
docLicense("BSD revised")
*/

#ifndef IODBIRESULT_DEFINED
#define IODBIRESULT_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoList.h"
#include <dbi/dbi.h>

/*
   This *is* defined in dbi/dbi.h but I continue to get " warning:
   implicit declaration of function 'dbi_result_get_int_idx'" which is driving
   me crazy, therefore I added this line in the .h.

   TODO: figure out why it is not working correctly with the dbi/dbi.h
*/

int dbi_result_get_int_idx(dbi_result Result, unsigned int fieldidx);

#define ISDBIRESULT(self) IoObject_hasCloneFunc_(self, \
			(IoTagCloneFunc *)IoDBIResult_rawClone)

typedef IoObject IoDBIResult;

typedef struct
{
	dbi_conn conn;
	dbi_result result;
} IoDBIResultData;

IoDBIResult *IoDBIResult_rawClone(IoDBIResult *self);
IoDBIResult *IoDBIResult_proto(void *state);
void IoDBIResult_free(IoDBIResult *self);
void IoDBIResult_mark(IoDBIResult *self);

/* ---------------------------------------------------------------- */

IoDBIResult *IoDBIResult_new(void *state, dbi_result result);

/* ---------------------------------------------------------------- */

IoObject *IoDBIResult_size(
		IoDBIResult *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIResult_fields(
		IoDBIResult *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIResult_first(
		IoDBIResult *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIResult_previous(
		IoDBIResult *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIResult_next(
		IoDBIResult *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIResult_last(
		IoDBIResult *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIResult_seek(
		IoDBIResult *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIResult_position(
		IoDBIResult *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIResult_at(IoDBIResult *self, IoObject *locals, IoMessage *m);
IoObject *IoDBIResult_populate(IoDBIResult *self, IoObject *locals,
			IoMessage *m);
IoObject *IoDBIResult_foreach(IoDBIResult *self, IoObject *locals,
			IoMessage *m);
IoObject *IoDBIResult_done(IoDBIResult *self, IoObject *locals, IoMessage *m);

#endif
