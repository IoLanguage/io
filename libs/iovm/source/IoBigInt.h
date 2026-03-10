// IoBigInt — Arbitrary precision integer type wrapping libtommath.
// Separate from Number (double). No implicit coercion (matches JS semantics).

#ifndef IOBIGINT_H
#define IOBIGINT_H

#include "IoObject.h"
#include <tommath.h>

IOVM_API IoObject *IoBigInt_proto(void *state);
IOVM_API IoObject *IoBigInt_new(void *state);
IOVM_API IoObject *IoBigInt_newWithMpInt(void *state, mp_int *src);
IOVM_API IoObject *IoBigInt_newFromCString(void *state, const char *str);
IOVM_API int IoBigInt_isBigInt(IoObject *self);
IOVM_API mp_int *IoBigInt_mp(IoObject *self);

#endif
