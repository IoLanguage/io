//metadoc Memcached Aleksey Yeschenko, 2009
//metadoc Memcached license BSD revised
//metadoc Memcached category Databases
//metadoc Memcached credits Aleksey Yeschenko, 2009
/*metadoc Memcached description
<a href="http://www.danga.com/memcached/">memcached</a> is a high-performance,
distributed memory object caching system, generic in nature,
but intended for use in speeding up dynamic web applications
by alleviating database load. Memcached is an Io client library for memcached,
based on C <a href="http://tangent.org/552/libmemcached.html">libmemcached</a>.
*/

#include "IoMemcached.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include "IoList.h"
#include "IoMap.h"

#define DATA(self) ((IoMemcachedData*) IoObject_dataPointer(self))

#define _FLAG_SEQUENCE 0
#define _FLAG_NUMBER   1
#define _FLAG_NIL      2
#define _FLAG_BOOLEAN  3
#define _FLAG_OBJECT   4

static const char *protoId = "Memcached";

IoTag *IoMemcached_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoMemcached_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoMemcached_rawClone);
	return tag;
}

IoObject *IoMemcached_proto(void *state)
{
	IoMemcached *self = IoObject_new(state);
	IoObject_tag_(self, IoMemcached_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoMemcachedData)));

	IoState_registerProtoWithFunc_(state, self, IoMemcached_proto);

	{
		IoMethodTable methodTable[] = {
		{"addServer",  IoMemcached_addServer},
		{"set",        IoMemcached_set},
		{"add",        IoMemcached_add},
		{"replace",    IoMemcached_replace},
		{"append",     IoMemcached_append},
		{"prepend",    IoMemcached_prepend},
		{"get",        IoMemcached_get},
		{"getMulti",   IoMemcached_getMulti},
		{"delete",     IoMemcached_delete},
		{"flushAll",   IoMemcached_flushAll},
		{"incr",       IoMemcached_incr},
		{"decr",       IoMemcached_decr},
		{"stats",      IoMemcached_stats},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoObject *IoMemcached_rawClone(IoMemcached *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoMemcachedData)));

	DATA(self)->mc = memcached_create(NULL);
	memcached_behavior_set(DATA(self)->mc, MEMCACHED_BEHAVIOR_NO_BLOCK, 1);
	memcached_behavior_set(DATA(self)->mc, MEMCACHED_BEHAVIOR_HASH, MEMCACHED_HASH_FNV1A_32);
	memcached_behavior_set(DATA(self)->mc, MEMCACHED_BEHAVIOR_DISTRIBUTION, MEMCACHED_DISTRIBUTION_CONSISTENT);
	memcached_behavior_set(DATA(self)->mc, MEMCACHED_BEHAVIOR_BUFFER_REQUESTS, 0);

	return self;
}

IoObject *IoMemcached_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoMemcached_free(IoMemcached *self)
{
	if (DATA(self) != NULL) {
		if (DATA(self)->mc != NULL) {
			memcached_free(DATA(self)->mc);
		}

		free(DATA(self));
	}
}

/*doc Memcached addServer(address)
Adds a memcached server. address is a "host:port" string, e.g., "127.0.0.1:11211"
Returns self.
*/
IoObject *IoMemcached_addServer(IoMemcached *self, IoObject *locals, IoMessage *m)
{
	memcached_server_list_st server;

	server = memcached_servers_parse(IoMessage_locals_cStringArgAt_(m, locals, 0));
	memcached_server_push(DATA(self)->mc, server);

	memcached_server_list_free(server);

	return self;
}

// Storage commands

/*doc Memcached set(key, value[, expiration])
Asks memcached to store the value identified by the key.
Returns true on success, otherwise raises an exception.
*/
IoObject *IoMemcached_set(IoMemcached *self, IoObject *locals, IoMessage *m)
{
	IoSeq    *key   = IoMessage_locals_seqArgAt_(m, locals, 0);
	IoObject *value = IoMessage_locals_quickValueArgAt_(m, locals, 1);

	time_t expiration = IoMessage_argCount(m) == 3 ? IoMessage_locals_intArgAt_(m, locals, 2) : 0;

	uint32_t flags;
	size_t size;
	char *cvalue = IoMemcached_serialize(self, locals, value, &size, &flags);

	memcached_return_t rc;
	rc = memcached_set(DATA(self)->mc,
		CSTRING(key), IOSEQ_LENGTH(key),
		cvalue, size,
		expiration, flags
	);

	free(cvalue);

	if(rc != MEMCACHED_SUCCESS)
		IoState_error_(IOSTATE, m, memcached_strerror(DATA(self)->mc, rc));

	return IOSTATE->ioTrue;
}

