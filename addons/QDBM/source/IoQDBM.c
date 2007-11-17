/*#io
QDBM ioDoc(
           docCopyright("Steve Dekorte", 2002)
           docLicense("BSD revised")
           docDescription("A key/value database.")
		 docCategory("Databases")
           */

#include "IoQDBM.h"

#include <depot.h>
#include <cabin.h>
#include <villa.h>
#include <stdlib.h>

#include "IoObject.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoState.h"
#include "IoNumber.h"

#define QDBM(self) ((VILLA *)(IoObject_dataPointer(self)))

int pathCompare(const char *p1, const char *p2, char sepChar)
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

int pathCompareFunc(const char *aptr, int asiz, const char *bptr, int bsiz)
{
	return pathCompare(aptr, bptr, '/');
}


int compareFunc(const char *aptr, int asiz, const char *bptr, int bsiz)
{
	return strcmp(aptr, bptr);
	/*
	//int r = strncmp(aptr, bptr, max);
	if (r > 0) return -1;
	if (r < 0) return 1;
	return 0;
	*/
}

/*
int compareStrNumFunc(const char *a, int asize, const char *b, int bsize)
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

int compareStrNumFunc(const char *a, int asize, const char *b, int bsize)
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

IoTag *IoQDBM_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("QDBM");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoQDBM_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoQDBM_rawClone);
	//IoTag_markFunc_(tag, (IoTagMarkFunc *)IoQDBM_mark);
	return tag;
}

IoQDBM *IoQDBM_proto(void *state)
{
	IoMethodTable methodTable[] = {
	{"open",      IoQDBM_open},    
	{"close",     IoQDBM_close},    
	
	{"atPut",     IoQDBM_atPut}, 
	{"at",        IoQDBM_at},
	{"sizeAt",    IoQDBM_sizeAt},
	{"removeAt",  IoQDBM_removeAt},
	//{"sync",      IoQDBM_sync},   
	
	{"size",      IoQDBM_size},   
	{"optimize",  IoQDBM_optimize},  
	{"name",      IoQDBM_name},  
	 
	{"begin",  IoQDBM_begin},
	{"commit",  IoQDBM_commit},
	{"abort",  IoQDBM_abort},
	
	{"cursorFirst",  IoQDBM_cursorFirst},
	{"cursorLast",  IoQDBM_cursorLast},
	{"cursorPrevious",  IoQDBM_cursorPrevious},
	{"cursorNext",  IoQDBM_cursorNext},
	{"cursorJumpForward",  IoQDBM_cursorJumpForward},
	{"cursorJumpBackward",  IoQDBM_cursorJumpBackward},
	{"cursorKey",  IoQDBM_cursorKey},
	{"cursorValue",  IoQDBM_cursorValue},
	{"cursorPut",  IoQDBM_cursorPut},
	{"cursorRemove",  IoQDBM_cursorRemove},
		
	{NULL, NULL},
	};
	
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoQDBM_newTag(state));
	
	IoObject_setDataPointer_(self, NULL);
	IoState_registerProtoWithFunc_((IoState *)state, self, IoQDBM_proto);
	
	IoObject_addMethodTable_(self, methodTable);
	return self;
}

IoQDBM *IoQDBM_rawClone(IoQDBM *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_tag_(self, IoObject_tag(proto));
	IoObject_setDataPointer_(self, NULL);
	return self; 
}

IoQDBM *IoQDBM_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoQDBM_proto);
	return IOCLONE(proto);
}

void IoQDBM_free(IoQDBM *self) 
{	
	if(QDBM(self)) 
	{
		vlclose(QDBM(self));
		IoObject_setDataPointer_(self, NULL);
	}
}

/*
void IoQDBM_mark(IoQDBM *self) 
{
}
*/

// -------------------------------------------------------- 

IoObject *IoQDBM_open(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("open(path)", "Opens the database.")
	*/
	VILLA *villa;
	IoSeq *path = IoMessage_locals_seqArgAt_(m, locals, 0);
	VLCFUNC cf = VL_CMPLEX;
	
	if(IoMessage_argCount(m) > 1)
	{
		IoSeq *compareType = IoMessage_locals_seqArgAt_(m, locals, 1);
		//printf("using compareType:%s\n", CSTRING(compareType));

		if(strcmp(CSTRING(compareType), "VL_CMPDEC") == 0) cf = VL_CMPDEC;
		else
		if(strcmp(CSTRING(compareType), "VL_CMPINT") == 0) cf = VL_CMPINT;
		else
		if(strcmp(CSTRING(compareType), "VL_CMPNUM") == 0) cf = VL_CMPNUM;
		else
		if(strcmp(CSTRING(compareType), "VL_CMPLEX") == 0) cf = VL_CMPLEX;
		else
		if(strcmp(CSTRING(compareType), "VL_CMPSNM") == 0) 
		{
			cf = compareStrNumFunc;
			//printf("using compareStrNumFunc\n");
		}
		else
		if(strcmp(CSTRING(compareType), "VL_CMPPTH") == 0) 
		{
			cf = pathCompareFunc;
			//printf("using pathCompareFunc\n");
		}		
		else
		{
			fprintf(stderr, "ivalid compare function name\n");
			return IONIL(self);
		}
	}
	
	if(!(villa = vlopen(CSTRING(path), VL_OWRITER | VL_OCREAT, cf)))
	{
		fprintf(stderr, "dpopen failed\n");
		return IONIL(self);
	}
	
	IoObject_setDataPointer_(self, villa);
	
	return self;
}

