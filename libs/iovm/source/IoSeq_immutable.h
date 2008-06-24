
#include "IoSeq.h"

IOVM_API void IoSeq_rawPrint(IoSeq *self);
IOVM_API void IoSeq_addImmutableMethods(IoSeq *self);

IOVM_API IoObject *IoSeq_with(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoSeq_itemType(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_itemSize(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_encoding(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_asUTF8(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_asUCS2(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_asUCS4(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_asFixedSizeType(IoSeq *self, IoObject *locals, IoMessage *m);

// conversion

IOVM_API IoObject *IoSeq_asBinaryNumber(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_asSymbol(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_isMutable(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_isSymbol(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_asNumber(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoSeq_whiteSpaceStrings(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoSeq_print(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_linePrint(IoSeq *self, IoObject *locals, IoMessage *m);

// access

IOVM_API IoObject *IoSeq_size(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_sizeInBytes(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_isEmpty(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_isZero(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_at(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_slice(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoSeq_between(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_asNumber(IoSeq *self, IoObject *locals, IoMessage *m);

// find

IOVM_API IoObject *IoSeq_findSeqs(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_findSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_reverseFindSeq(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoSeq_beginsWithSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_endsWithSeq(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoSeq_split(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoSeq_contains(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_containsSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_containsAnyCaseSeq(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoSeq_isLowercase(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_isUppercase(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_isEqualAnyCase(IoSeq *self, IoObject *locals, IoMessage *m);

// split

IOVM_API IoObject *IoSeq_split(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_splitAt(IoSeq *self, IoObject *locals, IoMessage *m);

// base

IOVM_API IoObject *IoSeq_fromBase(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_toBase(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoSeq_foreach(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_asMessage(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoSeq_cloneAppendSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_asMutable(IoSeq *self, IoObject *locals, IoMessage *m);

// case

IOVM_API IoObject *IoSeq_asUppercase(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_asLowercase(IoSeq *self, IoObject *locals, IoMessage *m);

// path

IOVM_API IoObject *IoSeq_lastPathComponent(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_pathExtension(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_fileName(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoSeq_cloneAppendPath(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_pathComponent(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoSeq_asOSPath(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_asIoPath(IoSeq *self, IoObject *locals, IoMessage *m);

// occurance

IOVM_API IoObject *IoSeq_beforeSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_afterSeq(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoSeq_occurancesOfSeq(IoSeq *self, IoObject *locals, IoMessage *m);

// encoding

IOVM_API IoObject *IoSeq_asBase64(IoSeq *self, IoObject *locals, IoMessage *m);
