
//metadoc TokyoCabinetPrefixCursor category Databases
//metadoc TokyoCabinetPrefixCursor copyright Steve Dekorte 2002
//metadoc TokyoCabinetPrefixCursor license BSD revised
/*metadoc TokyoCabinetPrefixCursor description
A database cursor.
*/

#include "IoTokyoCabinetPrefixCursor.h"

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


#define TokyoCabinetPrefixCursor(self) ((BDBCUR *)(IoObject_dataPointer(self)))
#define TokyoCabinet(self) (((BDBCUR *)(IoObject_dataPointer(self)))->bdb)

IoTag *IoTokyoCabinetPrefixCursor_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("TokyoCabinetPrefixCursor");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoTokyoCabinetPrefixCursor_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoTokyoCabinetPrefixCursor_rawClone);
	//IoTag_markFunc_(tag, (IoTagMarkFunc *)IoTokyoCabinetPrefixCursor_mark);
	return tag;
}

IoTokyoCabinetPrefixCursor *IoTokyoCabinetPrefixCursor_proto(void *state)
{
	IoMethodTable methodTable[] = {
	{"close",     IoTokyoCabinetPrefixCursor_close},
	{"first",  IoTokyoCabinetPrefixCursor_first},
	{"last",  IoTokyoCabinetPrefixCursor_last},
	{"previous",  IoTokyoCabinetPrefixCursor_previous},
	{"next",  IoTokyoCabinetPrefixCursor_next},
	{"jump",  IoTokyoCabinetPrefixCursor_jump},
	{"key",  IoTokyoCabinetPrefixCursor_key},
	{"value",  IoTokyoCabinetPrefixCursor_value},
	{"put",  IoTokyoCabinetPrefixCursor_put},
	{"remove",  IoTokyoCabinetPrefixCursor_remove},

	{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoTokyoCabinetPrefixCursor_newTag(state));

	IoObject_setDataPointer_(self, NULL);
	IoState_registerProtoWithFunc_((IoState *)state, self, IoTokyoCabinetPrefixCursor_proto);

	IoObject_addMethodTable_(self, methodTable);
	return self;
}

IoTokyoCabinetPrefixCursor *IoTokyoCabinetPrefixCursor_rawClone(IoTokyoCabinetPrefixCursor *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_tag_(self, IoObject_tag(proto));
	IoObject_setDataPointer_(self, tcbdbnew());
	return self;
}

IoTokyoCabinetPrefixCursor *IoTokyoCabinetPrefixCursor_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoTokyoCabinetPrefixCursor_proto);
	return IOCLONE(proto);
}

IoTokyoCabinetPrefixCursor *IoTokyoCabinetPrefixCursor_newWithDB_(void *state, void *bdb)
{
	IoObject *self = IoTokyoCabinetPrefixCursor_new(state);
	IoObject_setDataPointer_(self, tcbdbcurnew((TCBDB *)bdb));
	return self;
}

void IoTokyoCabinetPrefixCursor_free(IoTokyoCabinetPrefixCursor *self)
{
	if(TokyoCabinetPrefixCursor(self))
	{
		tcbdbcurdel(TokyoCabinetPrefixCursor(self));
		IoObject_setDataPointer_(self, NULL);
	}
}

/*
void IoTokyoCabinetPrefixCursor_mark(IoTokyoCabinetPrefixCursor *self)
{
}
*/

BDBCUR *tcbdbcurnew(TCBDB *bdb);

IoObject *IoTokyoCabinetPrefixCursor_close(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetPrefixCursor close
	Closes the database.
	*/

	if(TokyoCabinetPrefixCursor(self))
	{
		tcbdbcurdel(TokyoCabinetPrefixCursor(self));
		IoObject_setDataPointer_(self, NULL);
	}

	return self;
}

int IoTokyoCabinetPrefixCursor_keyBeginsWithPrefix_(IoObject *self, IoSeq *prefix)
{	
	int size;
	char *value = tcbdbcurkey(TokyoCabinetPrefixCursor(self), &size);
	
	if(value)
	{
		UArray o = UArray_stackAllocedWithData_type_size_(value, CTYPE_uint8_t, size);
		UArray *p = IoSeq_rawUArray(prefix);
		
		return UArray_beginsWith_(&o, p);
	}
	
	return 0;
}

/*
IoSeq *IoTokyoCabinetPrefixCursor_rawPrefix(IoObject *self)
{
	return IoObject_getSlot_(self, IOSYMBOL("prefix"));
}
*/

IoObject *IoTokyoCabinetPrefixCursor_first(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetPrefixCursor first
	Move cursor to first record. Returns self
	*/

	IoSeq *prefix = IoObject_getSlot_(self, IOSYMBOL("prefix"));
	IOASSERT(ISSEQ(prefix), "prefix must be a sequence");
	IOASSERT(TokyoCabinetPrefixCursor(self), "invalid TokyoCabinetPrefixCursor");
	
	tcbdbcurjump(TokyoCabinetPrefixCursor(self), (const void *)IoSeq_rawBytes(prefix), (int)IoSeq_rawSizeInBytes(prefix));
	
	if(!IoTokyoCabinetPrefixCursor_keyBeginsWithPrefix_(self, prefix))
	{
		tcbdbcurnext(TokyoCabinetPrefixCursor(self));
	}
	
	return IOBOOL(self, IoTokyoCabinetPrefixCursor_keyBeginsWithPrefix_(self, prefix));
}

