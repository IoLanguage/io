/*#io
docCopyright("Steve Dekorte", 2007)
docLicense("BSD revised")
docDescription("A fast in-memory tagging database using QDBM as a backing store. 
All key/tags records are read on start up and earches are done on the in-memory table.
Insert/updates are transactionally saved to QDBM as they are made.")
*/

#ifndef TagDB_DEFINED
#define TagDB_DEFINED 1

#ifdef __cplusplus
extern "C" {
#endif
	
#include "TagDBItem.h"
#include "SymbolDB.h"
#include "Datum.h"
	
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <villa_extras.h>

typedef struct
{
	char *path;
	char *k2tPath;
	SymbolDB *symbols;
	VILLA *keyToTags;
	TagDBItem **items;
	size_t itemCount;
	KeyIdArray *results;
} TagDB;

TagDB *TagDB_new(void);
void TagDB_free(TagDB *self);
void TagDB_delete(TagDB *self);

void TagDB_setPath_(TagDB *self, char *path);
char *TagDB_path(TagDB *self);

int TagDB_open(TagDB *self);
int TagDB_close(TagDB *self);

// low level API

int TagDB_atKey_putTags_(TagDB *self, keyid_t key, TagIdArray *tags);
TagIdArray *TagDB_tagsAt_(TagDB *self, keyid_t key);
int TagDB_removeKey_(TagDB *self, keyid_t key);
KeyIdArray *TagDB_keysForTags_(TagDB *self, TagIdArray *tags); 
size_t TagDB_size(TagDB *self);

void TagDB_show(TagDB *self);

Datum *TagDB_symbolForId_(TagDB *self, symbolid_t key);
symbolid_t TagDB_idForSymbol_size_(TagDB *self, const char *s, size_t size);
symbolid_t TagDB_idForSymbol_(TagDB *self, const char *s);

#ifdef __cplusplus
}
#endif
#endif
