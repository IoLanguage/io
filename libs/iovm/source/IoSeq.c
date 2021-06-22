#define IOSEQ_C
#include "IoSeq.h"
#undef IOSEQ_C
#include "IoSeq_mutable.h"
#include "IoSeq_immutable.h"
#include "IoState.h"
#include "IoCFunction.h"
#include "IoObject.h"
#include "IoNumber.h"
#include "IoMessage.h"
#include "IoList.h"
#include "IoSeq.h"
#include <ctype.h>

#define DATA(self) ((UArray *)(IoObject_dataPointer(self)))
//#define HASHIVAR(self) ((self)->extraData)
static const char *protoId = "Sequence";

int ISMUTABLESEQ(IoObject *self) {
    return ISSEQ(self) && !(IoObject_isSymbol(self));
}

int ioSymbolFindFunc(void *s, void *ioSymbol) {
    return strcmp((char *)s, (char *)UArray_bytes(DATA((IoObject *)ioSymbol)));
}

int IoObject_isStringOrBuffer(IoSeq *self) { return ISSEQ(self); }

int IoObject_isNotStringOrBuffer(IoSeq *self) { return !(ISSEQ(self)); }

void IoSeq_rawPrint(IoSeq *self) { IoState_justPrintba_(IOSTATE, DATA(self)); }

/*
void IoSymbol_writeToStream_(IoSymbol *self, BStream *stream)
{
        BStream_writeTaggedUArray_(stream, DATA(self));
}

IoSymbol *IoSymbol_allocFromStore_stream_(IoSymbol *self, BStream *stream)
{
        UArray *ba = BStream_readTaggedUArray(stream);

        if (!ba)
        {
                printf("String read error: missing byte array");
                IoState_exit(IOSTATE, -1);
        }

        return IoState_symbolWithUArray_copy_(IOSTATE, ba, 1);
}

void IoSeq_writeToStream_(IoSeq *self, BStream *stream)
{
        BStream_writeTaggedUArray_(stream, DATA(self));
}

void IoSeq_readFromStream_(IoSeq *self, BStream *stream)
{
        BStream_readTaggedUArray_(stream, DATA(self));
}
*/

IoTag *IoSeq_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSeq_rawClone);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSeq_free);
    IoTag_compareFunc_(tag, (IoTagCompareFunc *)IoSeq_compare);
    // IoTag_writeToStreamFunc_(tag, (IoTagWriteToStreamFunc
    // *)IoSeq_writeToStream_); IoTag_readFromStreamFunc_(tag,
    // (IoTagReadFromStreamFunc *)IoSeq_readFromStream_);
    return tag;
}

IoSeq *IoSeq_proto(void *state) {
    IoObject *self = IoObject_new(state);

    IoObject_tag_(self, IoSeq_newTag(state));
    IoObject_setDataPointer_(self, UArray_new());

    IoState_registerProtoWithId_((IoState *)state, self, protoId);
    return self;
}

IoSeq *IoSeq_protoFinish(IoSeq *self) {
    IoSeq_addImmutableMethods(self);
    IoSeq_addMutableMethods(self);
    return self;
}

IoSeq *IoSeq_rawClone(IoSeq *proto) {
    if (ISSYMBOL(proto)) {
        return proto;
    } else {
        IoSeq *self = IoObject_rawClonePrimitive(proto);
        IoObject_setDataPointer_(self, UArray_clone(DATA(proto)));
        return self;
    }
}

// -----------------------------------------------------------

