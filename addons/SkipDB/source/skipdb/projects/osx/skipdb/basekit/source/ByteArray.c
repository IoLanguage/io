/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#include "Base.h" 

#define BYTEARRAY_C
#include "ByteArray.h"
#undef BYTEARRAY_C

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

ByteArray *ByteArray_new(void)
{ 
	return ByteArray_newWithSize_(0); 
}

ByteArray *ByteArray_newWithCString_(const char *s)
{ 
	return ByteArray_newWithData_size_((const unsigned char *)s, strlen(s)); 
}

ByteArray *ByteArray_newWithCString_size_(const char *s, int size)
{ 
	return ByteArray_newWithData_size_((const unsigned char *)s, size); 
}

ByteArray *ByteArray_newWithSize_(int size)
{
	ByteArray *self = (ByteArray *)calloc(1, sizeof(ByteArray));
	self->bytes = (unsigned char *)calloc(1, size + 1);
	self->size = size;
	self->memSize = size;
	return self;
}

ByteArray *ByteArray_newWithData_size_(const unsigned char* buf, size_t size)
{
	ByteArray *self = (ByteArray *)calloc(1, sizeof(ByteArray));
	
        self->bytes = (unsigned char *)malloc(size + 1);
        self->bytes[size] = (unsigned char)0;
        memcpy(self->bytes, buf, size);
	
	self->size = size;
	
	return self;
}

ByteArray *ByteArray_clone(ByteArray *self)
{ 
	return ByteArray_newWithData_size_(self->bytes, self->size); 
}

// datum ------------------------------------------------ 

Datum ByteArray_asDatum(ByteArray *self)
{
	Datum d;
	d.data = self->bytes;
	d.size = self->size;
	return d;
}

Datum *ByteArray_asNewDatum(ByteArray *self)
{
	Datum *d = (Datum *)malloc(sizeof(Datum));
	d->data = self->bytes;
	d->size = self->size;
	return d;
}

Datum ByteArray_datumAt_(ByteArray *self, size_t i)
{
	Datum d = ByteArray_asDatum(self);
	return Datum_datumAt_(&d, i);
}

// ---------------------------------------------------------- 

void ByteArray_free(ByteArray *self)
{
	/*
	 if (self == (void *)0x5c5c990)
	 { 
		 printf("ByteArray_free(%p)\n", self); 
	 }
	 */
	
	if (self->bytes) 
	{ 
		free(self->bytes); 
	}
	
	free(self);
}

size_t ByteArray_memorySize(ByteArray *self)
{ 
	return sizeof(ByteArray) + self->memSize; 
}

void ByteArray_compact(ByteArray *self)
{ 
	size_t newSize = self->size + 1;
	
	if (self->memSize > newSize)
	{
		unsigned char *b = (unsigned char *)malloc(newSize);
		
		memcpy(b, self->bytes, newSize);
		free(self->bytes);
		self->memSize = newSize;
		self->bytes = b;
	}
}

void ByteArray_clear(ByteArray *self)
{
	memset(self->bytes, 0, self->size);
	self->size = 0;
}

void ByteArray_setAllBytesTo_(ByteArray *self, unsigned char c)
{
	memset(self->bytes, (int)c, self->size);
}

void ByteArray_sizeTo_(ByteArray *self, size_t size)
{
	if (size > self->memSize)
	{
		//if (self->memSize == 0) self->bytes = 0;
		self->bytes = (unsigned char *)realloc(self->bytes, size + 1);
		self->memSize = size;
	}
	
	self->bytes[size] = (unsigned char)0;
	
	if (self->size > size) 
	{
		self->size = size;
	}
}

void ByteArray_setSize_(ByteArray *self, size_t size)
{
	ByteArray_sizeTo_(self, size);
	self->size = size;
}

void ByteArray_copy_(ByteArray *self, ByteArray *other)
{ 
	ByteArray_setData_size_(self, other->bytes, other->size);
	//printf("self %p size %i memSize %i\n", (void *)self, (int)self->size, (int)self->memSize);
	//printf("other %p size %i memSize %i\n", (void *)other, (int)other->size, (int)other->memSize);
}

void ByteArray_setData_size_(ByteArray *self, const unsigned char *bytes, size_t size)
{
	ByteArray_setSize_(self, size);
	
	if (size) 
	{
		memcpy(self->bytes, bytes, size);
	}
}

void ByteArray_setCString_(ByteArray *self, const char *s)
{ 
	ByteArray_setData_size_(self, (unsigned char *)s, (size_t)strlen(s)); 
}

// single byte/character operations ------------------------

int ByteArray_hasDigit(ByteArray *self) 
{
	size_t i;
	
	for (i = 0; i < self->size; i ++) 
	{ 
		if (isdigit(self->bytes[i])) 
		{
			return 1;
		}
	}
	
	return 0;
}

unsigned long ByteArray_at_bytesCount_(ByteArray *self, int i, int l)
{
	int n;
	long d = 0;
	
	if (!((i >= 0) && ((size_t)(i + l) <= ByteArray_size(self))))
	{ 
		return 0; 
	}
	
	// pack bytes into a Number 
	
	for (n = i + l - 1; n >= i; n --)
	{
		d = d << 8;
		d = d | (self->bytes[n]);
	}
	
	return d;
}

int ByteArray_at_put_(ByteArray *self, int pos, unsigned char c) 
{
	int len = self->size;
	
	if (pos < - len) 
	{ 
		return 0; 
	}
	
	if (pos > len) 
	{
		ByteArray_setSize_(self, pos);
	}
	
	pos = ByteArray_wrapPos_(self, pos);
	
	if (len == -1 || pos > len - 1) 
	{ 
		return 0; 
	}
	
	self->bytes[ByteArray_wrapPos_(self, pos)] = c;
	
	return 1;
}

unsigned char ByteArray_dropLastByte(ByteArray *self) 
{ 
	unsigned char c = 0;
	
	if (self->size)
	{
		self->size --; 
		c = self->bytes[self->size];
		self->bytes[self->size] = 0;
	}
	
	return c;
}

void ByteArray_removeByteAt_(ByteArray *self, int pos) 
{ 
	ByteArray_removeSlice(self, pos, pos + 1); 
}

void ByteArray_removeCharAt_(ByteArray *self, int pos) 
{
	int csize = BYTEARRAY_BYTES_PER_CHARACTER;
	int len = self->size;
	
	if (len < 1) 
	{
		return;
	}
	
	pos = ByteArray_wrapPos_(self, pos);
	
	memmove(self->bytes + pos * csize, 
			self->bytes + (pos + 1) * csize, 
			(len - pos + 1) * csize);
}

void ByteArray_removeSlice(ByteArray *self, int from, int to)
{
	int len = self->size;
	
	if (len < 1) 
	{
		return;
	}
	
	from = ByteArray_wrapPos_(self, from);
	to   = ByteArray_wrapPos_(self, to);
	
	memmove(self->bytes + from, self->bytes + to, len - to);
	ByteArray_setSize_(self, len - (to - from));
}

// escape --------------------------------------------

