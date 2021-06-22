// metadoc Sequence category Core
// metadoc Sequence copyright Steve Dekorte 2002
// metadoc Sequence license BSD revised
/*metadoc Sequence description
A Sequence is a container for a list of data elements.
Immutable Sequences are also called "Symbols".
*/

#include "IoSeq.h"
#include "IoState.h"
#include "IoCFunction.h"
#include "IoObject.h"
#include "IoNumber.h"
#include "IoMessage.h"
#include "IoList.h"
#include <ctype.h>
#include <errno.h>
#include "parson.h"
#include "IoMap.h"

#define DATA(self) ((UArray *)IoObject_dataPointer(self))

IoObject *IoSeq_rawAsSymbol(IoSeq *self) {
    if (ISSYMBOL(self)) {
        return self;
    }

    return IoState_symbolWithUArray_copy_(IOSTATE, DATA(self), 1);
}

IO_METHOD(IoSeq, with) {
    /*doc Sequence with(aSequence, ...)
    Returns a new Sequence which is the concatenation of the arguments.
    The returned sequence will have the same mutability status as the receiver.
    */

    int n, argCount = IoMessage_argCount(m);
    UArray *ba = UArray_clone(DATA(self));

    for (n = 0; n < argCount; n++) {
        IoSeq *v = IoMessage_locals_seqArgAt_(m, locals, n);
        UArray_append_(ba, DATA(v));
    }

    if (ISSYMBOL(self)) {
        return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
    }

    return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
}

IO_METHOD(IoSeq, itemType) {
    /*doc Sequence itemType
    Returns machine type of elements.
    */

    return IOSYMBOL(CTYPE_name(UArray_itemType(DATA(self))));
}

IO_METHOD(IoSeq, itemSize) {
    /*doc Sequence itemSize
    Returns number of bytes in each element.
    */

    return IONUMBER(UArray_itemSize(DATA(self)));
}

IO_METHOD(IoSeq, encoding) {
    /*doc Sequence encoding
    Returns the encoding of the elements.
    */

    return IOSYMBOL(CENCODING_name(UArray_encoding(DATA(self))));
}

IO_METHOD(IoSeq, asUTF8) {
    /*doc Sequence asUTF8
    Returns a new copy of the receiver converted to utf8 encoding.
    */

    return IoSeq_newWithUArray_copy_(IOSTATE, UArray_asUTF8(DATA(self)), 0);
}

IO_METHOD(IoSeq, asUCS2) {
    /*doc Sequence asUCS2
    Returns a new copy of the receiver converted to UCS2 (fixed character width
    UTF16) encoding.
    */

    return IoSeq_newWithUArray_copy_(IOSTATE, UArray_asUCS2(DATA(self)), 0);
}

IO_METHOD(IoSeq, asUCS4) {
    /*doc Sequence asUCS4
    Returns a new copy of the receiver converted to UCS4 (fixed character width
    UTF32) encoding.
    */

    return IoSeq_newWithUArray_copy_(IOSTATE, UArray_asUCS4(DATA(self)), 0);
}

IO_METHOD(IoSeq, asFixedSizeType) {
    /*doc Sequence asFixedSizeType
    Returns a new sequence with the receiver encoded in the
    minimal fixed width text encoding that its characters can fit
    into (either, ascii, utf8, utf16 or utf32).
    */

    UArray *out = UArray_new();
    UArray_copy_(out, DATA(self));
    UArray_convertToFixedSizeType(out);
    return IoSeq_newWithUArray_copy_(IOSTATE, out, 0);
}

IO_METHOD(IoSeq, asBinaryUnsignedInteger) {
    /*doc Sequence asBinaryUnsignedInteger
    Returns a Number with the bytes of the receiver interpreted as a binary
    unsigned integer. Endian is same as machine.
    */

    const void *bytes = UArray_bytes(DATA(self));
    size_t byteCount = UArray_size(DATA(self));

    if (byteCount == 1) {
        return IONUMBER(*((const uint8_t *)bytes));
    } else if (byteCount == 2) {
        return IONUMBER(*((const uint16_t *)bytes));
    } else if (byteCount == 4) {
        return IONUMBER(*((const uint32_t *)bytes));
    } else {
        IoState_error_(IOSTATE, m,
                       "Sequence is %i bytes but only conversion of 1, 2, or 4 "
                       "bytes is supported",
                       byteCount);
    }

    return IONIL(self);
}

IO_METHOD(IoSeq, asBinarySignedInteger) {
    /*doc Sequence asBinarySignedInteger
    Returns a Number with the bytes of the receiver interpreted as a binary
    signed integer. Endian is same as machine.
    */

    const void *bytes = UArray_bytes(DATA(self));
    size_t byteCount = UArray_size(DATA(self));

    if (byteCount == 1) {
        return IONUMBER(*((const int8_t *)bytes));
    } else if (byteCount == 2) {
        return IONUMBER(*((const int16_t *)bytes));
    } else if (byteCount == 4) {
        return IONUMBER(*((const int32_t *)bytes));
    } else {
        IoState_error_(IOSTATE, m,
                       "Sequence is %i bytes but only conversion of 1, 2, or 4 "
                       "bytes is supported",
                       byteCount);
    }

    return IONIL(self);
}

IO_METHOD(IoSeq, asBinaryNumber) {
    /*doc Sequence asBinaryNumber
    Returns a Number containing the first 8 bytes of the
    receiver without casting them to a double. Endian is same as machine.
    */

    IoNumber *byteCount = IoMessage_locals_valueArgAt_(m, locals, 0);
    size_t max = UArray_size(DATA(self));
    int bc = sizeof(double);
    double d = 0;

    if (!ISNIL(byteCount)) {
        bc = IoNumber_asInt(byteCount);
    }

    if (max < bc) {
        IoState_error_(
            IOSTATE, m,
            "requested first %i bytes, but Sequence only contains %i bytes", bc,
            max);
    }

    memcpy(&d, UArray_bytes(DATA(self)), bc);
    return IONUMBER(d);
}

IO_METHOD(IoSeq, asSymbol) {
    /*doc Sequence asSymbol
    Returns a immutable Sequence (aka Symbol) version of the receiver.
    */

    return IoSeq_rawAsSymbol(self);
}

IO_METHOD(IoSeq, isSymbol) {
    /*doc Sequence isSymbol
    Returns true if the receiver is a
    immutable Sequence (aka, a Symbol) or false otherwise.
    */

    return IOBOOL(self, ISSYMBOL(self));
}

IO_METHOD(IoSeq, isMutable) {
    /*doc Sequence isMutable
    Returns true if the receiver is a mutable Sequence or false otherwise.
    */

    return IOBOOL(self, !ISSYMBOL(self));
}

IO_METHOD(IoSeq, print) {
    /*doc Sequence print
    Prints the receiver as a string. Returns self.
    */

    IoState_justPrintba_(IOSTATE, DATA(self));
    return self;
}

IO_METHOD(IoSeq, linePrint) {
    /*doc Sequence linePrint
    Prints the Sequence and a newline character.
    */

    IoState_justPrintba_(IOSTATE, DATA(self));
    IoState_justPrintln_(IOSTATE);
    return self;
}

//
// Sequence parseJson
//

static IoObject *parse_json_value(IoObject *self, JSON_Value *value);
static IoMap *parse_json_object(IoObject *self, JSON_Object *object);
static IoList *parse_json_array(IoObject *self, JSON_Array *array);

