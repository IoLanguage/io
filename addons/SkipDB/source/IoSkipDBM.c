//metadoc SkipDBM copyright Steve Dekorte 2002
//metadoc SkipDBM license BSD revised
//metadoc SkipDBM category Databases")
/*metadoc SkipDBM description
SkipDB is a skip-list based key-value database. SkipDBM manages any number of skipdbs within the same file. The root skipdb can be accessed using the root method.")
	docObject
*/

#include "IoSkipDBM.h"
#include "IoSkipDB.h"
#include "IoObject.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoState.h"
#include "IoNumber.h"

#define SKIPDBM(self) ((SkipDBM *)(IoObject_dataPointer(self)))

IoTag *IoSkipDBM_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("SkipDBM");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSkipDBM_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSkipDBM_rawClone);
	//IoTag_markFunc_(tag, (IoTagMarkFunc *)IoSkipDBM_mark);
	//IoTag_writeToStreamFunc_(tag, (IoTagWriteToStoreOnStreamFunc *)IoSkipDBM_writeToStore_stream_);
	///IoTag_readFromStreamFunc_(tag, (IoTagReadFromStoreOnStreamFunc *)IoSkipDBM_readFromStore_stream_);
	return tag;
}

void IoSkipDBM_writeToStream_(IoObject *self, BStream *stream)
{
}

void IoSkipDBM_readFromStream_(IoObject *self, BStream *stream)
{
}

IoSkipDBM *IoSkipDBM_proto(void *state)
{
	IoMethodTable methodTable[] = {
	{"setPath",  IoSkipDBM_setPath},
	{"path",  IoSkipDBM_path},
	{"open",     IoSkipDBM_open},
	{"close",    IoSkipDBM_close},
	{"isOpen",   IoSkipDBM_isOpen},
	{"delete",   IoSkipDBM_delete},
	{"root",     IoSkipDBM_root},
	{"begin",     IoSkipDBM_beginTransaction},
	{"commit",     IoSkipDBM_commitnTransaction},
	//{"at",       IoSkipDBM_at},
	{"compact",  IoSkipDBM_compact},
	{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoSkipDBM_newTag(state));

	IoObject_setDataPointer_(self, SkipDBM_new());
	IoState_registerProtoWithFunc_((IoState *)state, self, IoSkipDBM_proto);

	IoObject_addMethodTable_(self, methodTable);
	return self;
}

IoSkipDBM *IoSkipDBM_rawClone(IoSkipDBM *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_tag_(self, IoObject_tag(proto));
	IoObject_setDataPointer_(self, SkipDBM_new());
	return self;
}

IoSkipDBM *IoSkipDBM_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoSkipDBM_proto);
	return IOCLONE(proto);
}

void IoSkipDBM_free(IoSkipDBM *self)
{
	SkipDBM_free(SKIPDBM(self));
}

/*
void IoSkipDBM_mark(IoSkipDBM *self)
{
}
*/

// --------------------------------------------------------

IoObject *IoSkipDBM_setPath(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SkipDBM setPath(aString)
	Sets the path to the dbm folder. Returns self.
	*/
	IoSeq *v = IoMessage_locals_seqArgAt_(m, locals, 0);

	SkipDBM_setPath_(SKIPDBM(self), CSTRING(v));
	return self;
}


IoObject *IoSkipDBM_path(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SkipDBM path
	Returns the path to the dbm. Returns self.
	*/

	return IOSYMBOL(SkipDBM_path(SKIPDBM(self)));
}

IoObject *IoSkipDBM_open(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SkipDBM open
	Opens the dbm. Returns self.
	*/

	SkipDBM_open(SKIPDBM(self));
	return self;
}

IoObject *IoSkipDBM_close(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SkipDBM close
	Closes the dbm.
	*/

	SkipDBM_close(SKIPDBM(self));
	return self;
}

IoObject *IoSkipDBM_isOpen(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SkipDBM delete
	Deletes the db. Returns self.
	*/

	return IOBOOL(self, SkipDBM_isOpen(SKIPDBM(self)));
}

IoObject *IoSkipDBM_delete(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SkipDBM delete
	Deletes the db. Returns self.
	*/

	SkipDBM_delete(SKIPDBM(self));
	return self;
}

IoObject *IoSkipDBM_root(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SkipDBM root
	Returns the root SkipDB.
	*/
	IOASSERT(SkipDBM_isOpen(SKIPDBM(self)), "skipdbm not open");

	{
	SkipDB *sdb = SkipDBM_rootSkipDB(SKIPDBM(self));

	return IoSkipDB_newWithSDB(IOSTATE, sdb);
	//return self;
	}
}

IoObject *IoSkipDBM_at(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SkipDBM at(pidNumber)
	Returns the SkipDB at the specified persistent ID or nil if it is not found.
	*/
	/*
	PID_TYPE pid = IoMessage_locals_intArgAt_(m, locals, 0);

	SkipDB *sdb = SkipDBM_skipdbAtPid_(SKIPDBM(self));
	return IoSkipDB_newWithSDB(IOSTATE, sdb);
	*/
	IOASSERT(SkipDBM_isOpen(SKIPDBM(self)), "skipdbm not open");
	return self;
}

IoObject *IoSkipDBM_compact(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SkipDBM compact
	Compacts the database. Returns self.
	*/

	IOASSERT(SkipDBM_isOpen(SKIPDBM(self)), "skipdbm not open");
	SkipDBM_compact(SKIPDBM(self));
	return self;
}

IoObject *IoSkipDBM_beginTransaction(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SkipDBM beginTransaction
	Begin a transaction. Returns self.
	*/

	IOASSERT(SkipDBM_isOpen(SKIPDBM(self)), "skipdbm not open");
	SkipDBM_beginTransaction(SKIPDBM(self));
	return self;
}

IoObject *IoSkipDBM_commitnTransaction(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SkipDBM commitTransaction
	Commit a transaction. Returns self.
	*/

	IOASSERT(SkipDBM_isOpen(SKIPDBM(self)), "skipdbm not open");
	SkipDBM_commitTransaction(SKIPDBM(self));
	return self;
}


