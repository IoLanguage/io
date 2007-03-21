/*
 docCopyright("Steve Dekorte", 2004)
 docLicense("BSD revised") 
 */

#include "Base.h"

//#define BStream_C
#include "BStream.h"
//#undef BStream_C

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#define BSTREAM_IS_BIG_ENDIAN 1

BStream *BStream_new(void)
{
	BStream *self = (BStream *)calloc(1, sizeof(BStream));
	self->ba = ByteArray_new();
	self->ownsByteArray = 1;
	self->tmp = ByteArray_new();
	self->errorBa = ByteArray_new();
	self->index = 0;
	self->typeBuf = (unsigned char *)malloc(512);
	return self;
}

BStream *BStream_clone(BStream *self)
{
	BStream *child = BStream_new();
	ByteArray_copy_(child->ba, self->ba);
	child->index = self->index;
	return child;
}

void BStream_free(BStream *self)
{
	if (self->ownsByteArray) ByteArray_free(self->ba);
	ByteArray_free(self->tmp);
	ByteArray_free(self->errorBa);
	free(self->typeBuf);
	free(self);
}

void BStream_error_(BStream *self, const char *s)
{
	ByteArray_setCString_(self->errorBa, s);
}

char *BStream_error(BStream *self)
{
	return (char *)ByteArray_bytes(self->errorBa);
}

void BStream_setByteArray_(BStream *self, ByteArray *ba)
{
	if (self->ownsByteArray)
	{
		ByteArray_free(ba);
		self->ownsByteArray = 0;
	}
	self->ba = ba;
	self->index = 0;
}

Datum BStream_datum(BStream *self)
{
	return ByteArray_asDatum(self->ba);
}

void BStream_setData_length_(BStream *self, unsigned char *data, size_t length)
{
	ByteArray_setData_size_(self->ba, data, length);
	self->index = 0;
}

ByteArray *BStream_byteArray(BStream *self)
{
	return self->ba;
}

void BStream_empty(BStream *self)
{
	self->index = 0;
	ByteArray_setSize_(self->ba, 0);
}

int BStream_isEmpty(BStream *self)
{
	return ByteArray_size(self->ba) == 0;
}

// writing -------------------------------------- 

void BStream_writeByte_(BStream *self, unsigned char v)
{
	BStream_writeUint8_(self, v);
}

void BStream_writeUint8_(BStream *self, uint8_t v)
{
	ByteArray_appendByte_(self->ba, v);
	self->index ++;
}

static void reverseBytes(unsigned char *d, size_t length)
{
	size_t a = 0;
	size_t b = length - 1;
	
	while ( a < b)
	{
		unsigned char c = d[a];
		
		d[a] = d[b];
		d[b] = c;
		a ++;
		b --;
	}
}

void BStream_writeNumber_size_(BStream *self, unsigned char *v, size_t length)
{
	memcpy(self->typeBuf, v, length);
	
	if (self->flipEndian) 
	{
		reverseBytes(self->typeBuf, length);
	}
	
	ByteArray_appendBytes_size_(self->ba, (unsigned char *)self->typeBuf, length);
	self->index += length;
}

void BStream_writeData_length_(BStream *self, const unsigned char *data, size_t length)
{
	ByteArray_appendBytes_size_(self->ba, (unsigned char *)data, length);
	self->index += length;
}

void BStream_writeInt32_(BStream *self, int32_t v)
{
	BStream_writeNumber_size_(self, (unsigned char *)(&v), sizeof(int32_t));
}

void BStream_writeUint32_(BStream *self, uint32_t v)
{
	BStream_writeNumber_size_(self, (unsigned char *)(&v), sizeof(uint32_t));
}

#if !defined(__SYMBIAN32__)
void BStream_writeInt64_(BStream *self, int64_t v)
{
	BStream_writeNumber_size_(self, (unsigned char *)(&v), sizeof(int64_t));
}
#endif

void BStream_writeDouble_(BStream *self, double v)
{
	BStream_writeNumber_size_(self, (unsigned char *)(&v), sizeof(double));
}

void BStream_writeCString_(BStream *self, const char *s)
{
	int length = strlen(s);
	BStream_writeInt32_(self, length);
	BStream_writeData_length_(self, (unsigned char *)s, length);
}

