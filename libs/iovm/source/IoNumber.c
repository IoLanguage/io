
// metadoc Number category Core
// metadoc Number copyright Steve Dekorte 2002
// metadoc Number license BSD revised
/*metadoc Number description
A container for a double (a 64bit floating point number on most platforms).
*/

/*cmetadoc Number description
C implementation of Io's Number proto. A Number is an IoObject whose
inline data union stores a double — no separate heap payload, no
dataPointer. The CNUMBER / DATA macros just reinterpret the inline
slot, and IoNumber_free is a no-op because there is nothing to release.
Most arithmetic methods are thin shims that read the double out of
self (and any operand), compute, and return a freshly minted Number
via IONUMBER. Number is registered as a primitive proto on the state
so IONUMBER can clone without going through the message machinery.
Bitwise and character-class methods cast the double to long / int on
the fly; the limits methods expose the host's <limits.h> / <float.h>
constants.
*/

#include "IoNumber.h"
#include "IoObject.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoSeq.h"
#include "IoDate.h"
#include "IoState.h"

#include <ctype.h>
#include <assert.h>

#include <limits.h>

#include <float.h>

#ifndef log2
double log2(double n) { return log(n) / log(2); }
#endif

#define DATA(self) CNUMBER(self)

static const char *protoId = "Number";

/*cdoc Number IoNumber_numberForDouble_canUse_(self, n, other)
Returns one of the two supplied Numbers if its value already equals n,
otherwise allocates a fresh Number. Arithmetic helpers call this to
avoid minting a new IoObject for the common case where the result
coincides with an operand (e.g. x + 0).
*/
IoNumber *IoNumber_numberForDouble_canUse_(IoNumber *self, double n,
                                           IoNumber *other) {
    if (DATA(self) == n)
        return self;
    if (DATA(other) == n)
        return other;
    return IONUMBER(n);
}

/*cdoc Number IoNumber_newTag(state)
Builds the Number tag with clone/free/compare hooks. No stream read/write
hooks are installed — the commented block below shows the legacy
BStream-based serialization that was removed. The assert enforces that
a double fits into the two-pointer inline data slot that all IoObjects
reserve, which is what lets Number skip an allocated payload entirely.
*/
IoTag *IoNumber_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoNumber_rawClone);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoNumber_free);
    IoTag_compareFunc_(tag, (IoTagCompareFunc *)IoNumber_compare);
    // IoTag_writeToStreamFunc_(tag, (IoTagWriteToStreamFunc
    // *)IoNumber_writeToStream_); IoTag_readFromStreamFunc_(tag,
    // (IoTagReadFromStreamFunc *)IoNumber_readFromStream_);
    assert(sizeof(double) <= sizeof(void *) * 2);
    /*printf("Number tag = %p\n", (void *)tag);*/
    return tag;
}

/*
void IoNumber_writeToStream_(IoNumber *self, BStream *stream)
{
        BStream_writeTaggedDouble_(stream, DATA(self));
}

void *IoNumber_readFromStream_(IoNumber *self, BStream *stream)
{
        DATA(self) = BStream_readTaggedDouble(stream);
        return self;
}
*/

// #define IONUMBER_IS_MUTABLE

