/*
	copyright: Steve Dekorte, 2006. All rights reserved.
	license: See _BSDLicense.txt.
*/

#include "UArray.h"
#include "utf_convert.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include "utf8.h"
#include <assert.h>

int UArray_MachineIsLittleEndian(void)
{
	unsigned int i = 0x1;
	return ((unsigned char *)(&i))[0] == 1;
}

/*
int strlenUTF8(char *s) 
{
	int i = 0;
	int len = 0;
	
	while (s[i]) 
	{
		if ((s[i] & 0xc0) != 0x80) 
		{
			len ++;
		}
		
		i ++;
	}
	
	return len;
}
*/

static int UArray_SizeOfUTF8Char(const uint8_t *s)
{
	uint8_t c = *s;

	if (c & 0x80)
	{
		if((c & 0xE0) == 0xC0) return 2;
		if((c & 0xF0) == 0xE0) return 3;
		if((c & 0xF8) == 0xF0) return 4;
		if((c & 0xFC) == 0xF8) return 5;
		if((c & 0xFE) == 0xFC) return 6;
		return -1;
	}

	return 1;
}

int UArray_maxCharSize(const UArray *self)
{
	if (self->encoding == CENCODING_UTF8)
	{
		int maxCharSize = 1;
		size_t i = 0;

		while (i < self->size)
		{
			int charSize = UArray_SizeOfUTF8Char(self->data + i);
			if (charSize > maxCharSize)  maxCharSize = charSize;
			if (charSize == -1) return -1;
			i += charSize;
		}

		return maxCharSize;
	}

	return self->itemSize;
}

size_t UArray_numberOfCharacters(const UArray *self)
{
	if (self->encoding == CENCODING_UTF8)
	{
		size_t numChars = 0;
		size_t i = 0;

		while (i < self->size)
		{
			int charSize = UArray_SizeOfUTF8Char(self->data + i);
			if (charSize == -1) return 0;
			numChars ++;
			i += charSize;
		}

		return numChars;
	}

	return self->size;
}

int UArray_convertToFixedSizeType(UArray *self)
{
	if (self->encoding == CENCODING_UTF8)
	{
		int maxCharSize = UArray_maxCharSize(self);

		if (maxCharSize == 1)
		{
			self->encoding = CENCODING_ASCII;
		}
		else if(maxCharSize == 2)
		{
			UArray_convertToUCS2(self);
		}
		else
		{
			UArray_convertToUCS4(self);
		}

		return 1;
	}

	return 0;
}

int UArray_isMultibyte(const UArray *self)
{
	if (self->encoding == CENCODING_UTF8)
	{
		size_t i, max = UArray_sizeInBytes(self); 
		const uint8_t *bytes = UArray_bytes(self);
		for (i = 0; i < max; i ++)
		{
			if (UArray_SizeOfUTF8Char(bytes + i) > 1) return 1; 
		}
		//UARRAY_INTFOREACH(self, i, v, if (ismbchar((int)v)) return 1; );
	}

	return 0;
}

int UArray_isLegalUTF8(const UArray *self)
{
	void *sourceStart = self->data;
	void *sourceEnd   = self->data + self->size * self->itemSize;

	return isLegalUTF8Sequence(sourceStart, sourceEnd);
}

UArray *UArray_asNumberArrayString(const UArray *self)
{
	UArray *out = UArray_new();
	UArray_setEncoding_(out, CENCODING_ASCII);

	UARRAY_INTFOREACH(self, i, v,
		char s[128];

		if(UArray_isFloatType(self))
		{
			sprintf(s, "%f", (double)v);
		}
		else
		{
			sprintf(s, "%i", (int)v);
		}

		if(i != UArray_size(self) -1 ) strcat(s, ", ");
		UArray_appendBytes_size_(out, (unsigned char *)s, strlen(s));
	);

	return out;
}