IOVM_API IO_METHOD(IoSeq, parseJson) {
    /*doc Sequence parseJson
     Interprets the Sequence as JSON and returns a Map.
     */
    IoMap *result;
    JSON_Value *output;
    char *value = IoSeq_asCString(self);

    if (IoSeq_rawSizeInBytes(self) == 0)
        IoState_error_(IOSTATE, m, "Can't parse empty string.");

    output = json_parse_string_with_comments(value);

    if (!output)
        IoState_error_(IOSTATE, m, "Can't parse JSON.");

    result = parse_json_object(self, json_object(output));

    json_value_free(output);

    return result;
}

IoMap *parse_json_object(IoObject *self, JSON_Object *object) {
    IoMap *map = IoMap_new(IOSTATE);
    IoObject *value;
    char *key;
    size_t num_of_keys = json_object_get_count(object);
    size_t i;

    for (i = 0; i < num_of_keys; i++) {
        key = (char *)json_object_get_name(object, i);
        value = parse_json_value(self, json_object_get_value(object, key));
        IoMap_rawAtPut(map, IOSYMBOL(key), value);
    }

    return map;
}

IoObject *parse_json_value(IoObject *self, JSON_Value *value) {
    switch (json_type(value)) {
    case JSONError:
        return 0;
    case JSONNull:
        return IOSTATE->ioNil;
    case JSONString:
        return IoSeq_newWithCString_(IOSTATE, json_string(value));
    case JSONNumber:
        return IoNumber_newWithDouble_(IOSTATE, json_value_get_number(value));
    case JSONObject:
        return parse_json_object(self, json_object(value));
    case JSONArray:
        return parse_json_array(self, json_array(value));
    case JSONBoolean:
        return IOBOOL(self, json_boolean(value));
    default:
        return 0;
    }
}

IoList *parse_json_array(IoObject *self, JSON_Array *array) {
    IoList *list = IoList_new(IOSTATE);
    IoObject *value;
    size_t num_of_elements = json_array_get_count(array);
    size_t i;

    for (i = 0; i < num_of_elements; i++) {
        value = parse_json_value(self, json_array_get_value(array, i));
        IoList_rawAppend_(list, value);
    }

    return list;
}

//--------------------------------------

IO_METHOD(IoSeq, isEmpty) {
    /*doc Sequence isEmpty
    Returns true if the size of the receiver is 0, false otherwise.
    */

    return IOBOOL(self, UArray_size(DATA(self)) == 0);
}

IO_METHOD(IoSeq, isZero) {
    /*doc Sequence isZero
    Returns true if all elements are 0, false otherwise.
    */

    return IOBOOL(self, UArray_isZero(DATA(self)));
}

IO_METHOD(IoSeq, size) {
    /*doc Sequence size
    Returns the length in number of items (which may or may not
    be the number of bytes, depending on the item type) of the receiver. For
    example: <p> <pre> "abc" size == 3
    </pre>
    */

    return IONUMBER(UArray_size(DATA(self)));
}

IO_METHOD(IoSeq, sizeInBytes) {
    /*doc Sequence sizeInBytes
    Returns the length in bytes of the receiver.
    */

    return IONUMBER(UArray_sizeInBytes(DATA(self)));
}

IO_METHOD(IoSeq, at) {
    /*doc Sequence at(aNumber)
    Returns a value at the index specified by aNumber.
    Returns nil if the index is out of bounds.
    */

    size_t i = IoMessage_locals_sizetArgAt_(m, locals, 0);
    UArray *a = DATA(self);

    // IOASSERT((i < UArray_size(DATA(self))), "index out of bounds");
    if (i >= UArray_size(DATA(self)))
        return IONIL(self);

    if (UArray_isFloatType(a)) {
        return IONUMBER(UArray_doubleAt_(a, i));
    } else {
        return IONUMBER(UArray_longAt_(a, i));
    }
}

IO_METHOD(IoSeq, exclusiveSlice) {
    /*doc Sequence exclusiveSlice(inclusiveStartIndex, exclusiveEndIndex)
    Returns a new string containing the subset of the
    receiver from the inclusiveStartIndex to the exclusiveEndIndex. The
    exclusiveEndIndex argument is optional. If not given, it is assumed to be
    one beyond the end of the string.
    */

    long fromIndex = IoMessage_locals_longArgAt_(m, locals, 0);
    long last = UArray_size(DATA(self));
    UArray *ba;

    if (IoMessage_argCount(m) > 1) {
        last = IoMessage_locals_longArgAt_(m, locals, 1);
    }

    ba = UArray_slice(DATA(self), fromIndex, last);

    if (ISSYMBOL(self)) {
        return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
    }

    return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
}

IO_METHOD(IoSeq, inclusiveSlice) {
    /*doc Sequence inclusiveSlice(inclusiveStartIndex, inclusiveEndIndex)
    Returns a new string containing the subset of the
    receiver from the inclusiveStartIndex to the inclusiveEndIndex. The
    inclusiveEndIndex argument is optional. If not given, it is assumed to be
    the end of the string.
    */

    long fromIndex = IoMessage_locals_longArgAt_(m, locals, 0);
    long last = UArray_size(DATA(self));
    UArray *ba;

    if (IoMessage_argCount(m) > 1) {
        last = IoMessage_locals_longArgAt_(m, locals, 1);
    }

    if (last == -1) {
        last = UArray_size(DATA(self));
    } else {
        last = last + 1;
    }

    ba = UArray_slice(DATA(self), fromIndex, last);

    if (ISSYMBOL(self)) {
        return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
    }

    return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
}

IO_METHOD(IoSeq, between) {
    /*doc Sequence betweenSeq(aSequence, anotherSequence)
    Returns a new Sequence containing the bytes between the
    occurrence of aSequence and anotherSequence in the receiver.
    If aSequence is empty, this method is equivalent to
    beforeSeq(anotherSequence). If anotherSequence is nil, this method is
    equivalent to afterSeq(aSequence). nil is returned if no match is found.
    */

    long start = 0;
    long end = 0;
    IoSeq *fromSeq, *toSeq;

    fromSeq = (IoSeq *)IoMessage_locals_valueArgAt_(m, locals, 0);

    if (ISSEQ(fromSeq)) {
        if (IoSeq_rawSize(fromSeq) == 0) {
            start = 0;
        } else {
            start = UArray_find_from_(DATA(self), DATA(fromSeq), 0);

            if (start == -1) {
                // start = 0;
                return IONIL(self);
            }
            start += IoSeq_rawSize(fromSeq);
        }
    } else if (ISNIL(fromSeq)) {
        start = 0;
    } else {
        IoState_error_(IOSTATE, m,
                       "Nil or Sequence argument required for arg 0, not a %s",
                       IoObject_name((IoObject *)fromSeq));
    }

    toSeq = (IoSeq *)IoMessage_locals_valueArgAt_(m, locals, 1);

    if (ISSEQ(toSeq)) {
        end = UArray_find_from_(DATA(self), DATA(toSeq), start);
        // if (end == -1) start = UArray_size(DATA(self));
        if (end == -1)
            return IONIL(self);
    } else if (ISNIL(toSeq)) {
        end = UArray_size(DATA(self));
    } else {
        IoState_error_(IOSTATE, m,
                       "Nil or Sequence argument required for arg 1, not a %s",
                       IoObject_name((IoObject *)toSeq));
    }

    {
        UArray *ba = UArray_slice(DATA(self), start, end);
        IoSeq *result = IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
        return result;
    }
}

// find ----------------------------------------------------------