IoObject *IoQDBM_close(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("close", "Closes the database.")
	*/
	
	if(QDBM(self)) 
	{
		vlclose(QDBM(self));
		IoObject_setDataPointer_(self, NULL);
	}
	
	return self;
}

IoObject *IoQDBM_sync(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("sync", "Syncs the database. Returns self")
	*/
	int result;
	IOASSERT(QDBM(self), "invalid QDBM");
	result = vlsync(QDBM(self));
	IOASSERT(result, dperrmsg(dpecode));
	return self;
}

IoObject *IoQDBM_size(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("size", "Returns number of records in database. Returns self")
	*/
	int result;
	IOASSERT(QDBM(self), "invalid QDBM");
	result = vlrnum(QDBM(self));
	return IONUMBER(result);
}

IoObject *IoQDBM_optimize(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("optimize", "Optimizes the database. Returns self")
	*/
	
	IOASSERT(QDBM(self), "invalid QDBM");
	IOASSERT(vloptimize(QDBM(self)), dperrmsg(dpecode));
	return self;
}

IoObject *IoQDBM_name(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("name", "Returns the name of the database.")
	*/
	
	IOASSERT(QDBM(self), "invalid QDBM");
	return IOSYMBOL(vlname(QDBM(self)));
}

IoObject *IoQDBM_begin(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("begin", "Begin transaction. Returns self")
	*/

	IOASSERT(QDBM(self), "invalid QDBM");
	IOASSERT(vltranbegin(QDBM(self)), dperrmsg(dpecode));
	return self;
}

IoObject *IoQDBM_commit(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("commit", "Commit transaction. Returns self")
	*/

	IOASSERT(QDBM(self), "invalid QDBM");
	IOASSERT(vltrancommit(QDBM(self)), dperrmsg(dpecode));
	return self;
}

IoObject *IoQDBM_abort(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("abort", "Abort transaction. Returns self")
	*/
	int result;
	IOASSERT(QDBM(self), "invalid QDBM");
	result = vltranabort(QDBM(self));
	IOASSERT(result, dperrmsg(dpecode));
	return self;
}

// ---------------------------------------------------------------------------------

IoObject *IoQDBM_atPut(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("atPut(keySymbol, valueSequence)", "Sets the value of valueSequence with the key keySymbol. Returns self.")
	*/
	
	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);
	IoSeq *value = IoMessage_locals_seqArgAt_(m, locals, 1);
	int result;
	
	IOASSERT(QDBM(self), "invalid QDBM");
		
	result = vlput(QDBM(self), (const char *)IoSeq_rawBytes(key), IoSeq_rawSizeInBytes(key), (const char *)IoSeq_rawBytes(value), IoSeq_rawSizeInBytes(value), VL_DOVER);
	
	IOASSERT(result, dperrmsg(dpecode));
	
	return self;
}

IoObject *IoQDBM_at(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("at(keySymbol)", "Returns a Sequence for the value at the given key or nil if there is no such key.")
	*/
	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);
	char *value;
	int size;
	
	IOASSERT(QDBM(self), "invalid QDBM");
	
	value = vlget(QDBM(self), (const char *)IoSeq_rawBytes(key), IoSeq_rawSizeInBytes(key), &size);
	
	if (value)
	{
		IoSeq *v = IoSeq_newWithData_length_(IOSTATE, (unsigned char *)value, size);
		free(value);
		return v;
	}
	
	return IONIL(self);
}

IoObject *IoQDBM_sizeAt(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("sizeAt(keySymbol)", "Returns the size of the value at the given key or nil if there is no such key.")
	*/
	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);
	int size;
	
	IOASSERT(QDBM(self), "invalid QDBM");
	
	size = vlvsiz(QDBM(self), (const char *)IoSeq_rawBytes(key), IoSeq_rawSizeInBytes(key));
	
	if (size == -1)
	{
		return IONIL(self);
	}
	
	return IONUMBER(size);
}

