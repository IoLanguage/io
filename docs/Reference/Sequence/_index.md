# Sequence

A Sequence is a container for a list of data elements. Typically these elements
are each 1 byte in size. A Sequence can be either mutable or immutable. When
immutable, only the read-only methods can be used. <p> Terminology <ul> <li>
Buffer: A mutable Sequence of single byte elements, typically in a binary
encoding <li> Symbol or String: A unique immutable Sequence, typically in a
character encoding
</ul>

## -(aSeq)

Vector subtraction - Subtracts the values of aSeq from the corresponding
values of the receiver returning a new vector with the result. Only works on
Sequences whose item type is numeric.

## -=(aSeq)

Vector subtraction - subtracts the values of aSeq to those of the receiver.
Only works on Sequences whose item type is numeric. Returns self.

## ..(aSequence)

Returns a copy of the receiver with aSequence appended to it.

## /(aSeq)

Divides the values of the receiver by the corresponding values of aSeq
returning a new vector with the result.
Only works on Sequences whose item type is numeric.

## /=(aSeq)

Divides the values of aSeq to the corresponding values of the receiver.
Only works on Sequences whose item type is numeric. Returns self.

## +(aSeq)

Vector addition - adds the values of aSeq to the corresponding values of the
receiver returning a new vector with the result. Only works on Sequences
whose item type is numeric.

## +=(aSeq)

Vector addition - adds the values of aSeq to those of the receiver.
Only works on Sequences whose item type is numeric. Returns self.

## abs

Sets each value of the Sequence to its absolute value.
Returns self.

## acos

Sets each value of the Sequence to the trigonometric arcsine of its value.
Returns self.

## afterSeq(aSequence)

Returns the slice of the receiver (as a Symbol) after aSequence or
nil if aSequence is not found. If aSequence is empty, the receiver
(or a copy of the receiver if it is mutable) is returned.

## alignCenter(width, [padding]) ""

Example:
<code>
Io> "abc" alignCenter(10, "-")
==> ---abc----
Io> "abc" alignCenter(10, "-=")
==> -=-abc-=-=
</code>

## alignLeft(width, [padding])

Example:
<p>
<code>
Io> "abc" alignLeft(10, "+")
==> abc+++++++
Io> "abc" alignLeft(10, "-=")
==> abc-=-=-=-
</code>

## alignLeftInPlace(width,

[padding]) Same as align left but operation is performed on the receiver.

## alignRight(width, [padding])

Example:
<p>
<code>
Io> "abc" alignRight(10, "-")
==> -------abc
Io> "abc" alignRight(10, "-=")
==> -=-=-=-abc
</code>

## append(aNumber)

Appends aNumber (cast to a byte) to the receiver. Returns self.

## appendPathSeq(aSeq)

Appends argument to the receiver such that there is one
and only one path separator between the two. Returns self.

## appendSeq(object1, object2, ...)

Calls asString on the arguments and appends the string to the receiver.
Returns self.

## asBase64(optionalCharactersPerLine)

Returns an immutable, base64 encoded (according to RFC 1421) version of
self. optionalCharactersPerLine describes the number of characters between
line breaks and defaults to 0.

## asBinaryNumber

Returns a Number containing the first 8 bytes of the
receiver without casting them to a double. Endian is same as machine.

## asBinarySignedInteger

Returns a Number with the bytes of the receiver interpreted as a binary
signed integer. Endian is same as machine.

## asBinaryUnsignedInteger

Returns a Number with the bytes of the receiver interpreted as a binary
unsigned integer. Endian is same as machine.

## asCapitalized

Returns a copy of the receiver with the first charater made uppercase.

## asFile

Returns a new File object with the receiver as its path.

## asFixedSizeType

Returns a new sequence with the receiver encoded in the
minimal fixed width text encoding that its characters can fit
into (either, ascii, utf8, utf16 or utf32).

## asHex

Returns a hex string for the receiving sequence, e.g., \"abc\" asHex -> \"616263\".

## asin

Sets each value of the Sequence to the trigonometric arcsine of its value.
Returns self.

## asIoPath

Returns a Io style path for an OS style path.

## asJson

Converts to form that could be interpreted as json if it already contains json, e.g. {"aaa":"bbb"} --> "{\"aaa\":\"bbb\"}"

## asList

Returns the receiver converted to a List containing all elements of the
Sequence.

## asLowercase

Returns a symbol containing the reveiver made lowercase.

## asMessage(optionalLabel)

