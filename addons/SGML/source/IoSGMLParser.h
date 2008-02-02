//metadoc SGMLParser copyright Steve Dekorte 2002
//metadoc SGMLParser license BSD revised

#ifndef IOSGMLPARSER_DEFINED
#define IOSGMLPARSER_DEFINED 1

#include "IoSeq.h"
#include <sgml/libsgml.h>

typedef IoObject IoSGMLParser;

typedef struct
{
	int isInitialized;
	SGML_PARSER parser;
	SGML_HANDLERS handlers;
	char *tmpString;

	IoMessage *startElementMessage;
	IoMessage *endElementMessage;
	IoMessage *newAttributeMessage;
	IoMessage *newTextMessage;

} IoSGMLParserData;

IoSGMLParser *IoSGMLParser_proto(void *state);
void IoSGMLParser_free(IoSGMLParser *self);
IoSGMLParser *IoSGMLParser_rawClone(IoSGMLParser *self);
void IoSGMLParser_mark(IoSGMLParser *self);
void IoSGMLParser_free(IoSGMLParser *self);

/* --- callbacks ---------------------------------- */

void IoSGMLParser_startElementHandler(SGML_PARSER *parser,
	void *userContext,
	const char *elementName);

void IoSGMLParser_endElementHandler(SGML_PARSER *parser,
	void *userContext,
	const char *elementName);

void IoSGMLParser_newAttributeHandler(SGML_PARSER *parser,
	void *userContext,
	const char *attributeName,
	const char *attributeValue);

void IoSGMLParser_characterDataHandler(SGML_PARSER *parser,
	void *userContext,
	const char *text);

/* ------------------------------------------------ */

IoObject *IoSGMLParser_parse(IoObject *self, IoObject *locals, IoMessage *m);

#endif