IO_METHOD(IoSeq, findSeqs) {
    /*doc Sequence findSeqs(listOfSequences, optionalStartIndex)
    Returns an object with two slots - an \"index\" slot which contains
    the first occurrence of any of the sequences in listOfSequences found
    in the receiver after the startIndex, and a \"match\" slot, which
    contains a reference to the matching sequence from listOfSequences.
    If no startIndex is specified, the search starts at index 0.
    nil is returned if no occurrences are found.
    */

    IoList *others = IoMessage_locals_listArgAt_(m, locals, 0);
    List *delims = IoList_rawList(others);
    long f = 0;
    long firstIndex = -1;
    size_t match = 0;

    if (IoMessage_argCount(m) > 1) {
        f = IoMessage_locals_longArgAt_(m, locals, 1);
    }

    {
        size_t index;

        LIST_FOREACH(
            delims, i, s,
            if (!ISSEQ((IoSeq *)s)) {
                IoState_error_(IOSTATE, m,
                               "requires Sequences as arguments, not %ss",
                               IoObject_name((IoSeq *)s));
            }

            index = UArray_find_from_(DATA(self), DATA(((IoSeq *)s)), f);

            if (index != -1 && (firstIndex == -1 || index < firstIndex)) {
                firstIndex = (long)index;
                match = i;
            });
    }

    if (firstIndex == -1) {
        return IONIL(self);
    } else {
        IoObject *result = IoObject_new(IOSTATE);
        IoObject_setSlot_to_(result, IOSYMBOL("index"), IONUMBER(firstIndex));
        IoObject_setSlot_to_(result, IOSYMBOL("match"),
                             (IoObject *)List_at_(delims, match));
        return result;
    }
}

IO_METHOD(IoSeq, findSeq) {
    /*doc Sequence findSeq(aSequence, optionalStartIndex)
    Returns a number with the first occurrence of aSequence in
    the receiver after the startIndex. If no startIndex is specified,
    the search starts at index 0.
    nil is returned if no occurrences are found.
    */

    IoSeq *otherSequence = IoMessage_locals_seqArgAt_(m, locals, 0);
    long f = 0;
    long index;

    if (IoMessage_argCount(m) > 1) {
        f = IoMessage_locals_longArgAt_(m, locals, 1);
    }

    index = UArray_find_from_(DATA(self), DATA(otherSequence), f);

    return (index == -1) ? IONIL(self) : IONUMBER(index);
}

IO_METHOD(IoSeq, reverseFindSeq) {
    /*doc Sequence reverseFindSeq(aSequence, startIndex)
    Returns a number with the first occurrence of aSequence in
    the receiver before the startIndex. The startIndex argument is optional.
    By default reverseFind starts at the end of the string. Nil is
    returned if no occurrences are found.
    */

    IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
    long from = UArray_size(DATA(self));
    long index;

    if (IoMessage_argCount(m) > 1) {
        from = IoMessage_locals_intArgAt_(m, locals, 1);
    }

    index = UArray_rFind_from_(DATA(self), DATA(other), from);

    if (index == -1) {
        return IONIL(self);
    }

    return IONUMBER((double)index);
}

IO_METHOD(IoSeq, beginsWithSeq) {
    /*doc Sequence beginsWithSeq(aSequence)
    Returns true if the receiver begins with aSequence, false otherwise.
    */

    IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);

    return IOBOOL(self, UArray_beginsWith_(DATA(self), DATA(other)));
}

IO_METHOD(IoSeq, endsWithSeq) {
    /*doc Sequence endsWithSeq(aSequence)
    Returns true if the receiver ends with aSequence, false otherwise.
    */

    IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
    return IOBOOL(self, UArray_endsWith_(DATA(self), DATA(other)));
}

IO_METHOD(IoSeq, contains) {
    /*doc Sequence contains(aNumber)
    Returns true if the receiver contains an element equal in value to aNumber,
    false otherwise.
    */

    // will make this more efficient when Numbers are Arrays

    IoNumber *n = IoMessage_locals_numberArgAt_(m, locals, 0);

    UArray tmp = IoNumber_asStackUArray(n);
    return IOBOOL(self, UArray_contains_(DATA(self), &tmp));
}

IO_METHOD(IoSeq, containsSeq) {
    /*doc Sequence containsSeq(aSequence)
    Returns true if the receiver contains the substring
    aSequence, false otherwise.
    */

    IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);

    return IOBOOL(self, UArray_contains_(DATA(self), DATA(other)));
}

IO_METHOD(IoSeq, containsAnyCaseSeq) {
    /*doc Sequence containsAnyCaseSeq(aSequence)
    Returns true if the receiver contains the aSequence
    regardless of casing, false otherwise.
    */

    IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
    return IOBOOL(self, UArray_containsAnyCase_(DATA(self), DATA(other)));
}

IO_METHOD(IoSeq, isLowercase) {
    /*doc Sequence isLowercase
    Returns self if all the characters in the string are lower case.
    */

    return IOBOOL(self, UArray_isLowercase(DATA(self)));
}

IO_METHOD(IoSeq, isUppercase) {
    /*doc Sequence isUppercase
    Returns self if all the characters in the string are upper case.
    */

    return IOBOOL(self, UArray_isUppercase(DATA(self)));
}

IO_METHOD(IoSeq, isEqualAnyCase) {
    /*doc Sequence isEqualAnyCase(aSequence)
    Returns true if aSequence is equal to the receiver
    ignoring case differences, false otherwise.
    */

    IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);

    return IOBOOL(self, UArray_equalsAnyCase_(DATA(self), DATA(other)));
}

IO_METHOD(IoSeq, asNumber) {
    /*doc Sequence asNumber
    Returns the receiver converted to a number.
    Initial whitespace is ignored.
    */

    size_t size = UArray_size(DATA(self));
    char *s = (char *)UArray_bytes(DATA(self));
    char *endp;
    double d = strtod(s, &endp);

    if (size > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        return IONUMBER(IoSeq_rawAsDoubleFromHex(self));
    }

    if (errno == ERANGE || endp == s) {
        return IONUMBER(NAN);
    }

    return IONUMBER(d);
}

IO_METHOD(IoSeq, asList) {
    /*doc Sequence asList
    Returns the receiver converted to a List containing all elements of the
    Sequence.
    */

    List *list = List_new();

    size_t i;

    for (i = 0; i < UArray_size(DATA(self)); i++) {
        if (UArray_isFloatType(DATA(self))) {
            List_append_(list, IONUMBER(UArray_doubleAt_(DATA(self), i)));
        } else {
            long c = UArray_longAt_(DATA(self), i);
            List_append_(list, IOSYMBOL(&c));
        }
    }

    return IoList_newWithList_(IOSTATE, list);
}

IoList *IoSeq_whiteSpaceStrings(IoSeq *self, IoObject *locals, IoMessage *m) {
    /*doc Sequence whiteSpaceStrings
    Returns a List of strings. Each string contains a different
    whitespace character.
    */

    IoList *strings = IoList_new(IOSTATE);
    IoList_rawAppend_(strings, IOSYMBOL(" "));
    IoList_rawAppend_(strings, IOSYMBOL("\t"));
    IoList_rawAppend_(strings, IOSYMBOL("\n"));
    IoList_rawAppend_(strings, IOSYMBOL("\r"));
    return strings;
}

// split ---------------------------------------------------------------------

// this method is only used from split
IoList *IoSeq_stringListForArgs(IoSeq *self, IoObject *locals, IoMessage *m) {
    if (IoMessage_argCount(m) == 0) {
        return IoSeq_whiteSpaceStrings(self, locals, m);
    }

    return IoMessage_evaluatedArgs(m, locals, m);
}

// this method is only used from split
List *IoSeq_byteArrayListForSeqList(IoSeq *self, IoObject *locals, IoMessage *m,
                                    IoList *seqs) {
    List *args = IoList_rawList(seqs);
    List *list = List_new();

    LIST_FOREACH(
        args, i, s,
        if (!ISSEQ((IoSeq *)s)) {
            List_free(list);
            IoState_error_(IOSTATE, m,
                           "requires Sequences as arguments, not %ss",
                           IoObject_name((IoSeq *)s));
        }

        List_append_(list, DATA(((IoSeq *)s))););

    return list;
}

