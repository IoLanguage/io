//metadoc YajilParser copyright Steve Dekorte, 2004
//metadoc YajilParser license BSD revised
/*metadoc YajilParser description
This object can be used to parse Yajil / HTML / XML.
*/
//metadoc YajilParser category XML

#include "IoYajil.h"
#include "IoState.h"
#include "IoObject.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include <ctype.h>

#define DATA(self) ((IoYajilData *)(IoObject_dataPointer(self)))

IoTag *IoYajil_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("YajilParser");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoYajil_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoYajil_mark);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoYajil_free);
	return tag;
}

IoYajil *IoYajil_proto(void *state)
{
	IoYajil *self = IoObject_new(state);
	IoObject_tag_(self, IoYajil_newTag(state));

	IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoYajilData)));

	DATA(self)->addValueMessage = IoMessage_newWithName_label_(state,
													   IOSYMBOL("addValue"), IOSYMBOL("YajilParser"));
													   
	DATA(self)->startArrayMessage = IoMessage_newWithName_label_(state,
													   IOSYMBOL("startArray"), IOSYMBOL("YajilParser"));

	DATA(self)->endArrayMessage = IoMessage_newWithName_label_(state,
													 IOSYMBOL("endArray"), IOSYMBOL("YajilParser"));

	DATA(self)->startMapMessage = IoMessage_newWithName_label_(state,
													   IOSYMBOL("startMap"), IOSYMBOL("YajilParser"));

	DATA(self)->endMapMessage = IoMessage_newWithName_label_(state,
												   IOSYMBOL("endMap"), IOSYMBOL("YajilParser"));

	DATA(self)->addMapKeyMessage = IoMessage_newWithName_label_(state,
												   IOSYMBOL("addMapKey"), IOSYMBOL("YajilParser"));

	IoState_registerProtoWithFunc_(state, self, IoYajil_proto);

	{
		IoMethodTable methodTable[] = {
		{"parse", IoYajil_parse},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}


IoYajil *IoYajil_rawClone(IoYajil *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(DATA(proto), sizeof(IoYajilData)));
	return self;
}

IoYajil *IoYajil_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoYajil_proto);
	return IOCLONE(proto);
}

void IoYajil_mark(IoYajil *self)
{
	IoObject_shouldMark(DATA(self)->addValueMessage);
	IoObject_shouldMark(DATA(self)->startArrayMessage);
	IoObject_shouldMark(DATA(self)->endArrayMessage);
	IoObject_shouldMark(DATA(self)->startMapMessage);
	IoObject_shouldMark(DATA(self)->endMapMessage);
	IoObject_shouldMark(DATA(self)->addMapKeyMessage);
}

void IoYajil_free(IoYajil *self)
{


	io_free(DATA(self));
}

/* ---  callbacks ---------------------------------- */

static int IoYajil_callback_null(void *ctx)  
{  
	IoYajil *self = ctx;
	IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->addValueMessage;
	IoMessage_setCachedArg_to_(m, 0, IONIL(self));
	IoObject_perform(self, self, m);
	}
	IoState_popRetainPool(IOSTATE);
    return 1;  
}  
  
static int IoYajil_callback_boolean(void *ctx, int boolean)  
{  
	IoYajil *self = ctx;
	IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->addValueMessage;
	IoMessage_setCachedArg_to_(m, 0, boolean ? IOTRUE(self) : IOFALSE(self));
	IoObject_perform(self, self, m);
	}
	IoState_popRetainPool(IOSTATE);
    return 1;  
}  
  
#include <stdlib.h>
	 
static int IoYajil_callback_number(void *ctx, const char * s, unsigned int l)  
{  
	IoYajil *self = ctx;
	IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->addValueMessage;
	float f = atof(s);
	IoMessage_setCachedArg_to_(m, 0, IONUMBER(f));
	IoObject_perform(self, self, m);
	}
	IoState_popRetainPool(IOSTATE);
    return 1;  
}  

static int IoYajil_callback_string(void *ctx, const unsigned char * stringVal,  
                           unsigned int stringLen)  
{  
	IoYajil *self = ctx;
	IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->addValueMessage;
	IoMessage_setCachedArg_to_(m, 0, IOSEQ(stringVal, stringLen));
	IoObject_perform(self, self, m);
	}
	IoState_popRetainPool(IOSTATE);
    return 1;  
}   
  
