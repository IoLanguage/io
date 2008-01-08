/*#io
SkipDBCursor ioDoc(
		   docCopyright("Steve Dekorte", 2002)
		   docLicense("BSD revised")
		   docCategory("Databases")
		   docDescription("An interator object for a SkipDB.")
*/

#include "IoSkipDBCursor.h"
#include "SkipDB.h"
#include "IoObject.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoState.h"
#include "IoNumber.h"

#define CURSOR(self) ((SkipDBCursor *)(IoObject_dataPointer(self)))

IoTag *IoSkipDBCursor_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("SkipDBCursor");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSkipDBCursor_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSkipDBCursor_rawClone);
	//IoTag_markFunc_(tag, (IoTagMarkFunc *)IoSkipDBCursor_mark);
	//IoTag_writeToStreamFunc_(tag, (IoTagWriteToStoreOnStreamFunc *)IoSkipDBCursor_writeToStore_stream_);
	//IoTag_readFromStreamFunc_(tag, (IoTagReadFromStoreOnStreamFunc *)IoSkipDBCursor_readFromStore_stream_);
	return tag;
}

/*
void IoSkipDBCursor_writeToStream_(IoObject *self, BStream *stream)
{
}

void IoSkipDBCursor_readFromStream_(IoObject *self, BStream *stream)
{
}
*/

IoSkipDBCursor *IoSkipDBCursor_proto(void *state)
{
	IoMethodTable methodTable[] = {
	{"goto",     IoSkipDBCursor_goto},
	{"first",    IoSkipDBCursor_first},
	{"last",     IoSkipDBCursor_last},
	{"next",     IoSkipDBCursor_next},
	{"previous", IoSkipDBCursor_previous},
	{"key",  IoSkipDBCursor_key},
	{"value",  IoSkipDBCursor_value},
	{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoSkipDBCursor_newTag(state));

	IoObject_setDataPointer_(self, NULL);
	IoState_registerProtoWithFunc_((IoState *)state, self, IoSkipDBCursor_proto);

	IoObject_addMethodTable_(self, methodTable);
	return self;
}

IoSkipDBCursor *IoSkipDBCursor_rawClone(IoSkipDBCursor *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_tag_(self, IoObject_tag(proto));
	IoObject_setDataPointer_(self, NULL);
	return self;
}

IoSkipDBCursor *IoSkipDBCursor_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoSkipDBCursor_proto);
	return IOCLONE(proto);
}

IoSkipDBCursor *IoSkipDBCursor_newWithSDBCursor(void *state, SkipDBCursor *cursor)
{
	IoSkipDBCursor *self = IoSkipDBCursor_new(state);
	SkipDBCursor_retain(cursor);
	IoObject_setDataPointer_(self, cursor);
	return self;
}

void IoSkipDBCursor_free(IoSkipDBCursor *self)
{
	if (CURSOR(self)) SkipDBCursor_release(CURSOR(self));
}

/*
void IoSkipDBCursor_mark(IoSkipDBCursor *self)
{
}
*/

// --------------------------------------------------------

IoObject *IoSkipDBCursor_goto(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("goto(aKey)", "Move cursor to the specified key or nearest preceeding key. Returns self")
	*/

	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);
	IOASSERT(CURSOR(self), "SkipDBCursor invalid");
	SkipDBCursor_goto_(CURSOR(self), IoSeq_asDatum(key));
	return self;
}

IoObject *IoSkipDBCursor_first(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("first", "Move cursor to first item. Returns self.")
	*/

	IOASSERT(CURSOR(self), "SkipDBCursor invalid");
	SkipDBCursor_first(CURSOR(self));
	return self;
}

IoObject *IoSkipDBCursor_last(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("last", "Move cursor to last item. Returns self.")
	*/

	IOASSERT(CURSOR(self), "SkipDBCursor invalid");
	SkipDBCursor_last(CURSOR(self));
	return self;
}

IoObject *IoSkipDBCursor_next(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("next", "Move cursor to next item. Returns self.")
	*/

	IOASSERT(CURSOR(self), "SkipDBCursor invalid");
	SkipDBCursor_next(CURSOR(self));
	return self;
}

IoObject *IoSkipDBCursor_previous(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("previous", "Move cursor to previous item. Returns self.")
	*/

	IOASSERT(CURSOR(self), "SkipDBCursor invalid");
	SkipDBCursor_previous(CURSOR(self));
	return self;
}

IoObject *IoSkipDBCursor_key(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("key", "Returns the current cursor key, or nil if the cursor is out of range.")
	*/

	IOASSERT(CURSOR(self), "SkipDBCursor invalid");

	{
	SkipDBRecord *r = SkipDBCursor_current(CURSOR(self));

	if (r)
	{
		Datum k = SkipDBRecord_keyDatum(r);
		return IoSeq_newWithDatum_(IOSTATE, &k);
	}

	return IONIL(self);
	}
}

IoObject *IoSkipDBCursor_value(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("value", "Returns the current cursor key, or nil if the cursor is out of range.")
	*/

	IOASSERT(CURSOR(self), "SkipDBCursor invalid");

	{
	SkipDBRecord *r = SkipDBCursor_current(CURSOR(self));

	if (r)
	{
		Datum v = SkipDBRecord_valueDatum(r);
		return IoSeq_newWithDatum_(IOSTATE, &v);
	}

	return IONIL(self);
	}
}