UArray *UArray_asUTF8(const UArray *self)
{
	UArray *out = UArray_new();
	UArray_setItemType_(out, CTYPE_uint8_t);
	UArray_setEncoding_(out, CENCODING_UTF8);
	UArray_setSize_(out, self->size * 4);

	{
		//ConversionFlags options = lenientConversion;
		void *sourceStart = self->data;
		//void *sourceEnd   = self->data + self->size * self->itemSize;
		UTF8 *targetStart = out->data;
		//UTF8 *targetEnd   = out->data + out->size * out->itemSize;
		size_t outSize;

		switch(self->encoding)
		{
			case CENCODING_ASCII:
				UArray_copy_(out, self);
				break;
			case CENCODING_UTF8:
				UArray_copy_(out, self);
				break;
				/*
			case CENCODING_UTF16:
				r = ConvertUTF16toUTF8((const UTF16 **)&sourceStart, (const UTF16 *)sourceEnd, &targetStart, targetEnd, options);
				break;
			case CENCODING_UTF32:
				r = ConvertUTF32toUTF8((const UTF32 **)&sourceStart, (const UTF32 *)sourceEnd, &targetStart, targetEnd, options);
				break;
				*/
			case CENCODING_UCS2:
				// should the size be the num of chars or num of bytes??????????
				outSize = ucs2encode(targetStart, sourceStart, self->size, NULL); // ucs2 to utf8
				UArray_setSize_(out, outSize - 1);
				break;
			case CENCODING_UCS4:
				// should the size be the num of chars or num of bytes??????????
				outSize = ucs4encode(targetStart, sourceStart, self->size, NULL); // ucs4 to utf8
				UArray_setSize_(out, outSize - 1);
				break;
			case CENCODING_NUMBER:
				{
					UArray *nas = UArray_asNumberArrayString(self);
					UArray_free(out);
					out = UArray_asUTF8(nas);
					UArray_free(nas);
					break;
				}
			default:
				printf("UArray_asUTF8 - unknown source encoding\n");
		}
	}

	return out;
}

int UArray_isUTF8Compatible(const UArray *self)
{
	return (self->encoding == CENCODING_ASCII || self->encoding == CENCODING_UTF8);
}

UArray *UArray_asUCS2(const UArray *self)
{
	int convertToUtf8First = !UArray_isUTF8Compatible(self);
	const UArray *utf8Array = convertToUtf8First ? UArray_asUTF8(self) : self;
	size_t countedChars = UArray_numberOfCharacters(self);
	size_t numChars;
	
	UArray *out = UArray_new();
	UArray_setItemType_(out, CTYPE_uint16_t);
	UArray_setEncoding_(out, CENCODING_UCS2);
	UArray_setSize_(out, countedChars*2);

	numChars = ucs2decode((ucs2 *)(out->data), out->size, utf8Array->data);
	
	if ((numChars > 0) && (numChars > countedChars*2))
	{
		printf("UArray_asUCS2 error: numChars (%i) > countedChars (2*%i)\n", (int)numChars, (int)countedChars);
		printf("Exiting because we may have overwritten the usc2 decode output buffer.");
		exit(-1);
	}
	
	UArray_setSize_(out, numChars);
	
	if (convertToUtf8First) UArray_free((UArray *)utf8Array);
	return out;
}

UArray *UArray_asUCS4(const UArray *self)
{
	int convertToUtf8First = !UArray_isUTF8Compatible(self);
	const UArray *utf8Array = convertToUtf8First ? UArray_asUTF8(self) : self;
	size_t countedChars = UArray_numberOfCharacters(self);
	size_t numChars;
	
	UArray *out = UArray_new();
	UArray_setItemType_(out, CTYPE_uint32_t);
	UArray_setEncoding_(out, CENCODING_UCS4);
	UArray_setSize_(out, countedChars*2);

	numChars = ucs4decode((ucs4 *)out->data, out->size, utf8Array->data);

	if ((numChars > 0) && (numChars > countedChars*2))
	{
		printf("UArray_asUCS4 error: numChars %i != countedChars %i\n", (int)numChars, (int)countedChars);
		exit(-1);
	}
	
	UArray_setSize_(out, numChars);
	
	if (convertToUtf8First) UArray_free((UArray *)utf8Array);
	return out;
}

void UArray_convertToUTF8(UArray *self)
{
	UArray *a = UArray_asUTF8(self);
	UArray_swapWith_(self, a);
	UArray_free(a);
}

void UArray_convertToUCS2(UArray *self)
{
	UArray *a = UArray_asUCS2(self);
	UArray_swapWith_(self, a);
	UArray_free(a);
}

void UArray_convertToUCS4(UArray *self)
{
	UArray *a = UArray_asUCS4(self);
	UArray_swapWith_(self, a);
	UArray_free(a);
}
