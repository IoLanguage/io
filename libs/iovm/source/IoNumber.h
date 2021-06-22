
// metadoc Number copyright Steve Dekorte 2002
// metadoc Number license BSD revised

#ifndef IONUMBER_DEFINED
#define IONUMBER_DEFINED 1

#include <math.h>
#define _GNU_SOURCE // for NAN macro, round
#include <sys/types.h>

#include "IoVMApi.h"

#include "Common.h"
#include "IoState.h"
#include "IoObject.h"

#ifdef __cplusplus
extern "C" {
#endif

/* NAN! x86 hardware happens to set the sign bit for all those 0/0 divisions.
 * So 0/0, -0/0, 0/-0, -0/-0 all give -NaN.
 */
#if defined(USE_BUILTIN_NAN) || !defined(NAN)
static union {
    unsigned long long __c;
    double __d;
} __nan_union = {0x7ff8000000000000};
#undef NAN
#define NAN (__nan_union.__d)
#endif

#if !defined(isnan) && defined(_MSC_VER)
#define isnan _isnan
#endif

#define ISNUMBER(self)                                                         \
    IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoNumber_rawClone)
#define IONUMBER(num) IoState_numberWithDouble_((IoState *)IOSTATE, (double)num)
#define CNUMBER(self) IoObject_dataDouble((IoNumber *)self)

#if defined WIN32 && !defined __MINGW32__
#define log2(num) log(num) / log(2)
#endif

typedef IoObject IoNumber;

IOVM_API IoNumber *IoNumber_proto(void *state);
IOVM_API IoNumber *IoNumber_rawClone(IoNumber *self);
IOVM_API IoNumber *IoNumber_newWithDouble_(void *state, double n);
IOVM_API IoNumber *IoNumber_newCopyOf_(IoNumber *number);
IOVM_API void IoNumber_copyFrom_(IoNumber *self, IoNumber *number);

// IOVM_API void IoNumber_writeToStream_(IoNumber *self, BStream *stream);
// IOVM_API void *IoNumber_readFromStream_(IoNumber *self, BStream *stream);

IOVM_API void IoNumber_free(IoNumber *self);

IOVM_API UArray IoNumber_asStackUArray(IoNumber *self);
IOVM_API int IoNumber_asInt(IoNumber *self);
IOVM_API long IoNumber_asLong(IoNumber *self);
IOVM_API double IoNumber_asDouble(IoNumber *self);
IOVM_API float IoNumber_asFloat(IoNumber *self);
IOVM_API int IoNumber_compare(IoNumber *self, IoNumber *v);
IOVM_API void IoNumber_print(IoNumber *self);
// void IoNumber_rawSet(IoNumber *self, double v);
#define IoNumber_rawSet(self, v) CNUMBER(self) = v;

// -----------------------------------------------------------

IOVM_API IO_METHOD(IoNumber, htonl);
IOVM_API IO_METHOD(IoNumber, ntohl);

IOVM_API IO_METHOD(IoNumber, asNumber);
IOVM_API IO_METHOD(IoNumber, add_);
IOVM_API IO_METHOD(IoNumber, subtract);
IOVM_API IO_METHOD(IoNumber, divide);
IOVM_API IO_METHOD(IoNumber, multiply);
IOVM_API IO_METHOD(IoNumber, printNumber);

IOVM_API IO_METHOD(IoNumber, asBuffer);
IOVM_API IO_METHOD(IoNumber, asString);
IOVM_API IO_METHOD(IoNumber, justAsString);
IOVM_API IO_METHOD(IoNumber, asCharacter);
IOVM_API IO_METHOD(IoNumber, asUint32Buffer);
// IO_METHOD(IoNumber, asDate);

// no arg methods

IOVM_API IO_METHOD(IoNumber, abs);
IOVM_API IO_METHOD(IoNumber, acos);
IOVM_API IO_METHOD(IoNumber, asin);
IOVM_API IO_METHOD(IoNumber, atan);
IOVM_API IO_METHOD(IoNumber, atan2);
IOVM_API IO_METHOD(IoNumber, ceil);
IOVM_API IO_METHOD(IoNumber, cos);
IOVM_API IO_METHOD(IoNumber, deg);
IOVM_API IO_METHOD(IoNumber, exp);
IOVM_API IO_METHOD(IoNumber, factorial);
IOVM_API IO_METHOD(IoNumber, floor);
IOVM_API IO_METHOD(IoNumber, log);
IOVM_API IO_METHOD(IoNumber, log2);
IOVM_API IO_METHOD(IoNumber, log10);
IOVM_API IO_METHOD(IoNumber, max);
IOVM_API IO_METHOD(IoNumber, min);
IOVM_API IO_METHOD(IoNumber, mod);