/*cdoc Number IoNumber_proto(state)
Creates the Number proto: allocates a bare IoObject, sets its tag, seeds
the inline double to zero, registers the proto on the state, and installs
the (large) method table of arithmetic, bitwise, character-class, and
limit methods. Every Number returned by IONUMBER is a clone of this proto.
*/
IoNumber *IoNumber_proto(void *state) {
    IoMethodTable methodTable[] = {
        {"asNumber", IoNumber_asNumber},
        {"+", IoNumber_add_},
        {"-", IoNumber_subtract},
        {"*", IoNumber_multiply},
        {"/", IoNumber_divide},
        //{"print", IoNumber_printNumber},

        {"asString", IoNumber_asString},
        {"asBuffer", IoNumber_asBuffer},
        {"asCharacter", IoNumber_asCharacter},
        {"asUint32Buffer", IoNumber_asUint32Buffer},
        //{"asDate", IoNumber_asDate},

        {"abs", IoNumber_abs},
        {"acos", IoNumber_acos},
        {"asin", IoNumber_asin},
        {"atan", IoNumber_atan},
        {"atan2", IoNumber_atan2},
        {"ceil", IoNumber_ceil},
        {"cos", IoNumber_cos},
        // {"deg", IoNumber_deg}
        {"exp", IoNumber_exp},
        {"factorial", IoNumber_factorial},
        {"floor", IoNumber_floor},
        {"log", IoNumber_log},
        {"log2", IoNumber_log2},
        {"log10", IoNumber_log10},
        {"max", IoNumber_max},
        {"min", IoNumber_min},
        {"%", IoNumber_mod},
        {"mod", IoNumber_mod},
        {"**", IoNumber_pow},
        {"pow", IoNumber_pow},
        {"round", IoNumber_round},
        {"roundDown", IoNumber_roundDown},
        {"sin", IoNumber_sin},
        {"sqrt", IoNumber_sqrt},
        {"squared", IoNumber_squared},
        {"cubed", IoNumber_cubed},
        {"tan", IoNumber_tan},
        {"toggle", IoNumber_toggle},

        // logic operations

        {"&", IoNumber_bitwiseAnd},
        {"|", IoNumber_bitwiseOr},
        {"^", IoNumber_bitwiseXor},
        {"<<", IoNumber_bitShiftLeft},
        {">>", IoNumber_bitShiftRight},

        {"bitwiseAnd", IoNumber_bitwiseAnd},
        {"bitwiseOr", IoNumber_bitwiseOr},
        {"bitwiseXor", IoNumber_bitwiseXor},
        {"bitwiseComplement", IoNumber_bitwiseComplement},
        {"shiftLeft", IoNumber_bitShiftLeft},
        {"shiftRight", IoNumber_bitShiftRight},

        // even and odd

        {"isEven", IoNumber_isEven},
        {"isOdd", IoNumber_isOdd},

        // character operations

        {"isAlphaNumeric", IoNumber_isAlphaNumeric},
        {"isLetter", IoNumber_isLetter},
        {"isControlCharacter", IoNumber_isControlCharacter},
        {"isDigit", IoNumber_isDigit},
        {"isGraph", IoNumber_isGraph},
        {"isLowercase", IoNumber_isLowercase},
        {"isUppercase", IoNumber_isUppercase},
        {"isPrint", IoNumber_isPrint},
        {"isPunctuation", IoNumber_isPunctuation},
        {"isSpace", IoNumber_isSpace},
        {"isHexDigit", IoNumber_isHexDigit},

        {"asLowercase", IoNumber_asLowercase},
        {"asUppercase", IoNumber_asUppercase},

        {"between", IoNumber_between},
        {"clip", IoNumber_clip},
        {"negate", IoNumber_negate},
        {"at", IoNumber_at},

        {"integerMax", IoNumber_integerMax},
        {"integerMin", IoNumber_integerMin},
        {"longMax", IoNumber_longMax},
        {"longMin", IoNumber_longMin},
        {"shortMax", IoNumber_shortMax},
        {"shortMin", IoNumber_shortMin},
        {"unsignedLongMax", IoNumber_unsignedLongMax},
        {"unsignedIntMax", IoNumber_unsignedIntMax},
        {"floatMax", IoNumber_floatMax},
        {"floatMin", IoNumber_floatMin},
        {"isNan", IoNumber_isNan},

        {"repeat", IoNumber_repeat},

        {NULL, NULL},
    };

    IoObject *self = IoObject_new(state);

    IoObject_tag_(self, IoNumber_newTag(state));
    DATA(self) = 0;
    IoState_registerProtoWithId_((IoState *)state, self, protoId);

    IoObject_addMethodTable_(self, methodTable);
    return self;
}

/*cdoc Number IoNumber_rawClone(proto)
Registered as the tag's cloneFunc. Uses the primitive clone path (no
slot hashtable copy) and memcopies the inline double from the proto.
Numbers have no heap payload, so this is cheap.
*/
IoNumber *IoNumber_rawClone(IoNumber *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    DATA(self) = DATA(proto);
    return self;
}

/*cdoc Number IoNumber_newWithDouble_(state, n)
Primary factory for Numbers. Normalizes every NaN to the canonical NAN
bit pattern (x86 emits positive and negative NaNs from different paths;
collapsing them keeps equality and hashing well-defined). The comment
hints at a potential optimization: because Numbers hold no references,
the full IOCLONE dance could in principle be shortened.
*/
IoNumber *IoNumber_newWithDouble_(void *state, double n) {
    // Normalize NAN, since there are positive and negative NANs
    if (isnan(n)) {
        n = NAN;
    }
    IoNumber *proto = IoState_protoWithId_((IoState *)state, protoId);
    IoNumber *self =
        IOCLONE(proto); // since Numbers have no refs, we can avoid IOCLONE
    DATA(self) = n;
    return self;
}

IoNumber *IoNumber_newCopyOf_(IoNumber *self) { return IONUMBER(DATA(self)); }

void IoNumber_copyFrom_(IoNumber *self, IoNumber *number) {
    DATA(self) = DATA(number);
}

/*cdoc Number IoNumber_free(self)
Registered as the tag's freeFunc. Body is empty: the double lives in
the inline data slot, so there is nothing to release. The function
exists only to override IoObject's default freer, which would try to
io_free a dataPointer that was never allocated.
*/
void IoNumber_free(IoNumber *self) {
    /* need this so Object won't try to io_free IoObject_dataPointer(self) */
}

