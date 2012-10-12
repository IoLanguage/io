
//metadoc SkipDB copyright Steve Dekorte 2002
//metadoc SkipDB license BSD revised
/*metadoc SkipDB description
An ordered key/value database implemented using a skiplist data structure.
*/
//metadoc SkipDB category Databases

#include "IoSkipDB.h"
#include "IoSkipDBCursor.h"
#include "SkipDB.h"
#include "IoObject.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoState.h"
#include "IoNumber.h"

#define SKIPDB(self) ((SkipDB *)(IoObject_dataPointer(self)))
static const char *protoId = "SkipDB";

IoTag *IoSkipDB_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSkipDB_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSkipDB_rawClone);
	//IoTag_markFunc_(tag, (IoTagMarkFunc *)IoSkipDB_mark);
	//IoTag_writeToStreamFunc_(tag, (IoTagWriteToStoreOnStreamFunc *)IoSkipDB_writeToStore_stream_);
	//IoTag_readFromStreamFunc_(tag, (IoTagReadFromStoreOnStreamFunc *)IoSkipDB_readFromStore_stream_);
	return tag;
}

/*
void IoSkipDB_writeToStream_(IoObject *self, BStream *stream)
{
}

void IoSkipDB_readFromStream_(IoObject *self, BStream *stream)
{
}
*/

IoSkipDB *IoSkipDB_proto(void *state)
{
	IoMethodTable methodTable[] = {
	{"headerPid", IoSkipDB_headerPid},
	{"atPut",     IoSkipDB_atPut},
	{"at",        IoSkipDB_at},
	{"removeAt",  IoSkipDB_removeAt},
	{"size",      IoSkipDB_size},
	{"cursor",    IoSkipDB_cursor},
	{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoSkipDB_newTag(state));

	IoObject_setDataPointer_(self, NULL);
	IoState_registerProtoWithId_((IoState *)state, self, protoId);

	IoObject_addMethodTable_(self, methodTable);
	return self;
}

IoSkipDB *IoSkipDB_rawClone(IoSkipDB *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_tag_(self, IoObject_tag(proto));
	IoObject_setDataPointer_(self, NULL);
	return self;
}

IoSkipDB *IoSkipDB_new(void *state)
{
	IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
	return IOCLONE(proto);
}

IoSkipDB *IoSkipDB_newWithSDB(void *state, SkipDB *sdb)
{
	IoSkipDB *self = IoSkipDB_new(state);
	SkipDB_retain(sdb);
	IoObject_setDataPointer_(self, sdb);
	return self;
}

void IoSkipDB_free(IoSkipDB *self)
{
	if (SKIPDB(self)) SkipDB_release(SKIPDB(self));
}

/*
void IoSkipDB_mark(IoSkipDB *self)
{
}
*/

// --------------------------------------------------------

IoObject *IoSkipDB_headerPid(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SkipDB headerPid
	Returns the headerPid number.
	*/

	return self;
}

IoObject *IoSkipDB_atPut(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SkipDB atPut(keySymbol, valueSequence)
	Sets the value of valueSequence with the key keySymbol. Returns self.
	*/

	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);
	IoSeq *value = IoMessage_locals_seqArgAt_(m, locals, 1);

	IOASSERT(SKIPDB(self) && SkipDB_isOpen(SKIPDB(self)), "invalid skipdb");

	SkipDB_at_put_(SKIPDB(self), IoSeq_asDatum(key), IoSeq_asDatum(value));
	return self;
}

IoObject *IoSkipDB_at(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SkipDB at(keySymbol)
	Returns a Sequence for the value at the given key or nil if there is no such key.
	*/
	
	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);
	Datum v;

	IOASSERT(SKIPDB(self) && SkipDB_isOpen(SKIPDB(self)), "invalid skipdb");

	v = SkipDB_at_(SKIPDB(self), IoSeq_asDatum(key));

	if (Datum_size(&v))
	{
		return IoSeq_newWithDatum_(IOSTATE, &v);
	}

	return IONIL(self);
}

IoObject *IoSkipDB_removeAt(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SkipDB atRemove(keySymbol)
	Removes the specified key. Returns self
	*/
	
	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);
	IOASSERT(SKIPDB(self) && SkipDB_isOpen(SKIPDB(self)), "invalid skipdb");
	SkipDB_removeAt_(SKIPDB(self), IoSeq_asDatum(key));
	return self;
}

IoObject *IoSkipDB_size(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SkipDB size
	Returns the number of keys in the receiver.
	*/

	IOASSERT(SKIPDB(self) && SkipDB_isOpen(SKIPDB(self)), "invalid skipdb");

	return IONUMBER(SkipDB_count(SKIPDB(self)));
}

IoObject *IoSkipDB_cursor(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SkipDB cursor
	Returns a new cursor to enumerate the receiver.
	*/

	IOASSERT(SKIPDB(self) && SkipDB_isOpen(SKIPDB(self)), "invalid skipdb");

	{
	SkipDBCursor *c = SkipDB_createCursor(SKIPDB(self));
	return IoSkipDBCursor_newWithSDBCursor(IOSTATE, c);
	}
}
