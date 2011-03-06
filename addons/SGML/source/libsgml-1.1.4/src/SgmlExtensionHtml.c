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
#include "SgmlExtensionHtml.h"

#if defined(_MSC_VER)
#define strcasecmp strcmpi
#endif

const char *autocloseElements[] = {
	"br",
	"dd",
	"hr",
	"img",
	"input",
	"li",
	"link",
	"meta",
	"p",
	"wbr",
	NULL
};

void sgmlExtensionOnDivert(struct _sgml_parser *parser, unsigned long newIndex, unsigned long oldState, unsigned long newState, const char *lastBuffer, unsigned long lastBufferSize)
{
	SGML_EXTENSION_HTML *ext = (SGML_EXTENSION_HTML *)sgmlParserGetUserContext(parser);

	if ((oldState & SGML_PARSER_STATE_INELEMENT) && (newState & SGML_PARSER_STATE_INTEXT))
	{
		// If this element is autoclosing, we go up.

		if (oldState & SGML_PARSER_STATE_INATTRIBUTENAME_ACTUAL)
		{
			sgmlExtensionHtmlAttributeNew(parser, sgmlParserGetUserContext(parser), lastBuffer, "");
			ext->skipNextAttribute = 1;
		}
 		if (oldState & SGML_PARSER_STATE_INATTRIBUTEVALUE_S)
		{
			sgmlExtensionHtmlAttributeNew(parser, sgmlParserGetUserContext(parser), parser->internal.lastAttributeName, lastBuffer);
			ext->skipNextAttribute = 1;
		}

		if ((ext->currElement) && (ext->currElement->autoclose))
			ext->currElement = (ext->currElement)?ext->currElement->parent:NULL;	
	
		while ((ext->currElement) && (ext->currElement->deferredClosure))
			ext->currElement = ext->currElement->parent;
	}

	ext->saveDivert(parser, newIndex, oldState, newState, lastBuffer, lastBufferSize);
}

SGML_EXTENSION_HTML *sgmlExtensionHtmlNew(SGML_PARSER *parser)
{
	SGML_EXTENSION_HTML *ext = (SGML_EXTENSION_HTML *)malloc(sizeof(SGML_EXTENSION_HTML));
	SGML_HANDLERS handlers;

	memset(&handlers, 0, sizeof(SGML_HANDLERS));

	handlers.preparse     = sgmlExtensionHtmlInitialize;
	handlers.elementBegin = sgmlExtensionHtmlElementBegin;
	handlers.elementEnd   = sgmlExtensionHtmlElementEnd;
	handlers.attributeNew = sgmlExtensionHtmlAttributeNew;
	handlers.textNew      = sgmlExtensionHtmlTextNew;	
	handlers.commentNew   = sgmlExtensionHtmlCommentNew;
	handlers.postparse    = sgmlExtensionHtmlDeinitialize;
	ext->flags            = 0;

	ext->document         = NULL;
	ext->currElement      = NULL;

	memcpy(&parser->handlers, &handlers, sizeof(SGML_HANDLERS));

	parser->internal.userContext   = ext;

	ext->saveDivert                = parser->internal.onDivert;
	parser->internal.onDivert      = sgmlExtensionOnDivert;

	parser->internal.setExtensionParam = sgmlExtensionHtmlSetParam;
	parser->internal.getExtensionParam = sgmlExtensionHtmlGetParam;

	return ext;
}

void sgmlExtensionHtmlDestroy(SGML_PARSER *parser, SGML_EXTENSION_HTML *ext)
{
	domDocumentDestroy(ext->document);

	free(ext);

	sgmlParserGetUserContext(parser) = NULL;
}

void sgmlExtensionHtmlInitialize(SGML_PARSER *parser, void *userContext)
{
	SGML_EXTENSION_HTML *ext = (SGML_EXTENSION_HTML *)userContext;

	if (ext->document)
		domDocumentDestroy(ext->document);

	ext->currElement       = NULL;
	ext->document          = domDocumentNew();
	ext->skipNextAttribute = 0;
}

void sgmlExtensionHtmlElementBegin(SGML_PARSER *parser, void *userContext, const char *elementName)
{
	SGML_EXTENSION_HTML *ext = (SGML_EXTENSION_HTML *)userContext;
	DOM_ELEMENT *element   = domElementNew(elementName);
	int x = 0, match = 0;

	if (ext->flags & SGML_EXTENSION_HTML_FLAG_STRIPELEMENT)
		return;

	if (ext->flags & SGML_EXTENSION_HTML_FLAG_ESCAPEUNKNOWNTAGS && ext->knownTags)
	{
		for (match = 0, x = 0; !match && ext->knownTags[x]; x++)
		{
			if (!strcasecmp(ext->knownTags[x], elementName))
				match = 1;
		}

		if (!match)
			element->escapeTags = 1;
	}	

	if (ext->currElement)
		domNodeAppendChild(ext->currElement, element);
	else
		domNodeAppendChild(ext->document, element);

	for (match = 0, x = 0; !match && autocloseElements[x]; x++)
	{
		if (!strcasecmp(autocloseElements[x], elementName))
		{
			match = 1;
			break;
		}
	}

	if (match)
		element->autoclose = 1;

	ext->currElement = element;
}

