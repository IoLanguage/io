
// metadoc Sequence copyright Steve Dekorte 2002
// metadoc Sequence license BSD revised
/*metadoc Sequence description
A Sequence is a container for a list of data elements. Typically these elements
are each 1 byte in size. A Sequence can be either mutable or immutable. When
immutable, only the read-only methods can be used. <p> Terminology <ul> <li>
Buffer: A mutable Sequence of single byte elements, typically in a binary
encoding <li> Symbol or String: A unique immutable Sequence, typically in a
character encoding
</ul>
*/
// metadoc Sequence category Core

#pragma GCC diagnostic push

// Suppresses incorrect warning from GCC about CTYPE never having value of -1
#ifndef __MINGW64__
#pragma GCC diagnostic ignored "-Wtautological-constant-out-of-range-compare"
#endif

#include "IoSeq.h"
#include "IoState.h"
#include "IoCFunction.h"
#include "IoObject.h"
#include "IoNumber.h"
#include "IoMessage.h"
#include "IoList.h"
#include "IoMap.h"
#include <ctype.h>
#include <errno.h>

#define DATA(self) ((UArray *)IoObject_dataPointer(self))

#define IO_ASSERT_NOT_SYMBOL(self) IoAssertNotSymbol(self, m)
#define IO_ASSERT_NUMBER_ENCODING(self)                                        \
    IOASSERT(DATA(self)->encoding == CENCODING_NUMBER,                         \
             "operation not valid on non-number encodings")

static void IoAssertNotSymbol(IoSeq *self, IoMessage *m) {
    if (ISSYMBOL(self)) {
        IoState_error_(IOSTATE, m,
                       "'%s' cannot be called on an immutable Sequence",
                       CSTRING(IoMessage_name(m)));
    }
}

IO_METHOD(IoSeq, setItemType) {
    /*doc Sequence setItemType(aTypeName)
    Sets the underlying machine type for the elements.
    Valid names are uint8, uint16, uint32, uint64, int8, int16, int32,
    int64, float32, and float64. Note that 64 bit types are only available
    on platforms that support such types. Returns self.
    */

    CTYPE itemType;
    IoSeq *typeName;

    IO_ASSERT_NOT_SYMBOL(self);

    typeName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    itemType = CTYPE_forName(CSTRING(typeName));

    IOASSERT(itemType != -1, "invalid item type name");

    UArray_setItemType_(DATA(self), itemType);
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoSeq, convertToItemType) {
    /*doc Sequence convertToItemType(aTypeName)
    Converts the underlying machine type for the elements, expanding or
    contracting the size of the Sequence as needed. Valid names are uint8,
    uint16, uint32, uint64, int8, int16, int32, int64, float32, and float64.
    Note that 64 bit types are only available on platforms that support such
    types. Returns self.
    */
    IoSymbol *typeName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    CTYPE itemType = CTYPE_forName(CSTRING(typeName));

    IO_ASSERT_NOT_SYMBOL(self);

    IOASSERT(itemType != -1, "invalid item type name");

    UArray_convertToItemType_(DATA(self), itemType);
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoSeq, convertToFixedSizeType) {
    IO_ASSERT_NOT_SYMBOL(self);
    UArray_convertToFixedSizeType(DATA(self));
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoSeq, setEncoding) {
    /*doc Sequence setEncoding(encodingName)
    Sets the encoding flag of the receiver (only the encoding flag,
    itemSize and itemType will change, no conversion is done between UTF
    encodings - you can use convertToUTF8, etc methods for conversions).
    Valid encodings are number, utf8, utf16, and utf32. Returns self.
    */

    CENCODING encoding;
    IoSeq *encodingName;

    IO_ASSERT_NOT_SYMBOL(self);

    encodingName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    encoding = CENCODING_forName(CSTRING(encodingName));

    IOASSERT(encoding != -1, "invalid encoding name");

    UArray_setEncoding_(DATA(self), encoding);

    IoObject_isDirty_(self, 1);
    return self;
}

void IoSeq_rawCopy_(IoSeq *self, IoSeq *other) {
    UArray_copy_(DATA(self), DATA(other));
}

IO_METHOD(IoSeq, copy) {
    /*doc Sequence copy(aSequence)
    Replaces the bytes of the receiver with a copy of those in aSequence.
    Returns self.
    */

    IO_ASSERT_NOT_SYMBOL(self);

    IoSeq_rawCopy_(self, IoMessage_locals_seqArgAt_(m, locals, 0));
    return self;
}

IO_METHOD(IoSeq, appendSeq) {
    /*doc Sequence appendSeq(object1, object2, ...)
    Calls asString on the arguments and appends the string to the receiver.
    Returns self.
    */

    int i;

    IO_ASSERT_NOT_SYMBOL(self);
    IOASSERT(IoMessage_argCount(m), "requires at least one argument");

    for (i = 0; i < IoMessage_argCount(m); i++) {
        UArray_append_(DATA(self), DATA(IoMessage_locals_valueAsStringArgAt_(
                                       m, locals, i)));
    }
    return self;
}

IO_METHOD(IoSeq, append) {
    /*doc Sequence append(aNumber)
    Appends aNumber (cast to a byte) to the receiver. Returns self.
    */

    int i;

    IO_ASSERT_NOT_SYMBOL(self);
    IOASSERT(IoMessage_argCount(m), "requires at least one argument");

    for (i = 0; i < IoMessage_argCount(m); i++) {
        UArray_appendDouble_(DATA(self),
                             IoMessage_locals_doubleArgAt_(m, locals, i));
    }

    return self;
}

IO_METHOD(IoSeq, atInsertSeq) {
    /*doc Sequence atInsertSeq(indexNumber, object)
    Calls asString on object and inserts the string at position indexNumber.
    Returns self.
    */

    size_t n = IoMessage_locals_sizetArgAt_(m, locals, 0);
    IoSeq *otherSeq = IoMessage_locals_valueAsStringArgAt_(m, locals, 1);

    IO_ASSERT_NOT_SYMBOL(self);

    IOASSERT(n <= UArray_size(DATA(self)),
             "insert index out of sequence bounds");

    UArray_at_putAll_(DATA(self), n, DATA(otherSeq));

    return self;
}