/*cdoc Number IoNumber_asStackUArray(self)
Returns a stack-allocated UArray that aliases the receiver's inline
double — no copy, no heap. Used by code paths that want to treat a
Number as a 1-element numeric vector (comparisons, coercion into
Sequence ops). Caller must not mutate the view's data field.
*/
UArray IoNumber_asStackUArray(IoNumber *self) {
    UArray a = UArray_stackAllocedEmptyUArray();
    a.size = 1;

    if (sizeof(double) == 4) {
        a.itemType = CTYPE_float32_t;
        a.itemSize = 4;
    } else {
        a.itemType = CTYPE_float64_t;
        a.itemSize = 8;
    }

    a.data = (uint8_t *)(&DATA(self));
    return a;
}

int IoNumber_asInt(IoNumber *self) { return (int)(DATA(self)); }

long IoNumber_asLong(IoNumber *self) { return (long)(DATA(self)); }

float IoNumber_asFloat(IoNumber *self) { return (float)DATA(self); }

double IoNumber_asDouble(IoNumber *self) { return (double)DATA(self); }

/*cdoc Number IoNumber_compare(self, v)
Registered as the tag's compareFunc. Compares on numeric value when
v is another Number, otherwise delegates to IoObject_defaultCompare
(pointer-order fallback) so heterogeneous comparisons stay total for
sorting. NaN comparisons inherit IEEE semantics via ==/>.
*/
int IoNumber_compare(IoNumber *self, IoNumber *v) {
    if (ISNUMBER(v)) {
        if (DATA(self) == DATA(v)) {
            return 0;
        }
        return (DATA(self) > DATA(v)) ? 1 : -1;
    }
    return IoObject_defaultCompare(self, v);
}

/*cdoc Number IoNumber_Double_intoCString_(n, s, maxSize)
Formats a double into a fixed buffer. Integer values in long long range
print as integers (no trailing ".0"); very large or very small magnitudes
use %e; everything else is printed with 16-digit precision and then has
its trailing zeros — and trailing decimal point — trimmed in place.
Used by IoNumber_print and IoNumber_asAllocedCString.
*/
void IoNumber_Double_intoCString_(double n, char *s, size_t maxSize) {
    // Use long long (64-bit) so integers up to 2^53 (double's exact-integer
    // range) print without loss of precision.
    if (n >= (double)LLONG_MIN && n <= (double)LLONG_MAX &&
        n == (double)(long long)n) {
        snprintf(s, maxSize, "%lld", (long long)n);
    } else if (n > (double)LLONG_MAX || n < (double)LLONG_MIN) {
        snprintf(s, maxSize, "%e", n);
    } else {
        long l;

        snprintf(s, maxSize, "%.16f", n);

        // remove the trailing zeros ex: 10.00 -> 10

        l = (long)strlen(s) - 1;

        while (l > 0) {
            if (s[l] == '0') {
                s[l] = 0;
                l--;
                continue;
            }
            if (s[l] == '.') {
                s[l] = 0;
                l--;
                break;
            }
            break;
        }
    }
}

void IoNumber_print(IoNumber *self) {
    double d = DATA(self);
    char s[128];

    IoNumber_Double_intoCString_(d, s, 127);
    IoState_print_(IOSTATE, "%s", s);
}

// -----------------------------------------------------------

#include <arpa/inet.h>

// IO_METHOD(IoNumber, htonl)
//{
//    /* doc htonl
//    Returns a new number with the first 4 bytes of the receiver switched from
// host to network byte order.
//    */
//
//    IoNumber *num = IONUMBER(0);
//    IoObject_setDataUint32_(num, htonl(IoObject_dataUint32(self)));
//    return num;
//}

// IO_METHOD(IoNumber, ntohl)
//{
//    /* doc ntohl
// Returns a new number with the first 4 bytes of the receiver switched from
// network to host byte order.
//    */
//
//	IoNumber *num = IONUMBER(0);
//	IoObject_setDataUint32_(num, ntohl(IoObject_dataUint32(self)));
//	return num;
//}

// -----------------------------------------------------------

IO_METHOD(IoNumber, asNumber) {
    /*doc Number asNumber
    Returns self.
    */

    return self;
}

IO_METHOD(IoNumber, add_) {
    /*doc Number +(aNumber)
    Returns a new number that is the sum of the receiver and aNumber.
    */

    IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
    return IONUMBER(DATA(self) + DATA(other));
}

IO_METHOD(IoNumber, subtract) {
    /*doc Number -(aNumber)
    Returns a new number that is the difference of the receiver and aNumber.
    */

    IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
    return IONUMBER(DATA(self) - DATA(other));
}

IO_METHOD(IoNumber, divide) {
    /*doc Number /(aNumber)
    Returns a new number with the value of the receiver divided by aNumber.
    */

    IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
    return IONUMBER(DATA(self) / DATA(other));
}