void BStream_writeByteArray_(BStream *self, ByteArray *ba)
{
	BStream_writeInt32_(self, ByteArray_size(ba));
	ByteArray_append_(self->ba, ba);
	self->index += ByteArray_size(ba);
}

// reading -------------------------------------- 

unsigned char BStream_readByte(BStream *self)
{
	return BStream_readUint8(self);
}

uint8_t BStream_readUint8(BStream *self)
{
	if (self->index < ByteArray_size(self->ba))
	{
		unsigned char b = ByteArray_bytes(self->ba)[self->index];
		self->index ++;
		return b;
	}
	
	return 0;
}

void BStream_readNumber_size_(BStream *self, unsigned char *v, int size)
{
	if (self->index + size <= ByteArray_size(self->ba))
	{
		unsigned char *b = ByteArray_bytes(self->ba);
		memcpy(v, b + self->index, size);
		
		if (self->flipEndian) 
		{
			reverseBytes(v, size);
		}
		
		self->index += size;
		return;
	}
	
	while (size--)
	{
		*v = 0;
		v ++;
	}
}

uint32_t BStream_readUint32(BStream *self)
{
	uint32_t v;
	BStream_readNumber_size_(self, (unsigned char *)(&v), sizeof(uint32_t));
	return v;
}

int32_t BStream_readInt32(BStream *self)
{
	int32_t v;
	BStream_readNumber_size_(self, (unsigned char *)(&v), sizeof(int32_t));
	return v;
}

#if !defined(__SYMBIAN32__)
int64_t BStream_readInt64(BStream *self)
{
	int64_t v;
	BStream_readNumber_size_(self, (unsigned char *)(&v), sizeof(int64_t));
	return v;
}
#endif

double BStream_readDouble(BStream *self)
{
	double v;
	BStream_readNumber_size_(self, (unsigned char *)(&v), sizeof(double));
	return v;
}

unsigned char *BStream_readDataOfLength_(BStream *self, size_t length)
{
	if (self->index + length <= ByteArray_size(self->ba))
	{
		unsigned char *b = ByteArray_bytes(self->ba) + self->index;
		self->index += length;
		return b;
	}
	
	return NULL;
}

void BStream_readByteArray_(BStream *self, ByteArray *b)
{
	size_t size = BStream_readInt32(self);
	unsigned char *data = BStream_readDataOfLength_(self, size);
	ByteArray_setData_size_(b, data, size);
}

ByteArray *BStream_readByteArray(BStream *self)
{
	BStream_readByteArray_(self, self->tmp);
	return self->tmp;
}

const char *BStream_readCString(BStream *self)
{
	BStream_readByteArray_(self, self->tmp);
	return (const char *)ByteArray_bytes(self->tmp);
}

// tagged writing -------------------------------------- 

void BStream_writeTag(BStream *self, unsigned int t, unsigned int b, unsigned int a)
{
	BStreamTag tag;
	tag.isArray = a;
	tag.type = t;
	tag.byteCount = b;
	
	{
		unsigned char c = BStreamTag_asUnsignedChar(&tag);
		BStreamTag tag2 = BStreamTag_FromUnsignedChar(c);
		
		if (tag2.isArray != tag.isArray ||
		    tag2.type != tag.type ||
		    tag2.byteCount != tag.byteCount)
		{
			printf("tags don't match\n");
			exit(-1);
		}
		
		BStream_writeUint8_(self, c);
	}
}

void BStream_writeTaggedUint8_(BStream *self, uint8_t v)
{
	BStream_writeTag(self, BSTREAM_UNSIGNED_INT, 1, 0);
	BStream_writeUint8_(self, v);
}

void BStream_writeTaggedUint32_(BStream *self, uint32_t v)
{
	BStream_writeTag(self, BSTREAM_UNSIGNED_INT, 4, 0);
	BStream_writeUint32_(self, v);
}

