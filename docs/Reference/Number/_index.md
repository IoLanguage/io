# Number

A container for a double (a 64bit floating point number on most platforms).

## -(aNumber)

Returns a new number that is the difference of the receiver and aNumber.

## /(aNumber)

Returns a new number with the value of the receiver divided by aNumber.

## &(aNumber)

Returns a new number with the bitwise AND of the receiver and aNumber.

## %(aNumber)

Returns the receiver modulus aNumber.

## ^(aNumber)

Returns the bitwise xor with the receiver (both numbers are converted to
longs for the operation).

## +(aNumber)

Returns a new number that is the sum of the receiver and aNumber.

## <<(aNumber)

Shifts the bits of the receiver left by the number of places specified by
aNumber.

## >>(aNumber)

Shifts the bits of the receiver right by the number of places specified by
aNumber.

## |(aNumber)

Returns a new number with the bitwise OR of the receiver and aNumber.

## abs

Returns a number with the absolute value of the receiver.

## acos

Returns a number with the arc cosine of the receiver.

## asBinary

Returns the number as binary digits inside a string. 42 asBinary -> "101010"

## asBuffer(optionalNumberOfBytes)

Returns a Buffer containing a the number of bytes specified by
optionalNumberOfBytes (up to the size of a double on the platform) of the
receiver. If no optionalNumberOfBytes is specified, it is assumed to be the
number of bytes in a double on the host platform.

## asCharacter

Returns a String containing a single character whose
value is the value of the first byte of the receiver.
Returns nil if the number has no valid UCS mapping.

## asHex

Returns the number as hex digits inside a string. 97 asHex -> "61"

## asin

Returns a number with the arc sine of the receiver.

## asLowercase

Returns a new Number containing a lower case version of the receiver.

## asNumber

Returns self.

## asOctal

Returns the number as octal digits inside a string. 436 asOctal -> "664"

## asString(optionalIntegerDigits, optionalFactionDigits)

Returns a string representation of the receiver. For example:
<pre>
1234.5678 asString(0, 2)
</pre>
would return:
<pre>
1234.57
</pre>

## asUint32Buffer

Returns a Sequence containing a 4 byte representation of the uint32 value of
the receiver.

## asUppercase

Returns a new Number containing a upper case version of the receiver.

## at(bitIndexNumber)

Returns a new Number containing 1 if the receiver cast to a long
has its bit set to 1 at bitIndexNumber. Otherwise returns 0.

## atan

Returns a number with the arc tangent of the receiver.

## atan2(aNumber)

Returns a number with the arc tangent of y/x where y is the receiver and x
is aNumber.

## between(aNumber1, aNumber2)

Returns true if the receiver's value is between or
equal to aNumber1 and aNumber2, otherwise returns false.

## bitwiseAnd(aNumber)

Returns a new number with the bitwise AND of the receiver and aNumber.

## bitwiseComplement

Returns a new number with the bitwise complement of the
receiver. (The 0 bits become 1s and the 1 bits become 0s. )

## bitwiseOr(aNumber)

Returns a new number with the bitwise AND of the receiver and aNumber.

## bitwiseXor(aNumber)

Returns a new number with the bitwise XOR of the receiver and aNumber.

## ceil

Returns a number with the receiver's value rounded up to
the nearest integer if its fractional component is greater than 0.

## clip(aNumber1, aNumber2)

Returns self if the receiver is between aNumber1 and aNumber2.
Returns aNumber1 if it is less than aNumber1. Returns aNumber2 if it is
greater than aNumber2.

## combinations(size)

Returns the combinations where the receiver is the number of different objects and size is the number to be arranged.

## constants

Object containing number constants e, inf, nan and pi.

## cos

Returns the cosine of the receiver.

## cubed

Returns the cube of the receiver.

## doubleMin

Returns the minimum double precision float value.

## e

Returns the constant e.

## exp

Returns e to the power of the receiver.

## factorial

Returns the factorial of the receiver.