IO_METHOD(IoNumber, multiply) {
    /*doc Number *(aNumber)
    Returns a new number that is the product of the receiver and aNumber.
    */

    IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
    return IONUMBER(DATA(self) * DATA(other));
}

/*cdoc Number IoNumber_asAllocedCString(self)
Returns a heap-allocated zero-padded string representation of the
receiver (caller owns it and must io_free). Sized at 1024 bytes to
cover the worst case of a small whole field width combined with a
fully expanded %.16f. Used by printNumber and justAsString.
*/
char *IoNumber_asAllocedCString(IoNumber *self) {
    int size = 1024;
    char *s = (char *)io_calloc(1, size);
    memset(s, 0, size);
    IoNumber_Double_intoCString_(DATA(self), s, size - 1);
    return s;
}

IO_METHOD(IoNumber, printNumber) {
    /*doc Number print
    Prints the number.
    */

    char *s = IoNumber_asAllocedCString(self);
    IoState_print_((IoState *)IOSTATE, s);
    io_free(s);
    return self;
}

IO_METHOD(IoNumber, justAsString) {
    IoSymbol *string;
    char *s = IoNumber_asAllocedCString(self);
    string = IoSeq_newWithCString_((IoState *)IOSTATE, s);
    io_free(s);
    return string;
}

/*cdoc Number countBytes(ld)
Returns the minimum number of bytes needed to hold ld when shifted
right 8 bits at a time. Used by asCharacter to pick a UCS encoding
(1, 2, or 4 bytes) for the resulting single-character Sequence.
*/
static int countBytes(long ld) {
    int n = 1;
    for (;;) {
        ld >>= 8;
        if (ld == 0) {
            return n;
        }
        n++;
    }
}

IO_METHOD(IoNumber, asCharacter) {
    /*doc Number asCharacter
    Returns a String containing a single character whose
    value is the value of the first byte of the receiver.
    Returns nil if the number has no valid UCS mapping.
    */

    double d = DATA(self);
    long ld = d;

    if (d < 0 || d != ld) {
        return IONIL(self);
    } else {
        uint32_t i = io_uint32InBigEndian((uint32_t)d);
        int bytes = countBytes(ld);
        IoSeq *s;

        if (bytes == 0) {
            bytes = 1;
        }

        if (bytes == 3) {
            bytes = 4;
        }

        if (bytes > 4) {
            // no valid UCS encoding for this value
            return IONIL(self);
        }

        s = IoSeq_newWithData_length_(IOSTATE, (unsigned char *)&i, bytes);

        {
            UArray *u = IoSeq_rawUArray(s);
            int e = CENCODING_ASCII;

            switch (bytes) {
            case 1:
                e = CENCODING_ASCII;
                break;
            case 2:
                e = CENCODING_UCS2;
                break;
            case 4:
                e = CENCODING_UCS4;
                break;
            }

            UArray_setEncoding_(u, e);
        }

        return s;
    }
}

IO_METHOD(IoNumber, asUint32Buffer) {
    /*doc Number asUint32Buffer
    Returns a Sequence containing a 4 byte representation of the uint32 value of
    the receiver.
    */

    uint32_t i = (int)DATA(self);
    return IoSeq_newWithData_length_(IOSTATE, (unsigned char *)&i,
                                     sizeof(uint32_t));
}

IO_METHOD(IoNumber, asBuffer) {
    /*doc Number asBuffer(optionalNumberOfBytes)
    Returns a Buffer containing a the number of bytes specified by
    optionalNumberOfBytes (up to the size of a double on the platform) of the
    receiver. If no optionalNumberOfBytes is specified, it is assumed to be the
    number of bytes in a double on the host platform.
    */
    IoNumber *byteCount = IoMessage_locals_valueArgAt_(m, locals, 0);
    int bc = sizeof(double);

    if (!ISNIL(byteCount)) {
        bc = DATA(byteCount);
    }
    return IoSeq_newWithData_length_(IOSTATE, (unsigned char *)&(DATA(self)),
                                     bc);
}

IO_METHOD(IoNumber, asString) {
    /*doc Number asString(optionalIntegerDigits, optionalFactionDigits)
    Returns a string representation of the receiver. For example:
    <pre>
    1234.5678 asString(0, 2)
    </pre>
    would return:
    <pre>
    1234.57
    </pre>
    */

    if (IoMessage_argCount(m) >= 1) {
        int whole = IoMessage_locals_intArgAt_(m, locals, 0);
        if (IOSTATE->errorRaised) return IONIL(self);
        int part = 6;
        char *s;
        size_t length;
        IoObject *n;

        if (IoMessage_argCount(m) >= 2) {
            part = abs(IoMessage_locals_intArgAt_(m, locals, 1));
            if (IOSTATE->errorRaised) return IONIL(self);
        }

        part = abs(part);
        whole = abs(whole);

        // If whole == 0, printf might need an arbitary size string. Instead of
        // second guessing the size, pick a really big size: 1024.
        length = 1024;
        s = io_calloc(1, length);

        snprintf(s, length, "%*.*f", whole, part, DATA(self));

        n = IOSEQ((unsigned char *)s, (size_t)strlen(s));

        io_free(s);

        return n;
    }

    return IoNumber_justAsString(self, locals, m);
}

