/*
docCopyright("Jeremy Tregunna", 2005)
docLicense("BSD license")
*/

#ifndef IOSYSLOG_DEFINED
#define IOSYSLOG_DEFINED 1

#include "IoObject.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoMap.h"

#define ISSYSLOG(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSyslog_rawClone)

typedef IoObject IoSyslog;

typedef struct
{
	IoNumber *priority;
	IoNumber *facility;
	IoNumber *options;
	IoMap *priorityMap;
	IoMap *facilityMap;
	IoMap *optionsMap;
	IoMap *maskMap;
	IoList *mask;
	int syslog_mask;
	IoSymbol *ident;
	int syslog_opened;
} IoSyslogData;

IoSyslog *IoSyslog_rawClone(IoSyslog *self);
IoSyslog *IoSyslog_proto(void *state);
IoSyslog *IoSyslog_new(void *state);

void IoSyslog_free(IoSyslog *self);
void IoSyslog_mark(IoSyslog *self);

/* ----------------------------------------------------------- */

IoObject *IoSyslog_open(IoSyslog *self, IoObject *locals, IoMessage *m);
IoObject *IoSyslog_reopen(IoSyslog *self, IoObject *locals, IoMessage *m);
IoObject *IoSyslog_isOpen(IoSyslog *self, IoObject *locals, IoMessage *m);
IoObject *IoSyslog_close(IoSyslog *self, IoObject *locals, IoMessage *m);
IoObject *IoSyslog_identity(IoSyslog *self, IoObject *locals, IoMessage *m);
IoObject *IoSyslog_options(IoSyslog *self, IoObject *locals, IoMessage *m);
IoObject *IoSyslog_optionsMap(IoSyslog *self, IoObject *locals, IoMessage *m);
IoObject *IoSyslog_priority(IoSyslog *self, IoObject *locals, IoMessage *m);
IoObject *IoSyslog_priorityMap(IoSyslog *self, IoObject *locals, IoMessage *m);
IoObject *IoSyslog_facility(IoSyslog *self, IoObject *locals, IoMessage *m);
IoObject *IoSyslog_facilityMap(IoSyslog *self, IoObject *locals, IoMessage *m);
IoObject *IoSyslog_mask(IoSyslog *self, IoObject *locals, IoMessage *m);
IoObject *IoSyslog_maskMap(IoSyslog *self, IoObject *locals, IoMessage *m);
IoObject *IoSyslog_log(IoSyslog *self, IoObject *locals, IoMessage *m);
IoObject *IoSyslog_inspect(IoSyslog *self, IoObject *locals, IoMessage *m);

#endif
