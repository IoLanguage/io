//metadoc SGMLParser copyright Steve Dekorte, 2004
//metadoc SGMLParser license BSD revised
//metadoc SGMLParser category Parsers
/*metadoc SGMLParser description
This object can be used to parse SGML / HTML / XML.
*/

#include "IoSGMLParser.h"
#include "IoState.h"
#include "IoObject.h"
#include "IoSeq.h"
#include <ctype.h>

#define DATA(self) ((IoSGMLParserData *)(IoObject_dataPointer(self)))
static const char *protoId = "SGMLParser";

IoTag *IoSGMLParser_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSGMLParser_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoSGMLParser_mark);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSGMLParser_free);
	return tag;
}

IoSGMLParser *IoSGMLParser_proto(void *state)
{
	IoSGMLParser *self = IoObject_new(state);
	IoObject_tag_(self, IoSGMLParser_newTag(state));

	IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoSGMLParserData)));

	DATA(self)->startElementMessage = IoMessage_newWithName_label_(state,
													   IOSYMBOL("startElement"), IOSYMBOL("SGMLParser"));

	DATA(self)->endElementMessage = IoMessage_newWithName_label_(state,
													 IOSYMBOL("endElement"), IOSYMBOL("SGMLParser"));

	DATA(self)->newAttributeMessage = IoMessage_newWithName_label_(state,
													   IOSYMBOL("newAttribute"), IOSYMBOL("SGMLParser"));

	DATA(self)->newTextMessage = IoMessage_newWithName_label_(state,
												   IOSYMBOL("newText"), IOSYMBOL("SGMLParser"));

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"parse", IoSGMLParser_parse},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

void IoSGMLParser_initParser(IoSGMLParser *self)
{
	if (!DATA(self)->isInitialized)
	{
		IoSGMLParserData *data = DATA(self);

		data->handlers.elementBegin = IoSGMLParser_startElementHandler;
		data->handlers.elementEnd   = IoSGMLParser_endElementHandler;
		data->handlers.attributeNew = IoSGMLParser_newAttributeHandler;
		data->handlers.textNew      = IoSGMLParser_characterDataHandler;

		sgmlParserInitialize(&(data->parser),
						 SGML_EXTENSION_TYPE_CUSTOM,
						 &(data->handlers),
						 (void *)self);

		DATA(self)->isInitialized = 1;
	}
}

void IoSGMLParser_freeParser(IoSGMLParser *self)
{
	if (DATA(self)->isInitialized)
	{
		sgmlParserDestroy(&(DATA(self)->parser), 0);
		DATA(self)->isInitialized = 0;
	}
}

IoSGMLParser *IoSGMLParser_rawClone(IoSGMLParser *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(DATA(proto), sizeof(IoSGMLParserData)));
	return self;
}

IoSGMLParser *IoSGMLParser_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoSGMLParser_mark(IoSGMLParser *self)
{
	IoObject_shouldMark(DATA(self)->startElementMessage);
	IoObject_shouldMark(DATA(self)->endElementMessage);
	IoObject_shouldMark(DATA(self)->newAttributeMessage);
	IoObject_shouldMark(DATA(self)->newTextMessage);
}

void IoSGMLParser_free(IoSGMLParser *self)
{
	IoSGMLParser_freeParser(self);

	if (DATA(self)->tmpString)
	{
		io_free(DATA(self)->tmpString);
	}

	io_free(DATA(self));
}

char *IoSGMLParser_lowercase_(IoSGMLParser *self, const char *s)
{
	int max = strlen(s);
	int i;
	char *ts = DATA(self)->tmpString;

	ts = io_realloc(ts, max + 1);
	strcpy(ts, s);

	for (i = 0; i < max; i ++)
	{
		ts[i] = tolower(ts[i]);
	}

	DATA(self)->tmpString = ts;
	return ts;
}

/* ---  callbacks ---------------------------------- */

const char *IoSGMLParser_errorStringForCode(int code)
{
	/*
	 switch (code)
	 {
		 case ERR_SGMLP_MEMORY_ALLOC:   return "Out of memory";
	 }
	 */
	return "Unknown error code";
}



void IoSGMLParser_startElementHandler(SGML_PARSER *parser,
							   void *userContext,
							   const char *elementName)
{
	IoSGMLParser *self = userContext;
	IoState_pushRetainPool(IOSTATE);
	{
	char *e = IoSGMLParser_lowercase_(self, elementName);
	IoMessage *m = DATA(self)->startElementMessage;
	IoMessage_setCachedArg_to_(m, 0, IOSYMBOL(e));
	IoObject_perform(self, self, m);
	}
	IoState_popRetainPool(IOSTATE);
}

void IoSGMLParser_endElementHandler(SGML_PARSER *parser,
							 void *userContext,
							 const char *elementName)
{
	IoSGMLParser *self = userContext;
	IoState_pushRetainPool(IOSTATE);
	{
	char *e = IoSGMLParser_lowercase_(self, elementName);
	IoMessage *m = DATA(self)->endElementMessage;

	IoMessage_setCachedArg_to_(m, 0, IOSYMBOL(e));
	IoObject_perform(self, self, m);
	}
	IoState_popRetainPool(IOSTATE);
}

void IoSGMLParser_newAttributeHandler(SGML_PARSER *parser,
							   void *userContext,
							   const char *attributeName,
							   const char *attributeValue)
{
	IoSGMLParser *self = userContext;
	IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(self)->newAttributeMessage;

	{
		char *k = IoSGMLParser_lowercase_(self, attributeName);
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

void IoSGMLParser_characterDataHandler(SGML_PARSER *parser,
								void *userContext,
								const char *text)
{
	IoSGMLParser *self = userContext;
	IoState_pushRetainPool(IOSTATE);
	{
	int len = strlen(text);
	IoMessage *m = DATA(self)->newTextMessage;

	if (len)
	{
		IoMessage_setCachedArg_to_(m, 0, IoSeq_newWithCString_(IOSTATE, (char *)text));
		IoObject_perform(self, self, m);
	}
	}
	IoState_popRetainPool(IOSTATE);
}

/* ------------------------------------------------ */

/*doc SGMLParser tagForString(aSequence)
Parses aSequence and returns an SGMLTag object.
*/

IoObject *IoSGMLParser_parse(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc SGMLParser parse(aSequence)
	Parses aSequence and calls the following methods on self;
<pre>	
startElement(name)
endElement(name)
newAttribute(key, value)
newText(text)
</pre>	
for each of the items it finds. Returns self.
*/

	IoSeq *buffer = IoMessage_locals_seqArgAt_(m, locals, 0);
	int ret;
	IoState_pushRetainPool(IOSTATE); //needed?
	IoState_stackRetain_(IOSTATE, buffer); //needed?
	
	IoSGMLParser_initParser(self);

	//ret =_sgmlParseChunk(&(self->parser), s, strlen(s));
	ret = sgmlParserParseString(&(DATA(self)->parser),
						   CSTRING(buffer),
						   IoSeq_rawSize(buffer));

	IoSGMLParser_freeParser(self);

	IoState_popRetainPool(IOSTATE); //needed?

	if (ret == 0)
	{
		IoState_error_(IOSTATE, m, "SGMLParser parse: error code %i", ret);
		/*
		 "%s on line %i",
		 IoSGMLParser_errorStringForCode(DATA(self)->parser.ErrorCode),
		 DATA(self)->parser.ErrorLine);
		 */
	}

	return self;
}

