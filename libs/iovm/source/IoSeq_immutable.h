
#include "IoSeq.h"

IOVM_API void IoSeq_rawPrint(IoSeq *self);
IOVM_API void IoSeq_addImmutableMethods(IoSeq *self);

IOVM_API IO_METHOD(IoSeq, with);

IOVM_API IO_METHOD(IoSeq, itemType);
IOVM_API IO_METHOD(IoSeq, itemSize);
IOVM_API IO_METHOD(IoSeq, encoding);
IOVM_API IO_METHOD(IoSeq, asUTF8);
IOVM_API IO_METHOD(IoSeq, asUCS2);
IOVM_API IO_METHOD(IoSeq, asUCS4);
IOVM_API IO_METHOD(IoSeq, asFixedSizeType);

// conversion

IOVM_API IO_METHOD(IoSeq, asBinaryNumber);
IOVM_API IO_METHOD(IoSeq, asSymbol);
IOVM_API IO_METHOD(IoSeq, isMutable);
IOVM_API IO_METHOD(IoSeq, isSymbol);
IOVM_API IO_METHOD(IoSeq, asNumber);
IOVM_API IO_METHOD(IoSeq, asList);

IOVM_API IO_METHOD(IoSeq, whiteSpaceStrings);

IOVM_API IO_METHOD(IoSeq, print);
IOVM_API IO_METHOD(IoSeq, linePrint);

IOVM_API IO_METHOD(IoSeq, parseJson);

// access

IOVM_API IO_METHOD(IoSeq, size);
IOVM_API IO_METHOD(IoSeq, sizeInBytes);
IOVM_API IO_METHOD(IoSeq, isEmpty);
IOVM_API IO_METHOD(IoSeq, isZero);
IOVM_API IO_METHOD(IoSeq, at);
IOVM_API IO_METHOD(IoSeq, exclusiveSlice);
IOVM_API IO_METHOD(IoSeq, inclusiveSlice);

IOVM_API IO_METHOD(IoSeq, between);
IOVM_API IO_METHOD(IoSeq, asNumber);

// find

IOVM_API IO_METHOD(IoSeq, findSeqs);
IOVM_API IO_METHOD(IoSeq, findSeq);
IOVM_API IO_METHOD(IoSeq, reverseFindSeq);

IOVM_API IO_METHOD(IoSeq, beginsWithSeq);
IOVM_API IO_METHOD(IoSeq, endsWithSeq);

IOVM_API IO_METHOD(IoSeq, split);

IOVM_API IO_METHOD(IoSeq, contains);
IOVM_API IO_METHOD(IoSeq, containsSeq);
IOVM_API IO_METHOD(IoSeq, containsAnyCaseSeq);

IOVM_API IO_METHOD(IoSeq, isLowercase);
IOVM_API IO_METHOD(IoSeq, isUppercase);
IOVM_API IO_METHOD(IoSeq, isEqualAnyCase);

// split

IOVM_API IO_METHOD(IoSeq, split);
IOVM_API IO_METHOD(IoSeq, splitAt);

// base

IOVM_API IO_METHOD(IoSeq, fromBase);
IOVM_API IO_METHOD(IoSeq, toBase);

IOVM_API IO_METHOD(IoSeq, foreach);
IOVM_API IO_METHOD(IoSeq, asMessage);

IOVM_API IO_METHOD(IoSeq, cloneAppendSeq);
IOVM_API IO_METHOD(IoSeq, asMutable);

// case

IOVM_API IO_METHOD(IoSeq, asUppercase);
IOVM_API IO_METHOD(IoSeq, asLowercase);

// path

IOVM_API IO_METHOD(IoSeq, lastPathComponent);
IOVM_API IO_METHOD(IoSeq, pathExtension);
IOVM_API IO_METHOD(IoSeq, fileName);

IOVM_API IO_METHOD(IoSeq, cloneAppendPath);
IOVM_API IO_METHOD(IoSeq, pathComponent);

IOVM_API IO_METHOD(IoSeq, asOSPath);
IOVM_API IO_METHOD(IoSeq, asIoPath);

// occurrence

IOVM_API IO_METHOD(IoSeq, beforeSeq);
IOVM_API IO_METHOD(IoSeq, afterSeq);

IOVM_API IO_METHOD(IoSeq, occurrencesOfSeq);

// encoding

IOVM_API IO_METHOD(IoSeq, asBase64);
