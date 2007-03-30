
#include "IoSeq.h"

void IoSeq_addMutableMethods(IoSeq *self);
void IoSeq_rawPio_reallocateToSize_(IoSeq *self, size_t size);
int IoSeq_byteCompare(const void *a, const void *b);

IoObject *IoSeq_setItemType(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_convertToItemType(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_convertToFixedSizeType(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_setEncoding(IoSeq *self, IoObject *locals, IoMessage *m);

void IoSeq_rawCopy(IoSeq *self, IoSeq *other);
IoObject *IoSeq_copy(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_appendSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_append(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_atInsertSeq(IoSeq *self, IoObject *locals, IoMessage *m);

// removing

IoObject *IoSeq_removeSlice(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_removeLast(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_setSize(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_preallocateToSize(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_replaceSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_removeSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_replaceFirstSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_atPut(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_lowercase(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_uppercase(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_translate(IoSeq *self, IoObject *locals, IoMessage *m);

// clip

IoObject *IoSeq_clipBeforeSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_clipAfterSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_clipBeforeEndOfSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_clipAfterStartOfSeq(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_empty(IoSeq *self, IoObject *locals, IoMessage *m);

// sort

IoObject *IoSeq_sort(IoSeq *self, IoObject *locals, IoMessage *m);

// reverse

IoObject *IoSeq_reverse(IoSeq *self, IoObject *locals, IoMessage *m);

// removing indexwise

IoObject *IoSeq_removeOddIndexes(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_removeEvenIndexes(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_duplicateIndexes(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_replaceMap(IoSeq *self, IoObject *locals, IoMessage *m);

// strip

IoObject *IoSeq_strip(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_lstrip(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_rstrip(IoSeq *self, IoObject *locals, IoMessage *m);

// escape

IoObject *IoSeq_escape(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_unescape(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_removePrefix(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_removeSuffix(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_capitalize(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_appendPathSeq(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_asCapitalized(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_interpolateInPlace(IoSeq *self, IoObject *locals, IoMessage *m);

// math ---------------------------------------------------------------------

IoObject *IoSeq_addEquals(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_subtractEquals(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_multiplyEquals(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_divideEquals(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_add(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_subtract(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_multiply(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_divide(IoSeq *self, IoObject *locals, IoMessage *m);


IoObject *IoSeq_dotProduct(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_sum(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_product(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_min(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_max(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_arithmeticMeanAsDouble(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_arithmeticMeanSquareAsDouble(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_square(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_abs(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_ceil(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_floor(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_log(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_log10(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_negate(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_rangeFill(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_clear(IoSeq *self, IoObject *locals, IoMessage *m);