void ByteArray_escape(ByteArray *self)
{
	ByteArray *ba = ByteArray_new();
	char *s = (char *)self->bytes;
	size_t i;
	
	for (i = 0; i < self->size; i ++)
	{
		char c = s[i];
		
		switch (c)
		{ 
			case '"': ByteArray_appendCString_(ba, "\\\""); break;
case '\a': ByteArray_appendCString_(ba, "\\a"); break;
case '\b': ByteArray_appendCString_(ba, "\\b"); break;
case '\f': ByteArray_appendCString_(ba, "\\f"); break;
case '\n': ByteArray_appendCString_(ba, "\\n"); break;
case '\r': ByteArray_appendCString_(ba, "\\r"); break;
case '\t': ByteArray_appendCString_(ba, "\\t"); break;
case '\v': ByteArray_appendCString_(ba, "\\v"); break;
case '\\': ByteArray_appendCString_(ba, "\\\\"); break;
default: ByteArray_appendChar_(ba, c);
		}
    }

s[i] = (char)NULL;
ByteArray_copy_(self, ba);
ByteArray_free(ba);
}

void ByteArray_unescape(ByteArray *self)
{
	int mbskip   = 0;  /* multi-byte character size */
	size_t getIndex = 0;
	size_t putIndex = 0;
	char *s = (char *)self->bytes;
	
	while (getIndex < self->size)
	{
		int c = s[getIndex];
		int nextChar = s[getIndex + 1];
		
		if (mbskip <= 0 && ismbchar(c)) 
		{ 
			mbskip = mbcharlen(c); 
		}
		
		if (c != '\\' || mbskip > 0)
		{
			mbskip --;
			
			if (getIndex != putIndex) 
			{  
				s[putIndex] = c; 
			}
			
			putIndex ++;
		}
		else
		{
			char c = nextChar;
			
			switch (c)
			{
				case  'a': c = '\a'; break;
				case  'b': c = '\b'; break;
				case  'f': c = '\f'; break;
				case  'n': c = '\n'; break;
				case  'r': c = '\r'; break;
				case  't': c = '\t'; break;
				case  'v': c = '\v'; break;
				case '\0': c = '\\'; break;
				default:
					if (isdigit(c))
					{
						c -= 48; 
					}
			}
			
			s[putIndex] = c;
			getIndex ++; 
			putIndex ++;
		}
		
		getIndex++;
	}
	
	s[putIndex] = (char)NULL;
	ByteArray_setSize_(self, putIndex);
}

void ByteArray_quote(ByteArray *self)
{
	int oldSize = self->size;
	ByteArray_setSize_(self, self->size + 2);
	memmove(self->bytes + 1, self->bytes, oldSize);
	self->bytes[0] = '"';
	self->bytes[self->size - 1] = '"';
}

void ByteArray_unquote(ByteArray *self)
{
	if (self->size == 2)
	{ 
		ByteArray_setSize_(self, 0); 
	}
	else
	{
		memmove(self->bytes, self->bytes + 1, self->size - 1);
		ByteArray_setSize_(self, self->size - 2);
	}
}

// append --------------------------------------------

void ByteArray_appendChar_(ByteArray *self, char c)
{
	ByteArray_appendBytes_size_(self, (const unsigned char *)(&c), 1);
}

void ByteArray_appendByte_(ByteArray *self, unsigned char c)
{
	ByteArray_appendBytes_size_(self, &c, 1);
}

void ByteArray_append_(ByteArray *self, ByteArray *other)
{ 
	// Don't use, ByteArray_appendBytes_size_(self, other->bytes, other->size);
	// because it can copy the wrong memory when self == other and a resize occurs.
	
	int otherSize = other->size;
	
	if(otherSize)
	{
		int selfSize = self->size;
		ByteArray_setSize_(self, selfSize + otherSize);
		memcpy(self->bytes + selfSize, other->bytes, otherSize);
	}
}

void ByteArray_appendCString_(ByteArray *self, const char *s)
{ 
	ByteArray_appendBytes_size_(self, (const unsigned char *)s, strlen(s)); 
}

void ByteArray_appendAndEscapeCString_(ByteArray *self, const char *s)
{ 
	char oneChar[2];
	
	oneChar[1] = (char)0;
	
	while (*s)
	{
		if (*s == '\n')
		{ 
			ByteArray_appendBytes_size_(self, (const unsigned char *)"\\n", 2); 
		}
		else if (*s == '\t')
		{ 
			ByteArray_appendBytes_size_(self, (const unsigned char *)"\\t", 2); 
		}
		else if (*s == '"')
		{ 
			ByteArray_appendBytes_size_(self, (const unsigned char *)"\\\"", 2); 
		}
else
{
			oneChar[0] = *s;
			ByteArray_appendBytes_size_(self, (const unsigned char *)oneChar, 1);
}

s ++;
	}

self->bytes[self->size] = (unsigned char)0;
}

void ByteArray_appendBytes_size_(ByteArray *self, const unsigned char *bytes, size_t size)
{
	int oldSize;
	
	if(size == 0)
		return;
	
	oldSize = self->size;
	ByteArray_setSize_(self, oldSize + size);
	memcpy(self->bytes + oldSize, bytes, size);
	//self->bytes[self->size] = (unsigned char)0; // not needed? 
}

// prepend --------------------------------------------

void ByteArray_prepend_(ByteArray *self, ByteArray *other)
{ 
	ByteArray_prependBytes_size_(self, other->bytes, other->size); 
}

void ByteArray_prependCString_(ByteArray *self, const char *s)
{ 
	ByteArray_prependBytes_size_(self, (const unsigned char *)s, strlen(s)); 
}

void ByteArray_prependBytes_size_(ByteArray *self, const unsigned char *bytes, size_t size)
{
	int oldSize = self->size;
	ByteArray_setSize_(self, oldSize + size);
	memmove(self->bytes + size, self->bytes, oldSize);
	memcpy(self->bytes, bytes, size);
}

// subarray --------------------------------------------

ByteArray *ByteArray_newWithBytesFrom_to_(ByteArray *self, int startpos, int endpos) 
{
	int newlen;
	
	startpos = ByteArray_wrapPos_(self, startpos);
	endpos   = ByteArray_wrapPos_(self, endpos);
	newlen = endpos - startpos;
	
	if (newlen < 1) 
	{
		return ByteArray_new();
	}
	
	return ByteArray_newWithData_size_(self->bytes+startpos, newlen);
}

// insert --------------------------------------------

void ByteArray_insert_at_(ByteArray *self, ByteArray *other, size_t pos) 
{ 
	ByteArray_insertBytes_size_at_(self, other->bytes, other->size, pos); 
}

void ByteArray_insertCString_at_(ByteArray *self, const char *s, size_t pos) 
{ 
	ByteArray_insertBytes_size_at_(self, (const unsigned char *)s, strlen(s), pos); 
}

void ByteArray_insertBytes_size_at_(ByteArray *self, const unsigned char *bytes, size_t size, size_t pos) 
{
	long oldSize = self->size;
	pos = ByteArray_wrapPos_(self, pos);
	
	if (pos == self->size) 
	{ 
		ByteArray_appendBytes_size_(self, bytes, size); 
		return; 
	}
	
	ByteArray_setSize_(self, self->size + size);
	memmove(self->bytes + pos + size, self->bytes + pos, oldSize - pos);
	memcpy(self->bytes + pos, bytes, size);
}