IO_METHOD(IoSeq, insertSeqEvery) {
    /*doc MutableSequence IoSeq_insertSeqEvery(aSequence, aNumberOfItems)
    Inserts aSequence every aNumberOfItems.  Returns self.
    */

    IoSeq *otherSeq = IoMessage_locals_valueAsStringArgAt_(m, locals, 0);
    size_t itemCount = IoMessage_locals_sizetArgAt_(m, locals, 1);

    IO_ASSERT_NOT_SYMBOL(self);

    IOASSERT(itemCount > 0, "aNumberOfItems must be > 0");
    IOASSERT(itemCount <= UArray_size(DATA(self)),
             "aNumberOfItems out of sequence bounds");

    UArray_insert_every_(DATA(self), DATA(otherSeq), itemCount);

    return self;
}

/*
IO_METHOD(IoSeq, atInsert)
{
        doc Sequence atInsert(indexNumber, valueNumber)
        Inserts valueNumber at position indexNumber. Returns self.

        size_t n = IoMessage_locals_sizetArgAt_(m, locals, 0);
        IoNumber *value = IoMessage_locals_numberArgAt_(m, locals, 1);

        IO_ASSERT_NOT_SYMBOL(self);


        UArray_at_putAll_(DATA(self), n, DATA(otherSeq));
        return self;
}
*/

// removing ---------------------------------------

IO_METHOD(IoSeq, removeAt) {
    /*doc Sequence removeAt(index)
    Removes the item at index. Returns self.
    */

    long i = IoMessage_locals_longArgAt_(m, locals, 0);

    IO_ASSERT_NOT_SYMBOL(self);

    i = UArray_wrapPos_(DATA(self), i);

    UArray_removeRange(DATA(self), i, 1);
    return self;
}

IO_METHOD(IoSeq, removeSlice) {
    /*doc Sequence removeSlice(startIndex, endIndex)
    Removes the items from startIndex to endIndex.
    Returns self.
    */

    long start = IoMessage_locals_longArgAt_(m, locals, 0);
    long end = IoMessage_locals_longArgAt_(m, locals, 1);

    IO_ASSERT_NOT_SYMBOL(self);

    start = UArray_wrapPos_(DATA(self), start);
    end = UArray_wrapPos_(DATA(self), end);

    UArray_removeRange(DATA(self), start, end - start + 1);
    return self;
}

IO_METHOD(IoSeq, removeLast) {
    /*doc Sequence removeLast
    Removes the last element from the receiver. Returns self.
    */

    IO_ASSERT_NOT_SYMBOL(self);
    UArray_removeLast(DATA(self));
    return self;
}

IO_METHOD(IoSeq, leaveThenRemove) {
    /*doc MutableSequence IoSeq_leaveThenRemove(aNumberToLeave, aNumberToRemove)
    Leaves aNumberToLeave items then removes aNumberToRemove items.  Returns
    self.
    */

    size_t itemsToLeave = IoMessage_locals_sizetArgAt_(m, locals, 0);
    size_t itemsToRemove = IoMessage_locals_sizetArgAt_(m, locals, 1);

    IO_ASSERT_NOT_SYMBOL(self);

    IOASSERT(itemsToLeave > 0 || itemsToRemove > 0,
             "either aNumberToLeave or aNumberToRemove must be > 0");

    UArray_leave_thenRemove_(DATA(self), itemsToLeave, itemsToRemove);

    return self;
}

IO_METHOD(IoSeq, setSize) {
    /*doc Sequence setSize(aNumber)
    Sets the length in bytes of the receiver to aNumber. Return self.
    */

    size_t len = IoMessage_locals_sizetArgAt_(m, locals, 0);
    IO_ASSERT_NOT_SYMBOL(self);
    UArray_setSize_(DATA(self), len);
    return self;
}

void IoSeq_rawPio_reallocateToSize_(IoSeq *self, size_t size) {
    if (ISSYMBOL(self)) {
        IoState_error_(IOSTATE, NULL,
                       "attempt to resize an immutable Sequence");
    }

    UArray_sizeTo_(DATA(self), size);
}

IO_METHOD(IoSeq, preallocateToSize) {
    /*doc Sequence preallocateToSize(aNumber)
    If needed, resize the memory alloced for the receivers
    byte array to be large enough to fit the number of bytes specified by
    aNumber. This is useful for pio_reallocating the memory so it doesn't
    keep getting allocated as the Sequence is appended to. This operation
    will not change the Sequence's length or contents. Returns self.
    */

    size_t newSize = IoMessage_locals_sizetArgAt_(m, locals, 0);
    IO_ASSERT_NOT_SYMBOL(self);
    UArray_sizeTo_(DATA(self), newSize);
    return self;
}

IO_METHOD(IoSeq, replaceSeq) {
    /*doc Sequence replaceSeq(aSequence, anotherSequence)
    Returns a new Sequence with all occurrences of aSequence
    replaced with anotherSequence in the receiver. Returns self.
    */

    IoSeq *subSeq = IoMessage_locals_seqArgAt_(m, locals, 0);
    IoSeq *otherSeq = IoMessage_locals_seqArgAt_(m, locals, 1);
    IO_ASSERT_NOT_SYMBOL(self);
    UArray_replace_with_(DATA(self), DATA(subSeq), DATA(otherSeq));
    return self;
}

IO_METHOD(IoSeq, removeSeq) {
    /*doc Sequence removeSeq(aSequence)
    Removes occurrences of aSequence from the receiver.
    */

    IoSeq *subSeq = IoMessage_locals_seqArgAt_(m, locals, 0);
    IO_ASSERT_NOT_SYMBOL(self);
    UArray_remove_(DATA(self), DATA(subSeq));
    return self;
}