Returns the compiled message object for the string.

## asMutable

Returns a mutable copy of the receiver.

## asNumber

Returns the receiver converted to a number.
Initial whitespace is ignored.

## asOSPath

Returns a OS style path for an Io style path.

## asStruct(memberList)

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

## asSymbol

Returns a immutable Sequence (aka Symbol) version of the receiver.

## asUCS2

Returns a new copy of the receiver converted to UCS2 (fixed character width
UTF16) encoding.

## asUCS4

Returns a new copy of the receiver converted to UCS4 (fixed character width
UTF32) encoding.

## asUppercase

Returns a symbol containing the reveiver made uppercase.

## asUTF8

Returns a new copy of the receiver converted to utf8 encoding.

## at(aNumber)

Returns a value at the index specified by aNumber.
Returns nil if the index is out of bounds.

## atan

Sets each value of the Sequence to the trigonometric arctangent of its
value. Returns self.

## atInsertSeq(indexNumber, object)

Calls asString on object and inserts the string at position indexNumber.
Returns self.

## atPut(aNumberIndex, aNumber)

Sets the value at the index specified by aNumberIndex to aNumber. Returns
self.

## beforeSeq(aSequence)

Returns the slice of the receiver (as a Symbol) before
aSequence or self if aSequence is not found.

## beginsWithSeq(aSequence)

Returns true if the receiver begins with aSequence, false otherwise.

## betweenSeq(aSequence, anotherSequence)

Returns a new Sequence containing the bytes between the
occurrence of aSequence and anotherSequence in the receiver.
If aSequence is empty, this method is equivalent to
beforeSeq(anotherSequence). If anotherSequence is nil, this method is
equivalent to afterSeq(aSequence). nil is returned if no match is found.

## bitAt(bitIndex)

Returns a Number containing the bit at the bit index value.

## bitCount

Returns the number of bits in the sequence.

## bitwiseAnd(aSequence)

Updates the receiver to be the result of a bitwiseAnd with aSequence.
Returns self.

## bitwiseNot(aSequence)

Updates the receiver to be the result of a bitwiseNot with aSequence.
Returns self.

## bitwiseOr(aSequence)

Updates the receiver to be the result of a bitwiseOr with aSequence. Returns
self.

## bitwiseXor(aSequence)

Updates the receiver to be the result of a bitwiseXor with aSequence.
Returns self.

## byteAt(byteIndex)

Returns a Number containing the byte at the byte index value.

## capitalize

First charater of the receiver is made uppercase.

## ceil

Round each value to smallest integral value not less than x.
Returns self.

## clear

Set all values in the sequence to 0. Returns self.

## clipAfterSeq(aSequence)

Removes the contents of the receiver after the end of
the first occurrence of aSequence. Returns true if anything was
removed, or false otherwise.

## clipAfterStartOfSeq(aSequence)

Removes the contents of the receiver after the beginning of
the first occurrence of aSequence. Returns true if anything was
removed, or false otherwise.

## clipBeforeEndOfSeq(aSequence)

Removes the contents of the receiver before the end of
the first occurrence of aSequence. Returns true if anything was
removed, or false otherwise.

## clipBeforeSeq(aSequence)

Clips receiver before aSequence.

## cloneAppendPath(aSequence)

Appends argument to a copy the receiver such that there is one
and only one path separator between the two and returns the result.

## contains(aNumber)

Returns true if the receiver contains an element equal in value to aNumber,
false otherwise.

## containsAnyCaseSeq(aSequence)

Returns true if the receiver contains the aSequence
regardless of casing, false otherwise.

## containsSeq(aSequence)

Returns true if the receiver contains the substring
aSequence, false otherwise.

## convertToItemType(aTypeName)

Converts the underlying machine type for the elements, expanding or
contracting the size of the Sequence as needed. Valid names are uint8,
uint16, uint32, uint64, int8, int16, int32, int64, float32, and float64.
Note that 64 bit types are only available on platforms that support such
types. Returns self.

## copy(aSequence)

Replaces the bytes of the receiver with a copy of those in aSequence.
Returns self.

## cos

Sets each value of the Sequence to the trigonometric cosine of its value.
Returns self.

## cosh

Sets each value of the Sequence to the hyperbolic cosine of its value.
Returns self.

## distanceTo(aSeq)

Returns a number with the square root of the sum of the square
of the differences of the items between the sequences.

## dotProduct(aSeq)

