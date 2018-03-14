/*
 docCopyright("Jeremy Tregunna", 2006)
 docLicense("BSD")
 */

#ifndef IORANGE_DEFINED
#define IORANGE_DEFINED 1

#include "Common.h"
#include "IoObject.h"
#include "IoRangeApi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISRANGE(self) \
	IoObject_hasCloneFunc_((self), (IoTagCloneFunc *)IoRange_rawClone)

typedef IoObject IoRange;

typedef struct
{
	IoObject *start;
	IoObject *curr;
	IoObject *end;
	IoObject *increment;
	IoObject *index;
} IoRangeData;

IoRange *IoRange_rawClone(IoRange *self);
IoRange *IoRange_proto(void *state);
IORANGE_API IoRange *IoRange_new(void *state);

void IoRange_free(IoRange *self);
void IoRange_mark(IoRange *self);

/* ----------------------------------------------------------- */

IoObject *IoRange_first(IoRange *self, IoObject *locals, IoMessage *m);
IoObject *IoRange_last(IoRange *self, IoObject *locals, IoMessage *m);
IoObject *IoRange_next(IoRange *self, IoObject *locals, IoMessage *m);
IoObject *IoRange_previous(IoRange *self, IoObject *locals, IoMessage *m);
IoObject *IoRange_index(IoRange *self, IoObject *locals, IoMessage *m);
IoObject *IoRange_value(IoRange *self, IoObject *locals, IoMessage *m);

IORANGE_API IoRange *IoRange_setRange(IoRange *self, IoObject *locals, IoMessage *m);
IoRange *IoRange_rewind(IoRange *self, IoObject *locals, IoMessage *m);
IoObject *IoRange_foreach(IoRange *self, IoObject *locals, IoMessage *m);
IoObject *IoRange_select(IoRange *self, IoObject *locals, IoMessage *m);
IoObject *IoRange_map(IoRange *self, IoObject *locals, IoMessage *m);

/* ----------------------------------------------------------- */

#include "IoRange_inline.h"

#ifdef __cplusplus
}
#endif

#endif
