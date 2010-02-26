#include "IoBigNum.h"
//metadoc BigNum category Math
/*metadoc BigNum description A wrapper for <a href=http://gmplib.org/>GNU MP Bignum (arbitrary precision math) library</a>. 
Warning: GMP uses the restrictive GNU license which can be a problem if you are hard linking it into a distrbuted application.
*/

#define DATA(self) ((IoBigNumData *)(IoObject_dataPointer(self)))

IoTag *IoBigNum_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("BigNum");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoBigNum_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoBigNum_free);
	IoTag_compareFunc_(tag, (IoTagCompareFunc *)IoBigNum_cmp);
	return tag;
}

IoBigNum *IoBigNum_proto(void *state)
{
	IoBigNum *self = IoObject_new(state);
	IoObject_tag_(self, IoBigNum_newTag(state));
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoBigNumData)));
	mpz_init(DATA(self)->integer);
	IoState_registerProtoWithFunc_(state, self, IoBigNum_proto);
	IoMethodTable methodTable[] = {
		{"+", IoBigNum_add}
		//doc BigNum +(aNum) Add op. Returns result.
		
		, {"&", IoBigNum_and}
		//doc BigNum &(aNum)  AND op. Returns result.

		, {"/", IoBigNum_div}
		//doc BigNum /(aNum) Divide op. Returns result.
		
		, {"|", IoBigNum_ior}
		//doc BigNum |(aNum)  OR op. Returns result.

		, {"%", IoBigNum_mod}
		//doc BigNum %(aNum) Modulus op (same as mod()). Returns result.
		
		, {"*", IoBigNum_mul}
		//doc BigNum *(aNum) Multiply op. Returns result.
		
		, {"-", IoBigNum_sub}
		//doc BigNum -(aNum) Subtract op. Returns result.
		
		, {"^", IoBigNum_xor}
		//doc BigNum ^(aNum) XOR op. Returns result.
		
		, {"**", IoBigNum_pow}
		//doc BigNum **(aNum) Power op. Returns result.

		, {"<<", IoBigNum_shl}
		//doc BigNum <<(aNum) Shift left (towards higher bits) op. Returns result.
		
		, {">>", IoBigNum_shr}
		//doc BigNum >>(aNum) Shift right (towards lower bits) op. Returns result.
		
		, {"abs", IoBigNum_abs}
		//doc BigNum abs absolute op. Returns result.

		, {"gcd", IoBigNum_gcd}
		//doc BigNum gcd(aNum) Greatest common denominator op. Returns result.
		
		, {"lcm", IoBigNum_lcm}
		//doc BigNum lcm(aNum) Least common denominator op. Returns result.

		, {"mod", IoBigNum_mod}
		//doc BigNum mod(aNum) Modulus op (same as %). Returns result.

		, {"neg", IoBigNum_neg}
		//doc BigNum neg Returns negative version of receiver.
		
		, {"pow", IoBigNum_pow}
		//doc BigNum pow(aNum) Returns power of receiver to aNum.
		
		, {"powm", IoBigNum_powm}
		//doc BigNum powm(aNum) ?
		, {"root", IoBigNum_root}
		//doc BigNum root(aNum) Returns the aNum root of the receiver.

		, {"sqrt", IoBigNum_sqrt}
		//doc BigNum sqrt Returns square root of the receiver.
		
		, {"with", IoBigNum_with}
		//doc BigNum with(aNumber) Returns a BigNum version of the Io number aNumber.

//    , {"print", IoBigNum_print}
		, {"scan0", IoBigNum_scan0}
		//doc BigNum scan0(aNum)  ?
		, {"scan1", IoBigNum_scan1}
		//doc BigNum scan1(aNum)  ?
		
//    , {"clrbit", IoBigNum_clrbit}
//    , {"combit", IoBigNum_combit}

		, {"invert", IoBigNum_invert}
		//doc BigNum invert(aNum)  ?
		
		, {"jacobi", IoBigNum_jacobi}
		//doc BigNum jacobi(aNum)  ?
		
//    , {"setbit", IoBigNum_setbit}
		, {"tstbit", IoBigNum_tstbit}
		//doc BigNum tstbit(aNum)  ?
		
		, {"hamdist", IoBigNum_hamdist}
		//doc BigNum hamdist(aNum)  Returns hamming distance between receiver and aNum.
		
		, {"asNumber", IoBigNum_asNumber}
		//doc BigNum asNumber Returns an Io Number for the receiving BigNum.
		
		, {"asString", IoBigNum_asString}
		//doc BigNum asString Returns a string representation of the receiver.

		, {"legendre", IoBigNum_legendre}
		//doc BigNum legendre(aNum) ?

		, {"popcount", IoBigNum_popcount}
		//doc BigNum popcount ?
		
		, {"kronecker", IoBigNum_kronecker}
		//doc BigNum kronecker ?
		
		, {"nextprime", IoBigNum_nextprime}
		//doc BigNum nextprime Returns next prime larger than the receiver.
		
		, {"asSimpleString", IoBigNum_asString}
		//doc BigNum asSimpleString Returns simple string representation of the receiver.
		
		, {NULL, NULL}
	};
	IoObject_addMethodTable_(self, methodTable);
	return self;
}

