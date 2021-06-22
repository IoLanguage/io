
#include "IoSeq.h"

IOVM_API void IoSeq_addMutableMethods(IoSeq *self);
IOVM_API void IoSeq_rawPio_reallocateToSize_(IoSeq *self, size_t size);
IOVM_API int IoSeq_byteCompare(const void *a, const void *b);

IOVM_API IO_METHOD(IoSeq, setItemType);
IOVM_API IO_METHOD(IoSeq, convertToItemType);
IOVM_API IO_METHOD(IoSeq, convertToFixedSizeType);
IOVM_API IO_METHOD(IoSeq, setEncoding);

IOVM_API void IoSeq_rawCopy_(IoSeq *self, IoSeq *other);
IOVM_API IO_METHOD(IoSeq, copy);
IOVM_API IO_METHOD(IoSeq, appendSeq);
IOVM_API IO_METHOD(IoSeq, append);
IOVM_API IO_METHOD(IoSeq, atInsertSeq);
IOVM_API IO_METHOD(IoSeq, insertSeqEvery);

// removing

IOVM_API IO_METHOD(IoSeq, removeAt);
IOVM_API IO_METHOD(IoSeq, removeSlice);
IOVM_API IO_METHOD(IoSeq, removeLast);
IOVM_API IO_METHOD(IoSeq, leaveThenRemove);
IOVM_API IO_METHOD(IoSeq, setSize);
IOVM_API IO_METHOD(IoSeq, preallocateToSize);
IOVM_API IO_METHOD(IoSeq, replaceSeq);
IOVM_API IO_METHOD(IoSeq, removeSeq);
IOVM_API IO_METHOD(IoSeq, replaceFirstSeq);
IOVM_API IO_METHOD(IoSeq, atPut);
IOVM_API IO_METHOD(IoSeq, lowercase);
IOVM_API IO_METHOD(IoSeq, uppercase);
IOVM_API IO_METHOD(IoSeq, translate);

// clip

IOVM_API IO_METHOD(IoSeq, clipBeforeSeq);
IOVM_API IO_METHOD(IoSeq, clipAfterSeq);
IOVM_API IO_METHOD(IoSeq, clipBeforeEndOfSeq);
IOVM_API IO_METHOD(IoSeq, clipAfterStartOfSeq);
IOVM_API IO_METHOD(IoSeq, empty);

// sort

IOVM_API IO_METHOD(IoSeq, sort);

// reverseInPlace

IOVM_API IO_METHOD(IoSeq, reverseInPlace);

// removing indexwise

IOVM_API IO_METHOD(IoSeq, removeOddIndexes);
IOVM_API IO_METHOD(IoSeq, removeEvenIndexes);
IOVM_API IO_METHOD(IoSeq, duplicateIndexes);
IOVM_API IO_METHOD(IoSeq, replaceMap);

// strip

IOVM_API IO_METHOD(IoSeq, strip);
IOVM_API IO_METHOD(IoSeq, lstrip);
IOVM_API IO_METHOD(IoSeq, rstrip);

// escape

IOVM_API IO_METHOD(IoSeq, escape);
IOVM_API IO_METHOD(IoSeq, unescape);

IOVM_API IO_METHOD(IoSeq, removePrefix);
IOVM_API IO_METHOD(IoSeq, removeSuffix);
IOVM_API IO_METHOD(IoSeq, capitalize);
IOVM_API IO_METHOD(IoSeq, appendPathSeq);

IOVM_API IO_METHOD(IoSeq, asCapitalized);

IOVM_API IO_METHOD(IoSeq, interpolateInPlace);

// math ---------------------------------------------------------------------

IOVM_API IO_METHOD(IoSeq, addEquals);
IOVM_API IO_METHOD(IoSeq, subtractEquals);
IOVM_API IO_METHOD(IoSeq, multiplyEquals);
IOVM_API IO_METHOD(IoSeq, divideEquals);
IOVM_API IO_METHOD(IoSeq, powerEquals);

IOVM_API IO_METHOD(IoSeq, add);
IOVM_API IO_METHOD(IoSeq, subtract);
IOVM_API IO_METHOD(IoSeq, multiply);
IOVM_API IO_METHOD(IoSeq, divide);
IOVM_API IO_METHOD(IoSeq, power);

IOVM_API IO_METHOD(IoSeq, dotProduct);
IOVM_API IO_METHOD(IoSeq, sum);
IOVM_API IO_METHOD(IoSeq, product);
IOVM_API IO_METHOD(IoSeq, min);
IOVM_API IO_METHOD(IoSeq, max);
IOVM_API IO_METHOD(IoSeq, arithmeticMeanAsDouble);
IOVM_API IO_METHOD(IoSeq, arithmeticMeanSquareAsDouble);
IOVM_API IO_METHOD(IoSeq, square);
IOVM_API IO_METHOD(IoSeq, abs);
IOVM_API IO_METHOD(IoSeq, ceil);
IOVM_API IO_METHOD(IoSeq, floor);
IOVM_API IO_METHOD(IoSeq, log);
IOVM_API IO_METHOD(IoSeq, log10);
IOVM_API IO_METHOD(IoSeq, negate);
IOVM_API IO_METHOD(IoSeq, rangeFill);
IOVM_API IO_METHOD(IoSeq, clear);
