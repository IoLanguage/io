#ifndef IOBIGNUM_DEFINED
#  define IOBIGNUM_DEFINED 1

#  include "IoObject.h"
#  include "IoNumber.h"
#  include <gmp.h>

#  define ISBIGNUM(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoBigNum_rawClone)

typedef IoObject IoBigNum;

typedef struct
{
	mpz_t integer;
} IoBigNumData;

IoTag *IoBigNum_newTag(void *state);
IoBigNum *IoBigNum_proto(void *state);
IoBigNum *IoBigNum_rawClone(IoBigNum * self);
IoBigNum *IoBigNum_new(void *state);
void IoBigNum_free(IoBigNum * self);

/* Assignment Functions */
IoBigNum *IoBigNum_with(IoBigNum * self, IoObject * locals, IoMessage * m);
/* Conversion Functions */
IoObject *IoBigNum_asNumber(IoBigNum * self, IoObject * locals, IoMessage * m);
IoObject *IoBigNum_asString(IoBigNum * self, IoObject * locals, IoMessage * m);
//IoObject *IoBigNum_print(IoBigNum * self, IoObject * locals, IoMessage * m);
/* Arithmetic Functions */
IoBigNum *IoBigNum_add(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_sub(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_mul(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_shl(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_neg(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_abs(IoBigNum * self, IoObject * locals, IoMessage * m);
/* Division Functions */
IoBigNum *IoBigNum_div(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_shr(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_mod(IoBigNum * self, IoObject * locals, IoMessage * m);
/* Exponentiation Functions */
IoBigNum *IoBigNum_powm(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_pow(IoBigNum * self, IoObject * locals, IoMessage * m);
/* Root Extraction Functions */
IoBigNum *IoBigNum_root(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_sqrt(IoBigNum * self, IoObject * locals, IoMessage * m);
/* Number Theoretic Functions */
IoBigNum *IoBigNum_nextprime(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_gcd(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_lcm(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_invert(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_jacobi(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_legendre(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_kronecker(IoBigNum * self, IoObject * locals, IoMessage * m);
/* Comparison Functions */
int IoBigNum_cmp(IoBigNum * self, IoObject * other);
/* Logical and Bit Manipulation Functions */
IoBigNum *IoBigNum_and(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_ior(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_xor(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_popcount(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_hamdist(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_scan0(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_scan1(IoBigNum * self, IoObject * locals, IoMessage * m);
//IoBigNum *IoBigNum_setbit(IoBigNum * self, IoObject * locals, IoMessage * m);
//IoBigNum *IoBigNum_clrbit(IoBigNum * self, IoObject * locals, IoMessage * m);
//IoBigNum *IoBigNum_combit(IoBigNum * self, IoObject * locals, IoMessage * m);
IoBigNum *IoBigNum_tstbit(IoBigNum * self, IoObject * locals, IoMessage * m);

#endif
