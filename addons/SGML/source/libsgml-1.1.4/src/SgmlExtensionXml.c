/*
 * libsgml -- SGML state machine parsing library.
 *                                                                  
 * Copyright (c) 2002 Uninformed Research (http://www.uninformed.org)
 * All rights reserved.
 *
 * skape
 * mmiller@hick.org
 */
#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include "SgmlParser.h"
#include "SgmlExtensionXml.h"

SGML_EXTENSION_XML *sgmlExtensionXmlNew(SGML_PARSER *parser)
{
	SGML_EXTENSION_XML *ret = (SGML_EXTENSION_XML *)malloc(sizeof(SGML_EXTENSION_XML));
	SGML_HANDLERS handlers;

	if (ret)
		memset(ret, 0, sizeof(SGML_EXTENSION_XML));

	memset(&handlers, 0, sizeof(SGML_HANDLERS));

	handlers.preparse     = sgmlExtensionXmlInitialize;
	handlers.elementBegin = sgmlExtensionXmlElementBegin;
	handlers.elementEnd   = sgmlExtensionXmlElementEnd;
	handlers.attributeNew = sgmlExtensionXmlAttributeNew;
	handlers.textNew      = sgmlExtensionXmlTextNew;	
	handlers.commentNew   = sgmlExtensionXmlCommentNew;
	handlers.postparse    = sgmlExtensionXmlDeinitialize;

	memcpy(&parser->handlers, &handlers, sizeof(SGML_HANDLERS));
	
	parser->internal.setExtensionParam = sgmlExtensionXmlSetParam;
	parser->internal.getExtensionParam = sgmlExtensionXmlGetParam;

	return ret;
}

void sgmlExtensionXmlDestroy(SGML_PARSER *parser, SGML_EXTENSION_XML *ext)
{
	domDocumentDestroy(ext->document);

	free(ext);
}

void sgmlExtensionXmlInitialize(SGML_PARSER *parser, void *userContext)
{
	SGML_EXTENSION_XML *ext = (SGML_EXTENSION_XML *)sgmlParserGetExtensionContext(parser);

	if (ext->document)
		domDocumentDestroy(ext->document);

	ext->document    = domDocumentNew();
	ext->currElement = NULL;
}

void sgmlExtensionXmlElementBegin(SGML_PARSER *parser, void *userContext, const char *elementName)
{
	SGML_EXTENSION_XML *ext = (SGML_EXTENSION_XML *)sgmlParserGetExtensionContext(parser);
	DOM_ELEMENT *element;

	// This is a processing instruction of some sort, we shall ignore it.
	if (elementName[0] == '?') 
		return; 

	element = domElementNew(elementName);

	if (ext->currElement)
		domNodeAppendChild(ext->currElement, element);
	else
		domNodeAppendChild(ext->document, element);

	ext->currElement = element;
}

void sgmlExtensionXmlElementEnd(SGML_PARSER *parser, void *userContext, const char *elementName)
{
	SGML_EXTENSION_XML *ext = (SGML_EXTENSION_XML *)sgmlParserGetExtensionContext(parser);

	// End of processing instruction.
	if (elementName[0] == '?')
		return;

	if (ext->currElement)
		ext->currElement = ext->currElement->parent;
}

void sgmlExtensionXmlAttributeNew(SGML_PARSER *parser, void *userContext, const char *attributeName, const char *attributeValue)
{
	SGML_EXTENSION_XML *ext = (SGML_EXTENSION_XML *)sgmlParserGetExtensionContext(parser);

	if (ext->currElement)
		domElementSetAttribute(ext->currElement, attributeName, attributeValue);
}

void sgmlExtensionXmlTextNew(SGML_PARSER *parser, void *userContext, const char *textValue)
{
	SGML_EXTENSION_XML *ext = (SGML_EXTENSION_XML *)sgmlParserGetExtensionContext(parser);
	DOM_TEXT *text = domTextNew(textValue);

	if (ext->currElement)
		domNodeAppendChild(ext->currElement, text);
	else
		domNodeAppendChild(ext->document, text);
}

void sgmlExtensionXmlCommentNew(SGML_PARSER *parser, void *userContext, const char *comment)
{
	SGML_EXTENSION_XML *ext = (SGML_EXTENSION_XML *)sgmlParserGetExtensionContext(parser);
	DOM_TEXT *com = domCommentNew(comment);

	if (ext->currElement)
		domNodeAppendChild(ext->currElement, com);
	else
		domNodeAppendChild(ext->document, com);
}

void sgmlExtensionXmlDeinitialize(SGML_PARSER *parser, void *userContext)
{
}

void sgmlExtensionXmlSetParam(SGML_PARSER *parser, void *extensionContext, unsigned long param, void *value)
{
}

void sgmlExtensionXmlGetParam(SGML_PARSER *parser, void *extensionContext, unsigned long param, void *value)
{
}

DOM_DOCUMENT *sgmlExtensionXmlGetDocument(SGML_PARSER *parser)
{
	SGML_EXTENSION_XML *ext = (SGML_EXTENSION_XML *)sgmlParserGetExtensionContext(parser);

	return ext->document;
}
