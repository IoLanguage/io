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

#include "libsgml.h"

void elementBegin(SGML_PARSER *parser, void *userContext, const char *elementName);
void elementEnd(SGML_PARSER *parser, void *userContext, const char *elementName);
void attributeNew(SGML_PARSER *parser, void *userContext, const char *attributeName, const char *attributeValue);
void textNew(SGML_PARSER *parser, void *userContext, const char *text);

int main(int argc, char **argv)
{
	SGML_PARSER  sgml;
	SGML_HANDLERS handlers;

	memset(&handlers, 0, sizeof(handlers));

	handlers.elementBegin = elementBegin;
	handlers.elementEnd   = elementEnd;
	handlers.attributeNew = attributeNew;
	handlers.textNew      = textNew;

	sgmlParserInitialize(&sgml, SGML_EXTENSION_TYPE_CUSTOM, &handlers, NULL);

	sgmlParserParseFile(&sgml, (argc > 1)?argv[1]:"test.sgml");

	sgmlParserDestroy(&sgml, 0);

	return 1;
}

void elementBegin(SGML_PARSER *parser, void *userContext, const char *elementName)
{
	fprintf(stdout,"BEGIN ELEMENT: %s\n", elementName);
}

void elementEnd(SGML_PARSER *parser, void *userContext, const char *elementName)
{
	fprintf(stdout,"END ELEMENT: %s\n", elementName);
}

void attributeNew(SGML_PARSER *parser, void *userContext, const char *attributeName, const char *attributeValue)
{
	fprintf(stdout,"..attr='%s' value='%s'\n", attributeName, attributeValue);
}

void textNew(SGML_PARSER *parser, void *userContext, const char *text)
{
}