IoBigNum *IoBigNum_rawClone(IoBigNum * proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoBigNumData)));
	mpz_init(DATA(self)->integer);
	return self;
}

IoBigNum *IoBigNum_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoBigNum_proto);
	return IOCLONE(proto);
}

void IoBigNum_free(IoBigNum * self)
{
	mpz_clear(DATA(self)->integer);
	free(DATA(self));
}

/* ----------------------------------------------------------- */

/* Assignment Functions */

IoBigNum *IoBigNum_with(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISNUMBER(other))
		mpz_set_si(DATA(result)->integer, IoNumber_asLong(other));
	else if (ISSYMBOL(other))
		mpz_set_str(DATA(result)->integer, CSTRING(other), 10);
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Number or a Symbol, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return result;
}

/* Conversion Functions */

IoObject *IoBigNum_asNumber(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	return IONUMBER(mpz_get_d(DATA(self)->integer));
}

IoObject *IoBigNum_asString(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	return IOSYMBOL(mpz_get_str(NULL, 10, DATA(self)->integer));
}

//IoObject *IoBigNum_print(IoBigNum * self, IoObject * locals, IoMessage * m)
//{
//  printf("%s", mpz_get_str(NULL, 10, DATA(self)->integer));
//  return IONIL(self);
//}

/* Arithmetic Functions */

IoBigNum *IoBigNum_add(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISNUMBER(other))
	{
		long integer = IoNumber_asLong(other);
		if (integer < 0)
			mpz_sub_ui(DATA(result)->integer, DATA(self)->integer, -integer);
		else
			mpz_add_ui(DATA(result)->integer, DATA(self)->integer, integer);
	}
	else if (ISBIGNUM(other))
		mpz_add(DATA(result)->integer, DATA(self)->integer, DATA(other)->integer);
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Number or a BigNum, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return result;
}

IoBigNum *IoBigNum_sub(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISNUMBER(other))
	{
		long integer = IoNumber_asLong(other);
		if (integer < 0)
			mpz_add_ui(DATA(result)->integer, DATA(self)->integer, -integer);
		else
			mpz_sub_ui(DATA(result)->integer, DATA(self)->integer, integer);
	}
	else if (ISBIGNUM(other))
		mpz_sub(DATA(result)->integer, DATA(self)->integer, DATA(other)->integer);
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Number or a BigNum, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return result;
}

IoBigNum *IoBigNum_mul(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISNUMBER(other))
		mpz_mul_si(DATA(result)->integer, DATA(self)->integer, IoNumber_asLong(other));
	else if (ISBIGNUM(other))
		mpz_mul(DATA(result)->integer, DATA(self)->integer, DATA(other)->integer);
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Number or a BigNum, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return result;
}

IoBigNum *IoBigNum_shl(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	long shift = IoMessage_locals_longArgAt_(m, locals, 0);
	if (shift < 0)
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be non-negative\n", CSTRING(IoMessage_name(m)));
	mpz_mul_2exp(DATA(result)->integer, DATA(self)->integer, shift);
	return result;
}

IoBigNum *IoBigNum_neg(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	mpz_neg(DATA(result)->integer, DATA(self)->integer);
	return result;
}

IoBigNum *IoBigNum_abs(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	mpz_abs(DATA(result)->integer, DATA(self)->integer);
	return result;
}

/* Division Functions */