Returns a new Sequence containing the dot product of the receiver with aSeq.

## duplicateIndexes

Duplicates all indexes in the receiver.
For example, list(1,2,3) duplicateIndexes == list(1,1,2,2,3,3). Returns
self.

## empty

Sets all bytes in the receiver to 0x0 and sets
its length to 0. Returns self.

## encoding

Returns the encoding of the elements.

## endsWithSeq(aSequence)

Returns true if the receiver ends with aSequence, false otherwise.

## escape

Escape characters in the receiver are replaced with escape codes.
For example a string containing a single return character would contain the
following 2 characters after being escaped: "\n". Returns self.

## exclusiveSlice(inclusiveStartIndex, exclusiveEndIndex)

Returns a new string containing the subset of the
receiver from the inclusiveStartIndex to the exclusiveEndIndex. The
exclusiveEndIndex argument is optional. If not given, it is assumed to be
one beyond the end of the string.

## exp

Sets each value of the Sequence to e**value.
Returns self.

## fileName

Returns the last path component sans the path extension.

## findNthSeq(aSequence,

n) Returns a number with the nth occurrence of aSequence.

## findSeq(aSequence, optionalStartIndex)

Returns a number with the first occurrence of aSequence in
the receiver after the startIndex. If no startIndex is specified,
the search starts at index 0.
nil is returned if no occurrences are found.

## findSeqs(listOfSequences, optionalStartIndex)

Returns an object with two slots - an \"index\" slot which contains
the first occurrence of any of the sequences in listOfSequences found
in the receiver after the startIndex, and a \"match\" slot, which
contains a reference to the matching sequence from listOfSequences.
If no startIndex is specified, the search starts at index 0.
nil is returned if no occurrences are found.

## floor

Round each value to largest integral value not greater than x.
Returns self.

## foreach(optionalIndex, value, message)

For each element, set index to the index of the
element and value to the element value and execute message.
Example:
<pre>
aSequence foreach(i, v, writeln("value at index ", i, " is ", v))
aSequence foreach(v, writeln("value ", v))
</pre>

## fromBase(aNumber)

Returns a number with a base 10 representation of the receiver
converted from the specified base. Only base 2 through 32 are currently
supported.

## fromBase64

Returns an immutable, base64 decoded (according to RFC 1421) version of
self.

## greaterThan(aSeq)

Returns true if the receiver is greater than aSeq, false otherwise.

## greaterThanOrEqualTo(aSeq)

Returns true if the receiver is greater than or equal to aSeq, false
otherwise.

## hash

Returns a Number containing a hash of the Sequence.

## inclusiveSlice(inclusiveStartIndex, inclusiveEndIndex)

Returns a new string containing the subset of the
receiver from the inclusiveStartIndex to the inclusiveEndIndex. The
inclusiveEndIndex argument is optional. If not given, it is assumed to be
the end of the string.

## interpolate(ctx)

Returns immutable copy of self with interpolateInPlace(ctx) passed to the
copy.

## interpolateInPlace(optionalContext)

Replaces all #{expression} with expression evaluated in the optionalContext.
If optionalContext not given, the current context is used.  Returns self.

## isEmpty

Returns true if the size of the receiver is 0, false otherwise.

## isEqualAnyCase(aSequence)

Returns true if aSequence is equal to the receiver
ignoring case differences, false otherwise.

## isLowercase

Returns self if all the characters in the string are lower case.

## isMutable

Returns true if the receiver is a mutable Sequence or false otherwise.

## isSymbol

Returns true if the receiver is a
immutable Sequence (aka, a Symbol) or false otherwise.

## isUppercase

Returns self if all the characters in the string are upper case.

## isZero

Returns true if all elements are 0, false otherwise.

## itemCopy

Returns a new sequence containing the items from the receiver.

## itemSize

Returns number of bytes in each element.

## itemType

Returns machine type of elements.

## justSerialized(stream)

Writes the receiver's code into the stream.

## lastPathComponent

Returns a string containing the receiver clipped up
to the last path separator.

## lessThan(aSeq)

Returns true if the receiver is less than aSeq, false otherwise.

## lessThanOrEqualTo(aSeq)

Returns true if the receiver is less than or equal to aSeq, false otherwise.

## linePrint

Prints the Sequence and a newline character.

## log

Sets each value of the Sequence to the natural log of its value.
Returns self.

## log10

Sets each value of the Sequence to the base 10 log of its value.
Returns self.

## logicalAnd(aSequence)