IO_METHOD(IoSeq, replaceFirstSeq) {
    /*doc Sequence replaceFirstSeq(aSequence, anotherSequence,
    optionalStartIndex) Returns a new Sequence with the first occurrence of
    aSequence replaced with anotherSequence in the receiver. If
    optionalStartIndex is provided, the search for aSequence begins at that
    index. Returns self.
    */

    IoSeq *subSeq = IoMessage_locals_seqArgAt_(m, locals, 0);
    IoSeq *otherSeq = IoMessage_locals_seqArgAt_(m, locals, 1);
    size_t startIndex = 0;

    if (IoMessage_argCount(m) > 2) {
        startIndex = IoMessage_locals_longArgAt_(m, locals, 2);
    }

    IO_ASSERT_NOT_SYMBOL(self);

    {
        UArray *a = DATA(self);
        UArray *b = DATA(subSeq);
        UArray *c = DATA(otherSeq);
        long i = UArray_find_from_(a, b, startIndex);
        if (i != -1) {
            UArray_removeRange(a, i, UArray_size(b));
            UArray_at_putAll_(a, i, c);
        }
    }
    return self;
}

IO_METHOD(IoSeq, atPut) {
    /*doc Sequence atPut(aNumberIndex, aNumber)
    Sets the value at the index specified by aNumberIndex to aNumber. Returns
    self.
    */

    size_t i = IoMessage_locals_longArgAt_(m, locals, 0);
    UArray *a = DATA(self);

    IO_ASSERT_NOT_SYMBOL(self);

    if (UArray_isFloatType(a)) {
        double v = IoMessage_locals_doubleArgAt_(m, locals, 1);
        UArray_at_putDouble_(a, i, v);
    } else {
        long v = IoMessage_locals_longArgAt_(m, locals, 1);
        UArray_at_putLong_(a, i, v);
    }

    return self;
}

IO_METHOD(IoSeq, lowercase) {
    /*doc Sequence lowercase
    Makes all the uppercase characters in the receiver lowercase. Returns self.
    */

    IO_ASSERT_NOT_SYMBOL(self);
    UArray_tolower(DATA(self));
    return self;
}

IO_METHOD(IoSeq, uppercase) {
    /*doc Sequence uppercase
    Makes all characters of the receiver uppercase.
    */

    IO_ASSERT_NOT_SYMBOL(self);
    UArray_toupper(DATA(self));
    return self;
}

// clip --------------------------------------

IO_METHOD(IoSeq, clipBeforeSeq) {
    /*doc Sequence clipBeforeSeq(aSequence)
    Clips receiver before aSequence.
    */

    IoSeq *otherSeq = IoMessage_locals_seqArgAt_(m, locals, 0);
    IO_ASSERT_NOT_SYMBOL(self);
    UArray_clipBefore_(DATA(self), DATA(otherSeq));
    return self;
}

IO_METHOD(IoSeq, clipAfterSeq) {
    /*doc Sequence clipAfterSeq(aSequence)
    Removes the contents of the receiver after the end of
    the first occurrence of aSequence. Returns true if anything was
    removed, or false otherwise.
    */

    IoSeq *otherSeq = IoMessage_locals_seqArgAt_(m, locals, 0);

    IO_ASSERT_NOT_SYMBOL(self);
    UArray_clipAfter_(DATA(self), DATA(otherSeq));
    return self;
}

IO_METHOD(IoSeq, clipBeforeEndOfSeq) {
    /*doc Sequence clipBeforeEndOfSeq(aSequence)
    Removes the contents of the receiver before the end of
    the first occurrence of aSequence. Returns true if anything was
    removed, or false otherwise.
    */

    IoSeq *otherSeq = IoMessage_locals_seqArgAt_(m, locals, 0);
    IO_ASSERT_NOT_SYMBOL(self);
    UArray_clipBeforeEndOf_(DATA(self), DATA(otherSeq));
    return self;
}

IO_METHOD(IoSeq, clipAfterStartOfSeq) {
    /*doc Sequence clipAfterStartOfSeq(aSequence)
    Removes the contents of the receiver after the beginning of
    the first occurrence of aSequence. Returns true if anything was
    removed, or false otherwise.
    */

    IoSeq *otherSeq = IoMessage_locals_seqArgAt_(m, locals, 0);
    IO_ASSERT_NOT_SYMBOL(self);
    UArray_clipAfterStartOf_(DATA(self), DATA(otherSeq));
    return self;
}

// -----------------------------------------

IO_METHOD(IoSeq, empty) {
    /*doc Sequence empty
    Sets all bytes in the receiver to 0x0 and sets
    its length to 0. Returns self.
    */

    IO_ASSERT_NOT_SYMBOL(self);
    UArray_clear(DATA(self));
    UArray_setSize_(DATA(self), 0);
    return self;
}

int IoSeq_byteCompare(const void *a, const void *b) {
    char aa = *(char *)a;
    char bb = *(char *)b;

    if (aa < bb) {
        return -1;
    }

    if (aa == bb) {
        return 0;
    }

    return 1;
}

IO_METHOD(IoSeq, sort) {
    // doc Sequence sort Sorts the characters/numbers in the array. Returns
    // self.

    UArray *a = DATA(self);
    IO_ASSERT_NOT_SYMBOL(self);

    if (UArray_itemType(a) == CTYPE_uintptr_t) {
        UArray_sortBy_(a, (UArraySortCallback *)IoObject_compare);
    } else {
        UArray_sort(a);
    }

    return self;
}