IoBigNum *IoBigNum_div(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISNUMBER(other))
	{
		long integer = IoNumber_asLong(other);
		if (integer < 0)
		{
			mpz_neg(DATA(result)->integer, DATA(self)->integer);
			mpz_fdiv_q_ui(DATA(result)->integer, DATA(result)->integer, -integer);
		}
		else
			mpz_fdiv_q_ui(DATA(result)->integer, DATA(self)->integer, integer);
	}
	else if (ISBIGNUM(other))
		mpz_fdiv_q(DATA(result)->integer, DATA(self)->integer, DATA(other)->integer);
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Number or a BigNum, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return result;
}

IoBigNum *IoBigNum_shr(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	long shift = IoMessage_locals_longArgAt_(m, locals, 0);
	if (shift < 0)
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be non-negative\n", CSTRING(IoMessage_name(m)));
	mpz_fdiv_q_2exp(DATA(result)->integer, DATA(self)->integer, shift);
	return result;
}

IoBigNum *IoBigNum_mod(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISNUMBER(other))
	{
		long integer = IoNumber_asLong(other);
		if (integer < 0)
			mpz_mod_ui(DATA(result)->integer, DATA(self)->integer, -integer);
		else
			mpz_mod_ui(DATA(result)->integer, DATA(self)->integer, integer);
	}
	else if (ISBIGNUM(other))
		mpz_mod(DATA(result)->integer, DATA(self)->integer, DATA(other)->integer);
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Number or a BigNum, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return result;
}

/* Exponentiation Functions */

IoBigNum *IoBigNum_powm(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	IoObject *other0 = IoMessage_locals_valueArgAt_(m, locals, 0);
	IoObject *other1 = IoMessage_locals_valueArgAt_(m, locals, 1);
	if (ISBIGNUM(other1))
	{
		if (ISNUMBER(other0))
		{
			long integer = IoNumber_asLong(other0);
			if (integer < 0)
				IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be non-negative\n", CSTRING(IoMessage_name(m)));
			else
				mpz_powm_ui(DATA(result)->integer, DATA(self)->integer, integer, DATA(other1)->integer);
		}
		else if (ISBIGNUM(other0))
			mpz_powm(DATA(result)->integer, DATA(self)->integer, DATA(other0)->integer, DATA(other1)->integer);
		else
			IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Number or a BigNum, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other0));
	}
	else
		IoState_error_(IOSTATE, m, "argument 1 to method '%s' must be a BigNum, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other1));
	return result;
}

IoBigNum *IoBigNum_pow(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	long exponent = IoMessage_locals_longArgAt_(m, locals, 0);
	if (exponent < 0)
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be non-negative\n", CSTRING(IoMessage_name(m)));
	mpz_pow_ui(DATA(result)->integer, DATA(self)->integer, exponent);
	return result;
}

/* Root Extraction Functions */

IoBigNum *IoBigNum_root(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	long exponent = IoMessage_locals_longArgAt_(m, locals, 0);
	if (exponent < 0)
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be non-negative\n", CSTRING(IoMessage_name(m)));
	mpz_root(DATA(result)->integer, DATA(self)->integer, exponent);
	return result;
}

IoBigNum *IoBigNum_sqrt(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	mpz_sqrt(DATA(result)->integer, DATA(self)->integer);
	return result;
}

/* Number Theoretic Functions */

IoBigNum *IoBigNum_nextprime(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	mpz_nextprime(DATA(result)->integer, DATA(self)->integer);
	return result;
}

IoBigNum *IoBigNum_gcd(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISNUMBER(other))
	{
		long integer = IoNumber_asLong(other);
		if (integer < 0)
			mpz_gcd_ui(DATA(result)->integer, DATA(self)->integer, -integer);
		else
			mpz_gcd_ui(DATA(result)->integer, DATA(self)->integer, integer);
	}
	else if (ISBIGNUM(other))
		mpz_gcd(DATA(result)->integer, DATA(self)->integer, DATA(other)->integer);
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Number or a BigNum, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return result;
}

IoBigNum *IoBigNum_lcm(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISNUMBER(other))
	{
		long integer = IoNumber_asLong(other);
		if (integer < 0)
			mpz_lcm_ui(DATA(result)->integer, DATA(self)->integer, -integer);
		else
			mpz_lcm_ui(DATA(result)->integer, DATA(self)->integer, integer);
	}
	else if (ISBIGNUM(other))
		mpz_lcm(DATA(result)->integer, DATA(self)->integer, DATA(other)->integer);
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Number or a BigNum, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return result;
}