/*
IO_METHOD(IoNumber, asDate)
{
        return IoDate_newWithNumber_((IoState *)IOSTATE, DATA(self));
}
*/

IO_METHOD(IoNumber, abs) {
    /*doc Number abs
    Returns a number with the absolute value of the receiver.
    */

    return (DATA(self) < 0) ? (IoObject *)IONUMBER(-DATA(self))
                            : (IoObject *)self;
}

IO_METHOD(IoNumber, acos) {
    /*doc Number acos
    Returns a number with the arc cosine of the receiver.
    */

    return IONUMBER(acos(DATA(self)));
}

IO_METHOD(IoNumber, asin) {
    /*doc Number asin
    Returns a number with the arc sine of the receiver.
    */

    return IONUMBER(asin(DATA(self)));
}

IO_METHOD(IoNumber, atan) {
    /*doc Number atan
    Returns a number with the arc tangent of the receiver.
    */

    return IONUMBER(atan(DATA(self)));
}

IO_METHOD(IoNumber, atan2) {
    /*doc Number atan2(aNumber)
    Returns a number with the arc tangent of y/x where y is the receiver and x
    is aNumber.
    */

    IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
    return IONUMBER(atan2(DATA(self), DATA(other)));
}

IO_METHOD(IoNumber, ceil) {
    /*doc Number ceil
    Returns a number with the receiver's value rounded up to
    the nearest integer if its fractional component is greater than 0.
    */

    return IONUMBER(ceil(DATA(self)));
}

IO_METHOD(IoNumber, cos) {
    /*doc Number cos
    Returns the cosine of the receiver.
    */

    return IONUMBER(cos(DATA(self)));
}

/*
IO_METHOD(IoNumber, deg)
{
        return IONUMBER(deg(DATA(self)));
}
*/

IO_METHOD(IoNumber, exp) {
    /*doc Number exp
    Returns e to the power of the receiver.
    */

    return IONUMBER(exp(DATA(self)));
}

IO_METHOD(IoNumber, factorial) {
    /*doc Number factorial
    Returns the factorial of the receiver.
    */

    int n = DATA(self);
    double v = 1;
    while (n) {
        v *= n;
        n--;
    }
    return IONUMBER(v);
}

IO_METHOD(IoNumber, floor) {
    /*doc Number floor
    Returns a number with the receiver's value rounded
    down to the nearest integer if its fractional component is not 0.
    */

    return IONUMBER(floor(DATA(self)));
}

IO_METHOD(IoNumber, log) {
    /*doc Number log
    Returns the logarithm of the receiver.  The base
    is taken as the value of the first argument or the constant e if
    the first argument is omitted.
    */

    float base;
    if (IoMessage_argCount(m) > 0) {
        base = DATA(IoMessage_locals_numberArgAt_(m, locals, 0));
    } else {
        base = (float)M_E;
    }
    return IONUMBER(log(DATA(self)) / log(base));
}

IO_METHOD(IoNumber, log2) {
    /*doc Number log2
    Returns the base 2 logarithm of the receiver.
    */

    return IONUMBER(log2(DATA(self)));
}

IO_METHOD(IoNumber, log10) {
    /*doc Number log10
    Returns the base 10 logarithm of the receiver.
    */

    return IONUMBER(log10(DATA(self)));
}

IO_METHOD(IoNumber, max) {
    /*doc Number max(aNumber)
    Returns the greater of the receiver and aNumber.
    */

    IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
    return (DATA(self) > DATA(other)) ? (IoObject *)self : (IoObject *)other;
}

IO_METHOD(IoNumber, min) {
    /*doc Number min(aNumber)
    Returns the lesser of the receiver and aNumber.
    */

    IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
    return (DATA(self) < DATA(other)) ? (IoObject *)self : (IoObject *)other;
}

IO_METHOD(IoNumber, mod) {
    /*doc Number %(aNumber)
    Returns the receiver modulus aNumber.
    */

    /*doc Number mod(aNumber)
    Returns the receiver modulus aNumber.
    */

    IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
    return IONUMBER(fmod(DATA(self), DATA(other)));
}

/*
IO_METHOD(IoNumber, modf)
{
        IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
        if (DATA(self) < DATA(other)); return self;
        return other;
}

IO_METHOD(IoNumber, rad)
*/

IO_METHOD(IoNumber, pow) {
    /*doc Number pow(aNumber)
    Returns the value of the receiver to the aNumber power.
    */

    /*doc Number **(aNumber)
    Same as pow(aNumber).
    */

    IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
    return IONUMBER(pow(DATA(self), DATA(other)));
}