void sgmlExtensionHtmlElementEnd(SGML_PARSER *parser, void *userContext, const char *elementName)
{
	SGML_EXTENSION_HTML *ext = (SGML_EXTENSION_HTML *)userContext;

	if (ext->flags & SGML_EXTENSION_HTML_FLAG_STRIPELEMENT)
		return;	

	/*
	 * Ignore closure of element that isn't parent.
	 */
	if ((ext->currElement) && (strcasecmp(ext->currElement->name, elementName)))
	{
		DOM_ELEMENT *curr = ext->currElement;

		for (curr = ext->currElement;
				curr;
				curr = curr->parent)
		{
			if (!strcasecmp(curr->name, elementName))
			{
				curr->deferredClosure = 1;
				curr->autoclose       = 0;
				break;
			}
		}

		return;
	}

	ext->currElement = (ext->currElement)?ext->currElement->parent:NULL;	

	while ((ext->currElement) && (ext->currElement->deferredClosure))
		ext->currElement = ext->currElement->parent;
}

void sgmlExtensionHtmlAttributeNew(SGML_PARSER *parser, void *userContext, const char *attributeName, const char *attributeValue)
{
	SGML_EXTENSION_HTML *ext = (SGML_EXTENSION_HTML *)userContext;

	if (ext->flags & SGML_EXTENSION_HTML_FLAG_STRIPELEMENT)
		return;

	if (ext->skipNextAttribute)
	{
		ext->skipNextAttribute = 0;
		return;
	}

	domElementSetAttribute(ext->currElement, attributeName, attributeValue);
}

void sgmlExtensionHtmlTextNew(SGML_PARSER *parser, void *userContext, const char *textString)
{
	SGML_EXTENSION_HTML *ext = (SGML_EXTENSION_HTML *)userContext;
	DOM_TEXT *text          = domTextNew(textString);

	if (ext->flags & SGML_EXTENSION_HTML_FLAG_STRIPTEXT)
		return;

	if (ext->flags & SGML_EXTENSION_HTML_FLAG_IGNOREBLANKLINES)
	{
		int x = 0, ok = 0, len = strlen(textString);

		for (x = 0; !ok && x < len; x++)
		{
			switch (textString[x])
			{
				case '\n': case '\r': case '\t': case ' ':
					break;
				default:
					ok = 1;
					break;
			}
		}	

		if (!ok)
			return;
	}

	if (ext->currElement)
		domNodeAppendChild(ext->currElement, text);
	else
		domNodeAppendChild(ext->document, text);
}

void sgmlExtensionHtmlCommentNew(SGML_PARSER *parser, void *userContext, const char *commentString)
{
	SGML_EXTENSION_HTML *ext = (SGML_EXTENSION_HTML *)userContext;
	DOM_COMMENT *comment    = domCommentNew(commentString);

	if (ext->flags & SGML_EXTENSION_HTML_FLAG_STRIPCOMMENT)
		return;

	if (ext->currElement)
		domNodeAppendChild(ext->currElement, comment);
	else
		domNodeAppendChild(ext->document, comment);
}

void sgmlExtensionHtmlDeinitialize(SGML_PARSER *parser, void *userContext)
{
}

DOM_DOCUMENT *sgmlExtensionHtmlGetDocument(SGML_PARSER *parser)
{
	SGML_EXTENSION_HTML *ext = (SGML_EXTENSION_HTML *)sgmlParserGetUserContext(parser);

	return ext->document;
}

void sgmlExtensionHtmlSetParam(SGML_PARSER *parser, void *extensionContext, unsigned long param, void *value)
{
	SGML_EXTENSION_HTML *ext = (SGML_EXTENSION_HTML *)sgmlParserGetUserContext(parser);


	switch (param)
	{
		case SGML_EXTENSION_HTML_PARAM_FLAG:
			ext->flags = (unsigned long)value;			
			break;
		case SGML_EXTENSION_HTML_PARAM_SETKNOWNTAGS:
			ext->knownTags = (char **)value;
			break;
	}
}

void sgmlExtensionHtmlGetParam(SGML_PARSER *parser, void *extensionContext, unsigned long param, void *value)
{
	SGML_EXTENSION_HTML *ext = (SGML_EXTENSION_HTML *)sgmlParserGetUserContext(parser);

	switch (param)
	{
		case SGML_EXTENSION_HTML_PARAM_FLAG:
			*(unsigned long *)value = ext->flags;
			break;
		case SGML_EXTENSION_HTML_PARAM_SETKNOWNTAGS:
			*(char ***)value = ext->knownTags;
			break;
	}
}

