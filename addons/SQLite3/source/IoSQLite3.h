/*
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IOSQLITE_DEFINED
#define IOSQLITE_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoList.h"
#include <sqlite3.h>

#define ISSQLITE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSQLite3_rawClone)

typedef IoObject IoSQLite3;

typedef struct
{
	IoSymbol *path;
	sqlite3 *db;
	IoList *results;
	double timeoutSeconds;
	int debugOn;
} IoSQLite3Data;

IoSQLite3 *IoSQLite3_rawClone(IoSQLite3 *self);
IoSQLite3 *IoSQLite3_proto(void *state);
IoSQLite3 *IoSQLite3_new(void *state);
IoSQLite3 *IoSQLite3_newWithPath_(void *state, IoSymbol *path);

void IoSQLite3_free(IoSQLite3 *self);
void IoSQLite3_mark(IoSQLite3 *self);

void IoSQLite3_showError(IoSQLite3 *self);
const char *IoSQLite3_error(IoSQLite3 *self);

/* ----------------------------------------------------------- */
IoObject *IoSQLite3_path(IoSQLite3 *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite3_setPath(IoSQLite3 *self, IoObject *locals, IoMessage *m);

IoObject *IoSQLite3_timeoutSeconds(IoSQLite3 *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite3_setTimeoutSeconds(IoSQLite3 *self, IoObject *locals, IoMessage *m);

IoObject *IoSQLite3_open(IoSQLite3 *self, IoObject *locals, IoMessage *m);

IoObject *IoSQLite3_close(IoSQLite3 *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite3_exec(IoSQLite3 *self, IoObject *locals, IoMessage *m);
/*IoObject *IoSQLite3_results(IoSQLite3 *self, IoObject *locals, IoMessage *m);*/
IoObject *IoSQLite3_errorMessage(IoSQLite3 *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite3_version(IoSQLite3 *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite3_changes(IoSQLite3 *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite3_lastInsertRowId(IoSQLite3 *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite3_tableNames(IoSQLite3 *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite3_viewNames(IoSQLite3 *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite3_columnNamesOfTable(IoSQLite3 *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite3_debugOn(IoSQLite3 *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite3_debugOff(IoSQLite3 *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite3_isOpen(IoSQLite3 *self, IoObject *locals, IoMessage *m);
IoObject *IoSQLite3_escapeString(IoSQLite3 *self, IoObject *locals, IoMessage *m);

#endif