IoObject *IoSeq_splitToFunction(IoSeq *self, IoObject *locals, IoMessage *m,
                                IoSplitFunction *func) {
    IoList *output = IoList_new(IOSTATE);
    List *others = IoSeq_byteArrayListForSeqList(
        self, locals, m, IoSeq_stringListForArgs(self, locals, m));
    int i;

    for (i = 0; i < List_size(others); i++) {
        if (UArray_size(List_at_(others, i)) == 0) {
            IoState_error_(IOSTATE, m, "empty string argument");
        }
    }

    {
        UArray othersArray = List_asStackAllocatedUArray(others);
        UArray *results = UArray_split_(DATA(self), &othersArray);

        for (i = 0; i < UArray_size(results); i++) {
            UArray *ba = UArray_pointerAt_(results, i);
            IoObject *item = (*func)(IOSTATE, ba, 0);
            IoList_rawAppend_(output, item);
        }

        UArray_free(results);
    }

    List_free(others);
    return output;
}

IO_METHOD(IoSeq, split) {
    /*doc Sequence split(optionalArg1, optionalArg2, ...)
    Returns a list containing the sub-sequences of the receiver divided by the
    given arguments. If no arguments are given the sequence is split on white
    space. Examples: <pre> "a b c d" split == list("a", "b", "c", "d") "a*b*c*d"
    split("*") == list("a", "b", "c", "d") "a*b|c,d" split("*", "|", ",") ==
    list("a", "b", "c", "d") "a   b  c d" split == list("a", "", "", "", "b",
    "", "", "c", "", "d")
    </pre>
    */

    return IoSeq_splitToFunction(self, locals, m, IoSeq_newWithUArray_copy_);
}

IO_METHOD(IoSeq, splitAt) {
    /*doc Sequence splitAt(indexNumber)
    Returns a list containing the two parts of the receiver as split at the
    given index.
    */

    size_t index = IoMessage_locals_intArgAt_(m, locals, 0);
    IoList *splitSeqs = IoList_new(IOSTATE);
    index = UArray_wrapPos_(DATA(self), index);

    {
        const char *s = UArray_asCString(DATA(self));
        IoSeq *s1 = IoState_symbolWithCString_length_(IOSTATE, s, index);
        IoSeq *s2 = IoState_symbolWithCString_(IOSTATE, s + index);
        IoList_rawAppend_(splitSeqs, (IoObject *)s1);
        IoList_rawAppend_(splitSeqs, (IoObject *)s2);
    }

    return splitSeqs;
}

/* --- base -------------------------------------------------------------- */

IO_METHOD(IoSeq, fromBase) {
    /*doc Sequence fromBase(aNumber)
    Returns a number with a base 10 representation of the receiver
    converted from the specified base. Only base 2 through 32 are currently
    supported.
    */

    int base = IoMessage_locals_intArgAt_(m, locals, 0);
    char *s = CSTRING(self);
    unsigned long r;
    char *tail;
    errno = 0;
    r = strtoul(s, &tail, base);

    if (errno == EINVAL) {
        errno = 0;
        IoState_error_(IOSTATE, m, "conversion from base %i not supported",
                       base);
    } else if (errno == ERANGE) {
        errno = 0;
        IoState_error_(IOSTATE, m, "resulting value \"%s\" was out of range",
                       s);
    } else if (*s == 0 || *tail != 0 || errno != 0) {
        errno = 0;
        IoState_error_(IOSTATE, m, "conversion of \"%s\" to base %i failed", s,
                       base);
    }

    return IONUMBER(r);
}

IO_METHOD(IoSeq, toBase) {
    /*doc Sequence toBase(aNumber)
    Returns a Sequence containing the receiver (which is
    assumed to be a base 10 number) converted to the specified base.
    Only base 8 and 16 are currently supported.
    */

    const char *const table = "0123456789abcdefghijklmnopqrstuvwxyz";
    int base = IoMessage_locals_intArgAt_(m, locals, 0);
    char buf[64], *ptr = buf + 64;
    unsigned long n = 0;

    if (base < 2 || base > 36) {
        IoState_error_(IOSTATE, m, "conversion to base %i not supported", base);
    }

#if defined(_MSC_VER) || defined(__MINGW32__)
    n = strtoul(IoSeq_asCString(self), NULL, 0);
#else
    n = (unsigned long)IoSeq_asDouble(self);
#endif

    /* Build the converted string backwards. */
    *(--ptr) = '\0';

    if (n == 0) {
        *(--ptr) = '0';
    } else {
        do {
            *(--ptr) = table[n % base];
        } while ((n /= base) != 0);
    }

    return IoSeq_newWithCString_(IOSTATE, ptr);
}

// this function is only called by IoSeq_foreach()
IO_METHOD(IoSeq, each) {
    IoState *state = IOSTATE;
    IoObject *result = IONIL(self);
    IoMessage *doMessage = IoMessage_rawArgAt_(m, 0);
    size_t i;

    IoState_pushRetainPool(state);

    for (i = 0; i < UArray_size(DATA(self)); i++) {
        IoState_clearTopPool(IOSTATE);

        if (UArray_isFloatType(DATA(self))) {
            result = IoMessage_locals_performOn_(
                doMessage, locals, IONUMBER(UArray_doubleAt_(DATA(self), i)));
        } else {
            result = IoMessage_locals_performOn_(
                doMessage, locals, IONUMBER(UArray_longAt_(DATA(self), i)));
        }

        if (IoState_handleStatus(IOSTATE)) {
            goto done;
        }
    }

done:
    IoState_popRetainPoolExceptFor_(state, result);
    return result;
}

IO_METHOD(IoSeq, foreach) {
    /*doc Sequence foreach(optionalIndex, value, message)
    For each element, set index to the index of the
    element and value to the element value and execute message.
    Example:
    <pre>
    aSequence foreach(i, v, writeln("value at index ", i, " is ", v))
    aSequence foreach(v, writeln("value ", v))
    </pre>
    */

    IoObject *result = IONIL(self);
    IoMessage *doMessage;

    IoSymbol *indexSlotName;
    IoSymbol *characterSlotName;

    size_t i;

    if (IoMessage_argCount(m) == 1) {
        return IoSeq_each(self, locals, m);
    }

    IoMessage_foreachArgs(m, self, &indexSlotName, &characterSlotName,
                          &doMessage);

    IoState_pushRetainPool(IOSTATE);

    for (i = 0; i < UArray_size(DATA(self)); i++) {
        IoState_clearTopPool(IOSTATE);

        if (indexSlotName) {
            IoObject_setSlot_to_(locals, indexSlotName, IONUMBER(i));
        }

        if (UArray_isFloatType(DATA(self))) {
            IoObject_setSlot_to_(locals, characterSlotName,
                                 IONUMBER(UArray_doubleAt_(DATA(self), i)));
        } else {
            IoObject_setSlot_to_(locals, characterSlotName,
                                 IONUMBER(UArray_longAt_(DATA(self), i)));
        }
        result = IoMessage_locals_performOn_(doMessage, locals, locals);

        if (IoState_handleStatus(IOSTATE)) {
            goto done;
        }
    }
done:
    IoState_popRetainPoolExceptFor_(IOSTATE, result);
    return result;
}

