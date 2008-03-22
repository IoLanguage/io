
//metadoc TokyoCabinet category Databases
//metadoc TokyoCabinet copyright Steve Dekorte 2002
//metadoc TokyoCabinet license BSD revised
/*metadoc TokyoCabinet description
An ordered key/value database that supports transactions and arbitrary kay and value sizes.
*/

#include "IoTokyoCabinet.h"

#include <tcutil.h>
#include <tcbdb.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "IoObject.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoState.h"
#include "IoNumber.h"

#define TokyoCabinet(self) ((TCBDB *)(IoObject_dataPointer(self)))

static int pathCompare(const char *p1, const char *p2, char sepChar, void *op)
{
	int i;
	int len1 = strlen(p1);
	int len2 = strlen(p2);
	int len = len1 < len2 ? len1 : len2;

	for (i = 0; i < len; i ++)
	{
		char c1 = p1[i];
		char c2 = p2[i];

		if (c1 == sepChar && c2 != sepChar)
		{
			return -1;
		}


		if (c2 == sepChar && c1 != sepChar)
		{
			return 1;
		}

		if (c1 > c2) return 1;
		if (c1 < c2) return -1;
	}

	if (len1 > len2) return 1;
	if (len1 < len2) return -1;

	return 0;
}

/*
static int comparePathComponent(const char *p1, const char *p2, char sepChar, int *size)
{
	char *b1 = strchr(p1 + i, sepChar);
	char *b2 = strchr(p2 + i, sepChar);
	int len1 = b1 ? b1 - p1 : strlen(p1);
	int len2 = b2 ? b2 - p2 : strlen(p2);

	if (len1 && isdigit(p1[0]))
	{
		return strtod(p1, p1 + len1) -
	}

	if (len1 > len2) return 1;
	if (len1 < len2) return -1;
	if (len1 == 0)   return 0;

	return strcmpn(p1, p2, len1);
}

static int pathCompare(const char *p1, const char *p2, char sepChar)
{
	int i;
	int len1 = strlen(p1);
	int len2 = strlen(p2);
	//int len = len1 < len2 ? len1 : len2;


	char *b1 = strchr(p1 + i, '/');
	char *b2 = strchr(p2 + i, '/');

	int blen1 = p1 - b1;


	if (len1 > len2) return 1;
	if (len1 < len2) return -1;

	return 0;
}
*/

static int pathCompareFunc(const char *aptr, int asiz, const char *bptr, int bsiz, void *op)
{
	return pathCompare(aptr, bptr, '/', op);
}


/*
static int compareFunc(const char *aptr, int asiz, const char *bptr, int bsiz, void *op)
{
	return strcmp(aptr, bptr);
}
*/

/*
static int compareStrNumFunc(const char *a, int asize, const char *b, int bsize, void *op)
{
	long an = atol(a);
	long bn = atol(b);
	if (an > bn) return 1;
	if (an < bn) return -1;

	{
	int smaller = (asize < bsize) ? asize : bsize;
	return memcmp(a + smaller + 1, b + smaller + 1, smaller);
	}
}
*/

/*
static int compareStrNumFunc(const char *a, int asize, const char *b, int bsize, void *op)
{
	char *as = strchr((char *)a, '/');
	char *bs = strchr((char *)b, '/');
	long an;
	long bn;
	int r;

	if (as != NULL) *as = 0;
	if (bs != NULL) *bs = 0;

	an = atol(a);
	bn = atol(b);

	//printf("%i cmp %i ", an, bn);

	if (as != NULL) *as = '/';
	if (bs != NULL) *bs = '/';


	if (an > bn) { r = 1; }
	else
	if (an < bn) { r = -1; }
	else
	{
		// an and bn are the same, so just do a lex compare of the whole
		r = strcmp(a, b);
	}

	//printf("%s %s %s\n", a, (r == 0) ? "==" : ((r == 1) ? ">" : "<"), b);

	return r;
}
*/

IoTag *IoTokyoCabinet_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("TokyoCabinet");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoTokyoCabinet_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoTokyoCabinet_rawClone);
	//IoTag_markFunc_(tag, (IoTagMarkFunc *)IoTokyoCabinet_mark);
	return tag;
}