/*doc Memcached add(key, value[, expiration])
Asks memcached to store the value identified by the key,
but only if the server *doesn't* already hold data for this key.
Returns true on success, false in case of a collision.
Otherwise raises an exception.
*/
IoObject *IoMemcached_add(IoMemcached *self, IoObject *locals, IoMessage *m)
{
	IoSeq    *key   = IoMessage_locals_seqArgAt_(m, locals, 0);
	IoObject *value = IoMessage_locals_quickValueArgAt_(m, locals, 1);

	time_t expiration = IoMessage_argCount(m) == 3 ? IoMessage_locals_intArgAt_(m, locals, 2) : 0;

	uint32_t flags;
	size_t size;
	char *cvalue = IoMemcached_serialize(self, locals, value, &size, &flags);

	memcached_return_t rc;
	rc = memcached_add(DATA(self)->mc,
		CSTRING(key), IOSEQ_LENGTH(key),
		cvalue, size,
		expiration, flags
	);

	free(cvalue);

	if(rc != MEMCACHED_SUCCESS && rc != MEMCACHED_NOTSTORED)
		IoState_error_(IOSTATE, m, memcached_strerror(DATA(self)->mc, rc));

	// MEMCACHED_NOTSTORED is a legitmate error in the case of a collision.
	if(rc == MEMCACHED_NOTSTORED)
		return IOSTATE->ioFalse;

	return IOSTATE->ioTrue; // MEMCACHED_SUCCESS
}

/*doc Memcached replace(key, value[, expiration])
Asks memcached to store the value identified by the key,
but only if the server *does* already hold data for this key.
Returns true on success, false if there is already data for this key.
Otherwise raises an exception.
*/
IoObject *IoMemcached_replace(IoMemcached *self, IoObject *locals, IoMessage *m)
{
	IoSeq    *key   = IoMessage_locals_seqArgAt_(m, locals, 0);
	IoObject *value = IoMessage_locals_quickValueArgAt_(m, locals, 1);

	time_t expiration = IoMessage_argCount(m) == 3 ? IoMessage_locals_intArgAt_(m, locals, 2) : 0;

	uint32_t flags;
	size_t size;
	char *cvalue = IoMemcached_serialize(self, locals, value, &size, &flags);

	memcached_return_t rc;
	rc = memcached_replace(DATA(self)->mc,
		CSTRING(key), IOSEQ_LENGTH(key),
		cvalue, size,
		expiration, flags
	);

	free(cvalue);

	if(rc != MEMCACHED_SUCCESS && rc != MEMCACHED_NOTSTORED)
		IoState_error_(IOSTATE, m, memcached_strerror(DATA(self)->mc, rc));

	// MEMCACHED_NOTSTORED is a legitmate error in the case of a collision.
	if(rc == MEMCACHED_NOTSTORED)
		return IOSTATE->ioFalse;

	return IOSTATE->ioTrue; // MEMCACHED_SUCCESS
}

/*doc Memcached append(key, value)
Asks memcached to add this value to an existing key after existing value.
Returns true on success, otherwise raises an exception.
value should be a Sequence.
Supported by memcached 1.2.4+
*/
IoObject *IoMemcached_append(IoMemcached *self, IoObject *locals, IoMessage *m)
{
	IoSeq *key   = IoMessage_locals_seqArgAt_(m, locals, 0);
	IoSeq *value = IoMessage_locals_seqArgAt_(m, locals, 1);

	memcached_return_t rc;
	rc = memcached_append(DATA(self)->mc,
		CSTRING(key),   IOSEQ_LENGTH(key),
		CSTRING(value), IOSEQ_LENGTH(value),
		0, 0
	);

	if(rc != MEMCACHED_SUCCESS)
		IoState_error_(IOSTATE, m, memcached_strerror(DATA(self)->mc, rc));

	return IOSTATE->ioTrue;
}

