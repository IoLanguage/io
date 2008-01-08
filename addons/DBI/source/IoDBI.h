/*
docCopyright("Jeremy Cowgar", 2006)
docLicense("BSD revised")
*/

#ifndef IODBI_DEFINED
#define IODBI_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoList.h"
#include <dbi/dbi.h>

void ReportDBIError(dbi_conn conn, void *state, IoMessage *m);

#define ISDBI(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoDBI_rawClone)

typedef IoObject IoDBI;

typedef struct
{
	int didInit;
	int driverCount;
} IoDBIData;

IoDBI *IoDBI_rawClone(IoDBI *self);
IoDBI *IoDBI_proto(void *state);
void IoDBI_free(IoDBI *self);
void IoDBI_mark(IoDBI *self);

/* ---------------------------------------------------------------- */

IoObject *IoDBI_init(IoDBI *self, IoObject *locals, IoMessage *m);
IoObject *IoDBI_initWithDriversPath(IoDBI *self, IoObject *locals,
			IoMessage *m);
IoObject *IoDBI_version(IoDBI *self, IoObject *locals, IoMessage *m);
IoObject *IoDBI_drivers(IoDBI *self, IoObject *locals, IoMessage *m);
IoObject *IoDBI_with(IoDBI *self, IoObject *locals, IoMessage *m);
#endif
