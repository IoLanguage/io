
//metadoc Number copyright Steve Dekorte 2002
//metadoc Number license BSD revised

#ifndef IONUMBER_DEFINED
#define IONUMBER_DEFINED 1

#include "IoVMApi.h"

#include "Common.h"
#include "IoState.h"
#include "IoObject.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISNUMBER(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoNumber_rawClone)
#define IONUMBER(num) IoState_numberWithDouble_((IoState*)IOSTATE, (double)num)
#define CNUMBER(self) IoObject_dataDouble((IoNumber *)self)

#ifdef WIN32
#define log2(num) log(num) / log(2)
#endif

typedef IoObject IoNumber;

IOVM_API IoNumber *IoNumber_proto(void *state);
IOVM_API IoNumber *IoNumber_rawClone(IoNumber *self);
IOVM_API IoNumber *IoNumber_newWithDouble_(void *state, double n);
IOVM_API IoNumber *IoNumber_newCopyOf_(IoNumber *number);
IOVM_API void IoNumber_copyFrom_(IoNumber *self, IoNumber *number);

IOVM_API void IoNumber_writeToStream_(IoNumber *self, BStream *stream);
IOVM_API void *IoNumber_readFromStream_(IoNumber *self, BStream *stream);

IOVM_API void IoNumber_free(IoNumber *self);

IOVM_API UArray IoNumber_asStackUArray(IoNumber *self);
IOVM_API int IoNumber_asInt(IoNumber *self);
long IoNumber_asLong(IoNumber *self);
IOVM_API double IoNumber_asDouble(IoNumber *self);
float IoNumber_asFloat(IoNumber *self);
IOVM_API int IoNumber_compare(IoNumber *self, IoNumber *v);
IOVM_API void IoNumber_print(IoNumber *self);
//void IoNumber_rawSet(IoNumber *self, double v);
#define IoNumber_rawSet(self, v) CNUMBER(self) = v;

// -----------------------------------------------------------

IOVM_API IoObject *IoNumber_htonl(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_ntohl(IoNumber *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoNumber_asNumber(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_add_(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_subtract(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_divide(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_multiply(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_printNumber(IoNumber *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoNumber_asBuffer(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_asString(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_justAsString(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_asCharacter(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_asUint32Buffer(IoNumber *self, IoObject *locals, IoMessage *m);
//IoObject *IoNumber_asDate(IoNumber *self, IoObject *locals, IoMessage *m);

// no arg methods

IOVM_API IoObject *IoNumber_abs(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_acos(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_asin(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_atan(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_atan2(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_ceil(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_cos(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_deg(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_exp(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_factorial(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_floor(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_log(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_log2(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_log10(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_max(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_min(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_mod(IoNumber *self, IoObject *locals, IoMessage *m);

//IoObject *IoNumber_mod(IoNumber *self, IoObject *locals, IoMessage *m);
//IoObject *IoNumber_rad(IoNumber *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoNumber_pow(IoNumber *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoNumber_round(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_roundDown(IoNumber *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoNumber_sin(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_sqrt(IoNumber *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoNumber_squared(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_cubed(IoNumber *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoNumber_tan(IoNumber *self, IoObject *locals, IoMessage *m);

//IoObject *IoNumber_frexp(IoNumber *self, IoObject *locals, IoMessage *m);
//IoObject *IoNumber_ldexp(IoNumber *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoNumber_random(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_gaussianRandom(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_randomseed(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_toggle(IoNumber *self, IoObject *locals, IoMessage *m);

// bitwise operations

IOVM_API IoObject *IoNumber_newBitwiseAnd(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_newBitwiseOr(IoNumber *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoNumber_bitwiseAnd(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_bitwiseOr(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_bitwiseXor(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_bitwiseComplement(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_bitShiftLeft(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_bitShiftRight(IoNumber *self, IoObject *locals, IoMessage *m);

// even and odd

IOVM_API IoObject *IoNumber_isEven(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_isOdd(IoNumber *self, IoObject *locals, IoMessage *m);

// character operations

IOVM_API IoObject *IoNumber_isAlphaNumeric(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_isLetter(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_isControlCharacter(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_isDigit(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_isGraph(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_isLowercase(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_isUppercase(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_isPrint(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_isPunctuation(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_isSpace(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_isHexDigit(IoNumber *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoNumber_asLowercase(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_asUppercase(IoNumber *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoNumber_between(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_clip(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_negate(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_at(IoNumber *self, IoObject *locals, IoMessage *m);

// limits

IOVM_API IoObject *IoNumber_integerMax(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_integerMin(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_longMax(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_longMin(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_shortMax(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_shortMin(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_unsignedLongMax(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_unsignedIntMax(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_floatMax(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_floatMin(IoNumber *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoNumber_isNan(IoNumber *self, IoObject *locals, IoMessage *m);

// looping

IOVM_API IoObject *IoNumber_repeat(IoNumber *self, IoObject *locals, IoMessage *m);

#ifdef __cplusplus
}
#endif
#endif
