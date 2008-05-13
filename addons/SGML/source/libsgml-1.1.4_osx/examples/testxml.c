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
	SGML_PARSER  sgml;
	char         *string;
	unsigned long stringLength = 0;

	sgmlParserInitialize(&sgml, SGML_EXTENSION_TYPE_XML, NULL, NULL);

	sgmlParserParseFile(&sgml, (argc > 1)?argv[1]:"test.sgml");
	
	stringLength = domNodeSerializeToString(sgmlExtensionXmlGetDocument(&sgml), &string);

	fprintf(stdout, "XML document (length=%lu):\n\n", stringLength);

	fprintf(stdout, "%s\n\n", string);

	free(string);

	sgmlParserDestroy(&sgml, 0);

	return 1;
}
