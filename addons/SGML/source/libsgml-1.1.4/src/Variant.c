#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include "Variant.h"

#if defined(_MSC_VER)
#define strcasecmp strcmpi
#define snprintf _snprintf
#endif

char *_variantBase64Encode(const unsigned char *src, unsigned long srcLength);
char *_variantBase64Decode(const unsigned char *src, unsigned long srcLength, unsigned long *outLength);

void variantSet(enum VariantType type, void *value, unsigned long valueSize, void *userParam, void (*setter)(void *userParam, const char *string))
{
	switch (type)
	{
		case VARIANT_TYPE_STRING:
			setter(userParam, value);
			break;
		case VARIANT_TYPE_SIGNED_INTEGER:
			if (valueSize >= sizeof(signed long))
			{
				char trans[32];

				snprintf(trans, sizeof(trans) - 1, "%li", *(signed long *)value);

				setter(userParam, trans);
			}
			break;
		case VARIANT_TYPE_UNSIGNED_INTEGER:
			if (valueSize >= sizeof(unsigned long))
			{
				char trans[32];

				snprintf(trans, sizeof(trans) - 1, "%lu", *(unsigned long *)value);

				setter(userParam, trans);
			}
			break;
		case VARIANT_TYPE_SIGNED_SHORT:
			if (valueSize >= sizeof(signed short))
			{
				char trans[16];

				snprintf(trans, sizeof(trans) - 1, "%d", *(signed short *)value);

				setter(userParam, trans);
			}
			break;
		case VARIANT_TYPE_UNSIGNED_SHORT:
			if (valueSize >= sizeof(signed short))
			{
				char trans[16];

				snprintf(trans, sizeof(trans) - 1, "%u", *(unsigned short *)value);

				setter(userParam, trans);
			}
			break;
		case VARIANT_TYPE_FLOAT:
		case VARIANT_TYPE_DOUBLE:
			if ((valueSize >= sizeof(float)) || (valueSize >= sizeof(double)))
			{
				char trans[128];

				snprintf(trans, sizeof(trans) - 1, "%f", *(float *)value);

				setter(userParam, trans);
			}
			break;
		case VARIANT_TYPE_CHAR:
			if (valueSize >= sizeof(char))
			{
				char actual[2] = { *(char *)value, 0 };

				setter(userParam, actual);
			}
			break;
		case VARIANT_TYPE_BINARY_B64:
			{
				char *ret = _variantBase64Encode(value, valueSize);

				setter(userParam, ret);

				free(ret);
			}
			break;
		default:
			break;
	}

}

void *variantGet(enum VariantType type, const char *string)
{
	if (!string)
		return NULL;

	switch (type)
	{
		case VARIANT_TYPE_STRING:
			return (void *)string;
		case VARIANT_TYPE_SIGNED_INTEGER:
			return (void *)strtol(string, NULL, 10);
		case VARIANT_TYPE_UNSIGNED_INTEGER:
			return (void *)strtoul(string, NULL, 10);
		case VARIANT_TYPE_SIGNED_SHORT:
			{
				signed short  a;

				sscanf(string, "%hi", &a);

				return (void *)(signed long)a;
			}
		case VARIANT_TYPE_UNSIGNED_SHORT:
			{
				unsigned short a;

				sscanf(string, "%hu", &a);

				return (void *)(unsigned long)a;
			}
		case VARIANT_TYPE_FLOAT:
			{
				VARIANT_VALUE *value = (VARIANT_VALUE *)malloc(sizeof(VARIANT_VALUE));

				if (value)
				{
					memset(value, 0, sizeof(VARIANT_VALUE));

					value->flt = atof(string);
					value->length = sizeof(float);
				}

				return (void *)value;
			}
			break;
		case VARIANT_TYPE_DOUBLE:
			{
				VARIANT_VALUE *value = (VARIANT_VALUE *)malloc(sizeof(VARIANT_VALUE));

				if (value)
				{
					memset(value, 0, sizeof(VARIANT_VALUE));

					value->dbl    = atof(string);
					value->length = sizeof(double);
				}

				return (void *)value;
			}
			break;
		case VARIANT_TYPE_CHAR:
			return (void *)(unsigned long)string[0];
		case VARIANT_TYPE_BINARY_B64:
			{
				VARIANT_VALUE *value = (VARIANT_VALUE *)malloc(sizeof(VARIANT_VALUE));

				if (value)
				{
					memset(value, 0, sizeof(VARIANT_VALUE));

					value->binary = _variantBase64Decode(string, strlen(string), &value->length);
				}
				return value;
			}
			break;
		default:
			break;
	}

	return NULL;
}

void variantFree(VARIANT_VALUE *value)
{
	if (!value)
		return;

	if (value->binary)
		free(value->binary);

	free(value);
}

