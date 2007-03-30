/*#io
Sequence ioDoc(
			docCopyright("Steve Dekorte", 2002)
			docLicense("BSD revised")
			docObject("Sequence")
			docDescription("""A Sequence is a container for a list of data elements. Typically these elements are each 1 byte in size. A Sequence can be either mutable or immutable. When immutable, only the read-only methods can be used.
<p>
Terminology
<ul>
<li> Buffer: A mutable Sequence of single byte elements, typically in a binary encoding
<li> Symbol or String: A unique immutable Sequence, typically in a character encoding
</ul>
""")
			docCategory("Core")
			*/

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
#define IO_ASSERT_NUMBER_ENCODING(self) IOASSERT(DATA(self)->encoding == CENCODING_NUMBER, "operation not valid on non-number encodings")

static void IoAssertNotSymbol(IoSeq *self, IoMessage *m)
{
	if (ISSYMBOL(self))
	{
		IoState_error_(IOSTATE, m,
					"'%s' cannot be called on an immutable Sequence",
					CSTRING(IoMessage_name(m)));
	}
}

IoObject *IoSeq_setItemType(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setItemType(aTypeName)", "Sets the underlying machine type for the elements. Valid names are uint8, uint16, uint32, uint64, int8, int16, int32, int64, float32, and float64. Note that 64 bit types are only available on platforms that support such types. Returns self. ")
	*/

	CTYPE itemType;
	IoSeq *typeName;

	IO_ASSERT_NOT_SYMBOL(self);

	typeName = IoMessage_locals_symbolArgAt_(m, locals, 0);
	itemType = CTYPE_forName(CSTRING(typeName));

	IOASSERT(itemType != -1, "invalid item type name");

	UArray_setItemType_(DATA(self), itemType);

	return self;
}

IoObject *IoSeq_convertToItemType(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSymbol *typeName = IoMessage_locals_symbolArgAt_(m, locals, 0);
	CTYPE itemType = CTYPE_forName(CSTRING(typeName));

	IO_ASSERT_NOT_SYMBOL(self);

	IOASSERT(itemType != -1, "invalid item type name");

	UArray_convertToItemType_(DATA(self), itemType);
	return self;
}

IoObject *IoSeq_convertToFixedSizeType(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IO_ASSERT_NOT_SYMBOL(self);
	UArray_convertToFixedSizeType(DATA(self));
	return self;
}

IoObject *IoSeq_setEncoding(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setEncoding(encodingName)", "Sets the encoding flag of the receiver (only the encoding flag, itemSize and itemType will change, no conversion is done between utf encodings - you can use convertToUTF8, etc methods for conversions). Valid encodings are number, utf8, utf16, and utf32. Returns self. ")
	*/

	CENCODING encoding;
	IoSeq *encodingName;

	IO_ASSERT_NOT_SYMBOL(self);

	encodingName = IoMessage_locals_symbolArgAt_(m, locals, 0);
	encoding = CENCODING_forName(CSTRING(encodingName));

	IOASSERT(encoding != -1, "invalid encoding name");

	UArray_setEncoding_(DATA(self), encoding);

	return self;
}

void IoSeq_rawCopy_(IoSeq *self, IoSeq *other)
{
	UArray_copy_(DATA(self), DATA(other));
}

IoObject *IoSeq_copy(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("copy(aSequence)", "Replaces the bytes of the receiver with a copy of those in aSequence. Returns self. ")
	*/

	IoSeq_rawCopy_(self, IoMessage_locals_seqArgAt_(m, locals, 0));
	return self;
}

IoObject *IoSeq_appendSeq(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("appendSeq(aSequence1, aSequence2, ...)",
		   "Appends aSequence arguments to the receiver. Returns self. ")
	*/

	int i;

	IO_ASSERT_NOT_SYMBOL(self);
	IOASSERT(IoMessage_argCount(m), "requires at least one argument");

	for (i = 0; i < IoMessage_argCount(m); i ++)
	{
		IoObject *other = IoMessage_locals_valueArgAt_(m, locals, i);

		if (ISNUMBER(other))
		{
			double d = IoNumber_asDouble(other);
			char s[24];
			memset(s, 0, 24);

			if (d == (long)d)
			{
				snprintf(s, 24, "%ld", (long)d);
			}
			else
			{
				snprintf(s, 24, "%g", d);
			}

			UArray_appendCString_(DATA(self), s);
		}
		else if (ISSEQ(other))
		{
			UArray_append_(DATA(self), DATA(other));
		}
		else if (!ISNIL(other))
		{
			IoState_error_(IOSTATE, m,
						   "argument 0 to method '%s' must be a number, string or buffer, not a '%s'",
								  CSTRING(IoMessage_name(m)), IoObject_name(other));
		}
	}
	return self;
}