/*doc Memcached prepend(key, value)
Asks memcached to add this value to an existing key before existing value.
Returns true on success, otherwise raises an exception.
value should be a Sequence.
Supported by memcached 1.2.4+
*/
IoObject *IoMemcached_prepend(IoMemcached *self, IoObject *locals, IoMessage *m)
{
	IoSeq *key   = IoMessage_locals_seqArgAt_(m, locals, 0);
	IoSeq *value = IoMessage_locals_seqArgAt_(m, locals, 1);

	memcached_return_t rc;
	rc = memcached_prepend(DATA(self)->mc,
		CSTRING(key),   IOSEQ_LENGTH(key),
		CSTRING(value), IOSEQ_LENGTH(value),
		0, 0
	);

	if(rc != MEMCACHED_SUCCESS)
		IoState_error_(IOSTATE, m, memcached_strerror(DATA(self)->mc, rc));

	return IOSTATE->ioTrue;
}

// Retrieval commands

/*doc Memcached get(key)
Asks memcached to retrieve data corresponding to the key.
Raises "NOT FOUND" if the data is not there.
*/
IoObject *IoMemcached_get(IoMemcached *self, IoObject *locals, IoMessage *m)
{
	IoObject *key = IoMessage_locals_seqArgAt_(m, locals, 0);

	size_t size;
	uint32_t flags;
	memcached_return_t rc;

	char *cvalue;
	cvalue = memcached_get(DATA(self)->mc,
		CSTRING(key), IOSEQ_LENGTH(key),
		&size, &flags, &rc
	);

	if(cvalue == NULL)
		IoState_error_(IOSTATE, m, memcached_strerror(DATA(self)->mc, rc));

	IoObject *result = IoMemcached_deserialize(self, cvalue, size, flags);

	free(cvalue);

	return result;
}

/*doc Memcached getMulti(keys)
Asks memcached to retrieve data corresponding to the list of keys.
Returns a Map with the results.
If some of the keys appearing in a retrieval request are not sent back
by the server in the item list this means that the server does not
hold items with such keys
*/
IoObject *IoMemcached_getMulti(IoMemcached *self, IoObject *locals, IoMessage *m)
{
	IoList *keys_list = IoMessage_locals_listArgAt_(m, locals, 0);
	size_t keys_list_size = IoList_rawSize(keys_list);

	IoObject *results_map = IoMap_new(IOSTATE);

	if(keys_list_size == 0)
		return results_map;

	int i;
	for(i = 0; i < keys_list_size; i++) {
		IoSeq *key = IoList_rawAt_(keys_list, i);
		IOASSERT(ISSEQ(key), "key must be a Sequence");
		IOASSERT(IOSEQ_LENGTH(key) > 0, "key cannot be an empty Sequence");
		IOASSERT(IOSEQ_LENGTH(key) < MEMCACHED_MAX_KEY, "key is too long");
	}

	const char **ckeys = (const char **) malloc(sizeof(const char *) * keys_list_size);
	size_t *ckey_lengths = (size_t *) malloc(sizeof(size_t) * keys_list_size);

	for(i = 0; i < keys_list_size; i++) {
		ckeys[i] = CSTRING(IoList_rawAt_(keys_list, i));
		ckey_lengths[i] = strlen(ckeys[i]);
	}

	memcached_return_t rc = memcached_mget(DATA(self)->mc, ckeys, ckey_lengths, keys_list_size);

	free(ckeys);
	free(ckey_lengths);

	char returned_key[MEMCACHED_MAX_KEY], *returned_value;
	size_t returned_key_length, returned_value_length;
	uint32_t flags;

	returned_value = memcached_fetch(DATA(self)->mc,
		returned_key, &returned_key_length,
		&returned_value_length, &flags, &rc
	);

	while(returned_value != NULL) {
		IoMap_rawAtPut(results_map,
			IoSeq_newSymbolWithData_length_(IOSTATE, returned_key, returned_key_length),
			IoMemcached_deserialize(self, returned_value, returned_value_length, flags)
		);

		free(returned_value);

		returned_value = memcached_fetch(DATA(self)->mc,
			returned_key, &returned_key_length,
			&returned_value_length, &flags, &rc
		);
	}

	return results_map;
}