IoObject *IoTokyoCabinetPrefixCursor_last(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetPrefixCursor last
	Move cursor to last record. Returns self
	*/

	IoSeq *prefix = IoObject_getSlot_(self, IOSYMBOL("prefix"));
	IOASSERT(ISSEQ(prefix), "prefix must be a sequence");
	IOASSERT(TokyoCabinetPrefixCursor(self), "invalid TokyoCabinetPrefixCursor");
	
	{
		UArray *p = UArray_clone(IoSeq_rawUArray(prefix));
		UArray_appendCString_(p, " "); // space preceeds .
		
		tcbdbcurjump(TokyoCabinetPrefixCursor(self), (const void *)UArray_bytes(p), (int)UArray_size(p));
		
		UArray_free(p);
	}
	
	return IOBOOL(self, IoTokyoCabinetPrefixCursor_keyBeginsWithPrefix_(self, prefix));
}

IoObject *IoTokyoCabinetPrefixCursor_previous(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetPrefixCursor previous
	Move cursor to previous record. Returns true if there is another key, 
	or false if there is no previous record.
	*/

	IoSeq *prefix = IoObject_getSlot_(self, IOSYMBOL("prefix"));
IOASSERT(ISSEQ(prefix), "prefix must be a sequence");
	IOASSERT(TokyoCabinetPrefixCursor(self), "invalid TokyoCabinetPrefixCursor");
	tcbdbcurprev(TokyoCabinetPrefixCursor(self));
	return IOBOOL(self, IoTokyoCabinetPrefixCursor_keyBeginsWithPrefix_(self, prefix));
}

IoObject *IoTokyoCabinetPrefixCursor_next(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetPrefixCursor next
	Move cursor to next record. Returns true if there is another key, 
	or false if there is no next record.
	*/

	IoSeq *prefix = IoObject_getSlot_(self, IOSYMBOL("prefix"));
	IOASSERT(ISSEQ(prefix), "prefix must be a sequence");
	IOASSERT(TokyoCabinetPrefixCursor(self), "invalid TokyoCabinetPrefixCursor");
	tcbdbcurnext(TokyoCabinetPrefixCursor(self));
	return IOBOOL(self, IoTokyoCabinetPrefixCursor_keyBeginsWithPrefix_(self, prefix));
}

IoObject *IoTokyoCabinetPrefixCursor_jump(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetPrefixCursor jump(key)
	Move cursor to record before key. Returns self
	*/
	
	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);

	IOASSERT(TokyoCabinetPrefixCursor(self), "invalid TokyoCabinetPrefixCursor");
	return IOBOOL(self, tcbdbcurjump(TokyoCabinetPrefixCursor(self), (const void *)IoSeq_rawBytes(key), (int)IoSeq_rawSizeInBytes(key)));
}

IoObject *IoTokyoCabinetPrefixCursor_key(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetPrefixCursor key
	Returns current cursor key or nil.
	*/
	
	int size;
	char *ks;
	
	IoSeq *prefix = IoObject_getSlot_(self, IOSYMBOL("prefix"));
	IOASSERT(ISSEQ(prefix), "prefix must be a sequence");
	IOASSERT(TokyoCabinetPrefixCursor(self), "invalid TokyoCabinetPrefixCursor");
	ks = tcbdbcurkey(TokyoCabinetPrefixCursor(self), &size);

	if (ks)
	{
		UArray *k = UArray_newWithData_type_size_copy_(ks, CTYPE_uint8_t, size, 1);
	
		if (UArray_beginsWith_(k, IoSeq_rawUArray(prefix)))
		{
			//printf("before clip '%s'\n", UArray_bytes(k));
			UArray_clipBeforeEndOf_(k, IoSeq_rawUArray(prefix));
			UArray_removeFirst(k); // remove separator
			//printf("after clip  '%s'\n", UArray_bytes(k));
			return IoSeq_newWithUArray_copy_(IOSTATE, k, 0);
		}

		UArray_free(k);
	}

	return IONIL(self);
}

IoObject *IoTokyoCabinetPrefixCursor_value(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetPrefixCursor value
	Returns current cursor value or nil.
	*/
	
	int size;
	char *value;

	IOASSERT(TokyoCabinetPrefixCursor(self), "invalid TokyoCabinetPrefixCursor");
	value = tcbdbcurval(TokyoCabinetPrefixCursor(self), &size);

	if (value)
	{
		IoSeq *s = IoSeq_newWithData_length_(IOSTATE, (unsigned char *)value, size);
		free(value);
		return s;
	}

	return IONIL(self);
}

IoObject *IoTokyoCabinetPrefixCursor_put(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetPrefixCursor put(value)
	Sets the value at the current cursor postion. Returns self.
	*/
	
	IoSeq *value = IoMessage_locals_seqArgAt_(m, locals, 0);

	IOASSERT(TokyoCabinetPrefixCursor(self), "invalid TokyoCabinetPrefixCursor");

	IOASSERT(tcbdbcurput(TokyoCabinetPrefixCursor(self), (const char *)IoSeq_rawBytes(value), IoSeq_rawSizeInBytes(value), BDBCPCURRENT), tcbdberrmsg(tcbdbecode(TokyoCabinet(self))));

	return self;
}

IoObject *IoTokyoCabinetPrefixCursor_remove(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinetPrefixCursor remove
	Removes the current cursor postion. Returns self.
	*/

	IOASSERT(TokyoCabinetPrefixCursor(self), "invalid TokyoCabinetPrefixCursor");

	IOASSERT(tcbdbcurout(TokyoCabinetPrefixCursor(self)), tcbdberrmsg(tcbdbecode(TokyoCabinet(self))));

	return self;
}