IoTokyoCabinet *IoTokyoCabinet_proto(void *state)
{
	IoMethodTable methodTable[] = {
	{"open",      IoTokyoCabinet_open},
	{"close",     IoTokyoCabinet_close},

	{"atPut",     IoTokyoCabinet_atPut},
	{"atAppend",  IoTokyoCabinet_atAppend},
	{"at",        IoTokyoCabinet_at},
	{"sizeAt",    IoTokyoCabinet_sizeAt},
	{"removeAt",  IoTokyoCabinet_removeAt},
	{"sync",      IoTokyoCabinet_sync},

	{"size",      IoTokyoCabinet_size},
	//{"optimize",  IoTokyoCabinet_optimize},
	{"path",      IoTokyoCabinet_path},

	{"begin",  IoTokyoCabinet_begin},
	{"commit",  IoTokyoCabinet_commit},
	{"abort",  IoTokyoCabinet_abort},

	{"cursor",  IoTokyoCabinet_cursor},
	{"prefixCursor",  IoTokyoCabinet_prefixCursor},

	{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoTokyoCabinet_newTag(state));

	IoObject_setDataPointer_(self, NULL);
	IoState_registerProtoWithFunc_((IoState *)state, self, IoTokyoCabinet_proto);

	IoObject_addMethodTable_(self, methodTable);
	return self;
}

IoTokyoCabinet *IoTokyoCabinet_rawClone(IoTokyoCabinet *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_tag_(self, IoObject_tag(proto));
	IoObject_setDataPointer_(self, tcbdbnew());
	return self;
}

IoTokyoCabinet *IoTokyoCabinet_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoTokyoCabinet_proto);
	return IOCLONE(proto);
}

void IoTokyoCabinet_free(IoTokyoCabinet *self)
{
	if(TokyoCabinet(self))
	{
		tcbdbdel(TokyoCabinet(self));
		IoObject_setDataPointer_(self, NULL);
	}
}

/*
void IoTokyoCabinet_mark(IoTokyoCabinet *self)
{
}
*/

// --------------------------------------------------------

IoObject *IoTokyoCabinet_open(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinet open(path)
	Opens the database.
	*/
	
	IoSeq *path = IoMessage_locals_seqArgAt_(m, locals, 0);
	BDBCMP cf = pathCompareFunc;

	IoTokyoCabinet_close(self, locals, m);
	
	if(IoMessage_argCount(m) > 1)
	{
		IoSeq *compareType = IoMessage_locals_seqArgAt_(m, locals, 1);
		//printf("using compareType:%s\n", CSTRING(compareType));

		if(strcmp(CSTRING(compareType), "tcbdbcmplexical") == 0) { cf = tcbdbcmplexical; } else
		if(strcmp(CSTRING(compareType), "tcbdbcmpdecimal") == 0) { cf = tcbdbcmpdecimal; } else
		if(strcmp(CSTRING(compareType), "tcbdbcmpint32")   == 0) { cf = tcbdbcmpint32;   } else
		if(strcmp(CSTRING(compareType), "tcbdbcmpint64")   == 0) { cf = tcbdbcmpint64;   } else
		if(strcmp(CSTRING(compareType), "path")            == 0) { cf = pathCompareFunc; }
		else
		{
			fprintf(stderr, "ivalid compare function name\n");
			return IONIL(self);
		}
	}

	//tcbdbsetcmpfunc(TokyoCabinet(self), cf, NULL);
	
	IoObject_setDataPointer_(self, tcbdbnew());
	
	if(!tcbdbopen(TokyoCabinet(self), CSTRING(path), BDBOWRITER | BDBOCREAT | BDBOLCKNB))
	{
		fprintf(stderr, "tcbdbopen failed\n");
		return IONIL(self);
	}


	return self;
}

IoObject *IoTokyoCabinet_close(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinet close
	Closes the database.
	*/

	if(TokyoCabinet(self))
	{
		tcbdbclose(TokyoCabinet(self));
		IoObject_setDataPointer_(self, NULL);
	}

	return self;
}

IoObject *IoTokyoCabinet_sync(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinet sync
	Syncs the database. Returns self
	*/
	
	int result;
	IOASSERT(TokyoCabinet(self), "invalid TokyoCabinet");
	result = tcbdbsync(TokyoCabinet(self));
	IOASSERT(result, tcbdberrmsg(tcbdbecode(TokyoCabinet(self))));
	return self;
}

IoObject *IoTokyoCabinet_size(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinet size
	Returns number of records in database. Returns self
	*/
	
	int result;
	IOASSERT(TokyoCabinet(self), "invalid TokyoCabinet");
	result = tcbdbrnum(TokyoCabinet(self));
	return IONUMBER(result);
}

IoObject *IoTokyoCabinet_optimize(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinet optimize
	Optimizes the database. Returns self
	*/

	IOASSERT(TokyoCabinet(self), "invalid TokyoCabinet");
	//IOASSERT(vloptimize(TokyoCabinet(self)), tcbdberrmsg(tcbdbecode(TokyoCabinet(self))));
	return self;
}

IoObject *IoTokyoCabinet_path(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinet path
	Returns the path of the database.
	*/

	IOASSERT(TokyoCabinet(self), "invalid TokyoCabinet");
	return IOSYMBOL(tcbdbpath(TokyoCabinet(self)));
}

IoObject *IoTokyoCabinet_begin(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinet begin
	Begin transaction. Returns self
	*/

	IOASSERT(TokyoCabinet(self), "invalid TokyoCabinet");
	IOASSERT(tcbdbtranbegin(TokyoCabinet(self)), tcbdberrmsg(tcbdbecode(TokyoCabinet(self))));
	return self;
}