// Delete and flushAll

/*doc Memcached delete(key[, time])
Asks memcached to delete an item with the given key.
time is the amount of time in seconds (or Unix time until which)
the client wishes the server to refuse "add" and "replace" commands
with this key.
Returns true on success, false if there is no item with the given key.
Otherwise raises an exception.
*/
IoObject *IoMemcached_delete(IoMemcached *self, IoObject *locals, IoMessage *m)
{
	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);

	time_t time = IoMessage_argCount(m) == 2 ? IoMessage_locals_intArgAt_(m, locals, 1) : 0;

	memcached_return_t rc;
	rc = memcached_delete(DATA(self)->mc,
		CSTRING(key), IOSEQ_LENGTH(key),
		time
	);

	if(rc != MEMCACHED_SUCCESS && rc != MEMCACHED_NOTFOUND)
		IoState_error_(IOSTATE, m, memcached_strerror(DATA(self)->mc, rc));

	if(rc == MEMCACHED_NOTFOUND)
		return IOSTATE->ioFalse;

	return IOSTATE->ioTrue; // MEMCACHED_SUCCESS
}


/*doc Memcached flushAll([expiration])
Asks memcached to invalidate all existing items immediately (by default)
or after the expiration specified.
Always returns true.
*/
IoObject *IoMemcached_flushAll(IoMemcached *self, IoObject *locals, IoMessage *m)
{
	time_t expiration = IoMessage_argCount(m) == 1 ? IoMessage_locals_intArgAt_(m, locals, 0) : 0;
	memcached_flush(DATA(self)->mc, expiration); // always returns ok
	return self;
}

// Increment/Decrement

/*doc Memcached incr([offset])
Asks memcached to increment data for some item in place. The data for the item is
treated as decimal representation of a 64-bit unsigned integer. If the
current data value does not conform to such a representation, the
commands behave as if the value were 0.
Default offset is 1.
Returns the new value.
*/
IoObject *IoMemcached_incr(IoMemcached *self, IoObject *locals, IoMessage *m)
{
	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);

	uint32_t offset = IoMessage_argCount(m) == 2 ? IoMessage_locals_intArgAt_(m, locals, 1) : 1;

	uint64_t new_value;

	memcached_return_t rc;
	rc = memcached_increment(DATA(self)->mc,
		CSTRING(key), IOSEQ_LENGTH(key),
		offset, &new_value
	);

	if(rc != MEMCACHED_SUCCESS)
		IoState_error_(IOSTATE, m, memcached_strerror(DATA(self)->mc, rc));

	return IONUMBER(new_value);
}

/*doc Memcached decr([offset])
Asks memcached to decrement data for some item in place. The data for the item is
treated as decimal representation of a 64-bit unsigned integer. If the
current data value does not conform to such a representation, the
commands behave as if the value were 0.
Default offset is 1.
Returns the new value.
*/
IoObject *IoMemcached_decr(IoMemcached *self, IoObject *locals, IoMessage *m)
{
	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);

	uint32_t offset = IoMessage_argCount(m) == 2 ? IoMessage_locals_intArgAt_(m, locals, 1) : 1;

	uint64_t new_value;

	memcached_return_t rc;
	rc = memcached_decrement(DATA(self)->mc,
		CSTRING(key), IOSEQ_LENGTH(key),
		offset, &new_value
	);

	if(rc != MEMCACHED_SUCCESS)
		IoState_error_(IOSTATE, m, memcached_strerror(DATA(self)->mc, rc));

	return IONUMBER(new_value);
}

// Stats