// clipping --------------------------------------------

char ByteArray_clipBefore_(ByteArray *self, ByteArray *other)
{
	int i = ByteArray_find_(self, other);
	if (i == -1) return 0;
	ByteArray_removeSlice(self, 0, i);
	return 1;
}

char ByteArray_clipBeforeEndOf_(ByteArray *self, ByteArray *other)
{
	int i = ByteArray_find_(self, other);
	if (i == -1) return 0;
	ByteArray_removeSlice(self, 0, i + ByteArray_size(other));
	return 1;
}

char ByteArray_clipAfter_(ByteArray *self, ByteArray *other)
{
	int i = ByteArray_find_(self, other);
	if (i == -1) return 0;
	ByteArray_removeSlice(self, i + ByteArray_size(other), ByteArray_size(self));
	return 1;
}

char ByteArray_clipAfterStartOf_(ByteArray *self, ByteArray *other)
{
	int i = ByteArray_find_(self, other);
	if (i == -1) return 0;
	ByteArray_removeSlice(self, i, ByteArray_size(self));
	return 1;
}

// strip --------------------------------------------

int ByteArray_containsByte_(ByteArray *self, unsigned char b)
{
	size_t i;
	
	for (i = 0; i < self->size; i ++)
	{
		unsigned char c = self->bytes[i];
		
		if (c == b) 
		{
			return 1;
		}
	}
	
	return 0;
}


void ByteArray_strip_(ByteArray *self, ByteArray *other)
{
	ByteArray_lstrip_(self, other);
	ByteArray_rstrip_(self, other);
}

void ByteArray_lstrip_(ByteArray *self, ByteArray *other)
{
	size_t i;
	
	for (i = 0; i < self->size; i ++)
	{
		unsigned char c = self->bytes[i];
		
		if (!ByteArray_containsByte_(other, c))
		{
			break;
		}
	}
	
	ByteArray_removeSlice(self, 0, i);
}

void ByteArray_rstrip_(ByteArray *self, ByteArray *other)
{
	size_t i = self->size;
	
	do
	{
		i --;
		
		if (!ByteArray_containsByte_(other, self->bytes[i])) 
		{
			break;
		}
		
	} while (i != 0);
	
	ByteArray_removeSlice(self, i + 1, self->size);
}

// enumerating --------------------------------------------

typedef int (ByteArrayDetectFunc)(int);

int ByteArray_detect_(ByteArray *self, ByteArrayDetectFunc *func)
{
	unsigned char *s = self->bytes;
	unsigned char *end = self->bytes + self->size;
	
	while (s < end)
	{
		if ((*func)(*s)) 
		{
			return s - self->bytes;
		}
		
		s ++;
	}
	
	return -1;
}

typedef int (ByteArrayDetectWithFunc)(void *, Datum *);

int ByteArray_detect_with_(ByteArray *self, ByteArrayDetectWithFunc *func, void *arg)
{
	unsigned char *s = self->bytes;
	unsigned char *end = self->bytes + self->size;
	
	Datum d;
	
	while (s < end)
	{
		d.data = s;
		d.size = end - s;
		
		if ((*func)(arg, &d)) 
		{
			return s - self->bytes;
		}
		
		s ++;
	}
	
	return -1;
}

//typedef unsigned char (ByteArrayCollectFunc)(unsigned char);
// tolower and toupper use this signature:

typedef int (ByteArrayCollectFunc)(int);

void ByteArray_collectInPlace_(ByteArray *self, ByteArrayCollectFunc *func)
{
	unsigned char *s = self->bytes;
	unsigned char *end = self->bytes + self->size;
	
	while (s < end)
	{
		*s = (*func)(*s);
		s ++;
	}
}

// case --------------------------------------------

int ByteArray_isLowercase(ByteArray *self)
{
	return ByteArray_detect_(self, (ByteArrayDetectFunc *)isupper) == -1;
}

int ByteArray_isUppercase(ByteArray *self)
{
	return ByteArray_detect_(self, (ByteArrayDetectFunc *)islower) == -1;
}

void ByteArray_Lowercase(ByteArray *self)
{
	ByteArray_collectInPlace_(self, (ByteArrayCollectFunc *)tolower);
}

void ByteArray_uppercase(ByteArray *self)
{
	ByteArray_collectInPlace_(self, (ByteArrayCollectFunc *)toupper);
	
}

// string comparision operations --------------------------------

int ByteArray_equals_(ByteArray *self, ByteArray *other)		
{
	// returns 1 if equal, 0 if not equal. 
	
	if (self->size != other->size) 
	{
		return 0;
	}
	
	return (memcmp(self->bytes, other->bytes, self->size) == 0);
}

int ByteArray_equalsAnyCase_(ByteArray *self, ByteArray *other)		
{
	if (self->size == other->size)
	{
		if (self->size == 0) return 1; 
		return ByteArray_containsAnyCase_(self, other);
	}
	
	return 0;
}


int ByteArray_contains_(ByteArray *self, ByteArray *other)		
{ 
	return (ByteArray_find_(self, other) != -1); 
}

int ByteArray_containsAnyCase_(ByteArray *self, ByteArray *other)		
{ 
	return (ByteArray_findAnyCase_(self, other) != -1); 
}

int ByteArray_find_(ByteArray *self, ByteArray *other)		
{ 
	return ByteArray_find_from_(self, other, 0); 
}

int ByteArray_beginsWith_(ByteArray *self, ByteArray *other)		
{ 
	if (!other->size) 
	{
		return 1;
	}
	
	if (!self->size || other->size > self->size) 
	{
		return 0;
	}
	
	return (memcmp((char *)(self->bytes), (char *)(other->bytes), other->size) == 0); 
}

int ByteArray_endsWith_(ByteArray *self, ByteArray *other)		
{ 
	int sl = self->size;
	int ol = other->size;
	
	if (!ol) 
	{
		return 1;
	}
	
	if (!sl || ol > sl) 
	{
		return 0;
	}
	
	return (memcmp((char *)(self->bytes + sl - ol), (char *)other->bytes, ol) == 0); 
}

int ByteArray_findAnyCase_(ByteArray *self, ByteArray *other)		
{ 
	return ByteArray_findAnyCase_from_(self, other, 0); 
}

static int ByteArray_contains_at_(ByteArray *self, ByteArray *other, size_t from)
{
	if (from >= self->size || self->size - from < other->size) 
	{
		return 0;
	}
	
	return (memcmp(self->bytes + from, other->bytes, other->size) == 0);
}

// return -1 for no match, starting position of match if found 

int ByteArray_find_from_(ByteArray *self, ByteArray *other, int from)		
{
	int i, max = self->size - other->size + 1;
	
	from = ByteArray_wrapPos_(self, from);
	
	//string_caseInsensitiveBoyerMooreSearch()
	
	for (i = from; i < max; i ++)
	{
		if (ByteArray_contains_at_(self, other, i)) 
		{
			return i;
		}
	}
	
	return -1;
}

int ByteArray_findCString_from_(ByteArray *self, const char *other, int from)
{
	from = ByteArray_wrapPos_(self, from);
	
	{
		char *p = strstr((char *)(self->bytes + from), other);
		ptrdiff_t rval;
		
		if (!p) 
		{
			return -1;
		}
		
		rval = (ptrdiff_t)p - (ptrdiff_t)(self->bytes);
		return rval;
	}
}