IO_METHOD(IoSeq, replaceMap) {
    /*doc Sequence replaceMap(aMap)
    In the receiver, the keys of aMap replaced with its values. Returns self.
    */

    IoMap *map = IoMessage_locals_mapArgAt_(m, locals, 0);
    UArray *ba = DATA(self);

    IO_ASSERT_NOT_SYMBOL(self);

    PHASH_FOREACH(IoMap_rawHash(map), k, v, {
        IoSymbol *subSeq = k;
        IoSymbol *otherSeq = v;

        if (ISSEQ(otherSeq)) {
            UArray_replace_with_(ba, DATA(subSeq), DATA(otherSeq));
        } else {
            IoState_error_(IOSTATE, m,
                           "argument 0 to method '%s' must be a Map with "
                           "Sequence values, not '%s'",
                           CSTRING(IoMessage_name(m)), IoObject_name(otherSeq));
        }
    });

    return self;
}

// translate ------------------------------------------------------

IO_METHOD(IoSeq, translate) {
    /*doc Sequence translate(fromChars, toChars)
    In the receiver, the characters in fromChars are replaced with those in the
    same positions in toChars. Returns self.
    */

    UArray *ba = DATA(self);
    UArray *fc = DATA(IoMessage_locals_seqArgAt_(m, locals, 0));
    UArray *tc = DATA(IoMessage_locals_seqArgAt_(m, locals, 1));

    IO_ASSERT_NOT_SYMBOL(self);

    if (UArray_size(tc) != UArray_size(fc)) {
        IoState_error_(IOSTATE, m,
                       "translation strings must be of the same length");
    }

    UArray_translate(ba, fc, tc);

    return self;
}

// reverse --------------------------------------------------------

IO_METHOD(IoSeq, reverseInPlace) {
    /*doc Sequence reverseInPlace
    Reverses the bytes in the receiver, in-place.
    */

    IO_ASSERT_NOT_SYMBOL(self);

    UArray_reverse(DATA(self));
    return self;
}

// strip ----------------------------------------------------------

IO_METHOD(IoSeq, strip) {
    /*doc Sequence strip(optionalSequence)
    Trims the whitespace (or optionalSequence) off both ends:
    <p>
    <pre>
    "   Trim this string   \r\n" strip
    ==> "Trim this string"
    </pre>
    */

    IO_ASSERT_NOT_SYMBOL(self);

    if (IoMessage_argCount(m) > 0) {
        IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
        UArray_strip_(DATA(self), DATA(other));
    } else {
        UArray space = UArray_stackAllocedWithCString_(WHITESPACE);
        UArray_strip_(DATA(self), &space);
    }

    return self;
}

IO_METHOD(IoSeq, lstrip) {
    /*doc Sequence lstrip(aSequence)
    Strips the characters in aSequence
    stripped from the beginning of the receiver. Example:
    <p>
    <pre>
    "Keep the tail" lstrip(" eKp")
    ==> "the tail"
    </pre>
    */

    IO_ASSERT_NOT_SYMBOL(self);

    if (IoMessage_argCount(m) > 0) {
        IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
        UArray_lstrip_(DATA(self), DATA(other));
    } else {
        UArray space = UArray_stackAllocedWithCString_(WHITESPACE);
        UArray_lstrip_(DATA(self), &space);
    }

    return self;
}

IO_METHOD(IoSeq, rstrip) {
    /*doc Sequence rstrip(aSequence)
    Strips the characters in
    aSequence stripped from the end of the receiver. Example:
    <pre>
    "Cut the tail off" rstrip(" afilot")
    ==> "Cut the"
    </pre>
    */

    IO_ASSERT_NOT_SYMBOL(self);

    if (IoMessage_argCount(m) > 0) {
        IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
        UArray_rstrip_(DATA(self), DATA(other));
    } else {
        UArray space = UArray_stackAllocedWithCString_(WHITESPACE);
        UArray_rstrip_(DATA(self), &space);
    }

    return self;
}

// -----------------------------------------------------------

IO_METHOD(IoSeq, escape) {
    /*doc Sequence escape
    Escape characters in the receiver are replaced with escape codes.
    For example a string containing a single return character would contain the
    following 2 characters after being escaped: "\n". Returns self.
    */

    IO_ASSERT_NOT_SYMBOL(self);
    UArray_escape(DATA(self));
    return self;
}

IO_METHOD(IoSeq, unescape) {
    /*doc Sequence unescape
    Escape codes replaced with escape characters. Returns self.
    */

    IO_ASSERT_NOT_SYMBOL(self);
    UArray_unescape(DATA(self));
    return self;
}

IO_METHOD(IoSeq, removePrefix) {
    /*doc Sequence removePrefix(aSequence)
    If the receiver begins with aSequence, it is removed. Returns self.
    */

    IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);

    IO_ASSERT_NOT_SYMBOL(self);

    if (UArray_beginsWith_(DATA(self), DATA(other))) {
        UArray_removeRange(DATA(self), 0, UArray_size(DATA(other)));
    }

    return self;
}

IO_METHOD(IoSeq, removeSuffix) {
    /*doc Sequence removeSuffix(aSequence)
    If the receiver end with aSequence, it is removed. Returns self.
    */

    IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);

    IO_ASSERT_NOT_SYMBOL(self);

    if (UArray_endsWith_(DATA(self), DATA(other))) {
        UArray *ba = DATA(self);
        UArray_removeRange(ba, UArray_size(ba) - UArray_size(DATA(other)),
                           UArray_size(ba));
    }

    return self;
}

IO_METHOD(IoSeq, capitalize) {
    /*doc Sequence capitalize
    First charater of the receiver is made uppercase.
    */

    long firstChar = UArray_firstLong(DATA(self));

    IO_ASSERT_NOT_SYMBOL(self);
    UArray_at_putLong_(DATA(self), 0, toupper((int)firstChar));
    return self;
}

IO_METHOD(IoSeq, appendPathSeq) {
    /*doc Sequence appendPathSeq(aSeq)
    Appends argument to the receiver such that there is one
    and only one path separator between the two. Returns self.
    */

    IoSeq *component = IoMessage_locals_seqArgAt_(m, locals, 0);

    IO_ASSERT_NOT_SYMBOL(self);
    UArray_appendPath_(DATA(self), DATA(component));
    return self;
}

