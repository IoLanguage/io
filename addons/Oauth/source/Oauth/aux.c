/*
 *	aux.c
 *
 *	Auxilliary facilities
 */
#include "aux.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

void string_init(struct string *self)
{
	self->size = 0;
	self->len  = 0;
	self->data = NULL;
}

void string_adjustlen(struct string *self, const int factor)
{
	self->len-=factor;
	self->data[self->len] = '\0';
}

char *string_cString(struct string *self)
{
	char *cString = calloc(self->len + 1, 1);
	memcpy(cString, self->data, self->len);
	return cString;
}

void string_append_urlencoded_rfc3986(struct string *self, const char *p, const size_t len)
{
	const uint8_t *data = (uint8_t *)p, *cp = data;
	const uint8_t *end = data + len;
	char _t[3];

	while (data != end)
	{
		if ((*data >='a' && *data <='z') || (*data >='A' && *data <='Z') || (*data >='0' && *data <='9') || *data == '-' || *data == '.' || *data == '_' || *data == '~')
			++data;
		else
		{
			string_append(self, cp, data - cp);

			_t[0] = '%';
			_t[1] = ToHex(*data >> 4);
			_t[2] = ToHex(*data & 15);
			string_append(self, _t, 3);
			cp = ++data;
		}
	}

	string_append(self, cp, data - cp);
}

void string_reset(struct string *self)
{
	self->len = 0;
	if (self->data)
		self->data[0] = '\0';
}

void string_dealloc(struct string *self)
{
	if (self->data)
		free(self->data);
}

size_t string_capacity(struct string *self)
{
	return self->size - self->len;
}

void string_set_capacity(struct string *self, const size_t n)
{
	const size_t c = string_capacity(self);

	if (c < n)
	{
		self->size+=(n - c) + 2;
		self->data = (char *)realloc(self->data, self->size * sizeof(char));
	}
}	

void string_append(struct string *self, const void *p, const size_t l)
{
	string_set_capacity(self, l);
	memcpy(self->data + self->len, p, l);
	self->len+=l;
	self->data[self->len] = '\0';
}

void string_appendfmt(struct string *self, const char *fmt, ...)
{
	va_list args;
	const int capacity = string_capacity(self);

	va_start(args, fmt);
	const int len = vsnprintf(self->data + self->len, capacity, fmt, args);

	if (len >= capacity)
	{
		string_set_capacity(self, len + 16);
		// Some platforms require we va_end()/va_start() before we use args again
		va_end(args);
		va_start(args, fmt);
		vsnprintf(self->data + self->len, len + 1, fmt, args);
	}
	va_end(args);

	self->len += len;
	self->data[self->len] = '\0';
}





void vector_init(struct vector *self, const size_t is)
{
	self->size 	= 0;
	self->len  	= 0;
	self->item_size = is;
	self->values 	= NULL;
}

void vector_dealloc(struct vector *self)
{
	if (self->values)
		free(self->values);
}

void vector_set_capacity(struct vector *self, const size_t n)
{
	const size_t c = self->size - self->len;

	if (c < n)
	{
		self->size += n - c;
		self->values = realloc(self->values, self->size * self->item_size);
	}
}

void *vector_pushnew(struct vector *self)
{
	if (self->len == self->size)
		vector_set_capacity(self, 128);

	void *slot = vector_value(self, vector_size(self));

	self->len++;
	return slot;
}

void *vector_push(struct vector *self, const void *p)
{
	void *out = vector_pushnew(self);

	memcpy(out, p, self->item_size);
	return out;
}

void *vector_pop(struct vector *self)
{
	return vector_value(self, --self->len);
}
	
void vector_reset(struct vector *self)
{
	self->len = 0;
}




void allocator_init(struct allocator *self, const size_t bc)
{
	self->curBankUtilization= bc;
	self->curBank 		= NULL;
	self->curBanksIndex 	= 0;
	self->bankCapacity 	= bc;
	vector_init(&self->banks, sizeof(char *));
}

void allocator_dealloc(struct allocator *self)
{
	while (vector_size(&self->banks))
		free(*(char **)vector_pop(&self->banks));
	vector_dealloc(&self->banks);
}

void *allocator_alloc(struct allocator *self, const size_t size)
{
	if (self->curBankUtilization + size > self->bankCapacity)
	{
		if (self->curBanksIndex == vector_size(&self->banks))
		{
			self->curBank = (char *)malloc(self->bankCapacity);
			vector_push(&self->banks, &self->curBank);
		}
		else
			self->curBank = *(char **)vector_value(&self->banks, self->curBanksIndex);
		
		++(self->curBanksIndex);
		self->curBankUtilization = size;

		return self->curBank;
	}

	char *ret = self->curBank + self->curBankUtilization;

	self->curBankUtilization+=size;

	return ret;
}

void allocator_reuse(struct allocator *self)
{
	self->curBankUtilization= self->bankCapacity;
	self->curBank 		= NULL;
	self->curBanksIndex 	= 0;
}


#define LINELEN 76
void base64_encode(uint8_t *data, size_t length, struct string *outputBuf, const int forHeaders)
{
	unsigned int i;
	uint8_t dtable[256];


	for (i = 0; i < 9; i++)
    	{
	      dtable[i] 	= 'A' + i;
	      dtable[i + 9] 	= 'J' + i;
	      dtable[26 + i] 	= 'a' + i;
	      dtable[26 + i + 9]= 'j' + i;
    	}

  	for (i = 0; i < 8; i++)
    	{
	      dtable[i + 18] 		= 'S' + i;
	      dtable[26 + i + 18] 	= 's' + i;
    	}

  	for (i = 0; i < 10; i++)
      		dtable[52 + i] = '0' + i;
  	dtable[62] = '+';
  	dtable[63] = '/';


	/* Make sure we have enough space to hold the base64 encoded data */

	uint32_t mm = length / 3 * 4, required = 32 + mm + (mm / LINELEN * 2);
	string_set_capacity(outputBuf, required);

	uint8_t *output 	= (uint8_t *)(string_data(outputBuf) + string_len(outputBuf)), *initial = output;
	int nChars = 0, lineLength = 0;
	uint8_t ig[3] = {0 , 0 , 0};


	while (length--)
	{
	
		if (nChars < 3)
			ig[nChars++] = *data++;
		else
		{
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[ig[0] >> 2];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[((ig[0] & 3) << 4) | (ig[1] >> 4)];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[((ig[1] & 0xf) << 2) | (ig[2] >> 6)];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[ig[2] & 0x3f];

			nChars = 1;
			ig[0] = *data++;
			ig[1] = 0;
			ig[2] = 0;
		}
	}

	/* There bound to be some characters left to process */

	switch (nChars)
	{
		case 3:
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[ig[0] >> 2];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[((ig[0] & 3) << 4) | (ig[1] >> 4)];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[((ig[1] & 0xf) << 2) | (ig[2] >> 6)];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[ig[2] & 0x3f];
			break;

		case 2:
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[ig[0] >> 2];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[((ig[0] & 3) << 4) | (ig[1] >> 4)];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[((ig[1] & 0xf) << 2) | (ig[2] >> 6)];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = '=';
			break;

		case 1:
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[ig[0] >> 2];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[((ig[0] & 3) << 4) | (ig[1] >> 4)];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = '=';
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = '=';
			break;

	}


	outputBuf->len = string_len(outputBuf) + (output - initial); 	// explicitly
}
#undef LINELEN