int ByteArray_rFindCString_from_(ByteArray *self, const char *other, int from)		
{
	// return -1 for no match, starting position of match if found 
	
	unsigned char *bytes = self->bytes;
	size_t otherLen = strlen(other);
	
	from = ByteArray_wrapPos_(self, from);
	bytes = bytes + from - otherLen;
	
	while (bytes >= self->bytes)
	{
		if (memcmp(bytes, other, otherLen) == 0)
		{ 
			return bytes - self->bytes; 
		}
		
		bytes --;
	}
	
	return -1;
}

int ByteArray_rFind_from_(ByteArray *self, ByteArray *other, int from)		
{
	// return -1 for no match, starting position of match if found 
	unsigned char *bytes = self->bytes;
	
	from = ByteArray_wrapPos_(self, from);
	bytes = bytes + from - other->size;
	
	while (bytes >= self->bytes)
	{
		if (memcmp(bytes, other->bytes, other->size) == 0)
		{ 
			return bytes - self->bytes; 
		}
		
		bytes --;
	}
	
	return -1;
}

int ByteArray_rFindCharacters_from_(ByteArray *self, const char *chars, int from)
{
    // return -1 for no match, starting position of match if found 
	
    unsigned char *start = self->bytes;
    unsigned char *p = self->bytes;
    const char* pChar = NULL;
	
    from = ByteArray_wrapPos_(self, from);
    p = p + from - 1;  // -1 : the from index is not included in the search
	
    // for each byte in self
    while( start <= p )
    {
        // for each character to match
        for( pChar = chars; *pChar ; ++pChar )
        {
            if( *pChar == *p )
            { 
                return p - start;
            }
        }
		
        --p;
    }
	
    return -1;
}

int ByteArray_findAnyCase_from_(ByteArray *self, ByteArray *other, int from)		
{
	// return -1 for no match 
	
	size_t n, m;
	size_t len2 = other->size;
	size_t max = self->size - other->size;
	
	from = ByteArray_wrapPos_(self, from);
	
	if (self->size < len2) return -1; // too big 
	if (len2 <= 0) return -1; // too small 
	
	for (n = from; n <= max; n++) 
	{
		m = 0;
		
		while ((tolower(self->bytes[n+m]) == tolower(other->bytes[m]) && (m < len2 + 1))) 
		{
			if (m == len2 - 1) 
			{
				return n;
			}
			
			m ++;
		}
	}
	
	return -1;
}

int ByteArray_findByteWithValue_from_(ByteArray *self, unsigned char v, int from)
{
	unsigned char *b = self->bytes;
	size_t index = from;
	size_t max = self->size;
	
	while (index < max)
	{
		if (b[index] == v) 
		{
			return index;
		}
		
		index ++; 
	}
	
	return -1;
}

int ByteArray_findByteWithoutValue_from_(ByteArray *self, unsigned char v, int from)
{
	unsigned char *b = self->bytes;
	size_t index = from;
	
	while (index < self->size)
	{
		if (b[index] != v) 
		{
			return index;
		}
		
		index ++; 
	}
	
	return -1;
}

void ByteArray_setByteWithValue_from_to_(ByteArray *self, 
										 unsigned char v, 
										 size_t from, 
										 size_t to)
{
	if (from > to)
	{
		int x = to;
		to = from;
		from = x;
	}
	
	if (to > ByteArray_size(self)) 
	{
		ByteArray_setSize_(self, to + 1);
	}
	
	if (to < from) 
	{
		return;
	}
	
	if (to == from) 
	{ 
		//printf("bytes[%i] = %i -> %i\n", from, self->bytes[from], v);
		self->bytes[from] = v; 
		return; 
	}
	
	memset(self->bytes + from, v, to - from + 1);
}

size_t ByteArray_count_(ByteArray *self, ByteArray *other)		
{
	// return number of non-overlapping occurances of other in self 
	
	size_t count = 0;
	
	if (other->size > 0)
	{
		int index = ByteArray_find_from_(self, other, 0);
		
		while (index != -1) 
		{
			index = ByteArray_find_from_(self, other, index + other->size);
			count ++;
		}
	}
	
	return count;
}

void ByteArray_replaceCString_withCString_(ByteArray *self, 
										   const char *s1, 
										   const char *s2)
{
	ByteArray *b1 = ByteArray_newWithCString_(s1);
	ByteArray *b2 = ByteArray_newWithCString_(s2);
	ByteArray_replace_with_(self, b1, b2);
	ByteArray_free(b1);
	ByteArray_free(b2);
}

void ByteArray_replace_with_output_(ByteArray *self, 
									ByteArray *substring, 
									ByteArray *other, 
									ByteArray *output)
{
	int lastGetIndex = 0;
	int getIndex = 0;
	
	while (getIndex < (int)self->size)
	{
		getIndex = ByteArray_find_from_(self, substring, getIndex);
		if (getIndex == -1) getIndex = self->size;
		
		// append the non-matching chunk 
		ByteArray_appendBytes_size_(output, self->bytes + lastGetIndex, getIndex - lastGetIndex);
		
		if (getIndex == (int)self->size) 
		{
			break;
		}
		
		// append the other string 
		ByteArray_append_(output, other);
		getIndex += substring->size;
		lastGetIndex = getIndex;
	}
}

void ByteArray_replaceFrom_size_with_(ByteArray *self, 
									  size_t index, 
									  size_t substringSize, 
									  ByteArray *other)
{
	size_t oldSize = ByteArray_size(self);
	size_t otherSize = ByteArray_size(other);
	size_t newSize = oldSize - substringSize + otherSize;
	
	if (newSize > oldSize)
	{
		ByteArray_setSize_(self, newSize);
	}
	
	// move tail to end 
	
	{
		size_t oldTailPos = index + substringSize;
		size_t newTailPos = index + otherSize;
		size_t tailSize = oldSize - oldTailPos;
		memmove(self->bytes + newTailPos, self->bytes + oldTailPos, tailSize);
	}
	
	// copy in new chunk 
	memcpy(self->bytes + index, other->bytes, otherSize);
	
	ByteArray_setSize_(self, newSize);
}

size_t ByteArray_replaceFirst_from_with_(ByteArray *self, 
										 ByteArray *substring, 
										 size_t start, 
										 ByteArray *other)
{
	size_t substringSize = ByteArray_size(substring);
	int index = ByteArray_find_from_(self, substring, start);
	
	if (index != -1) 
	{
		ByteArray_replaceFrom_size_with_(self, index, substringSize, other);
		return 1;
	}
	
	return 0;
}

size_t ByteArray_replace_with_(ByteArray *self, ByteArray *substring, ByteArray *other)
{
	ByteArray *output = ByteArray_new();
	ByteArray_replace_with_output_(self, substring, other, output);
	ByteArray_copy_(self, output);
	ByteArray_free(output);
	return 0;
}

