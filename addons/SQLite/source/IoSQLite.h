
//metadoc SQLite copyright Steve Dekorte 2002
//metadoc SQLite license BSD revised

#ifndef IOSQLITE_DEFINED
#define IOSQLITE_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoList.h"
#include <sqlite.h>

#define ISSQLITE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSQLite_rawClone)

typedef IoObject IoSQLite;

typedef struct
{
	IoSymbol *path;
	sqlite *db;
	IoList *results;
	char *error;
	double timeoutSeconds;
	unsigned char debugOn;
} IoSQLiteData;

IoSQLite *IoSQLite_rawClone(IoSQLite *self);
IoSQLite *IoSQLite_proto(void *state);
IoSQLite *IoSQLite_new(void *state);
IoSQLite *IoSQLite_newWithPath_(void *state, IoSymbol *path);

void IoSQLite_free(IoSQLite *self);
void IoSQLite_mark(IoSQLite *self);

void IoSQLite_error_(IoSQLite *self, char *error);
char *IoSQLite_error(IoSQLite *self);

/* ----------------------------------------------------------- */
IoObject *IoSQLite_path(IoSQLite *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite_setPath(IoSQLite *self, IoObject *locals, IoMessage *m);

IoObject *IoSQLite_timeoutSeconds(IoSQLite *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite_setTimeoutSeconds(IoSQLite *self, IoObject *locals, IoMessage *m);

IoObject *IoSQLite_open(IoSQLite *self, IoObject *locals, IoMessage *m);

IoObject *IoSQLite_close(IoSQLite *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite_exec(IoSQLite *self, IoObject *locals, IoMessage *m);
/*IoObject *IoSQLite_results(IoSQLite *self, IoObject *locals, IoMessage *m);*/
IoObject *IoSQLite_errorMessage(IoSQLite *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite_version(IoSQLite *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite_changes(IoSQLite *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite_lastInsertRowId(IoSQLite *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite_viewNames(IoSQLite *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite_tableNames(IoSQLite *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite_columnNamesOfTable(IoSQLite *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite_debugOn(IoSQLite *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite_debugOff(IoSQLite *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite_isOpen(IoSQLite *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite_escapeString(IoSQLite *self, IoObject *locals, IoMessage *m);

#endif
