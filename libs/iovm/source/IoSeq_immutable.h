
#include "IoSeq.h"

void IoSeq_rawPrint(IoSeq *self);
void IoSeq_addImmutableMethods(IoSeq *self);

IoObject *IoSeq_with(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_itemType(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_itemSize(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_encoding(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_asUTF8(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_asUTF16(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_asUTF32(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_asFixedSizeType(IoSeq *self, IoObject *locals, IoMessage *m);

// conversion

IoObject *IoSeq_asBinaryNumber(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_asSymbol(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_isMutable(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_isSymbol(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_asNumber(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_whiteSpaceStrings(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_print(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_linePrint(IoSeq *self, IoObject *locals, IoMessage *m);

// access

IoObject *IoSeq_size(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_isEmpty(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_isZero(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_at(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_slice(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_between(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_asNumber(IoSeq *self, IoObject *locals, IoMessage *m);

// find

IoObject *IoSeq_findSeqs(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_findSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_reverseFindSeq(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_beginsWithSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_endsWithSeq(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_split(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_contains(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_containsSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_containsAnyCaseSeq(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_isLowercase(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_isUppercase(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_isEqualAnyCase(IoSeq *self, IoObject *locals, IoMessage *m);

// split

IoObject *IoSeq_split(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_splitAt(IoSeq *self, IoObject *locals, IoMessage *m);

// base

IoObject *IoSeq_fromBase(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_toBase(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_foreach(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_asMessage(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_cloneAppendSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_asMutable(IoSeq *self, IoObject *locals, IoMessage *m);

// case

IoObject *IoSeq_asUppercase(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_asLowercase(IoSeq *self, IoObject *locals, IoMessage *m);

// path

IoObject *IoSeq_lastPathComponent(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_pathExtension(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_fileName(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_cloneAppendPath(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_pathComponent(IoSeq *self, IoObject *locals, IoMessage *m);


IoObject *IoSeq_beforeSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_afterSeq(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_asCapitalized(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_occurancesOfSeq(IoSeq *self, IoObject *locals, IoMessage *m);
