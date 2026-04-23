/*cmetadoc Sequence description
C implementation of the Sequence / String / Symbol proto. A Sequence
is an IoObject whose dataPointer is a UArray — a typed, resizable byte
array from the basekit runtime that tracks itemType, encoding, and
item size in addition to raw bytes. A single proto serves three roles:
mutable Sequence, immutable Symbol (interned; pointer-equality for
equal strings), and typed numeric vector (uint8, int32, float32, etc.)
used by the fast math methods. The split across IoSeq.c,
IoSeq_immutable.c, IoSeq_mutable.c, and IoSeq_vector.c is purely a
code-organization partition: this file owns lifecycle (clone/free/
compare), the constructor API, and cross-partition helpers;
IoSeq_immutable.c holds read-only methods also usable on Symbols;
IoSeq_mutable.c holds the in-place-mutating methods; IoSeq_vector.c
holds the SIMD/vector math. Symbols are detected via
IoObject_isSymbol; rawClone short-circuits on them (returning the
original) so every "clone" of a Symbol is the same interned object.
*/

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

/*cdoc Sequence ISMUTABLESEQ(self)
Type predicate — a mutable Sequence is any Sequence that is not a
Symbol. Used by mutating methods to reject Symbol receivers and
preserve interning semantics.
*/
int ISMUTABLESEQ(IoObject *self) {
    return ISSEQ(self) && !(IoObject_isSymbol(self));
}

/*cdoc Sequence ioSymbolFindFunc(s, ioSymbol)
strcmp-based comparator registered with IoState's symbol table so a C
string key can find an interned IoSymbol. Reaches into the symbol's
UArray bytes rather than copying — the table is hot.
*/
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

/*cdoc Sequence IoSeq_newTag(state)
Builds the Sequence tag with clone/free/compare hooks. The stream
read/write hooks shown in the commented-out block above are the old
BStream-based serialization path. No markFunc is needed — a Sequence
holds no GC-visible references, only its UArray payload.
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

/*cdoc Sequence IoSeq_proto(state)
Creates the Sequence proto with an empty UArray payload and registers
it on the state. The method table is installed later by IoSeq_protoFinish
— splitting the two steps lets IoState install the proto early (so
IOSEQ-valued proto references work during bootstrap) and wire up the
immutable/mutable method partitions afterward.
*/
IoSeq *IoSeq_proto(void *state) {
    IoObject *self = IoObject_new(state);

    IoObject_tag_(self, IoSeq_newTag(state));
    IoObject_setDataPointer_(self, UArray_new());

    IoState_registerProtoWithId_((IoState *)state, self, protoId);
    return self;
}

/*cdoc Sequence IoSeq_protoFinish(self)
Second-phase proto setup — installs the full method table by calling
into IoSeq_immutable.c and IoSeq_mutable.c. Called by IoState after
all primitive protos are registered so immutable/mutable methods can
reference other protos (Number, List) safely.
*/
IoSeq *IoSeq_protoFinish(IoSeq *self) {
    IoSeq_addImmutableMethods(self);
    IoSeq_addMutableMethods(self);
    return self;
}

/*cdoc Sequence IoSeq_rawClone(proto)
Registered as the tag's cloneFunc. Short-circuits on Symbols and
returns the proto itself — that is what makes Symbols interned: every
"clone" of a Symbol is the same object, and equality is pointer
identity. Non-Symbols get a fresh IoObject whose UArray is a deep
clone of the proto's bytes.
*/
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

