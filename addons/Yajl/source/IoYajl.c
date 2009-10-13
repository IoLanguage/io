//metadoc YajlParser copyright Steve Dekorte, 2004
//metadoc YajlParser license BSD revised
/*metadoc YajlParser description
This object can be used to parse Yajl / HTML / XML.
*/
//metadoc YajlParser category XML

#include "IoYajl.h"
#include "IoState.h"
#include "IoObject.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoError.h"
#include <ctype.h>

#define DATA(self) ((IoYajlData *)(IoObject_dataPointer(self)))

IoTag *IoYajl_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("YajlParser");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoYajl_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoYajl_mark);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoYajl_free);
	return tag;
}

IoYajl *IoYajl_proto(void *state)
{
	IoYajl *self = IoObject_new(state);
	IoObject_tag_(self, IoYajl_newTag(state));

	IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoYajlData)));

	DATA(self)->addValueMessage = IoMessage_newWithName_label_(state,
													   IOSYMBOL("addValue"), IOSYMBOL("YajlParser"));
													   
	DATA(self)->startArrayMessage = IoMessage_newWithName_label_(state,
													   IOSYMBOL("startArray"), IOSYMBOL("YajlParser"));

	DATA(self)->endArrayMessage = IoMessage_newWithName_label_(state,
													 IOSYMBOL("endArray"), IOSYMBOL("YajlParser"));

	DATA(self)->startMapMessage = IoMessage_newWithName_label_(state,
													   IOSYMBOL("startMap"), IOSYMBOL("YajlParser"));

	DATA(self)->endMapMessage = IoMessage_newWithName_label_(state,
												   IOSYMBOL("endMap"), IOSYMBOL("YajlParser"));

	DATA(self)->addMapKeyMessage = IoMessage_newWithName_label_(state,
												   IOSYMBOL("addMapKey"), IOSYMBOL("YajlParser"));

	IoState_registerProtoWithFunc_(state, self, IoYajl_proto);

	{
		IoMethodTable methodTable[] = {
		{"parse", IoYajl_parse},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}


IoYajl *IoYajl_rawClone(IoYajl *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(DATA(proto), sizeof(IoYajlData)));
	return self;
}

IoYajl *IoYajl_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoYajl_proto);
	return IOCLONE(proto);
}

void IoYajl_mark(IoYajl *self)
{
	IoObject_shouldMark(DATA(self)->addValueMessage);
	IoObject_shouldMark(DATA(self)->startArrayMessage);
	IoObject_shouldMark(DATA(self)->endArrayMessage);
	IoObject_shouldMark(DATA(self)->startMapMessage);
	IoObject_shouldMark(DATA(self)->endMapMessage);
	IoObject_shouldMark(DATA(self)->addMapKeyMessage);
}

void IoYajl_free(IoYajl *self)
{


	io_free(DATA(self));
}

/* ---  callbacks ---------------------------------- */

static int IoYajl_callback_null(void *ctx)  
{  
	IoYajl *self = ctx;
	//IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->addValueMessage;
	IoMessage_setCachedArg_to_(m, 0, IONIL(self));
	IoObject_perform(self, self, m);
	}
	//IoState_popRetainPool(IOSTATE);
    return 1;  
}  
  
static int IoYajl_callback_boolean(void *ctx, int boolean)  
{  
	IoYajl *self = ctx;
	//IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->addValueMessage;
	IoMessage_setCachedArg_to_(m, 0, boolean ? IOTRUE(self) : IOFALSE(self));
	IoObject_perform(self, self, m);
	}
	//IoState_popRetainPool(IOSTATE);
    return 1;  
}  
  
#include <stdlib.h>
	 
static int IoYajl_callback_number(void *ctx, const char * s, unsigned int l)  
{  
	IoYajl *self = ctx;
	//IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->addValueMessage;
	float f = atof(s);
	IoMessage_setCachedArg_to_(m, 0, IONUMBER(f));
	IoObject_perform(self, self, m);
	}
	//IoState_popRetainPool(IOSTATE);
    return 1;  
}  

static int IoYajl_callback_string(void *ctx, const unsigned char * stringVal,  
                           unsigned int stringLen)  
{  
	IoYajl *self = ctx;
	//IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->addValueMessage;
	IoMessage_setCachedArg_to_(m, 0, IOSEQ(stringVal, stringLen));
	IoObject_perform(self, self, m);
	}
	//IoState_popRetainPool(IOSTATE);
    return 1;  
}   
  