/*doc Memcached stats
Returns a Map with servers' statistics. Keys are server addresses,
values are maps with actual stats.
*/
IoObject *IoMemcached_stats(IoMemcached *self, IoObject *locals, IoMessage *m)
{
	IoMap *results_map = IoMap_new(IOSTATE);

	int errors = 0;
	uint32_t pos = 0;
	while(pos < memcached_server_count(DATA(self)->mc)) {
		const memcached_instance_st *server = memcached_server_instance_by_position(DATA(self)->mc, pos);
		if(server == NULL)
			continue;

		const char *hostname = memcached_server_name(server);
		const in_port_t port = memcached_server_port(server);

		memcached_stat_st stats;
		memcached_return_t rc = memcached_stat_servername(&stats, "", hostname, port);
		if(rc != MEMCACHED_SUCCESS) {
			errors++;
			continue;
		}

		char **ckeys = memcached_stat_get_keys(DATA(self)->mc, &stats, &rc);
		if(rc != MEMCACHED_SUCCESS) {
			errors++;
			continue;
		}

		IoMap *per_server_map = IoMap_new(IOSTATE);
		char *ckey = *ckeys;
		while(ckey != NULL) {
			char *cvalue = memcached_stat_get_value(DATA(self)->mc, &stats, ckey, &rc);
			if(rc != MEMCACHED_SUCCESS) {
				errors++;
				continue;
			}

			IoMap_rawAtPut(per_server_map, IOSYMBOL(ckey), IOSYMBOL(cvalue));
			free(cvalue);
			ckey++;
		}

		free(ckeys);

		// "127.0.0.1:11211"
		char *server_key = (char *) malloc((strlen(hostname) + 1 + 5 + 1) * sizeof(char));
		sprintf(server_key, "%s:%d", hostname, port);

		IoMap_rawAtPut(results_map, IOSYMBOL(server_key), per_server_map);
		free(server_key);

		pos++;
	}

	if(errors > 0)
		IoState_error_(IOSTATE, m, memcached_strerror(DATA(self)->mc, MEMCACHED_SOME_ERRORS));

	return results_map;
}

// Serialize/Deserialize
char *IoMemcached_serialize(IoMemcached *self, IoObject *locals, IoObject *object, size_t *size, uint32_t *flags) {
	char *cvalue;

	if(ISSEQ(object)) {
		*flags = _FLAG_SEQUENCE;
		*size = IOSEQ_LENGTH(object);
		cvalue = (char *) malloc(*size);
		strncpy(cvalue, CSTRING(object), *size);
	}
	else if(ISNUMBER(object)) {
		*flags = _FLAG_NUMBER;
		double cnumber = IoNumber_asDouble(object);
		cvalue = (char *) malloc(128 * sizeof(char));
		*size = snprintf(cvalue, 127, "%.16f", cnumber);
	}
	else if(ISNIL(object)) {
		*flags = _FLAG_NIL;
		*size = 3;
		cvalue = (char *) malloc(3 * sizeof(char));
		strncpy(cvalue, "nil", 3);
	}
	else if(ISBOOL(object)) {
		*flags = _FLAG_BOOLEAN;
		*size = 1;
		cvalue = (char *) malloc(sizeof(char));
		if(object == IOSTATE->ioTrue)  strncpy(cvalue, "1", 1);
		if(object == IOSTATE->ioFalse) strncpy(cvalue, "0", 1);
	}
	else {
		*flags = _FLAG_OBJECT;
		IoMessage *serialize = IoMessage_newWithName_(IOSTATE, IOSYMBOL("serialized"));
		IoSeq *serialized = IoMessage_locals_performOn_(serialize, locals, object);
		*size = IOSEQ_LENGTH(serialized);
		cvalue = (char *) malloc(*size);
		strncpy(cvalue, CSTRING(serialized), *size);
	}

	return cvalue;
}

IoObject *IoMemcached_deserialize(IoMemcached *self, char *cvalue, size_t size, uint32_t flags) {
	IoObject *object;

	switch(flags) {
		case _FLAG_NUMBER:
			object = IONUMBER(atof(cvalue));
			break;
		case _FLAG_NIL:
			object = IOSTATE->ioNil;
			break;
		case _FLAG_BOOLEAN:
			if(strncmp(cvalue, "1", 1) == 0)
				object = IOSTATE->ioTrue;
			else
				object = IOSTATE->ioFalse;
			break;
		case _FLAG_OBJECT:
			//object = IoState_doCString_(self, cvalue);
			IoState_pushRetainPool(IOSTATE);
			IoSeq *serialized = IoSeq_newWithCString_length_(IOSTATE, cvalue, size);
			object = IoObject_rawDoString_label_(self, serialized, IOSYMBOL("IoMemcached_deserialize"));
			IoState_popRetainPoolExceptFor_(IOSTATE, object);
			break;
		default:
			object = IoSeq_newWithCString_length_(IOSTATE, cvalue, size);
	}

	return object;
}
