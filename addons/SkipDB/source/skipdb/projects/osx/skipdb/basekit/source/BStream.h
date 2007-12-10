/*
docCopyright("Steve Dekorte", 2004)
docLicense("BSD revised")
docDescription("A Binary Stream, supports tagged items.")
*/

#ifndef BSTREAM_DEFINED
#define BSTREAM_DEFINED 1

#include "Common.h"
#include "ByteArray.h"
#include "BStreamTag.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    ByteArray *ba;
    size_t index;
    unsigned char ownsByteArray;
    ByteArray *tmp;
    ByteArray *errorBa;
    int flipEndian;
    unsigned char *typeBuf;
} BStream;

BASEKIT_API BStream *BStream_new(void);
BASEKIT_API BStream *BStream_clone(BStream *self);
BASEKIT_API void BStream_free(BStream *self);

BASEKIT_API char *BStream_errorString(BStream *self);
BASEKIT_API void BStream_setByteArray_(BStream *self, ByteArray *ba);
BASEKIT_API void BStream_setData_length_(BStream *self, unsigned char *data, size_t length);
BASEKIT_API ByteArray *BStream_byteArray(BStream *self);
BASEKIT_API Datum BStream_datum(BStream *self);
BASEKIT_API void BStream_empty(BStream *self);
BASEKIT_API int BStream_isEmpty(BStream *self);

// writing -------------------------------------- 

BASEKIT_API void BStream_writeByte_(BStream *self, unsigned char v);

BASEKIT_API void BStream_writeUint8_(BStream *self, uint8_t v);
BASEKIT_API void BStream_writeUint32_(BStream *self, uint32_t v);
BASEKIT_API void BStream_writeInt32_(BStream *self, int32_t v);
#if !defined(__SYMBIAN32__)
BASEKIT_API void BStream_writeInt64_(BStream *self, int64_t v);
#endif
BASEKIT_API void BStream_writeDouble_(BStream *self, double v);
BASEKIT_API void BStream_writeData_length_(BStream *self, const unsigned char *data, size_t length);
BASEKIT_API void BStream_writeCString_(BStream *self, const char *s);
BASEKIT_API void BStream_writeByteArray_(BStream *self, ByteArray *ba);

// reading -------------------------------------- 

BASEKIT_API unsigned char BStream_readByte(BStream *self);

BASEKIT_API uint8_t BStream_readUint8(BStream *self);
BASEKIT_API uint32_t BStream_readUint32(BStream *self);
BASEKIT_API int32_t BStream_readInt32(BStream *self);
#if !defined(__SYMBIAN32__)
BASEKIT_API int64_t BStream_readInt64(BStream *self);
#endif
BASEKIT_API double BStream_readDouble(BStream *self);
BASEKIT_API uint8_t *BStream_readDataOfLength_(BStream *self, size_t length);
BASEKIT_API void BStream_readByteArray_(BStream *self, ByteArray *b);
BASEKIT_API ByteArray *BStream_readByteArray(BStream *self);
BASEKIT_API const char *BStream_readCString(BStream *self);

// tagged writing -------------------------------------- 

BASEKIT_API void BStream_writeTaggedUint8_(BStream *self, uint8_t v);
BASEKIT_API void BStream_writeTaggedUint32_(BStream *self, uint32_t v);
BASEKIT_API void BStream_writeTaggedInt32_(BStream *self, int32_t v);
#if !defined(__SYMBIAN32__)
BASEKIT_API void BStream_writeTaggedInt64_(BStream *self, int64_t v);
#endif
BASEKIT_API void BStream_writeTaggedDouble_(BStream *self, double v);
BASEKIT_API void BStream_writeTaggedData_length_(BStream *self, const unsigned char *data, size_t length);
BASEKIT_API void BStream_writeTaggedCString_(BStream *self, const char *s);
BASEKIT_API void BStream_writeTaggedByteArray_(BStream *self, ByteArray *ba);

// tagged reading --------------------------------------

BASEKIT_API uint8_t BStream_readTaggedUint8(BStream *self);
BASEKIT_API uint32_t BStream_readTaggedUint32(BStream *self);
BASEKIT_API int32_t BStream_readTaggedInt32(BStream *self);
#if !defined(__SYMBIAN32__)
BASEKIT_API int64_t BStream_readTaggedInt64(BStream *self);
#endif
BASEKIT_API double BStream_readTaggedDouble(BStream *self);
BASEKIT_API void BStream_readTaggedByteArray_(BStream *self, ByteArray *b);
BASEKIT_API ByteArray *BStream_readTaggedByteArray(BStream *self);
BASEKIT_API const char *BStream_readTaggedCString(BStream *self);

BASEKIT_API void BStream_show(BStream *self);

#ifdef __cplusplus
}
#endif
#endif