/*
 size_t ByteArray_replace_with_2(ByteArray *self, ByteArray *substring, ByteArray *other)
 {
	 int index;
	 size_t start;
	 size_t substringSize = ByteArray_size(substring);
	 size_t otherSize = ByteArray_size(other);
	 List *indexes = List_new();
	 
	 // find match indexes 
	 while ((index = ByteArray_find_from_(self, substring, start)) != -1)
	 {
		 List_add_(indexes, (void *)index);
		 start = index += substringSize;
	 }
	 
	 size_t matchCount = List_size(indexes);
	 size_t oldSize = ByteArray_size(self);
	 size_t newSize = oldSize - ((otherSize - substringSize) * matchCount);
	 
	 if (newSize > oldSize)
	 {
		 int i;
		 
		 // walk match indexes in reverse order and on each one,
		 // copy the tail to the right and 
		 // copy the other string  
		 
		 ByteArray_setSize_(self, newSize);
		 
		 for (i = matchCount; i > -1; i --)
		 {
			 index = (int)List_at_(indexes, i);
			 
		 }
	 }
	 else
	 {
		 // "a foo b" replace("foo", "c")
		 // walk match indexes in normal order and on each copy
		 // over the other string and the next chunk
		 
		 int start = 0;
		 int index = ByteArray_find_from_(self, substring, start);
		 size_t putIndex = List_at_(indexes, i);
		 size_t gapSize;
		 int nextIndex;
		 
		 for (;;)
		 {
			 index = (int)List_at_(indexes, i);
			 nextIndex = i < matchCount ? (int)List_at_(indexes, i + 1); 
			 // copy in other: 
			 // "a coo b"
			 
			 memcpy(self->bytes + putIndex, other->bytes, otherSize);
			 putIndex += otherSize;
			 
			 // copy chunk to next match: 
			 // "a c b"
			 
			 i ++;
			 if (i == matchCount)
			 {
				 nextIndex = oldSize;
			 }
			 
			 memmove(self->bytes + putIndex, self->bytes , otherSize);
			 putIndex += gapSize;
			 
		 } while (i < matchCount)
			 
			 ByteArray_setSize_(self, newSize);
	 }
	 
	 List_free(indexes);
	 return matchCount
 }
 */

// I/O ---------------------------------------------

void ByteArray_print(ByteArray *self)
{ 
	ByteArray_writeToCStream_(self, stdout); 
}

size_t ByteArray_writeToCStream_(ByteArray *self, FILE *stream)
{ 
	return fwrite(self->bytes, 1, self->size, stream); 
}

int ByteArray_writeToFilePath_(ByteArray *self, const char *path)
{
	FILE *fp = fopen(path, "w+");
	
	if (fp) 
	{
		size_t bytesWritten = ByteArray_writeToCStream_(self, fp);
		fclose(fp);
		return bytesWritten == self->size ? 0 : -1;
	}   
	
	return -1;
}

size_t sane_fread(void *buffer, size_t size, size_t nmemb, FILE *stream)
{
	size_t bytesToRead = size * nmemb;
	size_t bytesRead = 0;
	
	while(bytesRead < bytesToRead)
	{
		unsigned char c = (unsigned char)fgetc(stream);
		
		if(feof(stream) || ferror(stream)) 
		{
			break;
		}
		
		((unsigned char *)buffer)[bytesRead] = c;
		bytesRead ++;
	}
	
	return bytesRead;
}

int ByteArray_readFromCStream_(ByteArray *self, FILE *fp)
{
	size_t bufferSize = 4096;
	unsigned char *buffer = (unsigned char *)malloc(bufferSize);
	size_t totalBytesRead = 0;
	
	if (!fp) 
	{
		perror("ByteArray_readFromCStream_");
		return -1; 
	}
		
	while(!feof(fp) && !ferror(fp))
	{
		size_t pos = ftell(fp);
		size_t bytesRead = sane_fread(buffer, bufferSize, 1, fp);
		totalBytesRead += bytesRead;
		ByteArray_appendBytes_size_(self, buffer, bytesRead);
		if (bytesRead != bufferSize) break;
	}
	
	if (ferror(fp)) 
	{
		perror("ByteArray_readFromCStream_");
		return -1; 
	}
	
	//printf("feof(fp) = %i\n", feof(fp));
	//printf("ferror(fp) = %i\n", ferror(fp));
	
	free(buffer);
	return totalBytesRead;
}

int ByteArray_readFromFilePath_(ByteArray *self, const char *path)
{
	FILE *fp = fopen(path, "rb");
	int error;
	
	if (!fp) 
	{ 
		perror("ByteArray_readFromFilePath_");
		return -1;
	}
	
	error = ByteArray_readFromCStream_(self, fp);
	fclose(fp); 
	return error;
}

/*
 
 unsigned char ByteArray_readLineFromCStream_(ByteArray *self, FILE *stream)
 {
	 unsigned char readSomething = 0;
	 
	 while(ferror(stream) == 0)
	 {
		 int b = fgetc(stream);
		 readSomething = 1;
		 
		 if ( b == '\n' || b == '\r') 
		 {
			 break;
		 }
		 
		 ByteArray_appendByte_(self, b);
	 }
	 
	 return readSomething;
 }
 */

#define CHUNK_SIZE 4096

unsigned char ByteArray_readLineFromCStream_(ByteArray *self, FILE *stream)
{
	unsigned char readSomething = 0;
	char *s = (char *)malloc(CHUNK_SIZE);
	
	while (fgets(s, CHUNK_SIZE, stream) != NULL)
	{
		char *eol1 = strchr(s, '\n');
		char *eol2 = strchr(s, '\r');
		
		readSomething = 1;
		
		if (eol1) { *eol1 = 0; } // remove the \n return character
		if (eol2) { *eol2 = 0; } // remove the \r return character
		
		if (*s)
		{
			ByteArray_appendCString_(self, s);
		}
		
		if (eol1 || eol2)
		{
			break;
		}
	}
	
	free(s);
	
	return readSomething;
}

size_t ByteArray_readNumberOfBytes_fromCStream_(ByteArray *self, size_t size, FILE *stream)
{
	size_t readSize;
	size_t oldSize = ByteArray_size(self);
	
	ByteArray_setSize_(self, oldSize + size);
	readSize = sane_fread((void *)(self->bytes + oldSize), 1, size, stream);
	ByteArray_setSize_(self, oldSize + readSize);
	
	return readSize;
}

// private utility functions ---------------------------------

int ByteArray_wrapPos_(ByteArray *self, int pos)
{
	int len = self->size;
	
	if (pos > len - 1) 
	{
		return len;
	}
	
	if (pos < 0) 
	{
		pos = len + pos; 
		
		if (pos < 0) 
		{
			pos = 0;
		}
	}
	
	return pos;
}

ByteArray *ByteArray_newWithFormat_(const char *format, ...)
{
	ByteArray *self;
	va_list ap;
	va_start(ap, format);
	self = ByteArray_newWithVargs_(format, ap);
	va_end(ap);
	return self;
}

ByteArray *ByteArray_newWithVargs_(const char *format, va_list ap)
{
	ByteArray *self = ByteArray_new();
	ByteArray_fromVargs_(self, format,ap);
	return self;
}

ByteArray *ByteArray_fromFormat_(ByteArray *self, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	ByteArray_fromVargs_(self, format, ap);
	va_end(ap);
	return self;
}