// IO_METHOD(IoNumber, mod);
// IO_METHOD(IoNumber, rad);

IOVM_API IO_METHOD(IoNumber, pow);

IOVM_API IO_METHOD(IoNumber, round);
IOVM_API IO_METHOD(IoNumber, roundDown);

IOVM_API IO_METHOD(IoNumber, sin);
IOVM_API IO_METHOD(IoNumber, sqrt);

IOVM_API IO_METHOD(IoNumber, squared);
IOVM_API IO_METHOD(IoNumber, cubed);

IOVM_API IO_METHOD(IoNumber, tan);

// IO_METHOD(IoNumber, frexp);
// IO_METHOD(IoNumber, ldexp);

IOVM_API IO_METHOD(IoNumber, random);
IOVM_API IO_METHOD(IoNumber, gaussianRandom);
IOVM_API IO_METHOD(IoNumber, randomseed);
IOVM_API IO_METHOD(IoNumber, toggle);

// bitwise operations

IOVM_API IO_METHOD(IoNumber, newBitwiseAnd);
IOVM_API IO_METHOD(IoNumber, newBitwiseOr);

IOVM_API IO_METHOD(IoNumber, bitwiseAnd);
IOVM_API IO_METHOD(IoNumber, bitwiseOr);
IOVM_API IO_METHOD(IoNumber, bitwiseXor);
IOVM_API IO_METHOD(IoNumber, bitwiseComplement);
IOVM_API IO_METHOD(IoNumber, bitShiftLeft);
IOVM_API IO_METHOD(IoNumber, bitShiftRight);

// even and odd

IOVM_API IO_METHOD(IoNumber, isEven);
IOVM_API IO_METHOD(IoNumber, isOdd);

// character operations

IOVM_API IO_METHOD(IoNumber, isAlphaNumeric);
IOVM_API IO_METHOD(IoNumber, isLetter);
IOVM_API IO_METHOD(IoNumber, isControlCharacter);
IOVM_API IO_METHOD(IoNumber, isDigit);
IOVM_API IO_METHOD(IoNumber, isGraph);
IOVM_API IO_METHOD(IoNumber, isLowercase);
IOVM_API IO_METHOD(IoNumber, isUppercase);
IOVM_API IO_METHOD(IoNumber, isPrint);
IOVM_API IO_METHOD(IoNumber, isPunctuation);
IOVM_API IO_METHOD(IoNumber, isSpace);
IOVM_API IO_METHOD(IoNumber, isHexDigit);

IOVM_API IO_METHOD(IoNumber, asLowercase);
IOVM_API IO_METHOD(IoNumber, asUppercase);

IOVM_API IO_METHOD(IoNumber, between);
IOVM_API IO_METHOD(IoNumber, clip);
IOVM_API IO_METHOD(IoNumber, negate);
IOVM_API IO_METHOD(IoNumber, at);

// limits

IOVM_API IO_METHOD(IoNumber, integerMax);
IOVM_API IO_METHOD(IoNumber, integerMin);
IOVM_API IO_METHOD(IoNumber, longMax);
IOVM_API IO_METHOD(IoNumber, longMin);
IOVM_API IO_METHOD(IoNumber, shortMax);
IOVM_API IO_METHOD(IoNumber, shortMin);
IOVM_API IO_METHOD(IoNumber, unsignedLongMax);
IOVM_API IO_METHOD(IoNumber, unsignedIntMax);
IOVM_API IO_METHOD(IoNumber, floatMax);
IOVM_API IO_METHOD(IoNumber, floatMin);
IOVM_API IO_METHOD(IoNumber, isNan);

// looping

IOVM_API IO_METHOD(IoNumber, repeat);

#ifdef __cplusplus
}
#endif
#endif