IO_METHOD(IoSeq, interpolateInPlace) {
    /*doc Sequence interpolateInPlace(optionalContext)
    Replaces all #{expression} with expression evaluated in the optionalContext.
    If optionalContext not given, the current context is used.  Returns self.
    */

    IoObject *context;
    UArray *string;
    UArray *code;
    IoObject *evaluatedCode;
    IoSeq *codeString;
    UArray *evaluatedCodeAsString = NULL;
    // const char *label;
    long from, to;
    UArray begin = UArray_stackAllocedWithCString_("#{");
    UArray end = UArray_stackAllocedWithCString_("}");

    IO_ASSERT_NOT_SYMBOL(self);

    context = IoMessage_locals_valueArgAt_(m, locals, 0);
    string = DATA(self);
    // label = "IoSeq_interpolateInPlace()";
    from = 0;

    context = (context == IONIL(self)) ? locals : context;

    IoState_pushRetainPool(IOSTATE);

    for (;;) {
        IoState_clearTopPool(IOSTATE);

        from = UArray_find_from_(string, &begin, from);
        if (from == -1)
            break;

        to = UArray_find_from_(string, &end, from);
        if (to == -1)
            break;

        code = UArray_slice(string, from + 2, to);
        codeString = IoSeq_newWithUArray_copy_(IOSTATE, code, 0);

        if (UArray_size(code) == 0) {
            // we do not want "#{}" to interpolate into "nil"
            evaluatedCodeAsString =
                DATA(IoState_doCString_(IOSTATE, "Sequence clone"));
        } else {
            IoMessage *em = IoMessage_newWithName_andCachedArg_(
                IOSTATE, IOSYMBOL("doString"), codeString);
            evaluatedCode = IoObject_perform(context, context, em);
            evaluatedCode = IoObject_perform(evaluatedCode, context,
                                             IOSTATE->asStringMessage);

            if (ISSEQ(evaluatedCode)) {
                evaluatedCodeAsString = DATA(evaluatedCode);
            }
        }

        // UArray_free(code);

        if (evaluatedCodeAsString == NULL) {
            break;
        }

        UArray_removeRange(string, from, to - from + 1);
        UArray_at_putAll_(string, from, evaluatedCodeAsString);
        from = from + UArray_size(evaluatedCodeAsString);
    }

    IoState_popRetainPool(IOSTATE);

    if (from >= 0 && to >= 0) {
        IOASSERT(evaluatedCodeAsString != NULL, "bad asString results");
    }

    return self;
}

// math ---------------------------------------------------------------------

IO_METHOD(IoSeq, addEquals) {
    /*doc Sequence +=(aSeq)
    Vector addition - adds the values of aSeq to those of the receiver.
    Only works on Sequences whose item type is numeric. Returns self.
    */

    IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);

    IO_ASSERT_NOT_SYMBOL(self);
    IO_ASSERT_NUMBER_ENCODING(self);

    if (ISSEQ(other)) {
        // printf("IoMessage_argCount(m) = %i\n", IoMessage_argCount(m));
        if (IoMessage_argCount(m) > 1) {
            float offset = IoMessage_locals_floatArgAt_(m, locals, 1);
            float xscale = IoMessage_locals_floatArgAt_(m, locals, 2);
            float yscale = IoMessage_locals_floatArgAt_(m, locals, 3);
            UArray_addEqualsOffsetXScaleYScale(DATA(self), DATA(other), offset,
                                               xscale, yscale);
        } else {
            UArray_add_(DATA(self), DATA(other));
        }
    } else if (ISNUMBER(other)) {
        double value = IoNumber_asDouble(other);
        UArray_addScalarDouble_(DATA(self), value);
    } else {
        IoMessage_locals_numberArgAt_errorForType_(self, locals, 0,
                                                   "Sequence or Number");
    }

    return self;
}

IO_METHOD(IoSeq, subtractEquals) {
    /*doc Sequence -=(aSeq)
    Vector subtraction - subtracts the values of aSeq to those of the receiver.
    Only works on Sequences whose item type is numeric. Returns self.
    */

    IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);

    IO_ASSERT_NOT_SYMBOL(self);
    IO_ASSERT_NUMBER_ENCODING(self);

    if (ISSEQ(other)) {
        UArray_subtract_(DATA(self), DATA(other));
    } else if (ISNUMBER(other)) {
        double value = IoNumber_asDouble(other);
        UArray_subtractScalarDouble_(DATA(self), value);
    } else {
        IoMessage_locals_numberArgAt_errorForType_(self, locals, 0,
                                                   "Sequence or Number");
    }

    return self;
}

IO_METHOD(IoSeq, multiplyEquals) {
    /*doc Sequence *=(aSeq)
    Multiplies the values of aSeq to the corresponding values of the receiver.
    Only works on Sequences whose item type is numeric. Returns self.
    */

    IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);

    IO_ASSERT_NOT_SYMBOL(self);
    IO_ASSERT_NUMBER_ENCODING(self);

    if (ISSEQ(other)) {
        UArray_multiply_(DATA(self), DATA(other));
    } else if (ISNUMBER(other)) {
        double value = IoNumber_asDouble(other);
        UArray_multiplyScalarDouble_(DATA(self), value);
    } else {
        IoMessage_locals_numberArgAt_errorForType_(self, locals, 0,
                                                   "Sequence or Number");
    }

    return self;
}

