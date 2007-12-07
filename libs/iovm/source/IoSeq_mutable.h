
#include "IoSeq.h"

IOVM_API void IoSeq_addMutableMethods(IoSeq *self);
IOVM_API void IoSeq_rawPio_reallocateToSize_(IoSeq *self, size_t size);
IOVM_API int IoSeq_byteCompare(const void *a, const void *b);

IOVM_API IoObject *IoSeq_setItemType(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_convertToItemType(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_convertToFixedSizeType(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_setEncoding(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API void IoSeq_rawCopy_(IoSeq *self, IoSeq *other);
IOVM_API IoObject *IoSeq_copy(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_appendSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_append(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_atInsertSeq(IoSeq *self, IoObject *locals, IoMessage *m);

// removing

IOVM_API IoObject *IoSeq_removeAt(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_removeSlice(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_removeLast(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_setSize(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_preallocateToSize(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_replaceSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_removeSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_replaceFirstSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_atPut(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_lowercase(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_uppercase(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_translate(IoSeq *self, IoObject *locals, IoMessage *m);

// clip

IOVM_API IoObject *IoSeq_clipBeforeSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_clipAfterSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_clipBeforeEndOfSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_clipAfterStartOfSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_empty(IoSeq *self, IoObject *locals, IoMessage *m);

// sort

IOVM_API IoObject *IoSeq_sort(IoSeq *self, IoObject *locals, IoMessage *m);

// reverse

IOVM_API IoObject *IoSeq_reverse(IoSeq *self, IoObject *locals, IoMessage *m);

// removing indexwise

IOVM_API IoObject *IoSeq_removeOddIndexes(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_removeEvenIndexes(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_duplicateIndexes(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_replaceMap(IoSeq *self, IoObject *locals, IoMessage *m);

// strip

IOVM_API IoObject *IoSeq_strip(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_lstrip(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_rstrip(IoSeq *self, IoObject *locals, IoMessage *m);

// escape

IOVM_API IoObject *IoSeq_escape(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_unescape(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoSeq_removePrefix(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_removeSuffix(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_capitalize(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_appendPathSeq(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoSeq_asCapitalized(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoSeq_interpolateInPlace(IoSeq *self, IoObject *locals, IoMessage *m);

// math ---------------------------------------------------------------------

IOVM_API IoObject *IoSeq_addEquals(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_subtractEquals(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_multiplyEquals(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_divideEquals(IoSeq *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoSeq_add(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_subtract(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_multiply(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_divide(IoSeq *self, IoObject *locals, IoMessage *m);


IOVM_API IoObject *IoSeq_dotProduct(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_sum(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_product(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_min(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_max(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_arithmeticMeanAsDouble(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_arithmeticMeanSquareAsDouble(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_square(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_abs(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_ceil(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_floor(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_log(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_log10(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_negate(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_rangeFill(IoSeq *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoSeq_clear(IoSeq *self, IoObject *locals, IoMessage *m);
