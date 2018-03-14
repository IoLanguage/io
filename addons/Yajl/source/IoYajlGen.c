//metadoc YajlGenParser copyright Steve Dekorte, 2004
//metadoc YajlGenParser license BSD revised
/*metadoc YajlGenParser description
This object can be used to parse YajlGen / HTML / XML.
*/
//metadoc YajlGenParser category Parsers

#include "IoYajlGen.h"
#include "IoState.h"
#include "IoObject.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoError.h"
#include <ctype.h>

#define DATA(self) ((yajl_gen)(IoObject_dataPointer(self)))

static const char *protoId = "YajlGen";

IoTag *IoYajlGen_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoYajlGen_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoYajlGen_mark);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoYajlGen_free);
	return tag;
}



IoYajlGen *IoYajlGen_rawClone(IoYajlGen *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);

	yajl_gen g = yajl_gen_alloc(NULL);
	//yajl_gen_config(g, yajl_gen_beautify, 0);

	IoObject_setDataPointer_(self, g);
	return self;
}

IoYajlGen *IoYajlGen_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoYajlGen_mark(IoYajlGen *self)
{
}

void IoYajlGen_free(IoYajlGen *self)
{
	yajl_gen_free(DATA(self));
}

IoObject *IoYajlGen_pushNull(IoYajlGen *self, IoObject *locals, IoMessage *m)
{
	yajl_gen_null(DATA(self));
	return self;
}

IoObject *IoYajlGen_pushString(IoYajlGen *self, IoObject *locals, IoMessage *m)
{
	IoSeq *s = IoMessage_locals_seqArgAt_(m, locals, 0);

	yajl_gen_string(DATA(self),
		(const unsigned char *)IOSYMBOL_BYTES(s),
		IOSYMBOL_LENGTH(s));

	return self;
}

IoObject *IoYajlGen_pushInteger(IoYajlGen *self, IoObject *locals, IoMessage *m)
{
	int i = IoMessage_locals_intArgAt_(m, locals, 0);
	yajl_gen_integer(DATA(self), i);
	return self;
}

IoObject *IoYajlGen_pushDouble(IoYajlGen *self, IoObject *locals, IoMessage *m)
{
	double d = IoMessage_locals_doubleArgAt_(m, locals, 0);
	yajl_gen_double(DATA(self), d);
	return self;
}

IoObject *IoYajlGen_pushNumberString(IoYajlGen *self, IoObject *locals, IoMessage *m)
{
	IoSeq *s = IoMessage_locals_seqArgAt_(m, locals, 0);

	yajl_gen_number(DATA(self),
		(const  char *)IOSYMBOL_BYTES(s),
		IOSYMBOL_LENGTH(s));

	return self;
}

IoObject *IoYajlGen_pushBool(IoYajlGen *self, IoObject *locals, IoMessage *m)
{
	int b = IoMessage_locals_boolArgAt_(m, locals, 0);
	yajl_gen_bool(DATA(self), b);
	return self;
}


IoObject *IoYajlGen_openMap(IoYajlGen *self, IoObject *locals, IoMessage *m)
{
	yajl_gen_map_open(DATA(self));
	return self;
}

IoObject *IoYajlGen_closeMap(IoYajlGen *self, IoObject *locals, IoMessage *m)
{
	yajl_gen_map_close(DATA(self));
	return self;
}

IoObject *IoYajlGen_openArray(IoYajlGen *self, IoObject *locals, IoMessage *m)
{
	yajl_gen_array_open(DATA(self));
	return self;
}

IoObject *IoYajlGen_closeArray(IoYajlGen *self, IoObject *locals, IoMessage *m)
{
	yajl_gen_array_close(DATA(self));
	return self;
}

IoObject *IoYajlGen_generate(IoYajlGen *self, IoObject *locals, IoMessage *m)
{
	const unsigned char *jsonBuffer;
	size_t jsonBufferLength;

	yajl_gen_get_buf(DATA(self), &jsonBuffer, &jsonBufferLength);

	IoSeq *out = IOSEQ(jsonBuffer, jsonBufferLength);

	yajl_gen_free(DATA(self));
	yajl_gen g = yajl_gen_alloc(NULL);
	//yajl_gen_config(g, yajl_gen_beautify, 0);
	IoObject_setDataPointer_(self, g);

    return out;
}

IoYajlGen *IoYajlGen_proto(void *state)
{
	IoYajlGen *self = IoObject_new(state);
	IoObject_tag_(self, IoYajlGen_newTag(state));

	yajl_gen g = yajl_gen_alloc(NULL);
	//yajl_gen_config(g, yajl_gen_beautify, 0);

	IoObject_setDataPointer_(self, g);

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] =
		{
			{"pushNull", IoYajlGen_pushNull},
			{"pushString", IoYajlGen_pushString},
			{"pushInteger", IoYajlGen_pushInteger},
			{"pushDouble", IoYajlGen_pushDouble},
			{"pushNumberString", IoYajlGen_pushNumberString},
			{"pushBool", IoYajlGen_pushBool},
			{"openMap", IoYajlGen_openMap},
			{"closeMap", IoYajlGen_closeMap},
			{"openArray", IoYajlGen_openArray},
			{"closeArray", IoYajlGen_closeArray},
			{"generate", IoYajlGen_generate},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}