Updates the receiver's values to be the result of a logical OR operations
with the values of aSequence. Returns self.

## logicalOr(aSequence)

Updates the receiver's values to be the result of a logical OR operations
with the values of aSequence. Returns self.

## lowercase

Makes all the uppercase characters in the receiver lowercase. Returns self.

## lstrip(aSequence)

Strips the characters in aSequence
stripped from the beginning of the receiver. Example:
<p>
<pre>
"Keep the tail" lstrip(" eKp")
==> "the tail"
</pre>

## makeFirstCharacterLowercase

Receiver must be mutable (see also asMutable). Returns receiver.
<br/>
<pre>
Io> "ABC" asMutable makeFirstCharacterLowercase
==> aBC
</pre>

## makeFirstCharacterUppercase

Receiver must be mutable (see also asMutable). Returns receiver.
<br/>
<pre>
Io> "abc" asMutable makeFirstCharacterUppercase
==> Abc
</pre>

## max

Returns the maximum value of the Sequence.

## Max

Returns the maximum value in the sequence.

## mean

Returns the arithmetic mean of the sequence.

## meanSquare

Returns the arithmetic mean of the sequence's values after they have been
squared.

## min

Returns the minimum value of the Sequence.

## Min

Returns the minimum value in the sequence.

## negate

Negates the values of the receiver.
Returns self.

## normalize

Divides each value of the Sequence by the max value of the sequence.
Returns self.

## occurrencesOfSeq(aSeq)

Returns count of aSeq in the receiver.

## pack(format, value1, ...)

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

## parseJson

Interprets the Sequence as JSON and returns a Map.

## pathComponent

Returns a slice of the receiver before the last path separator as a symbol.

## pathExtension

Returns a string containing the receiver clipped up to the last period.

## percentDecoded

Returns percent decoded version of receiver.

## percentEncoded

Returns percent encoded version of receiver.

## preallocateToSize(aNumber)

If needed, resize the memory alloced for the receivers
byte array to be large enough to fit the number of bytes specified by
aNumber. This is useful for pio_reallocating the memory so it doesn't
keep getting allocated as the Sequence is appended to. This operation
will not change the Sequence's length or contents. Returns self.

## prependSeq(object1,

object2, ...) Prepends given objects asString in reverse order to the receiver.  Returns self.

## print

Prints the receiver as a string. Returns self.

## product

Returns the product of all the sequence's values multipled together.

## rangeFill

Sets the values of the Sequence to their index values.
Returns self.

## removeAt(index)

Removes the item at the specified index and returns the value removed.
Returns nil if the index is out of bounds.

## removeEvenIndexes

Removes even indexes in the receiver.
For example, list(1,2,3) removeEvenIndexes == list(1, 3). Returns self.

## removeLast

Removes the last element from the receiver. Returns self.

## removeOddIndexes

Removes odd indexes in the receiver.
For example, list(1,2,3) removeOddIndexes == list(2). Returns self.

## removePrefix(aSequence)

If the receiver begins with aSequence, it is removed. Returns self.

## removeSeq(aSequence)

Removes occurrences of aSequence from the receiver.

## removeSlice(startIndex, endIndex)

Removes the items from startIndex to endIndex.
Returns self.

## removeSuffix(aSequence)

If the receiver end with aSequence, it is removed. Returns self.

## repeated(n)

Returns a new sequence containing the receiver repeated n number of times.

## replaceFirstSeq(aSequence, anotherSequence,

optionalStartIndex) Returns a new Sequence with the first occurrence of
aSequence replaced with anotherSequence in the receiver. If
optionalStartIndex is provided, the search for aSequence begins at that
index. Returns self.

## replaceMap(aMap)

In the receiver, the keys of aMap replaced with its values. Returns self.

## replaceSeq(aSequence, anotherSequence)

Returns a new Sequence with all occurrences of aSequence
replaced with anotherSequence in the receiver. Returns self.

## reverse

Reverses the ordering of all the items of the receiver. Returns copy of receiver.

## reverseFindSeq(aSequence, startIndex)

Returns a number with the first occurrence of aSequence in
the receiver before the startIndex. The startIndex argument is optional.
By default reverseFind starts at the end of the string. Nil is
returned if no occurrences are found.

## reverseInPlace

Reverses the bytes in the receiver, in-place.

## rstrip(aSequence)

Strips the characters in
aSequence stripped from the end of the receiver. Example:
<pre>
"Cut the tail off" rstrip(" afilot")
==> "Cut the"
</pre>