/*cdoc Sequence IoSeq_new(state)
Convenience constructor: look up the proto and IOCLONE to get a fresh
empty Sequence. Most of the IoSeq_newWith* family funnel through this.
*/
IoSeq *IoSeq_new(void *state) {
    IoSeq *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

/*cdoc Sequence IoSeq_newWithData_length_(state, s, length)
Constructs a mutable Sequence holding a copy of the given byte buffer.
Forces itemType uint8 — typed-vector Sequences come through
UArray_setData_type_size_copy_ variants directly. Used by most
"string from bytes" call sites.
*/
IoSeq *IoSeq_newWithData_length_(void *state, const unsigned char *s,
                                 size_t length) {
    IoSeq *self = IoSeq_new(state);
    UArray_setData_type_size_copy_(DATA(self), (uint8_t *)s, CTYPE_uint8_t,
                                   length, 1);
    // UArray_convertToFixedSizeType(DATA(self));
    return self;
}

/*cdoc Sequence IoSeq_newWithData_length_copy_(state, s, length, copy)
Same as IoSeq_newWithData_length_ but lets the caller opt out of the
copy when they own the bytes and want the new Sequence to adopt them.
Zero-copy adoption is common in parser-to-AST paths.
*/
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

/*cdoc Sequence IoSeq_newWithUArray_copy_(state, ba, int copy)
Wraps an existing UArray as a Sequence, optionally adopting the
caller's array outright. The non-copy path frees the proto-cloned
empty UArray first and installs ba as the new dataPointer — the
Sequence takes full ownership of ba after this.
*/
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

/*cdoc Sequence IoSeq_asUTF8Seq(state, self)
Transcodes the receiver into a fresh UTF-8 Sequence via UArray_asUTF8.
Zero-copy adopts the new UArray since it is freshly minted.
*/
IoSeq *IoSeq_asUTF8Seq(void *state, IoSeq *self) {
    return IoSeq_newWithUArray_copy_(state, UArray_asUTF8(DATA(self)), 0);
}

/*cdoc Sequence IoSeq_newFromFilePath_(state, path)
Reads a file fully into a new Sequence using UArray_readFromFilePath_.
Builds a stack UArray for the path so no intermediate heap allocation
is needed for the filename.
*/
IoSeq *IoSeq_newFromFilePath_(void *state, const char *path) {
    IoSeq *self = IoSeq_new(state);
    UArray p = UArray_stackAllocedWithCString_((char *)path);
    UArray_readFromFilePath_(DATA(self), &p);
    return self;
}

/*cdoc Sequence IoSeq_rawMutableCopy(self)
Returns a mutable copy of a Sequence. Needed when the input might be
a Symbol: mutating it in place would corrupt the interned symbol
table, so callers that want to edit bytes first materialize a copy.
*/
IoSeq *IoSeq_rawMutableCopy(IoSeq *self) {
    return IoSeq_newWithUArray_copy_(IOSTATE, DATA(self), 1);
}

// these Symbol creation methods should only be called by IoState ------

/*cdoc Sequence IoSeq_newSymbolWithData_length_(state, s, length)
Internal Symbol builder — only IoState should call this. Allocates a
Sequence and copies the given bytes. IoState interns the result into
the global symbol table; subsequent lookups for the same string
return the same pointer, which is what makes Symbol equality a
pointer compare.
*/
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

/*cdoc Sequence IoSeq_newSymbolWithUArray_copy_(state, ba, copy)
Internal Symbol builder from a UArray — as with the data_length
variant, only IoState should call this. The copy flag controls
whether ba is cloned or adopted; adopt path transfers ownership
to the new Sequence and frees the proto-cloned placeholder UArray.
*/
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

/*cdoc Sequence IoSeq_newSymbolWithFormat_(state, format, ...)
printf-style Symbol constructor safe to call from anywhere. Renders
into a UArray via UArray_newWithVargs_ and hands it to the state's
interner with copy=0 so the freshly built bytes are adopted instead
of duplicated.
*/
IoSymbol *IoSeq_newSymbolWithFormat_(void *state, const char *format, ...) {
    UArray *ba;
    va_list ap;
    va_start(ap, format);
    ba = UArray_newWithVargs_(format, ap);
    va_end(ap);
    return IoState_symbolWithUArray_copy_(state, ba, 0);
}

// -----------------------------------------------------

/*cdoc Sequence IoSeq_free(self)
Registered as the tag's freeFunc. Symbols are removed from the state's
intern table first so stale entries do not leak — without this a future
lookup for the same string could hit a dangling pointer. Then the UArray
bytes are released.
*/
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

/*cdoc Sequence IoSeq_compare(self, v)
Registered as the tag's compareFunc. Identity fast path first, then
byte-wise UArray comparison honoring itemType. Falls back to
IoObject_defaultCompare for mixed types so generic sorting stays total.
*/
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

/*cdoc Sequence IoSeq_rawAsUntriquotedSymbol(self)
Strips three layers of surrounding quotes (Io's triple-quoted string
literal) and returns the interned Symbol form. Used by the parser
when it sees """...""" source syntax.
*/
IoSymbol *IoSeq_rawAsUntriquotedSymbol(IoSeq *self) {
    UArray *a = UArray_clone(DATA(self));
    UArray_unquote(a);
    UArray_unquote(a);
    UArray_unquote(a);
    return IoState_symbolWithUArray_copy_(IOSTATE, a, 0);
}

/*cdoc Sequence IoSeq_rawAsUnquotedSymbol(self)
Strips one layer of surrounding quotes and returns the interned Symbol
form. Used by the parser for normal "..." string literals. Clones
the UArray before mutating so the original Sequence (possibly a
Symbol) is untouched.
*/
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

/*cdoc Sequence IoSeq_rawAsUnescapedSymbol(self)
Resolves backslash escape sequences (\n, \t, \xNN, etc.) and interns
the result. Used by the parser after unquoting a string literal.
*/
IoSymbol *IoSeq_rawAsUnescapedSymbol(IoSeq *self) {
    UArray *a = UArray_clone(DATA(self));
    UArray_unescape(a);
    return IoState_symbolWithUArray_copy_(IOSTATE, a, 0);
}

/*cdoc Sequence IoSeq_rawAsDoubleFromHex(self)
Parses the receiver as a hexadecimal integer and returns it as a
double. Used by the parser for 0x... numeric literals. Uses 64-bit
intermediate to preserve precision up to 2^53.
*/
double IoSeq_rawAsDoubleFromHex(IoSeq *self) {
    char *s = IoSeq_asCString(self);
    unsigned long long i;

    sscanf(s, "%llx", &i);
    return (double)i;
}

/*cdoc Sequence IoSeq_rawAsDoubleFromOctal(self)
Parses the receiver as an octal integer and returns it as a double.
Used by the parser for 0NNN numeric literals.
*/
double IoSeq_rawAsDoubleFromOctal(IoSeq *self) {
    char *s = IoSeq_asCString(self);
    unsigned long long i;

    sscanf(s, "%llo", &i);
    return (double)i;
}

int IoSeq_rawEqualsCString_(IoSeq *self, const char *s) {
    return (strcmp((char *)UArray_bytes(DATA(self)), s) == 0);
}

/*cdoc Sequence IoSeq_rawIsNotAlphaNumeric(self)
Returns true if the receiver contains no alphanumeric character at
all. Scans until it either hits an alphanumeric byte (return false)
or reaches the terminator (return true). Used by the parser to decide
whether a symbol name needs quoting in error messages.
*/
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