IoObject *IoTokyoCabinet_commit(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinet commit
	Commit transaction. Returns self
	*/

	IOASSERT(TokyoCabinet(self), "invalid TokyoCabinet");
	IOASSERT(tcbdbtrancommit(TokyoCabinet(self)), tcbdberrmsg(tcbdbecode(TokyoCabinet(self))));
	return self;
}

IoObject *IoTokyoCabinet_abort(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinet abort
	Abort transaction. Returns self
	*/
	
	int result;
	IOASSERT(TokyoCabinet(self), "invalid TokyoCabinet");
	result = tcbdbtranabort(TokyoCabinet(self));
	IOASSERT(result, tcbdberrmsg(tcbdbecode(TokyoCabinet(self))));
	return self;
}

// ---------------------------------------------------------------------------------

IoObject *IoTokyoCabinet_atPut(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinet atPut(keySymbol, valueSequence)
	Sets the value of valueSequence with the key keySymbol. Returns self.
	*/

	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);
	IoSeq *value = IoMessage_locals_seqArgAt_(m, locals, 1);
	int result;

	IOASSERT(TokyoCabinet(self), "invalid TokyoCabinet");

	result = tcbdbput(TokyoCabinet(self), 
		(const void *)IoSeq_rawBytes(key),  (int)IoSeq_rawSizeInBytes(key), 
		(const void *)IoSeq_rawBytes(value), (int)IoSeq_rawSizeInBytes(value));

	IOASSERT(result, tcbdberrmsg(tcbdbecode(TokyoCabinet(self))));

	return self;
}

IoObject *IoTokyoCabinet_atAppend(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinet atApple(keySymbol, valueSequence)
	Appends valueSequence to the current value at keySymbol. Returns self.
	*/

	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);
	IoSeq *value = IoMessage_locals_seqArgAt_(m, locals, 1);
	int result;

	IOASSERT(TokyoCabinet(self), "invalid TokyoCabinet");

	result = tcbdbputcat(TokyoCabinet(self), 
		(const void *)IoSeq_rawBytes(key),  (int)IoSeq_rawSizeInBytes(key), 
		(const void *)IoSeq_rawBytes(value), (int)IoSeq_rawSizeInBytes(value));

	IOASSERT(result, tcbdberrmsg(tcbdbecode(TokyoCabinet(self))));

	return self;
}

IoObject *IoTokyoCabinet_at(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinet at(keySymbol)
	Returns a Sequence for the value at the given key or nil if there is no such key.
	*/
	
	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);
	void *value;
	int size;

	IOASSERT(TokyoCabinet(self), "invalid TokyoCabinet");

	value = tcbdbget(TokyoCabinet(self), (const char *)IoSeq_rawBytes(key), IoSeq_rawSizeInBytes(key), &size);

	if (value)
	{
		IoSeq *v = IoSeq_newWithData_length_(IOSTATE, (unsigned char *)value, size);
		free(value);
		return v;
	}

	return IONIL(self);
}

IoObject *IoTokyoCabinet_sizeAt(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinet sizeAt(keySymbol)
	Returns the size of the value at the given key or nil if there is no such key.
	*/
	
	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);
	int size;

	IOASSERT(TokyoCabinet(self), "invalid TokyoCabinet");

	size = tcbdbvsiz(TokyoCabinet(self), (const void *)IoSeq_rawBytes(key), (int)IoSeq_rawSizeInBytes(key));

	if (size == -1)
	{
		return IONIL(self);
	}

	return IONUMBER(size);
}

IoObject *IoTokyoCabinet_removeAt(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinet atRemove(keySymbol)
	Removes the specified key. Returns self
	*/
	
	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);
	int result;
	IOASSERT(TokyoCabinet(self), "invalid TokyoCabinet");
	result = tcbdbout(TokyoCabinet(self), (const void *)IoSeq_rawBytes(key), (int)IoSeq_rawSizeInBytes(key));
	//IOASSERT(result, tcbdberrmsg(tcbdbecode(TokyoCabinet(self)))); // commented to avoid 'no item found' exception
	return self;
}

#include "IoTokyoCabinetCursor.h"

IoObject *IoTokyoCabinet_cursor(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinet cursor
	Returns a new cursor object.
	*/
	
	return IoTokyoCabinetCursor_newWithDB_(IOSTATE, TokyoCabinet(self));
}

#include "IoTokyoCabinetPrefixCursor.h"

IoObject *IoTokyoCabinet_prefixCursor(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc TokyoCabinet prefixCursor
	Returns a new prefix cursor object.
	*/
	
	return IoTokyoCabinetPrefixCursor_newWithDB_(IOSTATE, TokyoCabinet(self));
}