IO_METHOD(IoNumber, round) {
    /*doc Number round
    Returns a number with the receiver's value rounded up to
    the nearest integer if its fraction component is >= .5 or rounded up to the
    nearest integer otherwise.
    */

    double x = DATA(self);
    if (x < 0.0)
        return IONUMBER(ceil(x - 0.5));
    else
        return IONUMBER(floor(x + 0.5));
}

IO_METHOD(IoNumber, roundDown) {
    /*doc Number roundDown
    Returns a number with the receiver's value rounded down to
    the nearest integer if its fraction component is <= .5 or rounded up the the
    nearest integer otherwise.
    */

    return IONUMBER(floor(DATA(self) + 0.5));
}

IO_METHOD(IoNumber, sin) {
    /*doc Number sin
    Returns the sine of the receiver.
    */

    return IONUMBER(sin(DATA(self)));
}

IO_METHOD(IoNumber, sqrt) {
    /*doc Number sqrt
    Returns the square root of the receiver.
    */

    return IONUMBER(sqrt(DATA(self)));
}

IO_METHOD(IoNumber, squared) {
    /*doc Number squared
    Returns the square of the receiver.
    */

    double v = DATA(self);
    return IONUMBER(v * v);
}

IO_METHOD(IoNumber, cubed) {
    /*doc Number cubed
    Returns the cube of the receiver.
    */

    double v = DATA(self);
    return IONUMBER(v * v * v);
}

IO_METHOD(IoNumber, tan) {
    /*doc Number tan
    Returns the tangent of the receiver.
    */

    return IONUMBER(tan(DATA(self)));
}

/*
IO_METHOD(IoNumber, frexp)
{
        return IONUMBER( frexp(DATA(self)) );
}

IO_METHOD(IoNumber, ldexp)
{
        return IONUMBER( ldexp(DATA(self)) );
}
*/

IO_METHOD(IoNumber, toggle) {
    /*doc Number toggle
    Returns 1 if the receiver is 0. Returns 0 otherwise.
    */

    return (DATA(self)) ? (IoObject *)IONUMBER(0) : (IoObject *)IONUMBER(1);
}

/* --- bitwise operations ---------------------------------------- */

IO_METHOD(IoNumber, bitwiseAnd) {
    /*doc Number &(aNumber)
    Returns a new number with the bitwise AND of the receiver and aNumber.
    */

    /*doc Number bitwiseAnd(aNumber)
    Returns a new number with the bitwise AND of the receiver and aNumber.
    */

    long other = IoMessage_locals_longArgAt_(m, locals, 0);
    return IONUMBER(((long)DATA(self) & other));
}

IO_METHOD(IoNumber, bitwiseOr) {
    /*doc Number |(aNumber)
    Returns a new number with the bitwise OR of the receiver and aNumber.
    */

    /*doc Number bitwiseOr(aNumber)
    Returns a new number with the bitwise AND of the receiver and aNumber.
    */

    long other = IoMessage_locals_longArgAt_(m, locals, 0);
    long n = DATA(self);
    long r = n | other;
    return IONUMBER(r);
}

IO_METHOD(IoNumber, bitwiseXor) {
    /*doc Number bitwiseXor(aNumber)
    Returns a new number with the bitwise XOR of the receiver and aNumber.
    */

    /*doc Number ^(aNumber)
    Returns the bitwise xor with the receiver (both numbers are converted to
    longs for the operation).
    */

    long other = IoMessage_locals_longArgAt_(m, locals, 0);
    long r = (double)((long)DATA(self) ^ other);
    return IONUMBER(r);
}

IO_METHOD(IoNumber, bitwiseComplement) {
    /*doc Number bitwiseComplement
    Returns a new number with the bitwise complement of the
    receiver. (The 0 bits become 1s and the 1 bits become 0s. )
    */

    long r = (double)(~(long)DATA(self));
    return IONUMBER(r);
}

IO_METHOD(IoNumber, bitShiftLeft) {
    /*doc Number <<(aNumber)
    Shifts the bits of the receiver left by the number of places specified by
    aNumber.
    */

    long other = IoMessage_locals_longArgAt_(m, locals, 0);
    long r = (double)((long)DATA(self) << other);
    return IONUMBER(r);
}

IO_METHOD(IoNumber, bitShiftRight) {
    /*doc Number >>(aNumber)
    Shifts the bits of the receiver right by the number of places specified by
    aNumber.
    */

    long other = IoMessage_locals_longArgAt_(m, locals, 0);
    long r = (double)((long)DATA(self) >> (long)other);
    return IONUMBER(r);
}

// even and odd ------------------------------

IO_METHOD(IoNumber, isEven) {
    /*doc Number isEven
    Returns true if
    integer form of the receiver is even
    , false otherwise.
    */

    int n = DATA(self);
    return IOBOOL(self, 0 == (n & 0x01));
}