## set(aNumber1, aNumber2, ...)

Sets the values of the receiver to the sequences of numbers in the
arguments. Unset values will remain unchanged. Returns self.

## setEncoding(encodingName)

Sets the encoding flag of the receiver (only the encoding flag,
itemSize and itemType will change, no conversion is done between UTF
encodings - you can use convertToUTF8, etc methods for conversions).
Valid encodings are number, utf8, utf16, and utf32. Returns self.

## setItemsToDouble(aNumber)

Sets all items in the Sequence to the double floating point value of
aNumber.

## setItemsToLong(aNumber)

Sets all items in the Sequence to the long integer value of aNumber.

## setItemType(aTypeName)

Sets the underlying machine type for the elements.
Valid names are uint8, uint16, uint32, uint64, int8, int16, int32,
int64, float32, and float64. Note that 64 bit types are only available
on platforms that support such types. Returns self.

## setSize(aNumber)

Sets the length in bytes of the receiver to aNumber. Return self.

## sin

Sets each value of the Sequence to the trigonometric sine of its value.
Returns self.

## sinh

Sets each value of the Sequence to the hyperbolic sine of its value.
Returns self.

## size

Returns the length in number of items (which may or may not
be the number of bytes, depending on the item type) of the receiver. For
example: <p> <pre> "abc" size == 3
</pre>

## sizeInBytes

Returns the length in bytes of the receiver.

## slice

Deprecated method. Use exSlice instead.

## slicesBetween(startSeq, endSeq)

Returns a list of slices delimited
by <tt>startSeq</tt> and <tt>endSeq</tt>.
<br>
<pre>
Io> "<a><b></b></a>" slicesBetween("<", ">")
==> list("a", "b", "/b", "/a")
</pre>

## sort

Sorts the characters/numbers in the array. Returns

## split Returns a list containing the sub-sequences of the receiver divided by the given arguments.

If no arguments are given the sequence is split on white space.

## split(optionalArg1, optionalArg2, ...)

Returns a list containing the sub-sequences of the receiver divided by the
given arguments. If no arguments are given the sequence is split on white
space. Examples: <pre> "a b c d" split == list("a", "b", "c", "d") "a*b*c*d"
split("*") == list("a", "b", "c", "d") "a*b|c,d" split("*", "|", ",") ==
list("a", "b", "c", "d") "a   b  c d" split == list("a", "", "", "", "b",
"", "", "c", "", "d")
</pre>

## splitAt(indexNumber)

Returns a list containing the two parts of the receiver as split at the
given index.

## splitNoEmpties(optionalArg1, optionalArg2, ...)

Returns a list containing the non-empty sub-sequences of the receiver divided by the given arguments.
If no arguments are given the sequence is split on white space.
Examples:
<code>
"a   b  c d" splitNoEmpties => list("a", "b", "c", "d")
"a***b**c*d" splitNoEmpties("*") => list("a", "b", "c", "d")
"a***b||c,d" splitNoEmpties("*", "|", ",") => list("a", "b", "c", "d")
</code>

## sqrt

Sets each value of the Sequence to the square root of its value.
Returns self.

## square

Sets each value of the Sequence to the square of its value.
Returns self.

## strip(optionalSequence)

Trims the whitespace (or optionalSequence) off both ends:
<p>
<pre>
"   Trim this string   \r\n" strip
==> "Trim this string"
</pre>

## sum

Returns the sum of the Sequence.

## tan

Sets each value of the Sequence to the trigonometric tangent of its value.
Returns self.

## tanh

Sets each value of the Sequence to the hyperbolic tangent of its value.
Returns self.

## toBase(aNumber)

Returns a Sequence containing the receiver (which is
assumed to be a base 10 number) converted to the specified base.

## translate(fromChars, toChars)

In the receiver, the characters in fromChars are replaced with those in the
same positions in toChars. Returns self.

## unescape

Escape codes replaced with escape characters. Returns self.

## unpack(optionalStartPosition, format)

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

## uppercase

Makes all characters of the receiver uppercase.

## urlDecoded

Returns url decoded version of receiver.

## urlEncoded

Returns url encoded version of receiver.

## whiteSpaceStrings

Returns a List of strings. Each string contains a different
whitespace character.

## with(aSequence, ...)

Returns a new Sequence which is the concatenation of the arguments.
The returned sequence will have the same mutability status as the receiver.

## withStruct(memberList)

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