IoObject *IoSeq_append(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("append(aNumber)", "Appends aNumber (cast to a byte) to the receiver. Returns self. ")
	*/

	int i;

	IO_ASSERT_NOT_SYMBOL(self);
	IOASSERT(IoMessage_argCount(m), "requires at least one argument");

	for (i = 0; i < IoMessage_argCount(m); i ++)
	{
		UArray_appendDouble_(DATA(self), IoMessage_locals_doubleArgAt_(m, locals, i));
	}

	return self;
}

IoObject *IoSeq_atInsertSeq(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("atInsertSeq(indexNumber, aSequence)",
		   "Returns a new Sequence with aSequence inserted at
indexNumber in the receiver.  ")
	*/

	size_t n = IoMessage_locals_sizetArgAt_(m, locals, 0);
	IoSeq *otherSeq = IoMessage_locals_seqArgAt_(m, locals, 1);

	IO_ASSERT_NOT_SYMBOL(self);

	IOASSERT(n <= UArray_size(DATA(self)), "insert index out of sequence bounds");

	UArray_at_putAll_(DATA(self), n, DATA(otherSeq));
	return self;
}

// removing ---------------------------------------

IoObject *IoSeq_removeSlice(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("removeSlice(startIndex, endIndex)",
		   "Removes the bytes from startIndex to endIndex.
Returns self.")
	*/

	long start = IoMessage_locals_longArgAt_(m, locals, 0);
	long end   = IoMessage_locals_longArgAt_(m, locals, 1);

	IO_ASSERT_NOT_SYMBOL(self);

	start = UArray_wrapPos_(DATA(self), start);
	end   = UArray_wrapPos_(DATA(self), end);

	UArray_removeRange(DATA(self), start, end - start + 1);
	return self;
}

IoObject *IoSeq_removeLast(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("removeLast",
		   "Removes the last element from the receiver. Returns self.")
	*/

	IO_ASSERT_NOT_SYMBOL(self);
	UArray_removeLast(DATA(self));
	return self;
}

IoObject *IoSeq_setSize(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setSize(aNumber)",
		   "Sets the length in bytes of the receiver to aNumber. Return self.")
	*/

	size_t len = IoMessage_locals_sizetArgAt_(m, locals, 0);
	IO_ASSERT_NOT_SYMBOL(self);
	UArray_setSize_(DATA(self), len);
	return self;
}

void IoSeq_rawPio_reallocateToSize_(IoSeq *self, size_t size)
{
	if (ISSYMBOL(self))
	{
		IoState_error_(IOSTATE, NULL, "attempt to resize an immutable Sequence");
	}

	UArray_sizeTo_(DATA(self), size);
}

IoObject *IoSeq_preallocateToSize(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("preallocateToSize(aNumber)",
		   "If needed, resize the memory alloced for the receivers
byte array to be large enough to fit the number of bytes specified by
aNumber. This is useful for pio_reallocating the memory so it doesn't
keep getting allocated as the Sequence is appended to. This operation
will not change the Sequence's length or contents. Returns self.")
	*/

	size_t newSize = IoMessage_locals_sizetArgAt_(m, locals, 0);
	IO_ASSERT_NOT_SYMBOL(self);
	UArray_sizeTo_(DATA(self), newSize);
	return self;
}

IoObject *IoSeq_replaceSeq(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("replaceSeq(aSequence, anotherSequence)",
		   "Returns a new Sequence with all occurances of aSequence
replaced with anotherSequence in the receiver. Returns self.")
	*/

	IoSeq *subSeq   = IoMessage_locals_seqArgAt_(m, locals, 0);
	IoSeq *otherSeq = IoMessage_locals_seqArgAt_(m, locals, 1);
	IO_ASSERT_NOT_SYMBOL(self);
	UArray_replace_with_(DATA(self), DATA(subSeq), DATA(otherSeq));
	return self;
}

IoObject *IoSeq_removeSeq(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("removeSeq(aSequence)", "Removes occurances of aSequence from the receiver.")
	*/

	IoSeq *subSeq   = IoMessage_locals_seqArgAt_(m, locals, 0);
	IO_ASSERT_NOT_SYMBOL(self);
	UArray_remove_(DATA(self), DATA(subSeq));
	return self;
}


IoObject *IoSeq_replaceFirstSeq(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("replaceFirstSeq(aSequence, anotherSequence, optionalStartIndex)",
		   "Returns a new Sequence with the first occurance of aSequence
replaced with anotherSequence in the receiver. If optionalStartIndex is
provided, the search for aSequence begins at that index. Returns self.")
	*/

	IoSeq *subSeq   = IoMessage_locals_seqArgAt_(m, locals, 0);
	IoSeq *otherSeq = IoMessage_locals_seqArgAt_(m, locals, 1);
	size_t startIndex = 0;

	if (IoMessage_argCount(m) > 2)
	{
		IoMessage_locals_longArgAt_(m, locals, 1);
	}

	IO_ASSERT_NOT_SYMBOL(self);

	{
		UArray *a = DATA(self);
		UArray *b = DATA(subSeq);
		UArray *c = DATA(otherSeq);
		long i = UArray_find_from_(a, b, startIndex);
		if(i != -1)
		{
			UArray_removeRange(a, startIndex, UArray_size(b));
			UArray_at_putAll_(a, startIndex, c);
		}
	}
	return self;
}

IoObject *IoSeq_atPut(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("atPut(aNumberIndex, aNumber)",
		   "Sets the value at the index specified by aNumberIndex to aNumber. Returns self. ")
	*/

	size_t i = IoMessage_locals_longArgAt_(m, locals, 0);
	UArray *a = DATA(self);

	IO_ASSERT_NOT_SYMBOL(self);

	if (UArray_isFloatType(a))
	{
		double v = IoMessage_locals_doubleArgAt_(m, locals, 1);
		UArray_at_putDouble_(a, i, v);
	}
	else
	{
		long v = IoMessage_locals_longArgAt_(m, locals, 1);
		UArray_at_putLong_(a, i, v);
	}

	return self;
}

IoObject *IoSeq_lowercase(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("Lowercase",
		   "Returns a copy of the receiver with all characters made Lowercase. ")
	*/

	IO_ASSERT_NOT_SYMBOL(self);
	UArray_tolower(DATA(self));
	return self;
}

IoObject *IoSeq_uppercase(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("uppercase",
		   "Makes all characters of the receiver uppercase. ")
	*/

	IO_ASSERT_NOT_SYMBOL(self);
	UArray_toupper(DATA(self));
	return self;
}

// clip --------------------------------------

IoObject *IoSeq_clipBeforeSeq(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("clipBeforeSeq(aSequence)",
		   "Clips receiver before aSequence.")
	*/

	IoSeq *otherSeq = IoMessage_locals_seqArgAt_(m, locals, 0);
	IO_ASSERT_NOT_SYMBOL(self);
	UArray_clipBefore_(DATA(self), DATA(otherSeq));
	return self;
}

IoObject *IoSeq_clipAfterSeq(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("clipAfterSeq(aSequence)",
		   "Removes the contents of the receiver after the end of
the first occurance of aSequence. Returns true if anything was
removed, or false otherwise.")
	*/

	IoSeq *otherSeq = IoMessage_locals_seqArgAt_(m, locals, 0);

	IO_ASSERT_NOT_SYMBOL(self);
	UArray_clipAfter_(DATA(self), DATA(otherSeq));
	return self;
}

IoObject *IoSeq_clipBeforeEndOfSeq(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("clipBeforeEndOfSeq(aSequence)",
		   "Removes the contents of the receiver before the end of
the first occurance of aSequence. Returns true if anything was
removed, or false otherwise.")
	*/

	IoSeq *otherSeq = IoMessage_locals_seqArgAt_(m, locals, 0);
	IO_ASSERT_NOT_SYMBOL(self);
	UArray_clipBeforeEndOf_(DATA(self), DATA(otherSeq));
	return self;
}

IoObject *IoSeq_clipAfterStartOfSeq(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("clipAfterStartOfSeq(aSequence)",
		   "Removes the contents of the receiver after the beginning of
the first occurance of aSequence. Returns true if anything was
removed, or false otherwise.")
	*/

	IoSeq *otherSeq = IoMessage_locals_seqArgAt_(m, locals, 0);
	IO_ASSERT_NOT_SYMBOL(self);
	UArray_clipAfterStartOf_(DATA(self), DATA(otherSeq));
    return self;
}

// -----------------------------------------

IoObject *IoSeq_empty(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("clear",
		   "Sets all bytes in the receiver to 0x0 and sets
it's length to 0. Returns self.")
	*/

	IO_ASSERT_NOT_SYMBOL(self);
	UArray_clear(DATA(self));
	UArray_setSize_(DATA(self), 0);
	return self;
}

int IoSeq_byteCompare(const void *a, const void *b)
{
	char aa = *(char *)a;
	char bb = *(char *)b;

	if (aa < bb)
	{
		return -1;
	}

	if (aa == bb)
	{
		return 0;
	}

	return 1;
}

IoObject *IoSeq_sort(IoSeq *self, IoObject *locals, IoMessage *m)
{
	UArray *a = DATA(self);
	IO_ASSERT_NOT_SYMBOL(self);

	if(UArray_itemType(a) == CTYPE_uintptr_t)
	{
		UArray_sortBy_(a, (UArraySortCallback *)IoObject_compare);
	}
	else
	{
		UArray_sort(a);
	}

	return self;
}

IoObject *IoSeq_replaceMap(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("replaceMap(aMap)",
		   "In the receiver, the keys of aMap replaced with it's values. Returns self.")
	*/

	IoMap *map = IoMessage_locals_mapArgAt_(m, locals, 0);
	UArray *ba = DATA(self);

	IO_ASSERT_NOT_SYMBOL(self);

	PHASH_FOREACH(IoMap_rawHash(map), k, v,
		{
		IoSymbol *subSeq = k;
		IoSymbol *otherSeq = v;

		if (ISSEQ(otherSeq))
		{
			UArray_replace_with_(ba, DATA(subSeq), DATA(otherSeq));
		}
		else
		{
			IoState_error_(IOSTATE, m,
								  "argument 0 to method '%s' must be a Map with Sequence values, not '%s'",
								  CSTRING(IoMessage_name(m)), IoObject_name(otherSeq));
		}
		}
	);

	return self;
}

// translate ------------------------------------------------------

IoObject *IoSeq_translate(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("translate(fromChars, toChars)",
		   "In the receiver, the characters in fromChars are replaced with those in the same positions in toChars. Returns self.")
	*/

	UArray *ba = DATA(self);
	UArray *fc = DATA(IoMessage_locals_seqArgAt_(m, locals, 0));
	UArray *tc = DATA(IoMessage_locals_seqArgAt_(m, locals, 1));

	IO_ASSERT_NOT_SYMBOL(self);

	if (UArray_size(tc) != UArray_size(fc))
	{
		IoState_error_(IOSTATE, m, "translation strings must be of the same length");
	}

	UArray_translate(ba, fc, tc);

	return self;
}

// reverse --------------------------------------------------------

IoObject *IoSeq_reverse(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
		   docSlot("reverse", "Reverses the bytes in the receiver, in-place.")
	*/

	UArray_reverse(DATA(self));
	return self;
}


// strip ----------------------------------------------------------

IoObject *IoSeq_strip(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("strip(optionalSequence)",
		   """Trims the whitespace (or optionalSequence) off both ends:
<pre>
"   Trim this string   \r\n" strip
==> "Trim this string"
</pre>
""")
	*/

	IO_ASSERT_NOT_SYMBOL(self);

	if (IoMessage_argCount(m) > 0)
	{
		IoSeq *other  = IoMessage_locals_seqArgAt_(m, locals, 0);
		UArray_strip_(DATA(self), DATA(other));
	}
	else
	{
		UArray space = UArray_stackAllocedWithCString_(WHITESPACE);
		UArray_strip_(DATA(self), &space);
	}

	return self;
}

IoObject *IoSeq_lstrip(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("lstrip(aSequence)",
		   """Strips the characters in aSequence
stripped from the beginning of the receiver. Example:
<pre>
"Keep the tail" lstrip(" eKp")
==> "the tail"
</pre>
""")
	*/

	IO_ASSERT_NOT_SYMBOL(self);

	if (IoMessage_argCount(m) > 0)
	{
		IoSeq *other  = IoMessage_locals_seqArgAt_(m, locals, 0);
		UArray_lstrip_(DATA(self), DATA(other));
	}
	else
	{
		UArray space = UArray_stackAllocedWithCString_(WHITESPACE);
		UArray_lstrip_(DATA(self), &space);
	}

	return self;
}

IoObject *IoSeq_rstrip(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("rstrip(aSequence)",
		   """Strips the characters in
aSequence stripped from the end of the receiver. Example:
<pre>
"Cut the tail off" rstrip(" afilot")
==> "Cut the"
</pre>
""")
	*/

	IO_ASSERT_NOT_SYMBOL(self);

	if (IoMessage_argCount(m) > 0)
	{
		IoSeq *other  = IoMessage_locals_seqArgAt_(m, locals, 0);
		UArray_rstrip_(DATA(self), DATA(other));
	}
	else
	{
		UArray space = UArray_stackAllocedWithCString_(WHITESPACE);
		UArray_rstrip_(DATA(self), &space);
	}

	return self;
}

// -----------------------------------------------------------

IoObject *IoSeq_escape(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("escape",
		   """Escape characters in the receiver are replaced with escape codes.
For example a string containing a single return character would contain the
following 2 characters after being escaped: "\n". Returns self.""")
	*/

	IO_ASSERT_NOT_SYMBOL(self);
	UArray_escape(DATA(self));
	return self;
}

IoObject *IoSeq_unescape(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("unescape",
		   "Escape codes replaced with escape characters. Returns self.")
	*/

	IO_ASSERT_NOT_SYMBOL(self);
	UArray_unescape(DATA(self));
	return self;
}

IoObject *IoSeq_removePrefix(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("removePrefix(aSequence)",
		   "If the receiver begins with aSequence, it is removed. Returns self.")
	*/

	IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);

	IO_ASSERT_NOT_SYMBOL(self);

	if (UArray_beginsWith_(DATA(self), DATA(other)))
	{
		UArray_removeRange(DATA(self), 0, UArray_size(DATA(other)));
	}

	return self;
}

IoObject *IoSeq_removeSuffix(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("removeSuffix(aSequence)",
		   "If the receiver end with aSequence, it is removed. Returns self.")
	*/

	IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);

	IO_ASSERT_NOT_SYMBOL(self);

	if (UArray_endsWith_(DATA(self), DATA(other)))
	{
		UArray *ba = DATA(self);
		UArray_removeRange(ba,
						  UArray_size(ba) - UArray_size(DATA(other)),
						  UArray_size(ba));
	}

	return self;
}

IoObject *IoSeq_capitalize(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("capitalize",
		   "First charater of the receiver is made uppercase.")
	*/

	int firstChar = UArray_firstLong(DATA(self));

	IO_ASSERT_NOT_SYMBOL(self);
	UArray_at_putLong_(DATA(self), 0, toupper(firstChar));
	return self;
}

IoObject *IoSeq_appendPathSeq(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("appendPathSeq(aSeq)",
		   "Appends argument to the receiver such that there is one
and only one path separator between the two. Returns self.")
	*/

	IoSeq *component = IoMessage_locals_seqArgAt_(m, locals, 0);

	IO_ASSERT_NOT_SYMBOL(self);
	UArray_appendPath_(DATA(self), DATA(component));
	return self;
}

IoObject *IoSeq_interpolateInPlace(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("interpolateInPlace(ctx)",
		"Replaces all #{<code>} with <code> evaluated in the context ctx.  "
		"If ctx not given the current context is used.  Returns self.")
	*/

	IoObject *context;
	UArray *string;
	UArray *code;
	IoObject *evaluatedCode;
	UArray *evaluatedCodeAsString;
	const char *label;
	int from, to;
	UArray begin = UArray_stackAllocedWithCString_("#{");
	UArray end = UArray_stackAllocedWithCString_("}");

	IO_ASSERT_NOT_SYMBOL(self);

	context = IoMessage_locals_valueArgAt_(m, locals, 0);
	string = DATA(self);
	label = "IoSeq_interpolateInPlace()";
	from = 0;

	context = (context == IONIL(self)) ? locals : context;

	IoState_pushRetainPool(IOSTATE);

	for(;;)
	{
		IoState_clearTopPool(IOSTATE);

		from = UArray_find_from_(string, &begin, from);
		if (from == -1) break;

		to = UArray_find_from_(string, &end, from);
		if (to == -1) break;

		code = UArray_slice(string, from+2, to);

		if (UArray_size(code) == 0)
		{
			// we don't want "#{}" to interpolate into "nil"
			evaluatedCodeAsString = DATA(IoState_doCString_(IOSTATE, "Sequence clone"));
		}
		else
		{
			evaluatedCode = IoState_on_doCString_withLabel_(IOSTATE, context, (char *)UArray_bytes(code), label);
			evaluatedCodeAsString = DATA(IoState_on_doCString_withLabel_(IOSTATE, evaluatedCode, "asString", label));
		}

		UArray_free(code);

		UArray_removeRange(string, from, to-from+1);
		UArray_at_putAll_(string, from, evaluatedCodeAsString);
		from = from + UArray_size(evaluatedCodeAsString);
	}
	
	IoState_popRetainPool(IOSTATE);

	return self;
}

// math ---------------------------------------------------------------------

IoObject *IoSeq_addEquals(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);

	IO_ASSERT_NOT_SYMBOL(self);
	IO_ASSERT_NUMBER_ENCODING(self);

	if (ISSEQ(other))
	{
		UArray_add_(DATA(self), DATA(other));
	}
	else if (ISNUMBER(other))
	{
		double value = IoNumber_asDouble(other);
		UArray_addScalarDouble_(DATA(self), value);
	}
	else
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, 0, "Sequence or Number");
	}

	return self;
}

IoObject *IoSeq_subtractEquals(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);

	IO_ASSERT_NOT_SYMBOL(self);
	IO_ASSERT_NUMBER_ENCODING(self);

	if (ISSEQ(other))
	{
		UArray_subtract_(DATA(self), DATA(other));
	}
	else if (ISNUMBER(other))
	{
		double value = IoNumber_asDouble(other);
		UArray_subtractScalarDouble_(DATA(self), value);
	}
	else
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, 0, "Sequence or Number");
	}

	return self;
}

IoObject *IoSeq_multiplyEquals(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);

	IO_ASSERT_NOT_SYMBOL(self);
	IO_ASSERT_NUMBER_ENCODING(self);

	if (ISSEQ(other))
	{
		UArray_multiply_(DATA(self), DATA(other));
	}
	else if (ISNUMBER(other))
	{
		double value = IoNumber_asDouble(other);
		UArray_multiplyScalarDouble_(DATA(self), value);
	}
	else
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, 0, "Sequence or Number");
	}

	return self;
}