IO_METHOD(IoSeq, divideEquals) {
    /*doc Sequence /=(aSeq)
    Divides the values of aSeq to the corresponding values of the receiver.
    Only works on Sequences whose item type is numeric. Returns self.
    */

    IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);

    IO_ASSERT_NOT_SYMBOL(self);
    IO_ASSERT_NUMBER_ENCODING(self);

    if (ISSEQ(other)) {
        UArray_divide_(DATA(self), DATA(other));
    } else if (ISNUMBER(other)) {
        double value = IoNumber_asDouble(other);
        UArray_divideScalarDouble_(DATA(self), value);
    } else {
        IoMessage_locals_numberArgAt_errorForType_(self, locals, 0,
                                                   "Sequence or Number");
    }

    return self;
}

IO_METHOD(IoSeq, powerEquals) {
    /*doc Sequence **=(aSeq)
    Raises the values of the receiver in the corresponding values of aSeq.
    Only works on Sequences whose item type is numeric. Returns self.
    */

    IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);

    IO_ASSERT_NOT_SYMBOL(self);
    IO_ASSERT_NUMBER_ENCODING(self);

    if (ISSEQ(other)) {
        UArray_power_(DATA(self), DATA(other));
    } else if (ISNUMBER(other)) {
        double value = IoNumber_asDouble(other);
        UArray_powerScalarDouble_(DATA(self), value);
    } else {
        IoMessage_locals_numberArgAt_errorForType_(self, locals, 0,
                                                   "Sequence or Number");
    }

    return self;
}

IoObject *IoSeq_clone(IoSeq *self) {
    return IoSeq_newWithUArray_copy_(IOSTATE, DATA(self), 1);
}

IO_METHOD(IoSeq, add) {
    /*doc Sequence +(aSeq)
    Vector addition - adds the values of aSeq to the corresponding values of the
    receiver returning a new vector with the result. Only works on Sequences
    whose item type is numeric.
    */

    return IoSeq_addEquals(IoSeq_clone(self), locals, m);
}

IO_METHOD(IoSeq, subtract) {
    /*doc Sequence -(aSeq)
    Vector subtraction - Subtracts the values of aSeq from the corresponding
    values of the receiver returning a new vector with the result. Only works on
    Sequences whose item type is numeric.
    */

    return IoSeq_subtractEquals(IoSeq_clone(self), locals, m);
}

IO_METHOD(IoSeq, multiply) {
    /*doc Sequence *(aSeq)
    Multiplies the values of aSeq to the corresponding values of the receiver
    returning a new vector with the result.
    Only works on Sequences whose item type is numeric.
    */

    return IoSeq_multiplyEquals(IoSeq_clone(self), locals, m);
}

IO_METHOD(IoSeq, divide) {
    /*doc Sequence /(aSeq)
    Divides the values of the receiver by the corresponding values of aSeq
    returning a new vector with the result.
    Only works on Sequences whose item type is numeric.
    */

    return IoSeq_divideEquals(IoSeq_clone(self), locals, m);
}

IO_METHOD(IoSeq, power) {
    /*doc Sequence **(aSeq)
    Raises the values of the receiver in the corresponding values of aSeq
    returning a new vector with the result.
    Only works on Sequences whose item type is numeric.
    */

    return IoSeq_powerEquals(IoSeq_clone(self), locals, m);
}

IO_METHOD(IoSeq, dotProduct) {
    /*doc Sequence dotProduct(aSeq)
    Returns a new Sequence containing the dot product of the receiver with aSeq.
    */

    IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
    IO_ASSERT_NOT_SYMBOL(self);
    return IONUMBER(UArray_dotProduct_(DATA(self), DATA(other)));
}

IO_METHOD(IoSeq, setItemsToLong_) {
    /*doc Sequence setItemsToLong(aNumber)
    Sets all items in the Sequence to the long integer value of aNumber.
    */

    long v = IoMessage_locals_longArgAt_(m, locals, 0);
    IO_ASSERT_NOT_SYMBOL(self);
    UArray_setItemsToLong_(DATA(self), v);
    return self;
}

IO_METHOD(IoSeq, setItemsToDouble_) {
    /*doc Sequence setItemsToDouble(aNumber)
    Sets all items in the Sequence to the double floating point value of
    aNumber.
    */

    double v = IoMessage_locals_doubleArgAt_(m, locals, 0);
    IO_ASSERT_NOT_SYMBOL(self);
    UArray_setItemsToLong_(DATA(self), v);
    return self;
}

IO_METHOD(IoSeq, set_) {
    /*doc Sequence set(aNumber1, aNumber2, ...)
    Sets the values of the receiver to the sequences of numbers in the
    arguments. Unset values will remain unchanged. Returns self.
    */

    double i, max = IoMessage_argCount(m);
    IO_ASSERT_NOT_SYMBOL(self);

    for (i = 0; i < max; i++) {
        double v = IoMessage_locals_doubleArgAt_(m, locals, i);
        UArray_at_putDouble_(DATA(self), i, v);
    }

    return self;
}

#define IoSeqMutateNoArgNoResultOp(name)                                       \
    IoObject *IoSeq_##name(IoSeq *self, IoObject *locals, IoMessage *m) {      \
        IO_ASSERT_NOT_SYMBOL(self);                                            \
        UArray_##name(DATA(self));                                             \
        return self;                                                           \
    }

