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

#include "libsgml.h"

int main(int argc, char **argv)
{
	unsigned long flags = 0;
	SGML_PARSER  sgml;
//	char *knownTags[] = {
//		"html",
//		"body",
//		"p",
//		NULL
//	};

	sgmlParserInitialize(&sgml, SGML_EXTENSION_TYPE_HTML, NULL, NULL);

//	flags = SGML_EXTENSION_HTML_FLAG_ESCAPEUNKNOWNTAGS | SGML_EXTENSION_HTML_FLAG_STRIPCOMMENT;
//	flags = SGML_EXTENSION_HTML_FLAG_STRIPTEXT | SGML_EXTENSION_HTML_FLAG_STRIPELEMENT | SGML_EXTENSION_HTML_FLAG_STRIPCOMMENT;
//	flags = SGML_EXTENSION_HTML_FLAG_STRIPELEMENT | SGML_EXTENSION_HTML_FLAG_STRIPCOMMENT;
//	flags = SGML_EXTENSION_HTML_FLAG_STRIPELEMENT;
//	flags = SGML_EXTENSION_HTML_FLAG_STRIPTEXT;
//	flags = SGML_EXTENSION_HTML_FLAG_STRIPCOMMENT;

	sgmlParserExtensionSetParam(&sgml, SGML_EXTENSION_HTML_PARAM_FLAG, (void *)flags);
//	sgmlParserExtensionSetParam(&sgml, SGML_EXTENSION_HTML_PARAM_SETKNOWNTAGS, (void *)knownTags);
	sgmlParserParseFile(&sgml, (argc > 1)?argv[1]:"test.sgml");

	fprintf(stdout,"HTML document:\n\n");

	domNodeSerializeToFd(sgmlExtensionHtmlGetDocument(&sgml), stdout);

	fprintf(stdout,"\n");

	sgmlParserDestroy(&sgml, 0);

	return 1;
}
