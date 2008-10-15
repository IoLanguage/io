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

int main(int argc, char **argv)
{
	DOM_ELEMENT *element = domElementNew("elmenent");
	unsigned long initial = 57;
	VARIANT_VALUE *value;
	unsigned char binary[] = "\xde\xad\xbe\x0ef AAAA BBBB CCCC DDDD EEEE";

	fprintf(stdout, "Setting the attribute 'example' to '%lu'.\n", initial);

	domElementSetAttributeVariant(element, "example", VARIANT_TYPE_UNSIGNED_INTEGER, &initial, sizeof(unsigned long));

	fprintf(stdout, "'example' as a...\n\n");

	// String
	fprintf(stdout, "string         -> '%s'\n", (const char *)domElementGetAttributeVariant(element, "example", VARIANT_TYPE_STRING));

	// Integers
	fprintf(stdout, "signed long    -> '%li'\n", (signed long)domElementGetAttributeVariant(element, "example", VARIANT_TYPE_SIGNED_INTEGER));
	fprintf(stdout, "unsigned long  -> '%li'\n", (unsigned long)domElementGetAttributeVariant(element, "example", VARIANT_TYPE_UNSIGNED_INTEGER));

	// Shorts
	fprintf(stdout, "signed short   -> '%hi'\n", (signed short)(unsigned long)domElementGetAttributeVariant(element, "example", VARIANT_TYPE_SIGNED_SHORT));
	fprintf(stdout, "unsigned short -> '%hu'\n", (unsigned short)(unsigned long)domElementGetAttributeVariant(element, "example", VARIANT_TYPE_UNSIGNED_SHORT));
	
	fprintf(stdout, "character      -> '%c'\n", (char)(unsigned long)domElementGetAttributeVariant(element, "example", VARIANT_TYPE_CHAR));

	// Float
	value = (VARIANT_VALUE *)domElementGetAttributeVariant(element, "example", VARIANT_TYPE_FLOAT);

	if (value)
		fprintf(stdout, "float          -> '%f'\n", value->flt);

	variantFree(value);

	// Double
	value = (VARIANT_VALUE *)domElementGetAttributeVariant(element, "example", VARIANT_TYPE_DOUBLE);

	if (value)
		fprintf(stdout, "double         -> '%f'\n", value->dbl);

	variantFree(value);
	
	/*
	 * Now we'll try our hand at base64 binary manipulation.
	 */

	fprintf(stdout, "\nSetting attribute 'binary_b64' to a %d byte buffer.\n\n", sizeof(binary) - 1);

	domElementSetAttributeVariant(element, "binary_b64", VARIANT_TYPE_BINARY_B64, binary, sizeof(binary) - 1);

	// Binary
	value = domElementGetAttributeVariant(element, "binary_b64", VARIANT_TYPE_BINARY_B64);

	if (value)
	{
		int x = 0;

		fprintf(stdout, "binary (%lu)   -> '", value->length);

		for (x = 0; x < value->length; x++)
		{
			fprintf(stdout,"%.2x", value->binary[x] & 0xFF);

			if (x+1 != value->length)
				fprintf(stdout, " ");
		}

		fprintf(stdout, "'\n");
	}

	variantFree(value);

	domNodeDestroy(element);

	return 1;
}