IoBigNum *IoBigNum_invert(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISBIGNUM(other))
		mpz_invert(DATA(result)->integer, DATA(self)->integer, DATA(other)->integer);
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a BigNum, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return result;
}

IoBigNum *IoBigNum_jacobi(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISBIGNUM(other))
		return IONUMBER(mpz_jacobi(DATA(self)->integer, DATA(other)->integer));
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a BigNum, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return IONIL(self);
}

IoBigNum *IoBigNum_legendre(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISBIGNUM(other))
		return IONUMBER(mpz_legendre(DATA(self)->integer, DATA(other)->integer));
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a BigNum, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return IONIL(self);
}

IoBigNum *IoBigNum_kronecker(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISNUMBER(other))
		return IONUMBER(mpz_kronecker_si(DATA(self)->integer, IoNumber_asLong(other)));
	else if (ISBIGNUM(other))
		return IONUMBER(mpz_kronecker(DATA(self)->integer, DATA(other)->integer));
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Number or a BigNum, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return IONIL(self);
}

/* Comparison Functions */

int IoBigNum_cmp(IoBigNum * self, IoObject * other)
{
	if (ISNUMBER(other))
		return mpz_cmp_si(DATA(self)->integer, IoNumber_asLong(other));
	else if (ISBIGNUM(other))
		return mpz_cmp(DATA(self)->integer, DATA(other)->integer);
	else
		return IoObject_defaultCompare(self, other);
}

/* Logical and Bit Manipulation Functions */

IoBigNum *IoBigNum_and(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISBIGNUM(other))
		mpz_and(DATA(result)->integer, DATA(self)->integer, DATA(other)->integer);
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a BigNum, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return result;
}

IoBigNum *IoBigNum_ior(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISBIGNUM(other))
		mpz_ior(DATA(result)->integer, DATA(self)->integer, DATA(other)->integer);
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a BigNum, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return result;
}

IoBigNum *IoBigNum_xor(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoBigNum *result = IoBigNum_new(IOSTATE);
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISBIGNUM(other))
		mpz_xor(DATA(result)->integer, DATA(self)->integer, DATA(other)->integer);
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a BigNum, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return result;
}

IoBigNum *IoBigNum_popcount(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	return IONUMBER(mpz_popcount(DATA(self)->integer));
}

IoBigNum *IoBigNum_hamdist(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISBIGNUM(other))
		return IONUMBER(mpz_hamdist(DATA(self)->integer, DATA(other)->integer));
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a BigNum, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return IONIL(self);
}

IoBigNum *IoBigNum_scan0(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISNUMBER(other))
		return IONUMBER(mpz_scan0(DATA(self)->integer, IoNumber_asLong(other)));
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Number, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return IONIL(self);
}

IoBigNum *IoBigNum_scan1(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISNUMBER(other))
		return IONUMBER((long)mpz_scan1(DATA(self)->integer, IoNumber_asLong(other)));
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Number, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return IONIL(self);
}

//IoBigNum *IoBigNum_setbit(IoBigNum * self, IoObject * locals, IoMessage * m)
//{
//  IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
//  if (ISNUMBER(other))
//    mpz_setbit(DATA(self)->integer, IoNumber_asLong(other));
//  else
//    IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Number, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
//  return self;
//}

//IoBigNum *IoBigNum_clrbit(IoBigNum * self, IoObject * locals, IoMessage * m)
//{
//  IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
//  if (ISNUMBER(other))
//    mpz_clrbit(DATA(self)->integer, IoNumber_asLong(other));
//  else
//    IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Number, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
//  return self;
//}

//IoBigNum *IoBigNum_combit(IoBigNum * self, IoObject * locals, IoMessage * m)
//{
//  IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
//  if (ISNUMBER(other))
//    mpz_combit(DATA(self)->integer, IoNumber_asLong(other));
//  else
//    IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Number, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
//  return self;
//}

IoBigNum *IoBigNum_tstbit(IoBigNum * self, IoObject * locals, IoMessage * m)
{
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISNUMBER(other))
		return IOBOOL(self, mpz_tstbit(DATA(self)->integer, IoNumber_asLong(other)));
	else
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Number, not a '%s'\n", CSTRING(IoMessage_name(m)), IoObject_name(other));
	return IONIL(self);
}