IO_METHOD(IoNumber, isOdd) {
    /*doc Number isOdd
    Returns true if
    integer form of the receiver is odd
    , false otherwise.
    */

    int n = DATA(self);
    return IOBOOL(self, 0x01 == (n & 0x01));
}

// character operations ---------------------------------

IO_METHOD(IoNumber, isAlphaNumeric) {
    /*doc Number isAlphaNumeric
    Returns true if
    receiver is an alphanumeric character value
    , false otherwise.
    */

    return IOBOOL(self, isalnum((int)DATA(self)));
}

IO_METHOD(IoNumber, isLetter) {
    /*doc Number isLetter
    Returns true if
    receiver is a letter character value
    , false otherwise.
    */

    return IOBOOL(self, isalpha((int)DATA(self)));
}

IO_METHOD(IoNumber, isControlCharacter) {
    /*doc Number isControlCharacter
    Returns true if
    receiver is a control character value
    , false otherwise.
    */

    return IOBOOL(self, iscntrl((int)DATA(self)));
}

IO_METHOD(IoNumber, isDigit) {
    /*doc Number isDigit
    Returns true if
    receiver is a numeric digit value
    , false otherwise.
    */

    return IOBOOL(self, isdigit((int)DATA(self)));
}

IO_METHOD(IoNumber, isGraph) {
    /*doc Number isGraph
    Returns true if
    the receiver is a printing character value except space
    , false otherwise.
    */

    return IOBOOL(self, isgraph((int)DATA(self)));
}

IO_METHOD(IoNumber, isLowercase) {
    /*doc Number isLowercase
    Returns true if
    the receiver is a lowercase character value
    , false otherwise.
    */

    return IOBOOL(self, islower((int)DATA(self)));
}

IO_METHOD(IoNumber, isUppercase) {
    /*doc Number isUppercase
    Returns true if
    the receiver is a uppercase character value
    , false otherwise.
    */

    return IOBOOL(self, isupper((int)DATA(self)));
}

IO_METHOD(IoNumber, isPrint) {
    /*doc Number isPrint
    Returns true if
    the receiver is a printing character value, including space
    , false otherwise.
    */

    return IOBOOL(self, isprint((int)DATA(self)));
}

IO_METHOD(IoNumber, isPunctuation) {
    /*doc Number isPunctuation
    Returns true if
    the receiver is a punctuation character value
    , false otherwise.

    */

    return IOBOOL(self, ispunct((int)DATA(self)));
}

IO_METHOD(IoNumber, isSpace) {
    /*doc Number isSpace
    Returns true if
    the receiver is a space, formfeed, newline carriage return, tab or vertical
    tab character value , false otherwise.
    */

    return IOBOOL(self, isspace((int)DATA(self)));
}

IO_METHOD(IoNumber, isHexDigit) {
    /*doc Number isHexDigit
    Returns true if
    the receiver is a hexadecimal character value
    , false otherwise.
    */

    return IOBOOL(self, isxdigit((int)DATA(self)));
}

// case ---------------------------------

IO_METHOD(IoNumber, asLowercase) {
    /*doc Number asLowercase
    Returns a new Number containing a lower case version of the receiver.
    */

    int r = tolower((int)DATA(self));
    return IONUMBER(r);
}

IO_METHOD(IoNumber, asUppercase) {
    /*doc Number asUppercase
    Returns a new Number containing a upper case version of the receiver.
    */

    int r = toupper((int)DATA(self));
    return IONUMBER(r);
}

IO_METHOD(IoNumber, between) {
    /*doc Number between(aNumber1, aNumber2)
    Returns true if the receiver's value is between or
    equal to aNumber1 and aNumber2, otherwise returns false.
    */

    double a = IoMessage_locals_doubleArgAt_(m, locals, 0);
    if (IOSTATE->errorRaised) return IONIL(self);
    double b = IoMessage_locals_doubleArgAt_(m, locals, 1);
    if (IOSTATE->errorRaised) return IONIL(self);
    double n = DATA(self);

    return IOBOOL(self, ((n >= a) && (n <= b)) || (n <= a && (n >= b)));
}

IO_METHOD(IoNumber, clip) {
    /*doc Number clip(aNumber1, aNumber2)
    Returns self if the receiver is between aNumber1 and aNumber2.
    Returns aNumber1 if it is less than aNumber1. Returns aNumber2 if it is
    greater than aNumber2.
    */

    double a = IoMessage_locals_doubleArgAt_(m, locals, 0);
    if (IOSTATE->errorRaised) return IONIL(self);
    double b = IoMessage_locals_doubleArgAt_(m, locals, 1);
    if (IOSTATE->errorRaised) return IONIL(self);
    double n = DATA(self);

    if (n < a)
        n = a;
    if (n > b)
        n = b;

    return IONUMBER(n);
}