void ByteArray_fromVargs_(ByteArray *self, const char *format, va_list ap)
{  
	while (*format)
	{
		if (*format == '%')
		{
			format ++;
			
			if (*format == 's')
			{
				char *s = va_arg(ap, char *);
				if (!s) { printf("IoState_print missing param"); return; }
				ByteArray_appendCString_(self, s);
			}
			else if (*format == 'i' || *format == 'd')
			{
				int i = va_arg(ap, int);
				char s[100];
				
				snprintf(s, 100, "%i", i);
				ByteArray_appendCString_(self, s);
			}
			else if (*format == 'f')
			{
				double d = va_arg(ap, double);
				char s[100];
				
				snprintf(s, 100, "%f", d);
				ByteArray_appendCString_(self, s);
			}
			else if (*format == 'p')
			{
				void *p = va_arg(ap, void *);
				char s[100];
				
				snprintf(s, 100, "%p", p);
				ByteArray_appendCString_(self, s);
			}
			// new format command for a given number adding spaces
			else if (*format == '#')  
			{
				int n, i = va_arg(ap, int);
				char *s = " ";
				
				for (n = 0; n < i; n ++) 
				{
					ByteArray_appendCString_(self, s);
				}
			}
		}
		else
		{
			char s[2];
			
			snprintf(s, 2, "%c", *format);
			ByteArray_appendCString_(self, s);
		}
		
		format ++;
	}
}

ByteArray *ByteArray_asNewHexStringByteArray(ByteArray *self)
{
	size_t i, newSize = self->size * 2;
	ByteArray *ba = ByteArray_newWithSize_(newSize);
	
	for(i = 0; i < self->size; i ++)
	{
		int c = self->bytes[i];
		char *s = (char *)(ba->bytes + i * 2);
		
		if (c < 16) 
		{
			snprintf(s, newSize, "0%x", c); 
		}
		else 
		{
			snprintf(s, newSize, "%x", c);
		}
	}
	
	return ba;
}

// file paths --------------------------------------

unsigned char ByteArray_lastByte(ByteArray *self)
{
	if (!self->size) 
	{
		return 0;
	}
	
	return self->bytes[self->size - 1];
}

void ByteArray_appendPathCString_(ByteArray *self, const char *path)
{
	char lastChar = ByteArray_lastByte(self);
	int selfEndsWithSep = IsPathSeparator(lastChar);
	int pathStartsWithSep = IsPathSeparator(*path);
	//int pathStartsWithDoubleSep = IsPathSeparator(*path) && strlen(path) > 1 && IsPathSeparator(*(path + 1));
	
	/*
	 if (pathStartsWithSep)
	 {
		 ByteArray_setCString_(self, path); 
	 }
	 else 
	 */
	
	if (ByteArray_size(self) != 0)
	{
		if (!selfEndsWithSep)
		{
			ByteArray_appendCString_(self, IO_PATH_SEPARATOR); 
		}
		
		if (pathStartsWithSep)
		{
			path ++;
		}
	}
	
	ByteArray_appendCString_(self, path); 
	
	ByteArray_replaceCString_withCString_(self, IO_PATH_SEPARATOR_DOT, IO_PATH_SEPARATOR);
	//ByteArray_replaceCString_withCString_(self, "//", "/");
}

void ByteArray_removeLastPathComponent(ByteArray *self)
{
	int pos = ByteArray_rFindCharacters_from_(self, IO_PATH_SEPARATORS, self->size - 1);
	if (pos == -1) pos = 0;
	ByteArray_setSize_(self, pos);
}

void ByteArray_clipBeforeLastPathComponent(ByteArray *self)
{
	int pos = ByteArray_rFindCharacters_from_(self, IO_PATH_SEPARATORS, self->size - 1);
	
	if (pos != -1) 
	{
		ByteArray_removeSlice(self, 0, pos + 1);
	}
}

ByteArray *ByteArray_lastPathComponent(ByteArray *self)
{ 
	return ByteArray_newWithCString_(ByteArray_lastPathComponentAsCString(self)); 
}

char *ByteArray_lastPathComponentAsCString(ByteArray *self)
{
	int pos = ByteArray_rFindCharacters_from_(self, IO_PATH_SEPARATORS, self->size-1);
	char *s;
	
	if (pos == -1) 
	{ 
		return (char *)self->bytes; 
	}
	
	s = (char *)(self->bytes+pos);
	
	while ( IsPathSeparator(*s) )
	{
		s ++;
	}
	
	return s;
}

void ByteArray_removePathExtension(ByteArray *self)
{ 
	int pos = ByteArray_rFindCString_from_(self, ".", self->size - 1);
	
	if (pos != -1) 
	{
		ByteArray_setSize_(self, pos);
	}
}

ByteArray *ByteArray_pathExtension(ByteArray *self)
{
	int pos = ByteArray_rFindCString_from_(self, ".", self->size - 1);
	
	if (pos == -1) 
	{
		return ByteArray_newWithCString_("");
	}
	
	return ByteArray_newWithCString_((char *)(self->bytes + pos + 1));
}

ByteArray *ByteArray_fileName(ByteArray *self)
{
	char *s = ByteArray_lastPathComponentAsCString(self);
	char *dot = strrchr(s, '.');
	
	if (!dot) 
	{
		return ByteArray_newWithCString_(s);
	}
	
	return ByteArray_newWithCString_size_(s, dot - s);
}

// bitwise ops ------------------------------------------------ 

void ByteArray_and_(ByteArray *self, ByteArray *other)
{
	int l1 = ByteArray_size(self);
	int l2 = ByteArray_size(other);
	
	int i = l1 < l2 ? l1 : l2;
	
	unsigned char *b1 = ByteArray_bytes(self);
	unsigned char *b2 = ByteArray_bytes(other);
	
	while (i)
	{
		*b1 = *b1 & *b2;
		b1 ++; 
		b2 ++;
		i --;
	}
}

void ByteArray_or_(ByteArray *self, ByteArray *other)
{
	int l1 = ByteArray_size(self);
	int l2 = ByteArray_size(other);
	
	int i = l1 < l2 ? l1 : l2;
	
	unsigned char *b1 = ByteArray_bytes(self);
	unsigned char *b2 = ByteArray_bytes(other);
	
	while (i)
	{
		*b1 = *b1 | *b2;
		b1 ++; 
		b2 ++;
		i --;
	}
}

void ByteArray_xor_(ByteArray *self, ByteArray *other)
{
	int l1 = ByteArray_size(self);
	int l2 = ByteArray_size(other);
	int i = l1 < l2 ? l1 : l2;
	unsigned char *b1 = ByteArray_bytes(self);
	unsigned char *b2 = ByteArray_bytes(other);
	
	while (i)
	{
		*b1 = *b1 ^ *b2;
		b1 ++; 
		b2 ++;
		i --;
	}
}

void ByteArray_compliment(ByteArray *self)
{
	int i = ByteArray_size(self);
	unsigned char *b = ByteArray_bytes(self);
	
	while (i)
	{
		*b = ~ (*b);
		b ++; 
		i --;
	}
}

void ByteArray_byteShiftLeft_(ByteArray *self, int s)
{
	ByteArray *ba = ByteArray_new();
	ByteArray_setSize_(ba, s);
	ByteArray_insert_at_(self, ba, 0);
}

