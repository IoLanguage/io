/*
docCopyright("Min-hee Hong", 2007)
docLicense("MIT License")
*/

#ifndef IOMYSQL_DEFINED
#define IOMYSQL_DEFINED 1

#include "IoObject.h"
#include <mysql.h>

#define ISMYSQL(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoMySQL_rawClone)

typedef unsigned bool;

typedef struct {
	MYSQL connection;
	bool connected;
} IoMySQLData;

IoObject* IoMySQL_rawClone(IoObject* self);
IoObject* IoMySQL_proto(void* state);
IoObject* IoMySQL_new(void* state);

void IoMySQL_free(IoObject* self);
void IoMySQL_mark(IoObject* self);

/* ----------------------------------------------------------- */

IoObject* IoMySQL_establish(IoObject* self, IoObject* locals, IoMessage* m);
IoObject* IoMySQL_connect(IoObject* self, IoObject* locals, IoMessage* m);
IoObject* IoMySQL_connected(IoObject* self, IoObject* locals, IoMessage* m);
IoObject* IoMySQL_close(IoObject* self, IoObject* locals, IoMessage* m);
IoObject* IoMySQL_query(IoObject* self, IoObject* locals, IoMessage* m);
IoObject* IoMySQL_lastInsertRowId(IoObject* self, IoObject* locals, IoMessage* m);

#endif