static int IoYajil_callback_map_key(void *ctx, const unsigned char * stringVal,  
                            unsigned int stringLen)  
{  
	IoYajil *self = ctx;
	IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->addMapKeyMessage;
	IoMessage_setCachedArg_to_(m, 0, IOSEQ(stringVal, stringLen));
	IoObject_perform(self, self, m);
	}
	IoState_popRetainPool(IOSTATE);
    return 1;  
}  
  
static int IoYajil_callback_start_map(void *ctx)  
{  
	IoYajil *self = ctx;
	IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->startMapMessage;
	IoObject_perform(self, self, m);
	}
	IoState_popRetainPool(IOSTATE);
    return 1;  
}

static int IoYajil_callback_end_map(void *ctx)  
{  
	IoYajil *self = ctx;
	IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->endMapMessage;
	IoObject_perform(self, self, m);
	}
	IoState_popRetainPool(IOSTATE);
    return 1;  
}  
  
static int IoYajil_callback_start_array(void *ctx)  
{  
	IoYajil *self = ctx;
	IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->startArrayMessage;
	IoObject_perform(self, self, m);
	}
	IoState_popRetainPool(IOSTATE);
    return 1;  
}  
  
static int IoYajil_callback_end_array(void *ctx)  
{  
	IoYajil *self = ctx;
	IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->endArrayMessage;
	IoObject_perform(self, self, m);
	}
	IoState_popRetainPool(IOSTATE);
    return 1;  
}  
  
static yajl_callbacks callbacks = {  
    IoYajil_callback_null,  
    IoYajil_callback_boolean,  
    NULL,  
    NULL,  
    IoYajil_callback_number,  
    IoYajil_callback_string,  
    IoYajil_callback_start_map,  
    IoYajil_callback_map_key,  
    IoYajil_callback_end_map,  
    IoYajil_callback_start_array,  
    IoYajil_callback_end_array  
};  
  
IoObject *IoYajil_parse(IoYajil *self, IoObject *locals, IoMessage *m)
{
	IoSeq *dataSeq = IoMessage_locals_seqArgAt_(m, locals, 0);
	size_t dataSize = IoSeq_rawSizeInBytes(dataSeq);
	const unsigned char *data = (const unsigned char *)CSTRING(dataSeq);
	
	yajl_parser_config cfg = { 1, 1 };  
	yajl_handle hand = yajl_alloc(&callbacks, &cfg,  NULL, (void *) self);  
    yajl_status stat;        
	
	stat = yajl_parse(hand, data, dataSize);  
	stat = yajl_parse_complete(hand);  

	if (stat != yajl_status_ok &&  
	stat != yajl_status_insufficient_data)  
	{  
		unsigned char * str = yajl_get_error(hand, 1, data, dataSize);  
		fprintf(stderr, (const char *) str);  
		yajl_free_error(hand, str);  
	} 

    yajl_free(hand);  
      
    return self;  
}  
/*
void IoYajil_startElementHandler(Yajil_PARSER *parser,
							   void *userContext,
							   const char *elementName)
{
	IoYajil *self = userContext;
	IoState_pushRetainPool(IOSTATE);
	{
	char *e = IoYajil_lowercase_(self, elementName);
	IoMessage *m = DATA(self)->startElementMessage;
	IoMessage_setCachedArg_to_(m, 0, IOSYMBOL(e));
	IoObject_perform(self, self, m);
	}
	IoState_popRetainPool(IOSTATE);
}

void IoYajil_endElementHandler(Yajil_PARSER *parser,
							 void *userContext,
							 const char *elementName)
{
	IoYajil *self = userContext;
	IoState_pushRetainPool(IOSTATE);
	{
	char *e = IoYajil_lowercase_(self, elementName);
	IoMessage *m = DATA(self)->endElementMessage;

	IoMessage_setCachedArg_to_(m, 0, IOSYMBOL(e));
	IoObject_perform(self, self, m);
	}
	IoState_popRetainPool(IOSTATE);
}

void IoYajil_newAttributeHandler(Yajil_PARSER *parser,
							   void *userContext,
							   const char *attributeName,
							   const char *attributeValue)
{
	IoYajil *self = userContext;
	IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->newAttributeMessage;

	{
		char *k = IoYajil_lowercase_(self, attributeName);
		char *v = (char *)attributeValue;
		k = k ? k : "";
		v = v ? v : "";
		IoMessage_setCachedArg_to_(m, 0, IOSYMBOL(k));
		IoMessage_setCachedArg_to_(m, 1, IOSYMBOL(v));
		IoObject_perform(self, self, m);
	}
	}
	IoState_popRetainPool(IOSTATE);
}

*/