void BStream_writeTaggedInt32_(BStream *self, int32_t v)
{
	/*
	 if (v =< MAX_INT && v -128)
	 {
		 BStream_writeTag(self, BSTREAM_SIGNED_INT, 1, 0);
		 BStream_writeInt8_(self, (int8_t)v);
	 }
	 else
	 */
{
	BStream_writeTag(self, BSTREAM_SIGNED_INT, 4, 0);
	BStream_writeInt32_(self, v);
}
}
#if !defined(__SYMBIAN32__)
void BStream_writeTaggedInt64_(BStream *self, int64_t v)
{
	BStream_writeTag(self, BSTREAM_SIGNED_INT, 8, 0);
	BStream_writeInt64_(self, v);
}
#endif

/*
#if sizeof(double) != 8
#error BStream expects doubles to be 64bit
#endif
 */

void BStream_writeTaggedDouble_(BStream *self, double v)
{
	BStream_writeTag(self, BSTREAM_FLOAT, 8, 0);
	BStream_writeDouble_(self, v);
}

void BStream_writeTaggedData_length_(BStream *self, const unsigned char *data, size_t length)
{
	BStream_writeTag(self, BSTREAM_UNSIGNED_INT, 1, 1);
	BStream_writeTaggedInt32_(self, length);
	ByteArray_appendBytes_size_(self->ba, (unsigned char *)data, length);
	self->index += length;
}

void BStream_writeTaggedCString_(BStream *self, const char *s)
{
	BStream_writeTaggedData_length_(self, (unsigned char *)s, strlen(s));
}

void BStream_writeTaggedByteArray_(BStream *self, ByteArray *ba)
{
	BStream_writeTaggedData_length_(self, ByteArray_bytes(ba), ByteArray_size(ba));
}

// reading -------------------------------------- 

int BStream_readTag(BStream *self, unsigned int t, unsigned int b, unsigned int a)
{
	unsigned char c = BStream_readUint8(self);
	BStreamTag readTag = BStreamTag_FromUnsignedChar(c);
	BStreamTag expectedTag = BStreamTag_TagArray_type_byteCount_(a, t, b);
	
	if (!BStreamTag_isEqual_(&readTag, &expectedTag))
	{
		printf("BStream error: read:\n ");
		BStreamTag_print(&readTag);
		printf(" but expected:\n ");
		BStreamTag_print(&expectedTag);
		printf("\n");
		BStream_show(self);
		printf("\n");
		return -1;
	}
	
	return 0;
}

/*
 unsigned char BStream_readTaggedByte(BStream *self)
 {
	 BStream_readTag(self, BSTREAM_UNSIGNED_INT, 1, 0);
	 return BStream_readByte(self);
 }
 
 int BStream_readTaggedInt(BStream *self)
 {
	 BStream_readTag(self, BSTREAM_SIGNED_INT, 4, 0);
	 return BStream_readInt32(self);
 }
 */

uint8_t BStream_readTaggedUint8(BStream *self)
{
	return BStream_readTaggedInt32(self);
}

uint32_t BStream_readTaggedUint32(BStream *self)
{
	unsigned char c = BStream_readByte(self);
	BStreamTag t = BStreamTag_FromUnsignedChar(c);
	
	if (t.type == BSTREAM_UNSIGNED_INT && t.byteCount == 1)
	{ return (uint32_t)BStream_readUint8(self); }
	
	if (t.type == BSTREAM_UNSIGNED_INT && t.byteCount == 4)
	{ return (uint32_t)BStream_readUint32(self); }
	
	BStream_error_(self, "unhandled int type/size combination");
	return 0;
}

int32_t BStream_readTaggedInt32(BStream *self)
{
	unsigned char c = BStream_readByte(self);
	BStreamTag t = BStreamTag_FromUnsignedChar(c);
	
	if (t.type == BSTREAM_UNSIGNED_INT && t.byteCount == 1)
	{ 
		return (int32_t)BStream_readUint8(self); 
	}
	
	if (t.type == BSTREAM_SIGNED_INT && t.byteCount == 4)
	{ 
		return (int32_t)BStream_readInt32(self); 
	}
	/*
	 if (t.type == BSTREAM_SIGNED_INT && t.byteCount == 8)
	 { 
		 return (int32_t)BStream_readInt64(self); 
	 }
	 */
	
	BStream_error_(self, "unhandled int type/size combination");
	
	return 0;
}