IO_METHOD(IoSeq, asMessage) {
    /*doc Sequence asMessage(optionalLabel)
    Returns the compiled message object for the string.
    */

    IoSymbol *label;

    if (IoMessage_argCount(m) >= 1)
        label = IoMessage_locals_symbolArgAt_(m, locals, 0);
    else
        label = IOSYMBOL("[asMessage]");

    return IoMessage_newFromText_labelSymbol_(IOSTATE, CSTRING(self), label);
}

/*doc Sequence ..(aSequence)
        Returns a copy of the receiver with aSequence appended to it.
*/

IO_METHOD(IoSeq, cloneAppendSeq) {
    IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
    UArray *ba;

    if (ISNUMBER(other)) {
        other = IoNumber_justAsString((IoNumber *)other, (IoObject *)locals, m);
    }

    if (!ISSEQ(other)) {
        IoState_error_(
            IOSTATE, m,
            "argument 0 to method '%s' must be a number or string, not a '%s'",
            CSTRING(IoMessage_name(m)), IoObject_name(other));
    }

    if (UArray_size(DATA(other)) == 0) {
        return self;
    }

    ba = UArray_clone(DATA(self));
    UArray_append_(ba, DATA(other));
    return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
}

IO_METHOD(IoSeq, asMutable) {
    /*doc Sequence asMutable
    Returns a mutable copy of the receiver.
    */

    return IoSeq_rawMutableCopy(self);
}

/* --- case ------------------------------------------------ */

IO_METHOD(IoSeq, asUppercase) {
    /*doc Sequence asUppercase
    Returns a symbol containing the reveiver made uppercase.
    */

    UArray *ba = UArray_clone(DATA(self));
    UArray_toupper(ba);
    return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
}

IO_METHOD(IoSeq, asLowercase) {
    /*doc Sequence asLowercase
    Returns a symbol containing the reveiver made lowercase.
    */

    UArray *ba = UArray_clone(DATA(self));
    UArray_tolower(ba);
    return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
}

/* --- path ------------------------------------------------ */

IO_METHOD(IoSeq, lastPathComponent) {
    /*doc Sequence lastPathComponent
    Returns a string containing the receiver clipped up
    to the last path separator.
    */

    UArray *ba = UArray_lastPathComponent(DATA(self));
    return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
}

IO_METHOD(IoSeq, pathExtension) {
    /*doc Sequence pathExtension
    Returns a string containing the receiver clipped up to the last period.
    */

    UArray *path = UArray_pathExtension(DATA(self));
    return IoState_symbolWithUArray_copy_(IOSTATE, path, 0);
}

IO_METHOD(IoSeq, fileName) {
    /*doc Sequence fileName
    Returns the last path component sans the path extension.
*/

    UArray *path = UArray_fileName(DATA(self));
    return IoState_symbolWithUArray_copy_(IOSTATE, path, 0);
}

IO_METHOD(IoSeq, cloneAppendPath) {
    /*doc Sequence cloneAppendPath(aSequence)
    Appends argument to a copy the receiver such that there is one
    and only one path separator between the two and returns the result.
    */

    IoSeq *component = IoMessage_locals_seqArgAt_(m, locals, 0);
    UArray *ba = UArray_clone(DATA(self));
    UArray_appendPath_(ba, DATA(component));
    return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
}

IO_METHOD(IoSeq, pathComponent) {
    /*doc Sequence pathComponent
    Returns a slice of the receiver before the last path separator as a symbol.
    */

    UArray *ba = UArray_clone(DATA(self));
    UArray_removeLastPathComponent(ba);
    return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
}

IO_METHOD(IoSeq, asOSPath) {
    // doc Sequence asOSPath Returns a OS style path for an Io style path.
    return IoSeq_newSymbolWithUArray_copy_(
        IOSTATE, UArray_asOSPath(IoSeq_rawUArray(self)), 0);
}

IO_METHOD(IoSeq, asIoPath) {
    // doc Sequence asIoPath Returns a Io style path for an OS style path.
    return IoSeq_newSymbolWithUArray_copy_(
        IOSTATE, UArray_asUnixPath(IoSeq_rawUArray(self)), 0);
}

// occurrences

IO_METHOD(IoSeq, beforeSeq) {
    /*doc Sequence beforeSeq(aSequence)
    Returns the slice of the receiver (as a Symbol) before
    aSequence or self if aSequence is not found.
    */

    IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
    long pos = UArray_find_(DATA(self), DATA(other));

    if (pos != -1) {
        UArray *ba = UArray_slice(DATA(self), 0, pos);

        if (ISSYMBOL(self)) {
            return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
        } else {
            return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
        }
    }

    if (ISSYMBOL(self)) {
        return self;
    }

    return IOCLONE(self);
}

IO_METHOD(IoSeq, afterSeq) {
    /*doc Sequence afterSeq(aSequence)
    Returns the slice of the receiver (as a Symbol) after aSequence or
    nil if aSequence is not found. If aSequence is empty, the receiver
    (or a copy of the receiver if it is mutable) is returned.
    */

    IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
    long pos = UArray_find_(DATA(self), DATA(other));

    if (pos != -1) {
        UArray *ba = UArray_slice(DATA(self), pos + UArray_size(DATA(other)),
                                  UArray_size(DATA(self)));

        if (ISSYMBOL(self)) {
            return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
        } else {
            return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
        }
    }

    return IONIL(self);
}

IO_METHOD(IoSeq, asCapitalized) {
    /*doc Sequence asCapitalized
    Returns a copy of the receiver with the first charater made uppercase.
    */

    /* need to fix for multi-byte characters */

    int firstChar = (int)UArray_firstLong(DATA(self));
    int upperChar = toupper(firstChar);

    if (ISSYMBOL(self) && (firstChar == upperChar)) {
        return self;
    } else {
        UArray *ba = UArray_clone(DATA(self));
        UArray_at_putLong_(ba, 0, upperChar);

        if (ISSYMBOL(self)) {
            return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
        }

        return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
    }
}

IO_METHOD(IoSeq, occurrencesOfSeq) {
    /*doc Sequence occurrencesOfSeq(aSeq)
    Returns count of aSeq in the receiver.
    */

    IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
    size_t count = UArray_count_(DATA(self), DATA(other));
    return IONUMBER(count);
}

IO_METHOD(IoSeq, asBase64) {
    /*doc Sequence asBase64(optionalCharactersPerLine)
    Returns an immutable, base64 encoded (according to RFC 1421) version of
    self. optionalCharactersPerLine describes the number of characters between
    line breaks and defaults to 0.
    */
    int charsPerLine = 0;

    if (IoMessage_argCount(m) > 0) {
        charsPerLine = IoMessage_locals_intArgAt_(m, locals, 0);
    }

    return IoSeq_newWithUArray_copy_(
        IOSTATE, UArray_asBase64(IoSeq_rawUArray(self), charsPerLine), 0);
}

IO_METHOD(IoSeq, fromBase64) {
    /*doc Sequence fromBase64
    Returns an immutable, base64 decoded (according to RFC 1421) version of
    self.
    */

    return IoSeq_newWithUArray_copy_(
        IOSTATE, UArray_fromBase64(IoSeq_rawUArray(self)), 0);
}

IO_METHOD(IoSeq, interpolate) {
    /*doc Sequence interpolate(ctx)
    Returns immutable copy of self with interpolateInPlace(ctx) passed to the
    copy.
    */

    IoSeq *s = IoSeq_newWithUArray_copy_(IOSTATE, IoSeq_rawUArray(self), 1);
    IoSeq_interpolateInPlace(s, locals, m);
    return IoSeq_rawAsSymbol(s);
}

IO_METHOD(IoSeq, distanceTo) {
    /*doc Sequence distanceTo(aSeq)
    Returns a number with the square root of the sum of the square
    of the differences of the items between the sequences.
    */

    IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
    double d;

    d = UArray_distanceTo_(DATA(self), DATA(other));
    return IONUMBER(d);
}