IO_METHOD(IoNumber, negate) {
    /*doc Number negate
    Returns new number that is negated version of the receiver.
    */

    return IONUMBER(-DATA(self));
}

IO_METHOD(IoNumber, at) {
    /*doc Number at(bitIndexNumber)
    Returns a new Number containing 1 if the receiver cast to a long
    has its bit set to 1 at bitIndexNumber. Otherwise returns 0.
    */

    int i = IoMessage_locals_intArgAt_(m, locals, 0);
    long l = (long)DATA(self);

    IOASSERT((i >= 0) && (i < sizeof(double) * 8), "index out of bit bounds");

    l = l >> i;
    l = l & 0x1;
    return IONUMBER(l);
}

// limits ------------------------------------

IO_METHOD(IoNumber, integerMax) {
    /*doc Number integerMax
    Returns the maximum integer value.
    */

    return IONUMBER(INT_MAX);
}

IO_METHOD(IoNumber, integerMin) {
    /*doc Number integerMin
    Returns the minimum integer value.
    */

    return IONUMBER(INT_MIN);
}

IO_METHOD(IoNumber, longMax) {
    /*doc Number longMax
    Returns the maximum long value.
    */

    return IONUMBER(LONG_MAX);
}

IO_METHOD(IoNumber, longMin) {
    /*doc Number longMin
    Returns the minimum long value.
    */

    return IONUMBER(LONG_MIN);
}

IO_METHOD(IoNumber, shortMax) {
    /*doc Number shortMax
    Returns the maximum short value.
    */

    return IONUMBER(SHRT_MAX);
}

IO_METHOD(IoNumber, shortMin) {
    /*doc Number shortMin
    Returns the minimum short value.
    */

    return IONUMBER(SHRT_MIN);
}

IO_METHOD(IoNumber, unsignedLongMax) {
    /*doc Number unsignedLongMax
    Returns the maximum unsigned long value.
    */

    return IONUMBER(ULONG_MAX);
}

IO_METHOD(IoNumber, unsignedIntMax) {
    /*doc Number unsignedIntMax
    Returns the maximum unsigned int value.
    */

    return IONUMBER(UINT_MAX);
}

IO_METHOD(IoNumber, unsignedShortMax) {
    /*doc Number unsignedShortMax
    Returns the minimum unsigned int value.
    */

    return IONUMBER(USHRT_MAX);
}

IO_METHOD(IoNumber, floatMax) {
    /*doc Number floatMax
    Returns the maximum float value.
    */

    return IONUMBER(FLT_MAX);
}

IO_METHOD(IoNumber, floatMin) {
    /*doc Number floatMin
    Returns the minimum float value.
    */

    return IONUMBER(FLT_MIN);
}

IO_METHOD(IoNumber, doubleMax) {
    /*doc Number floatMax
    Returns the maximum double precision float value.
    */

    return IONUMBER(DBL_MAX);
}

IO_METHOD(IoNumber, doubleMin) {
    /*doc Number doubleMin
    Returns the minimum double precision float value.
    */

    return IONUMBER(DBL_MIN);
}

IO_METHOD(IoNumber, isNan) {
    /*doc Number isNan
    Returns true if the receiver is not a number. Otherwise returns false.
    */

    return IOBOOL(self, isnan(CNUMBER(self)));
}

// looping ---------------------------------------------

IO_METHOD(IoNumber, repeat) {
    /*doc Number repeat(optionalIndex, expression)
    Evaluates message a number of times that corresponds to the receivers
    integer value. This is significantly faster than a for() or while() loop.
    */

    IoMessage_assertArgCount_receiver_(m, 1, self);
    if (IOSTATE->errorRaised) return IONIL(self);

    {
        IoState *state = IOSTATE;
        IoSymbol *indexSlotName;
        IoMessage *doMessage;
        double i, max = CNUMBER(self);
        IoObject *result = IONIL(self);

        if (IoMessage_argCount(m) > 1) {
            indexSlotName = IoMessage_name(IoMessage_rawArgAt_(m, 0));
            doMessage = IoMessage_rawArgAt_(m, 1);
        } else {
            indexSlotName = 0;
            doMessage = IoMessage_rawArgAt_(m, 0);
        }

        IoState_pushRetainPool(state);

        for (i = 0; i < max; i++) {
            /*
            if (result != locals && result != self)
            {
                    IoState_immediatelyFreeIfUnreferenced_(state, result);
            }
            */

            IoState_clearTopPool(state);

            if (indexSlotName) {
                IoObject_setSlot_to_(locals, indexSlotName, IONUMBER(i));
            }

            result = IoMessage_locals_performOn_(doMessage, locals, locals);

            if (IoState_handleStatus(IOSTATE)) {
                break;
            }
        }

        IoState_popRetainPoolExceptFor_(IOSTATE, result);
        return result;
    }
}