static int IoYajl_callback_map_key(void *ctx, const unsigned char * stringVal,  
                            unsigned int stringLen)  
{  
	IoYajl *self = ctx;
	//IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->addMapKeyMessage;
	IoMessage_setCachedArg_to_(m, 0, IOSEQ(stringVal, stringLen));
	IoObject_perform(self, self, m);
	}
	//IoState_popRetainPool(IOSTATE);
    return 1;  
}  
  
static int IoYajl_callback_start_map(void *ctx)  
{  
	IoYajl *self = ctx;
	//IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->startMapMessage;
	IoObject_perform(self, self, m);
	}
	//IoState_popRetainPool(IOSTATE);
    return 1;  
}

static int IoYajl_callback_end_map(void *ctx)  
{  
	IoYajl *self = ctx;
	//IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->endMapMessage;
	IoObject_perform(self, self, m);
	}
	//IoState_popRetainPool(IOSTATE);
    return 1;  
}  
  
static int IoYajl_callback_start_array(void *ctx)  
{  
	IoYajl *self = ctx;
	//IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->startArrayMessage;
	IoObject_perform(self, self, m);
	}
	//IoState_popRetainPool(IOSTATE);
    return 1;  
}  
  
static int IoYajl_callback_end_array(void *ctx)  
{  
	IoYajl *self = ctx;
	////IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->endArrayMessage;
	IoObject_perform(self, self, m);
	}
	////IoState_popRetainPool(IOSTATE);
    return 1;  
}  
  
static yajl_callbacks callbacks = {  
    IoYajl_callback_null,  
    IoYajl_callback_boolean,  
    NULL,  
    NULL,  
    IoYajl_callback_number,  
    IoYajl_callback_string,  
    IoYajl_callback_start_map,  
    IoYajl_callback_map_key,  
    IoYajl_callback_end_map,  
    IoYajl_callback_start_array,  
    IoYajl_callback_end_array  
};  
  
IoObject *IoYajl_parse(IoYajl *self, IoObject *locals, IoMessage *m)
{
	IoSeq *dataSeq = IoMessage_locals_seqArgAt_(m, locals, 0);
	size_t dataSize = IoSeq_rawSizeInBytes(dataSeq);
	const unsigned char *data = (const unsigned char *)CSTRING(dataSeq);
	
	yajl_parser_config cfg = { 1, 0 };
	yajl_handle hand = yajl_alloc(&callbacks, &cfg,  NULL, (void *) self);  
    yajl_status stat;        
	
	stat = yajl_parse(hand, data, dataSize);  
	stat = yajl_parse_complete(hand);  

	if (stat != yajl_status_ok &&  
		stat != yajl_status_insufficient_data)  
	{  
		char *str = (char *)yajl_get_error(hand, 1, data, dataSize);
		IoObject *error = IoError_newWithCStringMessage_(IOSTATE, str);
		yajl_free_error(hand, (unsigned char *)str);
		yajl_free(hand);
		return error;
		//fprintf(stderr, (const char *) str);
		
	} 

    yajl_free(hand);
      
    return self;  
}

/*
void IoYajl_startElementHandler(Yajl_PARSER *parser,
							   void *userContext,
							   const char *elementName)
{
	IoYajl *self = userContext;
	//IoState_pushRetainPool(IOSTATE);
	{
	char *e = IoYajl_lowercase_(self, elementName);
	IoMessage *m = DATA(self)->startElementMessage;
	IoMessage_setCachedArg_to_(m, 0, IOSYMBOL(e));
	IoObject_perform(self, self, m);
	}
	//IoState_popRetainPool(IOSTATE);
}

void IoYajl_endElementHandler(Yajl_PARSER *parser,
							 void *userContext,
							 const char *elementName)
{
	IoYajl *self = userContext;
	//IoState_pushRetainPool(IOSTATE);
	{
	char *e = IoYajl_lowercase_(self, elementName);
	IoMessage *m = DATA(self)->endElementMessage;

	IoMessage_setCachedArg_to_(m, 0, IOSYMBOL(e));
	IoObject_perform(self, self, m);
	}
	//IoState_popRetainPool(IOSTATE);
}

void IoYajl_newAttributeHandler(Yajl_PARSER *parser,
							   void *userContext,
							   const char *attributeName,
							   const char *attributeValue)
{
	IoYajl *self = userContext;
	//IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->newAttributeMessage;

	{
		char *k = IoYajl_lowercase_(self, attributeName);
		char *v = (char *)attributeValue;
		k = k ? k : "";
		v = v ? v : "";
		IoMessage_setCachedArg_to_(m, 0, IOSYMBOL(k));
		IoMessage_setCachedArg_to_(m, 1, IOSYMBOL(v));
		IoObject_perform(self, self, m);
	}
	}
	//IoState_popRetainPool(IOSTATE);
}
*/