IoSeq *IoSeq_new(void *state) {
    IoSeq *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

IoSeq *IoSeq_newWithData_length_(void *state, const unsigned char *s,
                                 size_t length) {
    IoSeq *self = IoSeq_new(state);
    UArray_setData_type_size_copy_(DATA(self), (uint8_t *)s, CTYPE_uint8_t,
                                   length, 1);
    // UArray_convertToFixedSizeType(DATA(self));
    return self;
}

IoSeq *IoSeq_newWithData_length_copy_(void *state, const unsigned char *s,
                                      size_t length, int copy) {
    IoSeq *self = IoSeq_new(state);
    UArray_setData_type_size_copy_(DATA(self), (uint8_t *)s, CTYPE_uint8_t,
                                   length, copy);
    // UArray_convertToFixedSizeType(DATA(self));
    return self;
}

IoSeq *IoSeq_newWithCString_(void *state, const char *s) {
    return IoSeq_newWithData_length_(state, (unsigned char *)s, strlen(s));
}

IoSeq *IoSeq_newWithCString_length_(void *state, const char *s, size_t length) {
    return IoSeq_newWithData_length_(state, (unsigned char *)s, length);
}

IoSeq *IoSeq_newWithUArray_copy_(void *state, UArray *ba, int copy) {
    IoSeq *self = IoSeq_new(state);

    if (copy) {
        UArray_copy_(DATA(self), ba);
    } else {
        UArray_free(DATA(self));
        IoObject_setDataPointer_(self, ba);
    }

    return self;
}

IoSeq *IoSeq_asUTF8Seq(void *state, IoSeq *self) {
    return IoSeq_newWithUArray_copy_(state, UArray_asUTF8(DATA(self)), 0);
}

IoSeq *IoSeq_newFromFilePath_(void *state, const char *path) {
    IoSeq *self = IoSeq_new(state);
    UArray p = UArray_stackAllocedWithCString_((char *)path);
    UArray_readFromFilePath_(DATA(self), &p);
    return self;
}

IoSeq *IoSeq_rawMutableCopy(IoSeq *self) {
    return IoSeq_newWithUArray_copy_(IOSTATE, DATA(self), 1);
}

// these Symbol creation methods should only be called by IoState ------

IoSymbol *IoSeq_newSymbolWithData_length_(void *state, const char *s,
                                          size_t length) {
    IoObject *self = IoSeq_new(state);
    UArray_setData_type_size_copy_(DATA(self), (unsigned char *)s,
                                   CTYPE_uint8_t, length, 1);
    return self;
}

IoSymbol *IoSeq_newSymbolWithCString_(void *state, const char *s) {
    return IoSeq_newSymbolWithData_length_(state, s, strlen(s));
}

IoSymbol *IoSeq_newSymbolWithUArray_copy_(void *state, UArray *ba, int copy) {
    IoObject *self = IoSeq_new(state);

    if (copy) {
        UArray_copy_(DATA(self), ba);
    } else {
        UArray_free(DATA(self));
        IoObject_setDataPointer_(self, ba);
    }

    return self;
}

// these Symbol creation methods can be called by anyone

IoSymbol *IoSeq_newSymbolWithFormat_(void *state, const char *format, ...) {
    UArray *ba;
    va_list ap;
    va_start(ap, format);
    ba = UArray_newWithVargs_(format, ap);
    va_end(ap);
    return IoState_symbolWithUArray_copy_(state, ba, 0);
}

// -----------------------------------------------------

void IoSeq_free(IoSeq *self) {
    if (IoObject_isSymbol(self)) {
        // if(strcmp(CSTRING(self), "_x_") == 0) { printf("Symbol free '%s'\n",
        // CSTRING(self)); } if(strlen(CSTRING(self)) < 100 && strncmp("0.",
        // CSTRING(self), 2) != 0 ) { printf("Symbol free '%s'\n",
        // CSTRING(self)); }
        IoState_removeSymbol_(IOSTATE, self);
    }

    if (DATA(self) != NULL) {
        UArray_free(DATA(self));
    }
}

int IoSeq_compare(IoSeq *self, IoSeq *v) {
    if (ISSEQ(v)) {
        if (self == v)
            return 0;
        return UArray_compare_(DATA(self), DATA(v));
    }

    return IoObject_defaultCompare(self, v);
}

/*
UArray *IoSeq_rawUArray(IoSeq *self)
{
        return DATA(self);
}
*/

char *IoSeq_asCString(IoSeq *self) { return (char *)UArray_bytes(DATA(self)); }

unsigned char *IoSeq_rawBytes(IoSeq *self) {
    return (unsigned char *)UArray_bytes(DATA(self));
}

size_t IoSeq_rawSize(IoSeq *self) { return (size_t)(UArray_size(DATA(self))); }

size_t IoSeq_rawSizeInBytes(IoSeq *self) {
    return (size_t)(UArray_sizeInBytes(DATA(self)));
}

double IoSeq_asDouble(IoSeq *self) {
    return strtod((char *)UArray_bytes(DATA(self)), NULL);
}

// -----------------------------------------------------------

void IoSeq_rawSetSize_(IoSeq *self, size_t size) {
    UArray_setSize_(DATA(self), size);
}

size_t IoSeq_memorySize(IoSeq *self) {
    // return sizeof(IoSeq) + UArray_memorySize(DATA(self));
    return 0;
}

void IoSeq_compact(IoSeq *self) {
    // UArray_compact(DATA(self));
}

// -----------------------------------------------------------

IoSymbol *IoSeq_rawAsUntriquotedSymbol(IoSeq *self) {
    UArray *a = UArray_clone(DATA(self));
    UArray_unquote(a);
    UArray_unquote(a);
    UArray_unquote(a);
    return IoState_symbolWithUArray_copy_(IOSTATE, a, 0);
}

IoSymbol *IoSeq_rawAsUnquotedSymbol(IoSeq *self) {
    UArray *a = UArray_clone(DATA(self));
    /*
    UArray *sa = DATA(self);
    UArray *a = UArray_new();
    UArray_setItemType_(a, UArray_itemType(sa));
    UArray_setEncoding_(a, UArray_encoding(sa));
    UArray_setSize_(a, UArray_size(sa));
    UArray_copy_(a, sa);
    */
    UArray_unquote(a);
    return IoState_symbolWithUArray_copy_(IOSTATE, a, 0);
}

IoSymbol *IoSeq_rawAsUnescapedSymbol(IoSeq *self) {
    UArray *a = UArray_clone(DATA(self));
    UArray_unescape(a);
    return IoState_symbolWithUArray_copy_(IOSTATE, a, 0);
}

double IoSeq_rawAsDoubleFromHex(IoSeq *self) {
    char *s = IoSeq_asCString(self);
    unsigned int i;

    sscanf(s, "%x", &i);
    return (double)i;
}

double IoSeq_rawAsDoubleFromOctal(IoSeq *self) {
    char *s = IoSeq_asCString(self);
    unsigned int i;

    sscanf(s, "%o", &i);
    return (double)i;
}

int IoSeq_rawEqualsCString_(IoSeq *self, const char *s) {
    return (strcmp((char *)UArray_bytes(DATA(self)), s) == 0);
}

int IoSeq_rawIsNotAlphaNumeric(IoSeq *self) {
    char *s = (char *)UArray_bytes(DATA(self));

    while (!isalnum((int)*s) && *s != 0) {
        s++;
    }

    return (*s == 0);
}

/*
int IoSeq_rawIsNotAlphaNumeric(IoSeq *self)
{
        char *s = (char *)UArray_bytes(DATA(self));

        while (!isalnum((int)*s) && *s != 0)
        {
                s ++;
        }

        return (*s == 0);
}

unsigned int IoSeq_rawHashCode(IoSeq *self)
{
        uintptr_t h = (uintptr_t)HASHIVAR(self);
        return (unsigned int)h;
}

void IoSeq_rawSetHashCode_(IoSeq *self, unsigned int h)
{
        HASHIVAR(self) = (void *)(uintptr_t)h;
}
*/
