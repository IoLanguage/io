
//metadoc TokyoCabinetCursor category Databases
//metadoc TokyoCabinetCursor copyright Steve Dekorte 2002
//metadoc TokyoCabinetCursor license BSD revised
/*metadoc TokyoCabinetCursor description
A database cursor.
*/

#include "IoTokyoCabinetCursor.h"

#include "IoObject.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoState.h"
#include "IoNumber.h"

#include <tcutil.h>
#include <tcbdb.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>


#define TokyoCabinetCursor(self) ((BDBCUR *)(IoObject_dataPointer(self)))
#define TokyoCabinet(self) (((BDBCUR *)(IoObject_dataPointer(self)))->bdb)

IoTag *IoTokyoCabinetCursor_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("TokyoCabinetCursor");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoTokyoCabinetCursor_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoTokyoCabinetCursor_rawClone);
	//IoTag_markFunc_(tag, (IoTagMarkFunc *)IoTokyoCabinetCursor_mark);
	return tag;
}

IoTokyoCabinetCursor *IoTokyoCabinetCursor_proto(void *state)
{
	IoMethodTable methodTable[] = {
	{"close",     IoTokyoCabinetCursor_close},
	{"first",  IoTokyoCabinetCursor_first},
	{"last",  IoTokyoCabinetCursor_last},
	{"previous",  IoTokyoCabinetCursor_previous},
	{"next",  IoTokyoCabinetCursor_next},
	{"jump",  IoTokyoCabinetCursor_jump},
	{"key",  IoTokyoCabinetCursor_key},
	{"value",  IoTokyoCabinetCursor_value},
	{"put",  IoTokyoCabinetCursor_put},
	{"remove",  IoTokyoCabinetCursor_remove},

	{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoTokyoCabinetCursor_newTag(state));

	IoObject_setDataPointer_(self, NULL);
	IoState_registerProtoWithFunc_((IoState *)state, self, IoTokyoCabinetCursor_proto);

	IoObject_addMethodTable_(self, methodTable);
	return self;
}

IoTokyoCabinetCursor *IoTokyoCabinetCursor_rawClone(IoTokyoCabinetCursor *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_tag_(self, IoObject_tag(proto));
	IoObject_setDataPointer_(self, tcbdbnew());
	return self;
}

IoTokyoCabinetCursor *IoTokyoCabinetCursor_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoTokyoCabinetCursor_proto);
	return IOCLONE(proto);
}

IoTokyoCabinetCursor *IoTokyoCabinetCursor_newWithDB_(void *state, void *bdb)
{
	IoObject *self = IoTokyoCabinetCursor_new(state);
	IoObject_setDataPointer_(self, tcbdbcurnew((TCBDB *)bdb));
	return self;
}

void IoTokyoCabinetCursor_free(IoTokyoCabinetCursor *self)
{
	if(TokyoCabinetCursor(self))
	{
		tcbdbcurdel(TokyoCabinetCursor(self));
		IoObject_setDataPointer_(self, NULL);
	}
}

/*
void IoTokyoCabinetCursor_mark(IoTokyoCabinetCursor *self)
{
}
*/

BDBCUR *tcbdbcurnew(TCBDB *bdb);

IoObject *IoTokyoCabinetCursor_close(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetCursor close
	Closes the database.
	*/

	if(TokyoCabinetCursor(self))
	{
		tcbdbcurdel(TokyoCabinetCursor(self));
		IoObject_setDataPointer_(self, NULL);
	}

	return self;
}

IoObject *IoTokyoCabinetCursor_first(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetCursor first
	Move cursor to first record. Returns self
	*/

	IOASSERT(TokyoCabinetCursor(self), "invalid TokyoCabinetCursor");
	return IOBOOL(self, tcbdbcurfirst(TokyoCabinetCursor(self)));
}

IoObject *IoTokyoCabinetCursor_last(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetCursor last
	Move cursor to last record. Returns self
	*/

	IOASSERT(TokyoCabinetCursor(self), "invalid TokyoCabinetCursor");
	return IOBOOL(self, tcbdbcurlast(TokyoCabinetCursor(self)));
}

IoObject *IoTokyoCabinetCursor_previous(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetCursor previous
	Move cursor to previous record. Returns true if there is another key, 
	or false if there is no previous record.
	*/

	IOASSERT(TokyoCabinetCursor(self), "invalid TokyoCabinetCursor");
	return IOBOOL(self, tcbdbcurprev(TokyoCabinetCursor(self)));
}

IoObject *IoTokyoCabinetCursor_next(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetCursor next
	Move cursor to next record. Returns true if there is another key, 
	or false if there is no next record.
	*/

	IOASSERT(TokyoCabinetCursor(self), "invalid TokyoCabinetCursor");
	return IOBOOL(self, tcbdbcurnext(TokyoCabinetCursor(self)));
}

IoObject *IoTokyoCabinetCursor_jump(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetCursor jump(key)
	Move cursor to record before key. Returns self
	*/
	
	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);

	IOASSERT(TokyoCabinetCursor(self), "invalid TokyoCabinetCursor");
	return IOBOOL(self, tcbdbcurjump(TokyoCabinetCursor(self), (const void *)IoSeq_rawBytes(key), (int)IoSeq_rawSizeInBytes(key)));
}

IoObject *IoTokyoCabinetCursor_key(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetCursor key
	Returns current cursor key or nil.
	*/
	
	int size;
	char *value;

	IOASSERT(TokyoCabinetCursor(self), "invalid TokyoCabinetCursor");
	value = tcbdbcurkey(TokyoCabinetCursor(self), &size);

	if (value)
	{
		IoSeq *s = IoSeq_newWithData_length_(IOSTATE, (unsigned char *)value, size);
		free(value);
		return s;
	}

	return IONIL(self);
}

IoObject *IoTokyoCabinetCursor_value(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetCursor value
	Returns current cursor value or nil.
	*/
	
	int size;
	char *value;

	IOASSERT(TokyoCabinetCursor(self), "invalid TokyoCabinetCursor");
	value = tcbdbcurval(TokyoCabinetCursor(self), &size);

	if (value)
	{
		IoSeq *s = IoSeq_newWithData_length_(IOSTATE, (unsigned char *)value, size);
		free(value);
		return s;
	}

	return IONIL(self);
}

IoObject *IoTokyoCabinetCursor_put(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetCursor put(value)
	Sets the value at the current cursor postion. Returns self.
	*/
	
	IoSeq *value = IoMessage_locals_seqArgAt_(m, locals, 0);

	IOASSERT(TokyoCabinetCursor(self), "invalid TokyoCabinetCursor");

	IOASSERT(tcbdbcurput(TokyoCabinetCursor(self), (const char *)IoSeq_rawBytes(value), IoSeq_rawSizeInBytes(value), BDBCPCURRENT), tcbdberrmsg(tcbdbecode(TokyoCabinet(self))));

	return self;
}

IoObject *IoTokyoCabinetCursor_remove(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetCursor remove
	Removes the current cursor postion. Returns self.
	*/

	IOASSERT(TokyoCabinetCursor(self), "invalid TokyoCabinetCursor");

	IOASSERT(tcbdbcurout(TokyoCabinetCursor(self)), tcbdberrmsg(tcbdbecode(TokyoCabinet(self))));

	return self;
}