## floatMax

Returns the maximum double precision float value.

## floatMin

Returns the minimum float value.

## floor

Returns a number with the receiver's value rounded
down to the nearest integer if its fractional component is not 0.

## inf

Returns a not-a-number constant.

## integerMax

Returns the maximum integer value.

## integerMin

Returns the minimum integer value.

## isAlphaNumeric

Returns true if
receiver is an alphanumeric character value
, false otherwise.

## isControlCharacter

Returns true if
receiver is a control character value
, false otherwise.

## isDigit

Returns true if
receiver is a numeric digit value
, false otherwise.

## isEven

Returns true if
integer form of the receiver is even
, false otherwise.

## isGraph

Returns true if
the receiver is a printing character value except space
, false otherwise.

## isHexDigit

Returns true if
the receiver is a hexadecimal character value
, false otherwise.

## isInASequenceSet

Return true if receiver is in one of the Sequence sequenceSets, otherwise false.

## isLetter

Returns true if
receiver is a letter character value
, false otherwise.

## isLowercase

Returns true if
the receiver is a lowercase character value
, false otherwise.

## isNan

Returns true if the receiver is not a number. Otherwise returns false.

## isOdd

Returns true if
integer form of the receiver is odd
, false otherwise.

## isPrint

Returns true if
the receiver is a printing character value, including space
, false otherwise.

## isPunctuation

Returns true if
the receiver is a punctuation character value
, false otherwise.

## isSpace

Returns true if
the receiver is a space, formfeed, newline carriage return, tab or vertical
tab character value , false otherwise.

## isUppercase

Returns true if
the receiver is a uppercase character value
, false otherwise.

## justSerialized(stream)

Writes the receiver's code into the stream.

## log

Returns the logarithm of the receiver.  The base
is taken as the value of the first argument or the constant e if
the first argument is omitted.

## log10

Returns the base 10 logarithm of the receiver.

## log2

Returns the base 2 logarithm of the receiver.

## longMax

Returns the maximum long value.

## longMin

Returns the minimum long value.

## max(aNumber)

Returns the greater of the receiver and aNumber.

## min(aNumber)

Returns the lesser of the receiver and aNumber.

## minMax(low,

high) Returns a number between or equal to low and high. If the receiver is equal to or between low and high, the receiver is returned. If the receiver is less than low, low is returned. If the receiver is greater than high, high is returned.

## mod(aNumber)

Returns the receiver modulus aNumber.

## nan

Returns a infinity constant.

## negate

Returns new number that is negated version of the receiver.

## permutations(size)

Returns the permutations where the receiver is the number of different objects and size is the number to be arranged.

## pi

Returns the constant pi.

## pow(aNumber)

Returns the value of the receiver to the aNumber power.

## print

Prints the number.

## repeat(optionalIndex, expression)

Evaluates message a number of times that corresponds to the receivers
integer value. This is significantly faster than a for() or while() loop.

## round

Returns a number with the receiver's value rounded up to
the nearest integer if its fraction component is >= .5 or rounded up to the
nearest integer otherwise.

## roundDown

Returns a number with the receiver's value rounded down to
the nearest integer if its fraction component is <= .5 or rounded up the the
nearest integer otherwise.

## shortMax

Returns the maximum short value.

## shortMin

Returns the minimum short value.

## sin

Returns the sine of the receiver.

## sqrt

Returns the square root of the receiver.

## squared

Returns the square of the receiver.

## tan

Returns the tangent of the receiver.

## toBase(base)

Returns the number in another base. 42 toBase(2) -> "101010"

## toBaseWholeBytes(base)

Returns the number in another base printing out entire bytes. 42 toBaseWholeBytes(2) -> "00101010"

## toggle

Returns 1 if the receiver is 0. Returns 0 otherwise.

## unsignedIntMax

Returns the maximum unsigned int value.

## unsignedLongMax

Returns the maximum unsigned long value.

## unsignedShortMax

Returns the minimum unsigned int value.