IoObject *IoQDBM_removeAt(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("atRemove(keySymbol)", "Removes the specified key. Returns self")
	*/
	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);
	int result;
	IOASSERT(QDBM(self), "invalid QDBM");
	result = vloutlist(QDBM(self), (const char *)IoSeq_rawBytes(key), IoSeq_rawSizeInBytes(key));
	//IOASSERT(result, dperrmsg(dpecode)); // commented to avoid 'no item found' exception
	return self;
}

IoObject *IoQDBM_cursorFirst(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("cursorFirst", "Move cursor to first record. Returns self")
	*/

	IOASSERT(QDBM(self), "invalid QDBM");
	return IOBOOL(self, vlcurfirst(QDBM(self)));
}

IoObject *IoQDBM_cursorLast(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("cursorLast", "Move cursor to last record. Returns self")
	*/

	IOASSERT(QDBM(self), "invalid QDBM");
	return IOBOOL(self, vlcurlast(QDBM(self)));
}

IoObject *IoQDBM_cursorPrevious(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("cursorPrevious", "Move cursor to previous record. Returns true if there is another key, or false if there is no previous record.")
	*/
	
	IOASSERT(QDBM(self), "invalid QDBM");
	return IOBOOL(self, vlcurprev(QDBM(self)));
}

IoObject *IoQDBM_cursorNext(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("cursorNext", "Move cursor to next record. Returns true if there is another key, or false if there is no next record.")
	*/
	
	IOASSERT(QDBM(self), "invalid QDBM");
	return IOBOOL(self, vlcurnext(QDBM(self)));
}

IoObject *IoQDBM_cursorJumpForward(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("cursorJumpForward(key)", "Move cursor to next record around key. Returns self")
	*/
	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);
	
	IOASSERT(QDBM(self), "invalid QDBM");
	//IOASSERT(vlcurjump(QDBM(self), (const char *)IoSeq_rawBytes(key), IoSeq_rawSizeInBytes(key), VL_JFORWARD), dperrmsg(dpecode));
	return IOBOOL(self, vlcurjump(QDBM(self), (const char *)IoSeq_rawBytes(key), IoSeq_rawSizeInBytes(key), VL_JFORWARD));
}

IoObject *IoQDBM_cursorJumpBackward(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("cursorJumpBackward(key)", "Move cursor to previous record around key. Returns self")
	*/
	IoSeq *key = IoMessage_locals_seqArgAt_(m, locals, 0);
	
	IOASSERT(QDBM(self), "invalid QDBM");
	//IOASSERT(vlcurjump(QDBM(self), (const char *)IoSeq_rawBytes(key), IoSeq_rawSizeInBytes(key), VL_JBACKWARD), dperrmsg(dpecode));
	return IOBOOL(self, vlcurjump(QDBM(self), (const char *)IoSeq_rawBytes(key), IoSeq_rawSizeInBytes(key), VL_JBACKWARD));
}

IoObject *IoQDBM_cursorKey(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("cursorKey", "Returns current cursor key or nil.")
	*/
	int size;
	char *value;
	
	IOASSERT(QDBM(self), "invalid QDBM");
	value = vlcurkey(QDBM(self), &size);
	
	if (value)
	{
		IoSeq *s = IoSeq_newWithData_length_(IOSTATE, (unsigned char *)value, size);
		free(value);
		return s;
	}
	
	return IONIL(self);
}

IoObject *IoQDBM_cursorValue(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("cursorValue", "Returns current cursor value or nil.")
	*/
	int size;
	char *value;
	
	IOASSERT(QDBM(self), "invalid QDBM");
	value = vlcurval(QDBM(self), &size);
	
	if (value)
	{
		IoSeq *s = IoSeq_newWithData_length_(IOSTATE, (unsigned char *)value, size);
		free(value);
		return s;
	}
	
	return IONIL(self);
}

IoObject *IoQDBM_cursorPut(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("cursorPut(value)", "Sets the value at the current cursor postion. Returns self.")
	*/
	IoSeq *value = IoMessage_locals_seqArgAt_(m, locals, 0);

	IOASSERT(QDBM(self), "invalid QDBM");

	IOASSERT(vlcurput(QDBM(self), (const char *)IoSeq_rawBytes(value), IoSeq_rawSizeInBytes(value), VL_CPCURRENT), dperrmsg(dpecode));
	
	return self;
}

IoObject *IoQDBM_cursorRemove(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("cursorRemove", "Removes the current cursor postion. Returns self.")
	*/
	
	IOASSERT(QDBM(self), "invalid QDBM");
	
	IOASSERT(vlcurout(QDBM(self)), dperrmsg(dpecode));
	
	return self;
}