IO_METHOD(IoSeq, greaterThan_) {
    /*doc Sequence greaterThan(aSeq)
    Returns true if the receiver is greater than aSeq, false otherwise.
    */

    IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
    return IOBOOL(self, UArray_greaterThan_(DATA(self), DATA(other)));
}

IO_METHOD(IoSeq, lessThan_) {
    /*doc Sequence lessThan(aSeq)
    Returns true if the receiver is less than aSeq, false otherwise.
    */

    IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
    return IOBOOL(self, UArray_lessThan_(DATA(self), DATA(other)));
}

IO_METHOD(IoSeq, greaterThanOrEqualTo_) {
    /*doc Sequence greaterThanOrEqualTo(aSeq)
    Returns true if the receiver is greater than or equal to aSeq, false
    otherwise.
    */

    IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
    return IOBOOL(self, UArray_greaterThanOrEqualTo_(DATA(self), DATA(other)));
}

IO_METHOD(IoSeq, lessThanOrEqualTo_) {
    /*doc Sequence lessThanOrEqualTo(aSeq)
    Returns true if the receiver is less than or equal to aSeq, false otherwise.
    */

    IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
    return IOBOOL(self, UArray_lessThanOrEqualTo_(DATA(self), DATA(other)));
}

// -----------------------------------------------------------