void ByteArray_byteShiftRight_(ByteArray *self, int s)
{
	ByteArray *ba = ByteArray_new();
	ByteArray_setSize_(ba, s);
	ByteArray_removeSlice(self, 0, s);
	ByteArray_append_(self, ba);
}

void ByteArray_bitShiftLeft_(ByteArray *self, int s)
{
	int bytes = s / 8;
	int bits  = s % 8;
	int carryBits  = 8 - bits;
	
	if (bytes > 0) 
	{ 
		ByteArray_byteShiftLeft_(self, bytes); 
	}
	else 
	{ 
		ByteArray_byteShiftRight_(self, bytes); 
	}
	
	{
		int i = ByteArray_size(self);
		unsigned char *b = ByteArray_bytes(self);
		unsigned char carry = 0;
		
		while (i)
		{
			*b = *b << s;
			*b = *b | carry;
			carry = *b >> carryBits;
			b ++; 
			i --;
		}
	}
}

unsigned char ByteArray_byteAt_(ByteArray *self, size_t i)
{
	if (i < self->size) 
	{
		return self->bytes[i];
	}
	
	return 0;
}

int ByteArray_bitAt_(ByteArray *self, size_t i)
{
	size_t byteIndex = i / 8;
	int bitIndex = i % 8;
	unsigned char byte = ByteArray_byteAt_(self, byteIndex);
	
	return ((byte >> bitIndex) & 0x1);  
}

void ByteArray_setBit_at_(ByteArray *self, int state, size_t i)
{
	size_t charPos = i / 8;
	
	if (state) 
	{ 
		state = 0x1;
	} 
	else 
	{ 
		state = 0x0;
	}
	
	if (charPos < self->size)
	{
		unsigned char bitPos = i % 8;
		unsigned char mask = 0x1 << bitPos;
		
		if (state)
		{
			self->bytes[charPos] |= mask;
		}
		else
		{
			self->bytes[charPos] &= (~mask);
		}
	}
	
	if (ByteArray_bitAt_(self, i) != state)
	{
		printf("error\n");
		ByteArray_bitAt_(self, i);
	}
}

// --------------------------------------------------- 

float ByteArray_aveAbsFloat32From_to_(ByteArray *self, size_t from, size_t to)
{
	double delta;
	double ave = 0;
	size_t max = ByteArray_size32(self);
	
	if (from > max) return 0;
	if (to > max) to = max;
	if (from >= to) return 0;
	delta = to - from;
	
	while (from != to)
	{
		double v = (double)ByteArray_float32At_(self, from);
		v = fabs(v);
		ave += v;
		from ++;
	}
	
	ave = ave ? ave / delta : 0;
	return (float)ave;
}

int ByteArray_aveAbsSignedInt32From_to_(ByteArray *self, size_t from, size_t to)
{
	double delta;
	double ave = 0;
	size_t max = ByteArray_size32(self);
	
	if (from > max) return 0;
	if (to > max)   to = max;
	if (from >= to) return 0;
	
	delta = to - from;
	
	while (from != to)
	{
		double v = (double)ByteArray_int32At_(self, from);
		v = fabs(v);
		ave += v;
		from ++;
	}
	
	ave = ave ? ave / delta : 0;
	return (int)ave;
}

void ByteArray_convertFloatArrayToInts(ByteArray *self)
{
	size_t size = self->size / sizeof(float);
	size_t n;
	int32_t *i = (int32_t *)self->bytes;
	float *f = (float *)self->bytes;
	
	for (n = 0; n < size; n ++) 
	{ 
		i[n] = (int32_t)(f[n] * INT_MAX); 
	}
}

void ByteArray_convertFloat32ArrayToInt16(ByteArray *self)
{
	size_t size = self->size / sizeof(float);
	size_t n;
	int16_t *i = (int16_t *)self->bytes;
	float *f = (float *)self->bytes;
	
	for (n = 0; n < size; n ++) 
	{ 
		i[n] = (int16_t)(f[n] * SHRT_MAX); 
	}
	
	ByteArray_setSize_(self, size * sizeof(int16_t));
}

void ByteArray_convertInt16ArrayToFloat32(ByteArray *self)
{
	size_t count = self->size / sizeof(int16_t);
	size_t n;
	int16_t *i = (int16_t *)self->bytes;
	float *f = (float *)malloc(count * sizeof(float));
	float d = (float)1.0 / (float)SHRT_MAX;
	
	for (n = 0; n < count; n ++) 
	{ 
		f[n] = i[n] * d;
	}
	
	ByteArray_setData_size_(self, (unsigned char *)f, count * sizeof(float));
	free(f);
}

void ByteArray_float32ArrayAdd_(ByteArray *self, ByteArray *other)
{
	size_t size = self->size / sizeof(float);
	size_t otherSize = other->size / sizeof(float);
	size_t count = size < otherSize ? size : otherSize;
	float *f1 = (float *)self->bytes;
	float *f2 = (float *)other->bytes;
	
	while (count --)
	{
		*f1 += *f2;
		f1 ++;
		f2 ++;
	}
}

void ByteArray_float32ArrayMultiplyScalar_(ByteArray *self, float s)
{
	size_t count = self->size / sizeof(float);
	float *f1 = (float *)self->bytes;
	
	while (count --)
	{
		*f1 *= s;
		f1 ++;
	}
}

void ByteArray_zero(ByteArray *self)
{
	memset(self->bytes, 0, self->size);
}


static inline int sameCompare(unsigned char *b1, unsigned char *b2, int size)
{
	while (size)
	{
		if (*b1 != *b2) 
		{
			return 0;
		}
		
		*b1 ++; 
		*b2 ++;
		size --;
	}
	
	return 1;
}

int ByteArray_splitCount_(ByteArray *self, List *delims)
{
	int count;
	List *r = ByteArray_split_(self, delims);
	count = List_size(r);
	List_free(r);
	return count;
}

List *ByteArray_split_(ByteArray *self, List *delims)
{
	Datum d = ByteArray_asDatum(self);
	List *datumDelims = List_map_(delims, (ListCollectCallback *)ByteArray_asNewDatum);
	List *resultDatums = (List *)Datum_split_(&d, datumDelims);
	List *results = List_map_(resultDatums, (ListCollectCallback *)Datum_asByteArray);
	
	// free result datums 
	List_do_(resultDatums, (ListDoCallback *)free);
	List_free(resultDatums);
	
	// free datum delims 
	List_do_(datumDelims, (ListDoCallback *)free);
	List_free(datumDelims);
	return results;
}

void ByteArray_printBits(ByteArray *self)
{
	int i, max = ByteArray_bitCount(self);
	
	printf("%i ", (int)(self->size));
	
	for (i = max - 1; i > - 1; i --)
	{
		printf("%i", ByteArray_bitAt_(self, i));
	}
}

unsigned int ByteArray_bitCount(ByteArray *self)
{
	return self->size * 8;
}

