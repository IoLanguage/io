
// metadoc Sequence copyright Steve Dekorte 2002
// metadoc Sequence license BSD revised

#ifndef IOSEQ_DEFINED
#define IOSEQ_DEFINED 1

#include "IoVMApi.h"

#include "Common.h"
#include "UArray.h"
#include "IoObject_struct.h"
#include "IoMessage.h"

#ifdef __cplusplus
extern "C" {
#endif

IOVM_API int ISMUTABLESEQ(IoObject *self);
#define ISSEQ(self) IOOBJECT_ISTYPE(self, Seq)
#define ISSYMBOL(self) IoObject_isSymbol(self)
#define ISBUFFER(self) ISMUTABLESEQ(self)

#define IOSEQ(data, size)                                                      \
    IoSeq_newWithData_length_((IoState *)IOSTATE, data, size)
#define IOSEQ_LENGTH(self) UArray_size((UArray *)(IoObject_dataPointer(self)))
#define IOSEQ_BYTES(self) UArray_bytes((UArray *)(IoObject_dataPointer(self)))

#define WHITESPACE " \t\n\r"

// Symbol defines

#define IOSYMBOL(s) IoState_symbolWithCString_((IoState *)IOSTATE, (char *)(s))
#define IOSYMBOLID(s) (IoObject_dataPointer(self))
#define CSTRING(uString) IoSeq_asCString(uString)

#if !defined(IoSymbol_DEFINED)
#define IoSymbol_DEFINED
typedef IoObject IoSymbol;
typedef IoObject IoSeq;
#endif

#define IOSYMBOL_LENGTH(self)                                                  \
    UArray_size(((UArray *)(IoObject_dataPointer(self))))
#define IOSYMBOL_BYTES(self)                                                   \
    UArray_bytes(((UArray *)(IoObject_dataPointer(self))))

typedef IoObject *(IoSplitFunction)(void *, UArray *, int);

typedef IoObject IoSeq;

IOVM_API int ioSeqCompareFunc(void *s1, void *s2);
IOVM_API int ioSymbolFindFunc(void *s, void *ioSymbol);

IOVM_API int IoObject_isStringOrBuffer(IoObject *self);
IOVM_API int IoObject_isNotStringOrBuffer(IoObject *self);

IOVM_API IoSeq *IoSeq_proto(void *state);
IOVM_API IoSeq *IoSeq_protoFinish(IoSeq *self);

IOVM_API IoSeq *IoSeq_rawClone(IoSeq *self);
IOVM_API IoSeq *IoSeq_new(void *state);
IOVM_API IoSeq *IoSeq_newWithUArray_copy_(void *state, UArray *ba, int copy);
IOVM_API IoSeq *IoSeq_newWithData_length_(void *state, const unsigned char *s,
                                          size_t length);
IOVM_API IoSeq *IoSeq_newWithData_length_copy_(void *state,
                                               const unsigned char *s,
                                               size_t length, int copy);
IOVM_API IoSeq *IoSeq_newWithCString_length_(void *state, const char *s,
                                             size_t length);
IOVM_API IoSeq *IoSeq_newWithCString_(void *state, const char *s);
IOVM_API IoSeq *IoSeq_newFromFilePath_(void *state, const char *path);
IOVM_API IoSeq *IoSeq_rawMutableCopy(IoSeq *self);

// these Symbol creation methods should only be called by IoState

IOVM_API IoSymbol *IoSeq_newSymbolWithCString_(void *state, const char *s);
IOVM_API IoSymbol *IoSeq_newSymbolWithData_length_(void *state, const char *s,
                                                   size_t length);
IOVM_API IoSymbol *IoSeq_newSymbolWithUArray_copy_(void *state, UArray *ba,
                                                   int copy);

// these Symbol creation methods can be called by anyone

IOVM_API IoSymbol *IoSeq_newSymbolWithFormat_(void *state, const char *format,
                                              ...);

//

IOVM_API void IoSeq_free(IoSeq *self);
IOVM_API int IoSeq_compare(IoSeq *self, IoSeq *v);

IOVM_API char *IoSeq_asCString(IoSeq *self);
IOVM_API unsigned char *IoSeq_rawBytes(IoSeq *self);
// IOVM_API UArray *IoSeq_rawUArray(IoSeq *self);

#define IoSeq_rawUArray(self) ((UArray *)(IoObject_dataPointer(self)))

IOVM_API IoSymbol *IoSeq_rawAsUntriquotedSymbol(IoSeq *self);

IOVM_API size_t IoSeq_rawSize(IoSeq *self);
IOVM_API size_t IoSeq_rawSizeInBytes(IoSeq *self);
IOVM_API void IoSeq_rawSetSize_(IoSeq *self, size_t size);

// conversion

IOVM_API double IoSeq_asDouble(IoSeq *self);
IOVM_API IoSymbol *IoSeq_rawAsSymbol(IoSeq *self);

IOVM_API IoSymbol *IoSeq_rawAsUnquotedSymbol(IoSeq *self);
IOVM_API IoSymbol *IoSeq_rawAsUnescapedSymbol(IoSeq *self);

IOVM_API int IoSeq_rawEqualsCString_(IoSeq *self, const char *s);
IOVM_API double IoSeq_rawAsDoubleFromHex(IoSeq *self);
IOVM_API double IoSeq_rawAsDoubleFromOctal(IoSeq *self);

IOVM_API IoSeq *IoSeq_asUTF8Seq(void *state, IoSeq *self);

#define UTF8CSTRING(s) IoSeq_asCString(IoSeq_asUTF8Seq((IoState *)IOSTATE, s))
//#define UTF8CSTRING(s) CSTRING(IoSeq_newSymbolWithCString_(IOSTATE,
// CSTRING(s)))

// hashing
/*
int IoSeq_rawEqualTo(IoSeq *self, IoObject *other);
uintptr_t IoSeq_rawHash(IoSeq *self);
*/

#include "IoSeq_immutable.h"
#include "IoSeq_mutable.h"
#include "IoSeq_vector.h"
#include "IoSeq_inline.h"

#ifdef __cplusplus
}
#endif
#endif