char *_variantBase64Encode(const unsigned char *src, unsigned long srcLength)
{
	/* the base64 encoding algorithm is found in RFC 2045 section 6.8 */
	/* the base64 alphabet */
	char code[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	unsigned long remainder = srcLength % 3, destLength = srcLength;
	char *dest;

	unsigned long i = 0; /* itteration of 3 byte chunks of src */
	unsigned char chunk[3];       /* the current 3 octet chunk */
	unsigned char w1,w2;          /* work vars */
	char cn;             /* number of octets in chunk */
	int j;

	if (remainder)
		destLength = srcLength + (3 - remainder);

	destLength = destLength + (destLength / 3) + 1;
	
	dest = (char *)malloc(destLength);

	if (!dest)
		return NULL;

	memset(dest, 0, destLength);

	/* 876543 218765 432187 654321 */
	/* 111111 112222 222233 333333 */
	/* 87654321 */

	while (1)
	{
		chunk[0] = 0;
		chunk[1] = 0;
		chunk[2] = 0;

		cn = srcLength - (i*3);
		cn = ( cn > 3 ? 3 : cn );
		
		/* fill this chunk with all we can */
		for (j = 0; j < cn; j++ )
			chunk[j] = *(src+(i*3)+j);

		if ( cn < 1 ) /* no octets */
			break;

		/* has the 6 highest order bits from octet 1 */
		w1 = chunk[0];    /* first octet */
		w1 &= ~3;         /* turn off two lowest bits (1 and 2) */
		w1 >>= 2;         /* shift right 2 */

		*(dest+(i*4)) = code[ w1 ];

		/* has the 2 lowest bits of first octet and higest 4 from 2nd octet */
		w1 = chunk[0];    /* first octet */
		w1 &= 3;          /* turn off all but two lowest bits */
		w1 <<= 4;         /* shift that left 4 */
		w2 = chunk[1];    /* second octet */
		w2 &= ~15;        /* turn off four lowest bits */ 
		w2 >>= 4;         /* shift that right 4 */
		w1 |= w2;         /* combine them together */

		*(dest+(i*4)+1) = code[ w1 ];

		if ( cn < 2 ) /* only one octet */
			break;

		/* has the 4 lowest bits from octet 2 and highest 2 from octet 3 */
		w1 = chunk[1];    /* second octet */
		w1 &= 15;         /* turn off all but lowest 4 bits */
		w1 <<= 2;         /* shift that left 2 */
		w2 = chunk[2];    /* third octet */
		w2 &= ~63;        /* turn off all but higest 2 bits */ 
		w2 >>= 6;         /* shift that right 6 */
		w1 |= w2;         /* combine them together */

		*(dest+(i*4)+2) = code[ w1 ];

		if ( cn < 3 ) /* only two octets */
			break;

		/* has the 6 lowest from octet 3 */
		w1 = chunk[2];    /* third octet */	
		w1 &= 63;         /* turn off higest 2 bits */

		*(dest+(i*4)+3) = code[ w1 ];

		i++;
	}

	/* Now we have to pad with '=' char according to how many octets were
	 * were in the last chunk. The cases below are described in the RFC.
	 */

	switch (cn)
	{
		case 1:
			/* padd with == */
			*(dest+(i*4)+2) = '=';
			*(dest+(i*4)+3) = '=';
			break;

		case 2:
			/* pad with = */
			*(dest+(i*4)+3) = '=';
			break;
	}

	return dest;
}

char *_variantBase64Decode(const unsigned char *src, unsigned long srcLength, unsigned long *outLength)
{
	char code[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	unsigned long srcLen = strlen( (const char *) src );
	unsigned long destLen = srcLen - (srcLen/4) + 1;
	char cn = 0; /* current number (0-3) char we are in of the current src 4 block */
	unsigned char w1 = 0, w2 = 0, w3 = 0; /* temp holding for newly decoded dest 3 block */
	unsigned long pos = 0; /* current position in dest buffer */
	unsigned long i;
	char * dest;
	char isValid[256];
	char decodeValue[256];

	if (!(dest = (char *)malloc(destLen)))
		return NULL;

	memset( isValid, 0, sizeof(isValid) );
	memset( decodeValue, 0, sizeof(isValid) );

	for (i = 0; i < sizeof(code)-1; i++)
	{
		char c = code[i];

		isValid[(int)c]     = 1;
		decodeValue[(int)c] = i;
	}
	
	/* while we aren't at end of string and are on a valid base64 char */

	for (i = 0; (i < srcLen) && isValid[src[i]]; i++)
	{
		char bitChunk = decodeValue[src[i]];

		/* 87654321 87654321 87654321 */

		switch ( cn )
		{
			case 0:
				/* highest 6 bits for octet 1 in dest */
				w1 = (bitChunk << 2);
				break;

			case 1:
				/* 2 lowest for octet 1 and 4 highest for octet 2 */
				w1 |= (bitChunk >> 4);
				w2 |= (bitChunk << 4);
				break;

			case 2:
				/* 4 lowest for octet 2 and 2 highest for octet 3 */
				w2 |= (bitChunk >> 2);
				w3 |= (bitChunk << 6);
				break;

			case 3:
				/* 6 lowest for octet 3 */
				w3 |= bitChunk;
				break;
		}
		
		cn++;

		if (cn == 4)
		{
			/* we have just finished a 4 block */
			dest[pos++] = w1;
			dest[pos++] = w2;
			dest[pos++] = w3;
			cn = w1 = w2 = w3 = 0;
		}
	}

	switch (cn)
	{
		case 2:
			/* it takes 2 base64 letters to make a whole 8bit number */
			dest[pos++] = w1;
			break;

		case 3:
			/* it takes 3 base64 letters to make two whole 8 bit numbers */
			dest[pos++] = w1;
			dest[pos++] = w2;
			break;
	}

	*outLength = pos;

	return dest;
}