IoObject *IoSeq_divideEquals(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);

	IO_ASSERT_NOT_SYMBOL(self);
	IO_ASSERT_NUMBER_ENCODING(self);

	if (ISSEQ(other))
	{
		UArray_divide_(DATA(self), DATA(other));
	}
	else if (ISNUMBER(other))
	{
		double value = IoNumber_asDouble(other);
		UArray_divideScalarDouble_(DATA(self), value);
	}
	else
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, 0, "Sequence or Number");
	}

	return self;
}

IoObject *IoSeq_clone(IoSeq *self)
{
	return IoSeq_newWithUArray_copy_(IOSTATE, DATA(self), 1);
}

IoObject *IoSeq_add(IoSeq *self, IoObject *locals, IoMessage *m)
{
	return IoSeq_addEquals(IoSeq_clone(self), locals, m);
}

IoObject *IoSeq_subtract(IoSeq *self, IoObject *locals, IoMessage *m)
{
	return IoSeq_subtractEquals(IoSeq_clone(self), locals, m);
}

IoObject *IoSeq_multiply(IoSeq *self, IoObject *locals, IoMessage *m)
{
	return IoSeq_multiplyEquals(IoSeq_clone(self), locals, m);
}

IoObject *IoSeq_divide(IoSeq *self, IoObject *locals, IoMessage *m)
{
	return IoSeq_divideEquals(IoSeq_clone(self), locals, m);
}