/*
 unsigned int ByteArray_HashData_size_(const unsigned char *bytes, unsigned int size)
 {
	 unsigned int h = 5381;
	 
	 while (size-- > 0)
	 {
		 h += (h << 5); // h(i) = (h(i-1) * 33) ^ key(i) 
		 h ^= *bytes ++;
	 }
	 return h;
 }
 
 unsigned int ByteArray_hash(ByteArray *self)
 {
	 return ByteArray_HashData_size_(self->bytes, self->size);
 }
 
 uint32_t ByteArray_orderedHash32(ByteArray *self)
 {
	 uint32_t hash;
	 memcpy(&hash, self->bytes, 4);
	 return hash;
 }
 */


void ByteArray_removeOddIndexesOfSize_(ByteArray *self, size_t typeSize)
{
	size_t di = 1;
	size_t si = 2;
	size_t max = self->size / typeSize;
	uint8_t *b = (uint8_t *)self->bytes;
	
	if (max == 0)
	{
		return;
	}
	
	while (si < max)
	{
		uint8_t *src  = b + (si * typeSize);
		uint8_t *dest = b + (di * typeSize);
		memcpy(dest, src, typeSize);
		//printf("copy %i <- %i \n", (int)di, (int)si);
		si = si + 2;
		di = di + 1;
	}
	
	ByteArray_setSize_(self, di * typeSize);
}

void ByteArray_removeEvenIndexesOfSize_(ByteArray *self, size_t typeSize)
{
	size_t di = 0;
	size_t si = 1;
	size_t max = self->size / (typeSize);
	uint8_t *b = (uint8_t *)self->bytes;
	
	while (si < max)
	{
		uint8_t *src  = b + (si * typeSize);
		uint8_t *dest = b + (di * typeSize);
		memcpy(dest, src, typeSize);
		//printf("copy %i <- %i \n", (int)di, (int)si);
		si = si + 2;
		di = di + 1;
	}
	
	ByteArray_setSize_(self, di * typeSize);
}

void ByteArray_duplicateIndexesOfSize_(ByteArray *self, size_t typeSize)
{
	size_t size = (self->size / typeSize);
	
	if (size)
	{
		size_t si = size - 1;
		size_t di = (size * 2) - 1;
		uint8_t *b;
		
		ByteArray_setSize_(self, self->size * 2);
		
		b = (uint8_t *)self->bytes;
		
		for (;;)
		{
			uint8_t *src;
			uint8_t *dest;
			
			src  = b + si * typeSize;
			dest = b + di * typeSize;
			
			//printf("copy %i -> %i, %i \n", (int)si, (int)(di - 1), (int)di);
			memcpy(dest, src, typeSize);
			memcpy(dest - typeSize, src, typeSize);
			
			if (si == 0) break;
			di = di - 2;
			si --;
		}
	}
}

int ByteArray_endsWithCString_(ByteArray *self, const char *suffix)
{
	return (strcmp((char *)(self->bytes + self->size - strlen(suffix)), suffix)== 0);
}

size_t ByteArray_matchingPrefixSizeWith_(ByteArray *self, ByteArray *other)
{
	Datum d1 = ByteArray_asDatum(self);
	Datum d2 = ByteArray_asDatum(other);
	return Datum_matchingPrefixSizeWith_(&d1, &d2);
}

// -------------------------

/*
char *string_caseInsensitiveBoyerMooreSearch(char *buffer, size_t length, char *needle, size_t needleLen)
{
        register char *p;

        switch (needleLen)
        {
                case 0:
                return NULL;

                case 1:
                for (p = buffer; *p; p++)
                {
                        if (toupper(*p) == toupper(*needle))
                                return p;
                }
                return NULL;
                break;

        }
	
		{
        register const size_t needleSafetyBouncer = needleLen - 1;
        register size_t i;
        register byte delta[256];
        register const char *end = buffer + length;
        int k;

        for (i = 0; i < 256; i++)
                delta[i] = needleLen;

        for (i = needleLen - 1, k = 0; i >=0 ; k++, i--)
        {
                if (delta[(byte)toupper(needle[i])] == needleLen)
                        delta[(byte)toupper(needle[i])] = k;
        }


        for (p = buffer; p < end; )
        {
                for (i = needleSafetyBouncer ; ;)
                {
                        if (toupper(p[i]) != toupper(needle[i]))
                        {
                                p+=delta[(byte)toupper(p[i])];
                                break;
                        }

                        if (!i--)
                                return p;
                }
        }
		}

        return NULL;
}

char *string_caseSensitiveBoyerMooreSearch(char *buffer, size_t length, char *needle, size_t needleLen)
{
	
	register char *p;
	
	switch (needleLen)
	{
		case 0:
			return NULL;
			
		case 1:
			for (p = buffer; *p; p++)
			{
				if (*p == *needle)
					return p;
			}
			return NULL;
			break;
			
	}
	
	
	{
        register const size_t needleSafetyBouncer = needleLen - 1;
        register size_t i;
        register uint8_t delta[256];
        register const char *end = buffer + length;
        int k;
		
        for (i = 0; i < 256; i++)
			delta[i] = needleLen;
		
        for (i = needleLen - 1, k = 0; i >=0 ; k++, i--)
        {
			if (delta[(uint8_t)needle[i]] == needleLen)
				delta[(uint8_t)needle[i]] = k;
        }
		
		
        for (p = buffer; p < end; )
        {
			for (i = needleSafetyBouncer ; ;)
			{
				if (p[i] != needle[i])
				{
					p+=delta[(uint8_t)p[i]];
					break;
				}
				
				if (!i--)
					return p;
			}
        }
	}
	
	return NULL;
}
*/

// cursor -------------------------------------------

/*
size_t ByteArray_bytesInCharacterAtByteOffset_(ByteArray *self, size_t offset)
{
	// utf8
	
	const char c = self->bytes[offset];

	if ((c & 0xFE) == 0xFC) return 6;
	if ((c & 0xFC) == 0xF8) return 5;
	if ((c & 0xF8) == 0xF0) return 4;
	if ((c & 0xF0) == 0xE0) return 3;
	if ((c & 0xE0) == 0xC0) return 2;
	if ((c & 0x80) == 0x00) return 1;

	return 0;
}

size_t utf8_prev(const char *s, size_t o)
{
        while (o != 0 && (s[-- o] & 0xC0) == 0x80) {}
        return o;
}

size_t utf8_next(const char *s, size_t l, size_t o)
{
        while (o != l && (s[++ o] & 0xC0) == 0x80) {}
        return o;
}

typedef struct 
{	
	ByteArray *array;
	size_t offset; // byte offset, not character offset 
} ByteArrayCursor;

ByteArrayCursor ByteArray_cursor(ByteArray *self)
{
	ByteArrayCursor lc;
	lc.array = self;
	lc.offset = 0;
	return lc;
}

void ByteArrayCursor_next(ByteArrayCursor *self)
{
	size_t offset = self->p - ByteArray_bytes(self->array);
	
	if (self->offset >= (size_t)ByteArray_sizeInBytes(self->list))
	{
		return;
	}
	
	self->offset += ByteArray_bytesInCharacterAtByteOffset_(self->array, self->offset);
}

IOINLINE size_t ByteArrayCursor_index(ByteArrayCursor *self)
{
	return self->index;
}

IOINLINE void *ByteArrayCursor_value(ByteArrayCursor *self)
{
	return List_at_(self->list, self->index);
}

*/
