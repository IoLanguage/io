#ifndef IOMemcached_DEFINED
#define IOMemcached_DEFINED 1

#include "IoObject.h"
#include <libmemcached/memcached.h>

#define ISMEMCACHED(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoMemcached_rawClone)

typedef IoObject IoMemcached;

typedef struct {
	memcached_st* mc;
} IoMemcachedData;

IoTag *IoMemcached_newTag(void *state);
IoObject *IoMemcached_proto(void *state);
IoObject *IoMemcached_rawClone(IoMemcached *self);
IoObject *IoMemcached_new(void *state);
void IoMemcached_free(IoMemcached *self);

// addServer
IoObject *IoMemcached_addServer(IoMemcached *self, IoObject *locals, IoMessage *m);

// Storage commands
IoObject *IoMemcached_set(IoMemcached *self, IoObject *locals, IoMessage *m);
IoObject *IoMemcached_add(IoMemcached *self, IoObject *locals, IoMessage *m);
IoObject *IoMemcached_replace(IoMemcached *self, IoObject *locals, IoMessage *m);
IoObject *IoMemcached_append(IoMemcached *self, IoObject *locals, IoMessage *m);
IoObject *IoMemcached_prepend(IoMemcached *self, IoObject *locals, IoMessage *m);

// Retrieval commands
IoObject *IoMemcached_get(IoMemcached *self, IoObject *locals, IoMessage *m);
IoObject *IoMemcached_getMulti(IoMemcached *self, IoObject *locals, IoMessage *m);

// Delete and flushAll
IoObject *IoMemcached_delete(IoMemcached *self, IoObject *locals, IoMessage *m);
IoObject *IoMemcached_flushAll(IoMemcached *self, IoObject *locals, IoMessage *m);

// Increment/Decrement
IoObject *IoMemcached_incr(IoMemcached *self, IoObject *locals, IoMessage *m);
IoObject *IoMemcached_decr(IoMemcached *self, IoObject *locals, IoMessage *m);

// Stats
IoObject *IoMemcached_stats(IoMemcached *self, IoObject *locals, IoMessage *m);

// Serialize/Deserialize
char *IoMemcached_serialize(IoMemcached *self, IoObject *locals, IoObject *object, size_t *size, uint32_t *flags);
IoObject *IoMemcached_deserialize(IoMemcached *self, char *cvalue, size_t size, uint32_t flags);

#endif
