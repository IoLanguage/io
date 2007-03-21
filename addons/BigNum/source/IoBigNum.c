#include "IoBigNum.h"

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
		, {"&", IoBigNum_and}
		, {"/", IoBigNum_div}
		, {"|", IoBigNum_ior}
		, {"%", IoBigNum_mod}
		, {"*", IoBigNum_mul}
		, {"-", IoBigNum_sub}
		, {"^", IoBigNum_xor}
		, {"**", IoBigNum_pow}
		, {"<<", IoBigNum_shl}
		, {">>", IoBigNum_shr}
		, {"abs", IoBigNum_abs}
		, {"gcd", IoBigNum_gcd}
		, {"lcm", IoBigNum_lcm}
		, {"mod", IoBigNum_mod}
		, {"neg", IoBigNum_neg}
		, {"pow", IoBigNum_pow}
		, {"powm", IoBigNum_powm}
		, {"root", IoBigNum_root}
		, {"sqrt", IoBigNum_sqrt}
		, {"with", IoBigNum_with}
//    , {"print", IoBigNum_print}
		, {"scan0", IoBigNum_scan0}
		, {"scan1", IoBigNum_scan1}
//    , {"clrbit", IoBigNum_clrbit}
//    , {"combit", IoBigNum_combit}
		, {"invert", IoBigNum_invert}
		, {"jacobi", IoBigNum_jacobi}
//    , {"setbit", IoBigNum_setbit}
		, {"tstbit", IoBigNum_tstbit}
		, {"hamdist", IoBigNum_hamdist}
		, {"asNumber", IoBigNum_asNumber}
		, {"asString", IoBigNum_asString}
		, {"legendre", IoBigNum_legendre}
		, {"popcount", IoBigNum_popcount}
		, {"kronecker", IoBigNum_kronecker}
		, {"nextprime", IoBigNum_nextprime}
		, {"asSimpleString", IoBigNum_asString}
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
