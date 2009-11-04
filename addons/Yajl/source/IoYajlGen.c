//metadoc YajlGenParser copyright Steve Dekorte, 2004
//metadoc YajlGenParser license BSD revised
/*metadoc YajlGenParser description
This object can be used to parse YajlGen / HTML / XML.
*/
//metadoc YajlGenParser category XML

#include "IoYajlGen.h"
#include "IoState.h"
#include "IoObject.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoError.h"
#include <ctype.h>

#define DATA(self) ((yajl_gen)(IoObject_dataPointer(self)))

IoTag *IoYajlGen_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("YajlGen");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoYajlGen_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoYajlGen_mark);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoYajlGen_free);
	return tag;
}

IoYajlGen *IoYajlGen_proto(void *state)
{
	IoYajlGen *self = IoObject_new(state);
	IoObject_tag_(self, IoYajlGen_newTag(state));

	yajl_gen_config config = { 0, "" };
	IoObject_setDataPointer_(self, yajl_gen_alloc(&config, NULL));

	IoState_registerProtoWithFunc_(state, self, IoYajlGen_proto);

	{
		IoMethodTable methodTable[] = 
		{
			{"generate", IoYajlGen_generate},
			{"pushString", IoYajlGen_pushString},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
		
	return self;
}

IoYajlGen *IoYajlGen_rawClone(IoYajlGen *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	yajl_gen_config config = { 0, "" };
	IoObject_setDataPointer_(self, yajl_gen_alloc(&config, NULL));
	return self;
}

IoYajlGen *IoYajlGen_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoYajlGen_proto);
	return IOCLONE(proto);
}

void IoYajlGen_mark(IoYajlGen *self)
{
}

void IoYajlGen_free(IoYajlGen *self)
{
	yajl_gen_free(DATA(self));
}

IoObject *IoYajlGen_pushString(IoYajlGen *self, IoObject *locals, IoMessage *m)
{
	IoSeq *s = IoMessage_locals_seqArgAt_(m, locals, 0);
	
	yajl_gen_string(DATA(self), 
		(const unsigned char *)IOSYMBOL_BYTES(s), 
		IOSYMBOL_LENGTH(s));

	return self;
}

IoObject *IoYajlGen_generate(IoYajlGen *self, IoObject *locals, IoMessage *m)
{
	const unsigned char *jsonBuffer;
	unsigned int jsonBufferLength;
		
	yajl_gen_get_buf(DATA(self), &jsonBuffer, &jsonBufferLength);
	
	IoSeq *out = IOSEQ(jsonBuffer, jsonBufferLength);
	
	yajl_gen_free(DATA(self));
	yajl_gen_config config = { 0, "" };
	IoObject_setDataPointer_(self, yajl_gen_alloc(&config, NULL));
	
    return out;  
}