#define ASSTRUCT(type)                                                         \
    if (!strcmp(mt, #type)) {                                                  \
        int typeSize = sizeof(type##_t);                                       \
        IOASSERT(offset + typeSize <= size, "not enough data for struct");     \
        v = IONUMBER(*(type##_t *)(data + offset));                            \
        offset += typeSize;                                                    \
    }

IO_METHOD(IoSeq, asStruct) {
    /*doc Sequence asStruct(memberList)
    For a sequence that contains the data for a raw memory data structure (as
    used in C), this method can be used to extract its members into an Object.
    The memberList argument specifies the layout of the datastructure. Its form
    is: <p> list(memberType1, memberName1, memberType2, memberName2, ...) <p>
    Member types include:
    <pre>
    int8, int16, int32, int64
    uint8, uint16, uint32, uint64
    float32, float64
    </pre>
    Example:
    <pre>
    pointObject := structPointSeq asStruct(list("float32", "x", "float32", "y"))
    </pre>
    The output pointObject would contain x and y slots with Number objects.
    */

    IoObject *st = IoObject_new(IOSTATE);
    const unsigned char *data = UArray_bytes(DATA(self));
    size_t size = UArray_sizeInBytes(DATA(self));
    size_t offset = 0;
    List *members = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
    int memberIndex;

    IOASSERT(List_size(members) % 2 == 0, "members list must be even number");

    for (memberIndex = 0; memberIndex < List_size(members) / 2 && offset < size;
         memberIndex++) {
        IoSeq *memberType = List_at_(members, memberIndex * 2);
        IoSeq *memberName = List_at_(members, memberIndex * 2 + 1);
        char *mt;
        IoObject *v = NULL;

        IOASSERT(ISSEQ(memberType), "memberTypes must be strings");
        IOASSERT(ISSEQ(memberName), "memberNames must be strings");

        mt = CSTRING(memberType);

        ASSTRUCT(int8);
        ASSTRUCT(uint8);
        ASSTRUCT(int16);
        ASSTRUCT(uint16);
        ASSTRUCT(int32);
        ASSTRUCT(uint32);
        ASSTRUCT(int64);
        ASSTRUCT(uint64);
        ASSTRUCT(float32);
        ASSTRUCT(float64);

        IoObject_setSlot_to_(st, memberName, v);
    }

    return st;
}

#define WITHSTRUCT(type)                                                       \
    if (!strcmp(mt, #type)) {                                                  \
        int typeSize = sizeof(type##_t);                                       \
        *(type##_t *)(data + offset) = CNUMBER(memberValue);                   \
        offset += typeSize;                                                    \
        continue;                                                              \
    }

IO_METHOD(IoSeq, withStruct) {
    /*doc Sequence withStruct(memberList)
    This method is useful for producing a Sequence containing a raw
    datastructure with the specified types and values. The memberList format is:
    <p>
    list(memberType1, memberName1, memberType2, memberName2, ...)
    <p>
    Member types include:
    <pre>
    int8, int16, int32, int64
    uint8, uint16, uint32, uint64
    float32, float64
    </pre>
    Example:
    <pre>
    pointStructSeq := Sequence withStruct(list("float32", 1.2, "float32", 3.5))
    </pre>
    The output pointStructSeq would contain 2 raw 32 bit floats.
    */

    List *members = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
    int memberIndex;
    size_t maxSize = List_size(members) * 8;
    IoSeq *s = IoSeq_newWithData_length_(
        IOSTATE, (unsigned char *)malloc(maxSize), maxSize);
    unsigned char *data = IoSeq_rawBytes(s);
    size_t offset = 0;

    IOASSERT(List_size(members) % 2 == 0, "members list must be even number");

    for (memberIndex = 0;
         memberIndex < List_size(members) / 2 && offset < maxSize;
         memberIndex++) {
        IoSeq *memberType = List_at_(members, memberIndex * 2);
        IoSeq *memberValue = List_at_(members, memberIndex * 2 + 1);
        char *mt;

        IOASSERT(ISSEQ(memberType), "memberTypes must be strings");
        IOASSERT(ISNUMBER(memberValue), "memberValues must be strings");

        mt = CSTRING(memberType);

        WITHSTRUCT(int8);
        WITHSTRUCT(uint8);
        WITHSTRUCT(int16);
        WITHSTRUCT(uint16);
        WITHSTRUCT(int32);
        WITHSTRUCT(uint32);
        WITHSTRUCT(int64);
        WITHSTRUCT(uint64);
        WITHSTRUCT(float32);
        WITHSTRUCT(float64);
    }

    IoSeq_rawSetSize_(s, offset);

    return s;
}

// ------------------

/* Converts a hex character to its integer value */
static char from_hex(char ch) {
    return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
static char to_hex(char code) {
    static char hex[] = "0123456789ABCDEF";
    return hex[code & 15];
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
static char *url_encode(const char *str, int isPercentEncoded) {
    const char *pstr = str;
    char *buf = (char *)malloc(strlen(str) * 3 + 1);
    char *pbuf = buf;

    while (*pstr) {
        if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' ||
            *pstr == '~') {
            *pbuf++ = *pstr;
        } else if (!isPercentEncoded && *pstr == ' ') {
            *pbuf++ = '+';
        } else {
            *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4),
            *pbuf++ = to_hex(*pstr & 15);
        }

        pstr++;
    }

    *pbuf = '\0';
    return buf;
}

/* Returns a url-decoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
static char *url_decode(const char *str, int isPercentEncoded) {
    const char *pstr = str;
    char *buf = (char *)malloc(strlen(str) + 1);
    char *pbuf = buf;

    while (*pstr) {
        if (*pstr == '%') {
            if (pstr[1] && pstr[2]) {
                *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
                pstr += 2;
            }
        } else if (!isPercentEncoded && *pstr == '+') {
            *pbuf++ = ' ';
        } else {
            *pbuf++ = *pstr;
        }
        pstr++;
    }

    *pbuf = '\0';
    return buf;
}

IO_METHOD(IoSeq, percentEncoded) {
    /*doc Sequence percentEncoded
    Returns percent encoded version of receiver.
    */
    char *s = url_encode((const char *)UArray_bytes(DATA(self)), 1);
    IoObject *result = IOSYMBOL(s);
    free(s);
    return result;
}

IO_METHOD(IoSeq, percentDecoded) {
    /*doc Sequence percentDecoded
    Returns percent decoded version of receiver.
    */
    char *s = url_decode((const char *)UArray_bytes(DATA(self)), 1);
    IoObject *result = IOSYMBOL(s);
    free(s);
    return result;
}

// ----------------------------

IO_METHOD(IoSeq, urlEncoded) {
    /*doc Sequence urlEncoded
    Returns url encoded version of receiver.
    */
    char *s = url_encode((const char *)UArray_bytes(DATA(self)), 0);
    IoObject *result = IOSYMBOL(s);
    free(s);
    return result;
}

IO_METHOD(IoSeq, urlDecoded) {
    /*doc Sequence urlDecoded
    Returns url decoded version of receiver.
    */
    char *s = url_decode((const char *)UArray_bytes(DATA(self)), 0);
    IoObject *result = IOSYMBOL(s);
    free(s);
    return result;
}

// -------------------------

IO_METHOD(IoSeq, pack) {
    /*doc Sequence pack(format, value1, ...)

    Returns a new Sequence with the values packed in.

    Codes:

    *: (one at the beginning of the format string) declare format string as
    BigEndian B: unsigned byte b: byte C: unsigned char c: char H: unsigned
    short h: short I: unsigned int i: int L: unsigned long l: long f: float F:
    double s: string

    A '*' at the begging of the format string indicates native types are to be
    treated as Big Endiand.

    A number preceding a code declares an array of that type.

    In the case of 's', the preceding number indicates the size of the string to
    be packed. If the string passed is shorter than size, 0 padding will be used
    to fill to size. If the string passed is longer than size, only size chars
    will be packed.

    The difference between b/B and c/C is in the values passed to pack. For b/B
    pack expects a number. For c/C pack expects a one-char-string (this is the
    same as '1s' or 's')

    Examples:

    s := Sequence pack("IC5s", 100, "a", "hello")
    s := Sequence pack("5c", "h", "e", "l", "l", "o")
    s := Sequence pack("I", 0x01020304)
    s := Sequence pack("*I", 0x01020304)

    */

    char *strFmt = IoMessage_locals_cStringArgAt_(m, locals, 0);
    size_t strFmtLen = strlen(strFmt);
    int argCount = IoMessage_argCount(m);
    int isBigEndian = 0;
    int doBigEndian = 0;
    size_t i = 0;
    int argIdx = 0;
    size_t count = 0;

    char *from = NULL;
    size_t size = 0;
    size_t padding = 0;
    char val[16];
    UArray *ua = UArray_new();

    memset(val, 0x0, 16);

    UArray_setItemType_(ua, CTYPE_uint8_t);
    UArray_setEncoding_(ua, CENCODING_NUMBER);

    if (strFmt[0] == '*')
        i = doBigEndian = isBigEndian = 1;

    for (argIdx = 1; i < strFmtLen && argIdx < argCount; i++) {
        if (isdigit(strFmt[i])) {
            count = (count * 10) + (strFmt[i] - 0x30);
            continue;
        }

        count = count > 1 ? count : 1;

        doBigEndian = isBigEndian;

        for (; count > 0; count--, argIdx++) {
            from = val;
            padding = 0;
            size = 0;

            switch (strFmt[i]) {
            case 'B': // unsigned byte
            case 'b': // byte
                val[0] = IoMessage_locals_intArgAt_(m, locals, argIdx);
                size = sizeof(char);
                break;

            case 'C': // unsigned char
            case 'c': // char
                val[0] = IoMessage_locals_cStringArgAt_(m, locals, argIdx)[0];
                size = sizeof(char);
                break;

            case 'H': // unsigned short
            case 'h': // short
                *((short *)val) =
                    (short)IoMessage_locals_intArgAt_(m, locals, argIdx);
                size = sizeof(short);
                break;

            case 'I': // unsigned int
            case 'i': // int
                *((int *)val) = IoMessage_locals_intArgAt_(m, locals, argIdx);
                size = sizeof(int);
                break;

            case 'L': // unsigned long
            case 'l': // long
                *((long *)val) = IoMessage_locals_intArgAt_(m, locals, argIdx);
                size = sizeof(long);
                break;

            case 'f': // float
                *((float *)val) =
                    IoMessage_locals_floatArgAt_(m, locals, argIdx);
                size = sizeof(float);
                break;

            case 'F': // double
                *((double *)val) =
                    IoMessage_locals_doubleArgAt_(m, locals, argIdx);
                size = sizeof(double);
                break;

            case 's': // string
                from = IoMessage_locals_cStringArgAt_(m, locals, argIdx);
                size = strlen(from);
                if (count > size)
                    padding = count - size;
                else
                    size = count;
                doBigEndian = 0;
                count = 1; // finish processing
                break;
            }

            {
                long inc = doBigEndian ? -1 : 1;
                long pos = doBigEndian ? size - 1 : 0;
                int j = 0;

                for (j = 0; j < size; j++, pos += inc) {
                    UArray_appendLong_(ua, from[pos]);
                }

                for (j = 0; j < padding; j++) {
                    UArray_appendLong_(ua, 0);
                }
            }
        }
    }

    return IoSeq_newWithUArray_copy_(IOSTATE, ua, 0);
}

/*#define SEQ_UNPACK_VALUE_ASSIGN_LOOP(code, type, dest, toObj) \
case code: \
{ \
        int inc = isBigEndian ? -1 : 1; \
        int pos = isBigEndian ? seqPos + sizeof(type) - 1 : seqPos; \
        int j; \
 \
        for(j = 0 ; j < sizeof(type) ; j ++, pos += inc) \
                dest[j] = UArray_longAt_(selfUArray, pos); \
 \
        toObj = IONUMBER(*((type *)dest)); \
        seqPos += sizeof(type); \
} \
break;*/

#define SEQ_UNPACK_VALUE_ASSIGN_LOOP(code, type, dest, toObj)                  \
    case code: {                                                               \
        size_t inc = isBigEndian ? -1 : 1;                                     \
        size_t pos = isBigEndian ? seqPos + sizeof(type) - 1 : seqPos;         \
        size_t j;                                                              \
                                                                               \
        for (j = 0; j < sizeof(type); j++, pos += inc)                         \
            dest[j] = selfUArray[pos];                                         \
                                                                               \
        toObj = IONUMBER(*((type *)dest));                                     \
        seqPos += sizeof(type);                                                \
    } break;

IO_METHOD(IoSeq, unpack) {
    /*doc Sequence unpack(optionalStartPosition, format)

    Unpacks self into a list using the format passed in. See Sequence pack.

    Returns a List.

    Examples:

    s := Sequence pack("IC5s", 100, "a", "hello")
    l := s unpack("IC5s")

    s := Sequence pack("5c", "h", "e", "l", "l", "o")
    l := s unpack("5c")

    s := Sequence pack("I", 0x01020304)
    l := s unpack("I")

    s := Sequence pack("*I", 0x01020304)
    l := s unpack("*I")

    l := "hello" unpack("5c")
    */

    char *strFmt = NULL;
    size_t strFmtLen = 0;
    int isBigEndian = 0;
    size_t i = 0;
    size_t count = 0;
    size_t seqPos = 0;
    char val[16];

    IoList *values = IoList_new(IOSTATE);
    // UArray *selfUArray = DATA(self);
    char *selfUArray = (char *)DATA(self)->data;
    size_t selfUArraySize = UArray_size(DATA(self));

    if (IoMessage_argCount(m) == 1) {
        strFmt = IoMessage_locals_cStringArgAt_(m, locals, 0);
    } else {
        seqPos = IoMessage_locals_intArgAt_(m, locals, 0);
        strFmt = IoMessage_locals_cStringArgAt_(m, locals, 1);
    }

    strFmtLen = strlen(strFmt);

    if (strFmt[0] == '*')
        i = isBigEndian = 1;

    for (count = 0; i < strFmtLen && seqPos < selfUArraySize; i++) {
        if (isdigit(strFmt[i])) {
            count = (count * 10) + (strFmt[i] - 0x30);
            continue;
        }

        count = count > 1 ? count : 1;

        for (; count > 0; count--) {
            IoObject *v;

            switch (strFmt[i]) {
                SEQ_UNPACK_VALUE_ASSIGN_LOOP('b', char, val, v)
                SEQ_UNPACK_VALUE_ASSIGN_LOOP('B', unsigned char, val, v)
                SEQ_UNPACK_VALUE_ASSIGN_LOOP('c', char, val, v)
                SEQ_UNPACK_VALUE_ASSIGN_LOOP('C', unsigned char, val, v)
                SEQ_UNPACK_VALUE_ASSIGN_LOOP('h', short, val, v)
                SEQ_UNPACK_VALUE_ASSIGN_LOOP('H', unsigned short, val, v)
                SEQ_UNPACK_VALUE_ASSIGN_LOOP('i', int, val, v)
                SEQ_UNPACK_VALUE_ASSIGN_LOOP('I', unsigned int, val, v)
                SEQ_UNPACK_VALUE_ASSIGN_LOOP('l', long, val, v)
                SEQ_UNPACK_VALUE_ASSIGN_LOOP('L', unsigned long, val, v)
                SEQ_UNPACK_VALUE_ASSIGN_LOOP('f', float, val, v)
                SEQ_UNPACK_VALUE_ASSIGN_LOOP('F', double, val, v)

            case 's': // string
            {
                UArray *ua = UArray_new();
                char *uap = (char *)ua->data;
                UArray_setItemType_(ua, CTYPE_uint8_t);
                UArray_setEncoding_(ua, CENCODING_ASCII);

                if (count == 1) {
                    // while(c = UArray_longAt_(selfUArray, seqPos ++)) {
                    //	UArray_appendLong_(ua, c);
                    //}
                    count = strlen(&selfUArray[seqPos]);
                    UArray_setSize_(ua, count);
                    strcpy(uap, &selfUArray[seqPos]);
                } else {
                    // for( ; count > 0 ; count --) {
                    //	UArray_appendLong_(ua, UArray_longAt_(selfUArray, seqPos
                    //++));
                    //}
                    UArray_setSize_(ua, count);
                    memcpy(uap, &selfUArray[seqPos], count);
                }
                seqPos += count;
                count = 1;
                v = IoSeq_newWithUArray_copy_(IOSTATE, ua, 0);
                break;
            }
            }

            IoList_rawAppend_(values, v);
        }
    }

    return values;
}

// -------------------------

void IoSeq_addImmutableMethods(IoSeq *self) {
    IoMethodTable methodTable[] = {
        {"itemType", IoSeq_itemType},
        {"itemSize", IoSeq_itemSize},
        {"encoding", IoSeq_encoding},
        {"asUTF8", IoSeq_asUTF8},
        {"asUCS2", IoSeq_asUCS2},
        {"asUCS4", IoSeq_asUCS4},
        {"asFixedSizeType", IoSeq_asFixedSizeType},

        {"asBinaryNumber", IoSeq_asBinaryNumber},
        {"asBinaryUnsignedInteger", IoSeq_asBinaryUnsignedInteger},
        {"asBinarySignedInteger", IoSeq_asBinaryUnsignedInteger},
        {"isSymbol", IoSeq_isSymbol},
        {"isMutable", IoSeq_isMutable},
        {"asSymbol", IoSeq_asSymbol},
        {"asString", IoSeq_asSymbol},
        {"asNumber", IoSeq_asNumber},
        {"asList", IoSeq_asList},
        {"whiteSpaceStrings", IoSeq_whiteSpaceStrings},
        {"print", IoSeq_print},
        {"linePrint", IoSeq_linePrint},
        {"parseJson", IoSeq_parseJson},
        {"size", IoSeq_size},
        {"sizeInBytes", IoSeq_sizeInBytes},
        {"isZero", IoSeq_isZero},
        {"isEmpty", IoSeq_isEmpty},
        {"at", IoSeq_at},
        {"exclusiveSlice", IoSeq_exclusiveSlice},
        {"exSlice", IoSeq_exclusiveSlice},
        {"inclusiveSlice", IoSeq_inclusiveSlice},
        {"inSlice", IoSeq_inclusiveSlice},
        {"between", IoSeq_between},
        {"betweenSeq", IoSeq_between},
        {"findSeqs", IoSeq_findSeqs},
        {"findSeq", IoSeq_findSeq},
        {"reverseFindSeq", IoSeq_reverseFindSeq},
        {"beginsWithSeq", IoSeq_beginsWithSeq},
        {"endsWithSeq", IoSeq_endsWithSeq},
        {"split", IoSeq_split},
        {"contains", IoSeq_contains},
        {"containsSeq", IoSeq_containsSeq},
        {"containsAnyCaseSeq", IoSeq_containsAnyCaseSeq},
        {"isLowercase", IoSeq_isLowercase},
        {"isUppercase", IoSeq_isUppercase},
        {"isEqualAnyCase", IoSeq_isEqualAnyCase},
        {"splitAt", IoSeq_splitAt},
        {"fromBase", IoSeq_fromBase},
        {"toBase", IoSeq_toBase},
        {"foreach", IoSeq_foreach},
        {"asMessage", IoSeq_asMessage},
        {"..", IoSeq_cloneAppendSeq},
        {"cloneAppendSeq", IoSeq_cloneAppendSeq},
        {"asMutable", IoSeq_asMutable},
        {"asBuffer", IoSeq_asMutable},

        // paths

        {"fileName", IoSeq_fileName},
        {"pathExtension", IoSeq_pathExtension},
        {"lastPathComponent", IoSeq_lastPathComponent},
        {"cloneAppendPath", IoSeq_cloneAppendPath},
        {"pathComponent", IoSeq_pathComponent},
        {"asOSPath", IoSeq_asOSPath},
        {"asIoPath", IoSeq_asIoPath},

        {"afterSeq", IoSeq_afterSeq},
        {"beforeSeq", IoSeq_beforeSeq},

        {"asCapitalized", IoSeq_asCapitalized},
        {"asUppercase", IoSeq_asUppercase},
        {"asLowercase", IoSeq_asLowercase},
        {"with", IoSeq_with},
        {"occurrencesOfSeq", IoSeq_occurrencesOfSeq},
        {"interpolate", IoSeq_interpolate},
        {"distanceTo", IoSeq_distanceTo},

        {"asBase64", IoSeq_asBase64},
        {"fromBase64", IoSeq_fromBase64},

        {">", IoSeq_greaterThan_},
        {"<", IoSeq_lessThan_},
        {">=", IoSeq_greaterThanOrEqualTo_},
        {"<=", IoSeq_lessThanOrEqualTo_},

        {"asStruct", IoSeq_asStruct},
        {"withStruct", IoSeq_withStruct},

        {"percentEncoded", IoSeq_percentEncoded},
        {"percentDecoded", IoSeq_percentDecoded},

        {"urlEncoded", IoSeq_urlEncoded},
        {"urlDecoded", IoSeq_urlDecoded},

        {"pack", IoSeq_pack},
        {"unpack", IoSeq_unpack},

        {NULL, NULL},
    };

    IoObject_addMethodTable_(self, methodTable);
}