ptrdiff_t BStream_readTaggedPointer(BStream *self)
{
	unsigned char c = BStream_readByte(self);
	BStreamTag t = BStreamTag_FromUnsignedChar(c);
	
	if (t.type == BSTREAM_POINTER)
	{
		BStream_error_(self, "expected pointer");
		return 0;
	}
	
	if (t.byteCount == 1)
	{ 
		return (ptrdiff_t)BStream_readUint8(self); 
	}
	
	if (t.byteCount == 4)
	{ 
		return (ptrdiff_t)BStream_readInt32(self); 
	}
	
#if !defined(__SYMBIAN32__)
	if (t.byteCount == 8)
	{ 
		return (ptrdiff_t)BStream_readInt64(self); 
	}
#endif
	
	BStream_error_(self, "unhandled pointer size");
	return 0;
}

double BStream_readTaggedDouble(BStream *self)
{
	unsigned char c = BStream_readByte(self);
	BStreamTag t = BStreamTag_FromUnsignedChar(c);
	/*
	 if (t.type == BSTREAM_FLOAT && t.byteCount == 4)
	 { 
		 return BStream_readFloat(self); 
	 }
	 */
	if (t.type == BSTREAM_FLOAT && t.byteCount == 8)
	{ 
		return BStream_readDouble(self); 
	}
	
	BStream_error_(self, "unhandled float type/size combination");
	return 0;
}

void BStream_readTaggedByteArray_(BStream *self, ByteArray *b)
{
	BStream_readTag(self, BSTREAM_UNSIGNED_INT, 1, 1);
	{
		size_t size = BStream_readTaggedInt32(self);
		unsigned char *data = BStream_readDataOfLength_(self, size);
		ByteArray_setData_size_(b, data, size);
	}
}

ByteArray *BStream_readTaggedByteArray(BStream *self)
{
	BStream_readTaggedByteArray_(self, self->tmp);
	return self->tmp;
}

const char *BStream_readTaggedCString(BStream *self)
{
	BStream_readTag(self, BSTREAM_UNSIGNED_INT, 1, 1);
	
	{
		size_t size = BStream_readTaggedInt32(self);
		return (char *)BStream_readDataOfLength_(self, size);
	}
}

int BStream_atEnd(BStream *self)
{
	return self->index >= ByteArray_size(self->ba);
}

int BStream_showInt(BStream *self)
{
	unsigned char c = BStream_readUint8(self);
	BStreamTag t = BStreamTag_FromUnsignedChar(c);
	int v = 0;
	
	printf("%s%i ", BStreamTag_typeName(&t), t.byteCount * 8);
	
	if (t.byteCount < 5)
	{
		BStream_readNumber_size_(self, (unsigned char *)(&v), t.byteCount);
	}
	else
	{
		printf("ERROR: byteCount out of range\n");
		exit(-1);
	}
	
	printf("%i", v);
	return v;
}

void BStream_show(BStream *self)
{
	int pos = self->index;
	int v = 0;
	
	self->index = 0;
	
	while (!BStream_atEnd(self))
	{
		unsigned char c = BStream_readUint8(self);
		BStreamTag t = BStreamTag_FromUnsignedChar(c);
		
		/*printf("isArray:%i type:%s byteCount:%i value:", t.isArray, BStreamTag_typeName(t), t.byteCount);*/
		printf("  %s%i %s", BStreamTag_typeName(&t), t.byteCount * 8, t.isArray ? "array " : "");
		fflush(stdout);
		
		if (t.isArray)
		{
			printf("[");
			
			if (t.byteCount == 1)
			{
				int size = BStream_showInt(self);
				if (size == 0)
				{
					printf(" '']\n");
				}
				else
				{
					unsigned char *data = BStream_readDataOfLength_(self, size);
					printf(" '%s']\n", data);
				}
			}
			else
			{
				printf("ERROR: array element byteCount not 1\n");
				exit(-1);
			}
		}
		else
		{
			if (t.byteCount > 0 && t.byteCount < 5)
			{
				BStream_readNumber_size_(self, (unsigned char *)(&v), t.byteCount);
			}
			else
			{
				printf("ERROR: byteCount out of range\n");
				exit(1);
			}
			
			/*
			 if (t.byteCount == 1)
			 {
				 printf("%c\n", v);
			 }
			 else
			 {
				 */
			printf("%i\n", v);
			/*}*/
		}
	}
	self->index = pos;
}