/*doc Sequence negate
Negates the values of the receiver.
Returns self.
*/
IoSeqMutateNoArgNoResultOp(negate)

    /*doc Sequence rangeFill
    Sets the values of the Sequence to their index values.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(rangeFill)

    /*doc Sequence sin
    Sets each value of the Sequence to the trigonometric sine of its value.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(sin)

    /*doc Sequence cos
    Sets each value of the Sequence to the trigonometric cosine of its value.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(cos)

    /*doc Sequence tan
    Sets each value of the Sequence to the trigonometric tangent of its value.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(tan)

    /*doc Sequence asin
    Sets each value of the Sequence to the trigonometric arcsine of its value.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(asin)

    /*doc Sequence acos
    Sets each value of the Sequence to the trigonometric arcsine of its value.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(acos)

    /*doc Sequence atan
    Sets each value of the Sequence to the trigonometric arctangent of its
    value. Returns self.
    */
    IoSeqMutateNoArgNoResultOp(atan)

    /*doc Sequence sinh
    Sets each value of the Sequence to the hyperbolic sine of its value.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(sinh)

    /*doc Sequence cosh
    Sets each value of the Sequence to the hyperbolic cosine of its value.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(cosh)

    /*doc Sequence tanh
    Sets each value of the Sequence to the hyperbolic tangent of its value.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(tanh)

    /*doc Sequence exp
    Sets each value of the Sequence to e**value.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(exp)

    /*doc Sequence log
    Sets each value of the Sequence to the natural log of its value.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(log)

    /*doc Sequence log10
    Sets each value of the Sequence to the base 10 log of its value.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(log10);

/*doc Sequence ceil
Round each value to smallest integral value not less than x.
Returns self.
*/
IoSeqMutateNoArgNoResultOp(ceil)

    /*doc Sequence floor
    Round each value to largest integral value not greater than x.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(floor)

    /*doc Sequence abs
    Sets each value of the Sequence to its absolute value.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(abs)

    /*doc Sequence square
    Sets each value of the Sequence to the square of its value.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(square)

    /*doc Sequence sqrt
    Sets each value of the Sequence to the square root of its value.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(sqrt)

    /*doc Sequence normalize
    Divides each value of the Sequence by the max value of the sequence.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(normalize)

#define IoSeqNoArgNumberResultOp(name)                                         \
    IoObject *IoSeq_##name(IoSeq *self, IoObject *locals, IoMessage *m) {      \
        return IONUMBER(UArray_##name(DATA(self)));                            \
    }

    /*doc Sequence sum
    Returns the sum of the Sequence.
    */
    IoSeqNoArgNumberResultOp(sumAsDouble)

    /*doc Sequence product
    Returns the product of all the sequence's values multipled together.
    */
    IoSeqNoArgNumberResultOp(productAsDouble)

    /*doc Sequence min
    Returns the minimum value of the Sequence.
    */
    IoSeqNoArgNumberResultOp(minAsDouble)

    /*doc Sequence max
    Returns the maximum value of the Sequence.
    */
    IoSeqNoArgNumberResultOp(maxAsDouble)

    /*doc Sequence mean
    Returns the arithmetic mean of the sequence.
    */
    IoSeqNoArgNumberResultOp(arithmeticMeanAsDouble)

    /*doc Sequence meanSquare
    Returns the arithmetic mean of the sequence's values after they have been
    squared.
    */
    IoSeqNoArgNumberResultOp(arithmeticMeanSquareAsDouble)

    /*doc Sequence hash
    Returns a Number containing a hash of the Sequence.
    */
    IoSeqNoArgNumberResultOp(hash)

#define IoSeqLongArgNumberResultOp(name)                                       \
    IoObject *IoSeq_##name(IoSeq *self, IoObject *locals, IoMessage *m) {      \
        return IONUMBER(UArray_##name(                                         \
            DATA(self), IoMessage_locals_longArgAt_(m, locals, 0)));           \
    }

    // IoSeqLongArgNumberResultOp(setAllBitsTo_)

    /*doc Sequence byteAt(byteIndex)
    Returns a Number containing the byte at the byte index value.
    */
    IoSeqLongArgNumberResultOp(byteAt_)

    /*doc Sequence bitAt(bitIndex)
    Returns a Number containing the bit at the bit index value.
    */
    IoSeqLongArgNumberResultOp(bitAt_)

    /*doc Sequence bitCount
    Returns the number of bits in the sequence.
    */
    IoSeqNoArgNumberResultOp(bitCount)

