/*#io
docCopyright("Steve Dekorte", 2007)
docLicense("BSD revised")
*/

#ifndef SymbolDB_DEFINED
#define SymbolDB_DEFINED 1

#ifdef __cplusplus
extern "C" {
#endif
	
#include "Datum.h"
	
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <villa_extras.h>

typedef struct
{
	char *path;
	char *s2iPath;
	char *i2sPath;
	VILLA *i2s;
	VILLA *s2i;
} SymbolDB;

SymbolDB *SymbolDB_new(void);
void SymbolDB_free(SymbolDB *self);

void SymbolDB_setPath_(SymbolDB *self, char *path);
char *SymbolDB_path(SymbolDB *self);
void SymbolDB_delete(SymbolDB *self);

int SymbolDB_open(SymbolDB *self);
int SymbolDB_close(SymbolDB *self);
void SymbolDB_show(SymbolDB *self);

typedef uint64_t symbolid_t;

Datum *SymbolDB_symbolForId_(SymbolDB *self, symbolid_t key); // caller should Datum_free result
symbolid_t SymbolDB_idForSymbol_(SymbolDB *self, Datum *symbol);

#ifdef __cplusplus
}
#endif
#endif
