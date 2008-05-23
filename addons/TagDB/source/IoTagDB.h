/*   copyright: Steve Dekorte, 2007
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IoTagDB_DEFINED
#define IoTagDB_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include <tagdb/TagDB.h>

typedef IoObject IoTagDB;

IoTagDB *IoTagDB_proto(void *state);
IoTagDB *IoTagDB_new(void *state);
IoTagDB *IoTagDB_rawClone(IoTagDB *self);

void IoTagDB_free(IoTagDB *self);

// -----------------------------------------------------------

IoObject *IoTagDB_open(IoTagDB *self, IoObject *locals, IoMessage *m);
IoObject *IoTagDB_close(IoTagDB *self, IoObject *locals, IoMessage *m);

IoObject *IoTagDB_atKeyPutTags(IoTagDB *self, IoObject *locals, IoMessage *m);
IoObject *IoTagDB_tagsAtKey(IoTagDB *self, IoObject *locals, IoMessage *m);
IoObject *IoTagDB_keyAtIndex(IoTagDB *self, IoObject *locals, IoMessage *m);
IoObject *IoTagDB_removeKey(IoTagDB *self, IoObject *locals, IoMessage *m);
IoObject *IoTagDB_keysForTags(IoTagDB *self, IoObject *locals, IoMessage *m);
IoObject *IoTagDB_size(IoTagDB *self, IoObject *locals, IoMessage *m);

IoObject *IoTagDB_symbolForId(IoTagDB *self, IoObject *locals, IoMessage *m);
IoObject *IoTagDB_idForSymbol(IoTagDB *self, IoObject *locals, IoMessage *m);
IoObject *IoTagDB_allUniqueTagIds(IoTagDB *self, IoObject *locals, IoMessage *m);

#endif