IoObject *IoSeq_dotProduct(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
	IO_ASSERT_NOT_SYMBOL(self);
	UArray_dotProduct_(DATA(self), DATA(other));
	return self;
}

IoObject *IoSeq_setItemsToLong_(IoSeq *self, IoObject *locals, IoMessage *m)
{
	long v = IoMessage_locals_longArgAt_(m, locals, 0);
	IO_ASSERT_NOT_SYMBOL(self);
	UArray_setItemsToLong_(DATA(self), v);
	return self;
}

IoObject *IoSeq_setItemsToDouble_(IoSeq *self, IoObject *locals, IoMessage *m)
{
	double v = IoMessage_locals_doubleArgAt_(m, locals, 0);
	IO_ASSERT_NOT_SYMBOL(self);
	UArray_setItemsToLong_(DATA(self), v);
	return self;
}

#define IoSeqMutateNoArgNoResultOp(name) \
IoObject *IoSeq_ ## name (IoSeq *self, IoObject *locals, IoMessage *m) \
{ IO_ASSERT_NOT_SYMBOL(self); UArray_ ## name (DATA(self)); return self; }

IoSeqMutateNoArgNoResultOp(negate);
IoSeqMutateNoArgNoResultOp(rangeFill);

IoSeqMutateNoArgNoResultOp(sin);
IoSeqMutateNoArgNoResultOp(cos);
IoSeqMutateNoArgNoResultOp(tan);

IoSeqMutateNoArgNoResultOp(asin);
IoSeqMutateNoArgNoResultOp(acos);
IoSeqMutateNoArgNoResultOp(atan);

IoSeqMutateNoArgNoResultOp(sinh);
IoSeqMutateNoArgNoResultOp(cosh);
IoSeqMutateNoArgNoResultOp(tanh);

IoSeqMutateNoArgNoResultOp(exp);
IoSeqMutateNoArgNoResultOp(log);
IoSeqMutateNoArgNoResultOp(log10);

IoSeqMutateNoArgNoResultOp(ceil);
IoSeqMutateNoArgNoResultOp(floor);
IoSeqMutateNoArgNoResultOp(abs);

IoSeqMutateNoArgNoResultOp(square);
IoSeqMutateNoArgNoResultOp(sqrt);
IoSeqMutateNoArgNoResultOp(normalize);

#define IoSeqNoArgNumberResultOp(name) \
IoObject *IoSeq_ ## name (IoSeq *self, IoObject *locals, IoMessage *m) \
{ return IONUMBER(UArray_ ## name (DATA(self))); }

IoSeqNoArgNumberResultOp(sumAsDouble);
IoSeqNoArgNumberResultOp(productAsDouble);
IoSeqNoArgNumberResultOp(minAsDouble);
IoSeqNoArgNumberResultOp(maxAsDouble);
IoSeqNoArgNumberResultOp(arithmeticMeanAsDouble);
IoSeqNoArgNumberResultOp(arithmeticMeanSquareAsDouble);
IoSeqNoArgNumberResultOp(hash);

#define IoSeqLongArgNumberResultOp(name) \
IoObject *IoSeq_ ## name (IoSeq *self, IoObject *locals, IoMessage *m) \
{ return IONUMBER(UArray_ ## name (DATA(self), IoMessage_locals_longArgAt_(m, locals, 0))); }

//IoSeqLongArgNumberResultOp(setAllBitsTo_);
IoSeqLongArgNumberResultOp(byteAt_);
IoSeqLongArgNumberResultOp(bitAt_);
IoSeqNoArgNumberResultOp(bitCount);

#define IoSeqSeqArgNoResultOp(name) \
IoObject *IoSeq_ ## name (IoSeq *self, IoObject *locals, IoMessage *m) \
{ IO_ASSERT_NOT_SYMBOL(self); UArray_ ## name (DATA(self), DATA(IoMessage_locals_seqArgAt_(m, locals, 0))); return self; }

IoSeqSeqArgNoResultOp(bitwiseOr_);
IoSeqSeqArgNoResultOp(bitwiseAnd_);
IoSeqSeqArgNoResultOp(bitwiseXor_);
IoSeqMutateNoArgNoResultOp(bitwiseNot);

IoSeqSeqArgNoResultOp(logicalOr_);
IoSeqSeqArgNoResultOp(logicalAnd_);

/*#io
docSlot("removeOddIndexes",
	   "Removes odd indexes in the receiver.
For example, list(1,2,3) removeOddIndexes == list(2). Returns self.")
*/


/*#io
docSlot("removeEvenIndexes",
	   "Removes even indexes in the receiver.
For example, list(1,2,3) removeEvenIndexes == list(1, 3). Returns self.")
*/

/*#io
docSlot("duplicateIndexes",
	   "Duplicates all indexes in the receiver.
For example, list(1,2,3) duplicateIndexes == list(1,1,2,2,3,3). Returns self.")
*/

IoSeqMutateNoArgNoResultOp(duplicateIndexes);
IoSeqMutateNoArgNoResultOp(removeOddIndexes);
IoSeqMutateNoArgNoResultOp(removeEvenIndexes);

IoSeqMutateNoArgNoResultOp(clear);


void IoSeq_addMutableMethods(IoSeq *self)
{
	IoMethodTable methodTable[] = {
	{"setItemType", IoSeq_setItemType},
	{"setEncoding", IoSeq_setEncoding},
	{"convertToItemType", IoSeq_convertToItemType},
	{"convertToFixedSizeType", IoSeq_convertToFixedSizeType},
	{"copy", IoSeq_copy},
	{"appendSeq", IoSeq_appendSeq},
	{"append", IoSeq_append},
	{"atInsertSeq", IoSeq_atInsertSeq},
	{"removeSlice", IoSeq_removeSlice},
	{"removeLast", IoSeq_removeLast},
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
	{"clipAfterSeq",  IoSeq_clipAfterSeq},
	{"clipBeforeEndOfSeq",  IoSeq_clipBeforeEndOfSeq},
	{"clipAfterStartOfSeq", IoSeq_clipAfterStartOfSeq},

	{"empty", IoSeq_empty},
	{"sort", IoSeq_sort},
	{"reverse", IoSeq_reverse},
	{"replaceMap", IoSeq_replaceMap},

	{"strip",  IoSeq_strip},
	{"lstrip", IoSeq_lstrip},
	{"rstrip", IoSeq_rstrip},

	{"zero", IoSeq_clear},

	{"escape",   IoSeq_escape},
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

	{"+", IoSeq_add},
	{"-", IoSeq_subtract},
	{"*", IoSeq_multiply},
	{"/", IoSeq_divide},

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

	{"sin", IoSeq_sin},
	{"cos", IoSeq_cos},
	{"tan", IoSeq_tan},

	{"asin", IoSeq_asin},
	{"acos", IoSeq_acos},
	{"atan", IoSeq_atan},

	{"sinh", IoSeq_sinh},
	{"cosh", IoSeq_cosh},
	{"tanh", IoSeq_tanh},

	{"removeOddIndexes",  IoSeq_removeOddIndexes},
	{"removeEvenIndexes", IoSeq_removeEvenIndexes},
	{"duplicateIndexes",  IoSeq_duplicateIndexes},


	//{"setAllBitsTo",  IoSeq_setAllBitsTo_},
	{"byteAt",  IoSeq_byteAt_},
	{"bitAt",  IoSeq_bitAt_},
	{"bitCount",  IoSeq_bitCount},

	{"bitwiseOr",  IoSeq_bitwiseOr_},
	{"bitwiseAnd",  IoSeq_bitwiseAnd_},
	{"bitwiseXor",  IoSeq_bitwiseXor_},
	{"bitwiseNot",  IoSeq_bitwiseNot},

	{"logicalOr",  IoSeq_logicalOr_},
	{"setItemsToLong", IoSeq_setItemsToLong_},
	{"setItemsToDouble", IoSeq_setItemsToDouble_},

	{NULL, NULL},
	};

	IoObject_addMethodTable_(self, methodTable);
}