#define IoSeqSeqArgNoResultOp(name)                                            \
    IoObject *IoSeq_##name(IoSeq *self, IoObject *locals, IoMessage *m) {      \
        IO_ASSERT_NOT_SYMBOL(self);                                            \
        UArray_##name(DATA(self),                                              \
                      DATA(IoMessage_locals_seqArgAt_(m, locals, 0)));         \
        return self;                                                           \
    }

    /*doc Sequence bitwiseOr(aSequence)
    Updates the receiver to be the result of a bitwiseOr with aSequence. Returns
    self.
    */
    IoSeqSeqArgNoResultOp(bitwiseOr_)

    /*doc Sequence bitwiseAnd(aSequence)
    Updates the receiver to be the result of a bitwiseAnd with aSequence.
    Returns self.
    */
    IoSeqSeqArgNoResultOp(bitwiseAnd_)

    /*doc Sequence bitwiseXor(aSequence)
    Updates the receiver to be the result of a bitwiseXor with aSequence.
    Returns self.
    */
    IoSeqSeqArgNoResultOp(bitwiseXor_)

    /*doc Sequence bitwiseNot(aSequence)
    Updates the receiver to be the result of a bitwiseNot with aSequence.
    Returns self.
    */
    IoSeqMutateNoArgNoResultOp(bitwiseNot)

    /*doc Sequence logicalOr(aSequence)
    Updates the receiver's values to be the result of a logical OR operations
    with the values of aSequence. Returns self.
    */
    IoSeqSeqArgNoResultOp(logicalOr_)

    /*doc Sequence logicalAnd(aSequence)
    Updates the receiver's values to be the result of a logical OR operations
    with the values of aSequence. Returns self.
    */
    IoSeqSeqArgNoResultOp(logicalAnd_)

    /*doc Sequence Max
    Returns the maximum value in the sequence.
    */
    IoSeqSeqArgNoResultOp(Max)

    /*doc Sequence Min
    Returns the minimum value in the sequence.
    */
    IoSeqSeqArgNoResultOp(Min)

    /*doc Sequence duplicateIndexes
    Duplicates all indexes in the receiver.
    For example, list(1,2,3) duplicateIndexes == list(1,1,2,2,3,3). Returns
    self.
    */
    IoSeqMutateNoArgNoResultOp(duplicateIndexes)

    /*doc Sequence removeOddIndexes
    Removes odd indexes in the receiver.
    For example, list(1,2,3) removeOddIndexes == list(2). Returns self.
    */
    IoSeqMutateNoArgNoResultOp(removeOddIndexes)

    /*doc Sequence removeEvenIndexes
    Removes even indexes in the receiver.
    For example, list(1,2,3) removeEvenIndexes == list(1, 3). Returns self.
    */
    IoSeqMutateNoArgNoResultOp(removeEvenIndexes)

    /*doc Sequence clear
    Set all values in the sequence to 0. Returns self.
    */
    IoSeqMutateNoArgNoResultOp(clear)

        void IoSeq_addMutableMethods(IoSeq *self) {
    IoMethodTable methodTable[] = {
        {"setItemType", IoSeq_setItemType},
        {"setEncoding", IoSeq_setEncoding},
        {"convertToItemType", IoSeq_convertToItemType},
        {"convertToFixedSizeType", IoSeq_convertToFixedSizeType},
        {"copy", IoSeq_copy},
        {"appendSeq", IoSeq_appendSeq},
        {"append", IoSeq_append},
        {"atInsertSeq", IoSeq_atInsertSeq},
        {"insertSeqEvery", IoSeq_insertSeqEvery},
        {"removeAt", IoSeq_removeAt},
        {"removeSlice", IoSeq_removeSlice},
        {"removeLast", IoSeq_removeLast},
        {"leaveThenRemove", IoSeq_leaveThenRemove},
        {"setSize", IoSeq_setSize},
        {"preallocateToSize", IoSeq_preallocateToSize},
        {"replaceSeq", IoSeq_replaceSeq},
        {"removeSeq", IoSeq_removeSeq},
        {"replaceFirstSeq", IoSeq_replaceFirstSeq},
        {"atPut", IoSeq_atPut},
        {"lowercase", IoSeq_lowercase},
        {"uppercase", IoSeq_uppercase},
        {"translate", IoSeq_translate},

        {"clipBeforeSeq", IoSeq_clipBeforeSeq},
        {"clipAfterSeq", IoSeq_clipAfterSeq},
        {"clipBeforeEndOfSeq", IoSeq_clipBeforeEndOfSeq},
        {"clipAfterStartOfSeq", IoSeq_clipAfterStartOfSeq},

        {"empty", IoSeq_empty},
        {"sort", IoSeq_sort},
        {"reverseInPlace", IoSeq_reverseInPlace},
        {"replaceMap", IoSeq_replaceMap},

        {"strip", IoSeq_strip},
        {"lstrip", IoSeq_lstrip},
        {"rstrip", IoSeq_rstrip},

        {"zero", IoSeq_clear},

        {"escape", IoSeq_escape},
        {"unescape", IoSeq_unescape},
        {"removePrefix", IoSeq_removePrefix},
        {"removeSuffix", IoSeq_removeSuffix},
        {"capitalize", IoSeq_capitalize},
        {"appendPathSeq", IoSeq_appendPathSeq},

        {"interpolateInPlace", IoSeq_interpolateInPlace},

        {"+=", IoSeq_addEquals},
        {"-=", IoSeq_subtractEquals},
        {"*=", IoSeq_multiplyEquals},
        {"/=", IoSeq_divideEquals},
        {"**=", IoSeq_powerEquals},

        {"+", IoSeq_add},
        {"-", IoSeq_subtract},
        {"*", IoSeq_multiply},
        {"/", IoSeq_divide},
        {"**", IoSeq_power},

        //
        {"dotProduct", IoSeq_dotProduct},
        {"sum", IoSeq_sumAsDouble},
        {"product", IoSeq_productAsDouble},
        {"min", IoSeq_minAsDouble},
        {"max", IoSeq_maxAsDouble},
        {"mean", IoSeq_arithmeticMeanAsDouble},
        {"meanSquare", IoSeq_arithmeticMeanSquareAsDouble},
        {"square", IoSeq_square},
        {"sqrt", IoSeq_sqrt},
        {"normalize", IoSeq_normalize},
        {"hash", IoSeq_hash},

        {"abs", IoSeq_abs},
        {"ceil", IoSeq_ceil},
        {"floor", IoSeq_floor},
        {"log", IoSeq_log},
        {"log10", IoSeq_log10},
        {"negate", IoSeq_negate},
        {"rangeFill", IoSeq_rangeFill},
        {"Min", IoSeq_Min},
        {"Max", IoSeq_Max},

        {"sin", IoSeq_sin},
        {"cos", IoSeq_cos},
        {"tan", IoSeq_tan},

        {"asin", IoSeq_asin},
        {"acos", IoSeq_acos},
        {"atan", IoSeq_atan},

        {"sinh", IoSeq_sinh},
        {"cosh", IoSeq_cosh},
        {"tanh", IoSeq_tanh},

        {"removeOddIndexes", IoSeq_removeOddIndexes},
        {"removeEvenIndexes", IoSeq_removeEvenIndexes},
        {"duplicateIndexes", IoSeq_duplicateIndexes},

        //{"setAllBitsTo",  IoSeq_setAllBitsTo_},
        {"byteAt", IoSeq_byteAt_},
        {"bitAt", IoSeq_bitAt_},
        {"bitCount", IoSeq_bitCount},

        {"bitwiseOr", IoSeq_bitwiseOr_},
        {"bitwiseAnd", IoSeq_bitwiseAnd_},
        {"bitwiseXor", IoSeq_bitwiseXor_},
        {"bitwiseNot", IoSeq_bitwiseNot},

        {"logicalOr", IoSeq_logicalOr_},
        {"setItemsToLong", IoSeq_setItemsToLong_},
        {"setItemsToDouble", IoSeq_setItemsToDouble_},
        {"set", IoSeq_set_},

        {NULL, NULL},
    };

    IoObject_addMethodTable_(self, methodTable);
}
#pragma GCC pop
