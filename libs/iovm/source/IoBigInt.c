// IoBigInt — Arbitrary precision integer wrapping libtommath mp_int.
// Separate type from Number. No implicit coercion (matches JS BigInt semantics).

#include "IoBigInt.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include "IoTag.h"
#include "IoMessage.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

static const char *protoId = "BigInt";

typedef struct {
	mp_int value;
} IoBigIntData;

#define DATA(self) ((IoBigIntData *)IoObject_dataPointer(self))

// ---- Tag ----

static IoObject *IoBigInt_rawClone(IoObject *proto) {
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_tag_(self, IoObject_tag(proto));
	IoBigIntData *data = calloc(1, sizeof(IoBigIntData));
	mp_init(&data->value);
	IoObject_setDataPointer_(self, data);
	return self;
}

static void IoBigInt_free(IoObject *self) {
	IoBigIntData *data = DATA(self);
	if (data) {
		mp_clear(&data->value);
		free(data);
	}
}

static IoTag *IoBigInt_newTag(void *state) {
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoBigInt_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoBigInt_free);
	return tag;
}

// ---- Helpers ----

// Get the other operand as a BigInt, raising error if not
static IoBigIntData *IoBigInt_argData(IoObject *self, IoObject *locals,
                                       IoMessage *m, int argIndex) {
	IoObject *arg = IoMessage_locals_valueArgAt_(m, locals, argIndex);
	IoState *state = IOSTATE;
	if (state->errorRaised) return NULL;
	if (!IoBigInt_isBigInt(arg)) {
		IoState_error_(state, m,
			"BigInt operation requires BigInt argument, got %s",
			IoObject_name(arg));
		return NULL;
	}
	return DATA(arg);
}

// Create a new BigInt holding a copy of the result mp_int
static IoObject *IoBigInt_newResult(void *state, mp_int *result) {
	IoObject *obj = IoBigInt_new(state);
	mp_copy(result, &DATA(obj)->value);
	return obj;
}

// ---- Arithmetic methods ----

IO_METHOD(IoObject, BigInt_add) {
	IoBigIntData *b = IoBigInt_argData(self, locals, m, 0);
	if (!b) return IONIL(self);
	mp_int result;
	mp_init(&result);
	mp_add(&DATA(self)->value, &b->value, &result);
	IoObject *r = IoBigInt_newResult(IOSTATE, &result);
	mp_clear(&result);
	return r;
}

IO_METHOD(IoObject, BigInt_sub) {
	IoBigIntData *b = IoBigInt_argData(self, locals, m, 0);
	if (!b) return IONIL(self);
	mp_int result;
	mp_init(&result);
	mp_sub(&DATA(self)->value, &b->value, &result);
	IoObject *r = IoBigInt_newResult(IOSTATE, &result);
	mp_clear(&result);
	return r;
}

IO_METHOD(IoObject, BigInt_mul) {
	IoBigIntData *b = IoBigInt_argData(self, locals, m, 0);
	if (!b) return IONIL(self);
	mp_int result;
	mp_init(&result);
	mp_mul(&DATA(self)->value, &b->value, &result);
	IoObject *r = IoBigInt_newResult(IOSTATE, &result);
	mp_clear(&result);
	return r;
}

IO_METHOD(IoObject, BigInt_div) {
	IoBigIntData *b = IoBigInt_argData(self, locals, m, 0);
	if (!b) return IONIL(self);
	if (mp_iszero(&b->value)) {
		IoState_error_(IOSTATE, m, "BigInt division by zero");
		return IONIL(self);
	}
	mp_int quotient;
	mp_init(&quotient);
	mp_div(&DATA(self)->value, &b->value, &quotient, NULL);
	IoObject *r = IoBigInt_newResult(IOSTATE, &quotient);
	mp_clear(&quotient);
	return r;
}

IO_METHOD(IoObject, BigInt_mod) {
	IoBigIntData *b = IoBigInt_argData(self, locals, m, 0);
	if (!b) return IONIL(self);
	if (mp_iszero(&b->value)) {
		IoState_error_(IOSTATE, m, "BigInt modulo by zero");
		return IONIL(self);
	}
	mp_int remainder;
	mp_init(&remainder);
	mp_mod(&DATA(self)->value, &b->value, &remainder);
	IoObject *r = IoBigInt_newResult(IOSTATE, &remainder);
	mp_clear(&remainder);
	return r;
}

IO_METHOD(IoObject, BigInt_pow) {
	IoObject *arg = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (IOSTATE->errorRaised) return IONIL(self);
	if (!ISNUMBER(arg)) {
		IoState_error_(IOSTATE, m, "BigInt ** requires Number exponent");
		return IONIL(self);
	}
	double exp = IoNumber_asDouble(arg);
	if (exp < 0 || exp != floor(exp)) {
		IoState_error_(IOSTATE, m, "BigInt ** requires non-negative integer exponent");
		return IONIL(self);
	}
	mp_int result;
	mp_init(&result);
	mp_expt_n(&DATA(self)->value, (int)exp, &result);
	IoObject *r = IoBigInt_newResult(IOSTATE, &result);
	mp_clear(&result);
	return r;
}

// ---- Comparison methods ----

IO_METHOD(IoObject, BigInt_compare) {
	IoBigIntData *b = IoBigInt_argData(self, locals, m, 0);
	if (!b) return IONIL(self);
	mp_ord cmp = mp_cmp(&DATA(self)->value, &b->value);
	return IONUMBER(cmp == MP_LT ? -1 : cmp == MP_GT ? 1 : 0);
}

IO_METHOD(IoObject, BigInt_eq) {
	IoBigIntData *b = IoBigInt_argData(self, locals, m, 0);
	if (!b) return IOSTATE->ioFalse;
	return mp_cmp(&DATA(self)->value, &b->value) == MP_EQ
		? IOSTATE->ioTrue : IOSTATE->ioFalse;
}

IO_METHOD(IoObject, BigInt_lt) {
	IoBigIntData *b = IoBigInt_argData(self, locals, m, 0);
	if (!b) return IOSTATE->ioFalse;
	return mp_cmp(&DATA(self)->value, &b->value) == MP_LT
		? IOSTATE->ioTrue : IOSTATE->ioFalse;
}

IO_METHOD(IoObject, BigInt_gt) {
	IoBigIntData *b = IoBigInt_argData(self, locals, m, 0);
	if (!b) return IOSTATE->ioFalse;
	return mp_cmp(&DATA(self)->value, &b->value) == MP_GT
		? IOSTATE->ioTrue : IOSTATE->ioFalse;
}

IO_METHOD(IoObject, BigInt_lte) {
	IoBigIntData *b = IoBigInt_argData(self, locals, m, 0);
	if (!b) return IOSTATE->ioFalse;
	mp_ord cmp = mp_cmp(&DATA(self)->value, &b->value);
	return (cmp == MP_LT || cmp == MP_EQ)
		? IOSTATE->ioTrue : IOSTATE->ioFalse;
}

IO_METHOD(IoObject, BigInt_gte) {
	IoBigIntData *b = IoBigInt_argData(self, locals, m, 0);
	if (!b) return IOSTATE->ioFalse;
	mp_ord cmp = mp_cmp(&DATA(self)->value, &b->value);
	return (cmp == MP_GT || cmp == MP_EQ)
		? IOSTATE->ioTrue : IOSTATE->ioFalse;
}

IO_METHOD(IoObject, BigInt_neq) {
	IoBigIntData *b = IoBigInt_argData(self, locals, m, 0);
	if (!b) return IOSTATE->ioTrue;
	return mp_cmp(&DATA(self)->value, &b->value) != MP_EQ
		? IOSTATE->ioTrue : IOSTATE->ioFalse;
}

// ---- Conversion methods ----

IO_METHOD(IoObject, BigInt_asString) {
	(void)locals; (void)m;
	int size = 0;
	mp_radix_size(&DATA(self)->value, 10, &size);
	char *buf = malloc(size + 1);
	size_t written = 0;
	mp_to_radix(&DATA(self)->value, buf, size + 1, &written, 10);
	IoObject *str = IoState_symbolWithCString_(IOSTATE, buf);
	free(buf);
	return str;
}

IO_METHOD(IoObject, BigInt_asNumber) {
	(void)locals; (void)m;
	return IONUMBER(mp_get_double(&DATA(self)->value));
}

IO_METHOD(IoObject, BigInt_asBigInt) {
	(void)locals; (void)m;
	return self;
}

// ---- Unary methods ----

IO_METHOD(IoObject, BigInt_negate) {
	(void)locals; (void)m;
	mp_int result;
	mp_init(&result);
	mp_neg(&DATA(self)->value, &result);
	IoObject *r = IoBigInt_newResult(IOSTATE, &result);
	mp_clear(&result);
	return r;
}

IO_METHOD(IoObject, BigInt_abs) {
	(void)locals; (void)m;
	mp_int result;
	mp_init(&result);
	mp_abs(&DATA(self)->value, &result);
	IoObject *r = IoBigInt_newResult(IOSTATE, &result);
	mp_clear(&result);
	return r;
}

// ---- Construction ----

IO_METHOD(IoObject, BigInt_from) {
	IoObject *arg = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (IOSTATE->errorRaised) return IONIL(self);

	if (ISNUMBER(arg)) {
		double d = IoNumber_asDouble(arg);
		IoObject *obj = IoBigInt_new(IOSTATE);
		mp_set_double(&DATA(obj)->value, d);
		return obj;
	}

	if (ISSEQ(arg)) {
		const char *str = CSTRING(arg);
		IoObject *obj = IoBigInt_new(IOSTATE);
		mp_err err = mp_read_radix(&DATA(obj)->value, str, 10);
		if (err != MP_OKAY) {
			IoState_error_(IOSTATE, m, "BigInt from: invalid string \"%s\"", str);
			return IONIL(self);
		}
		return obj;
	}

	if (IoBigInt_isBigInt(arg)) {
		IoObject *obj = IoBigInt_new(IOSTATE);
		mp_copy(&DATA(arg)->value, &DATA(obj)->value);
		return obj;
	}

	IoState_error_(IOSTATE, m,
		"BigInt from requires a Number, Sequence, or BigInt argument");
	return IONIL(self);
}

IO_METHOD(IoObject, BigInt_type) {
	(void)locals; (void)m;
	return IoState_symbolWithCString_(IOSTATE, "BigInt");
}

// ---- Proto ----

IoObject *IoBigInt_proto(void *state) {
	IoMethodTable methodTable[] = {
		{"+",        IoObject_BigInt_add},
		{"-",        IoObject_BigInt_sub},
		{"*",        IoObject_BigInt_mul},
		{"/",        IoObject_BigInt_div},
		{"%",        IoObject_BigInt_mod},
		{"**",       IoObject_BigInt_pow},
		{"==",       IoObject_BigInt_eq},
		{"<",        IoObject_BigInt_lt},
		{">",        IoObject_BigInt_gt},
		{"<=",       IoObject_BigInt_lte},
		{">=",       IoObject_BigInt_gte},
		{"!=",       IoObject_BigInt_neq},
		{"compare",  IoObject_BigInt_compare},
		{"negate",   IoObject_BigInt_negate},
		{"abs",      IoObject_BigInt_abs},
		{"asString", IoObject_BigInt_asString},
		{"asNumber", IoObject_BigInt_asNumber},
		{"asBigInt", IoObject_BigInt_asBigInt},
		{"from",     IoObject_BigInt_from},
		{"type",     IoObject_BigInt_type},
		{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoBigInt_newTag(state));
	IoBigIntData *data = calloc(1, sizeof(IoBigIntData));
	mp_init(&data->value);
	IoObject_setDataPointer_(self, data);
	IoState_registerProtoWithId_((IoState *)state, self, protoId);
	IoObject_addMethodTable_(self, methodTable);
	return self;
}

// ---- Public API ----

IoObject *IoBigInt_new(void *state) {
	IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
	return IOCLONE(proto);
}

IoObject *IoBigInt_newWithMpInt(void *state, mp_int *src) {
	IoObject *self = IoBigInt_new(state);
	mp_copy(src, &DATA(self)->value);
	return self;
}

IoObject *IoBigInt_newFromCString(void *state, const char *str) {
	IoObject *self = IoBigInt_new(state);
	mp_read_radix(&DATA(self)->value, str, 10);
	return self;
}

int IoBigInt_isBigInt(IoObject *self) {
	return IoObject_tag(self)->name &&
	       strcmp(IoObject_tag(self)->name, protoId) == 0;
}

mp_int *IoBigInt_mp(IoObject *self) {
	return &DATA(self)->value;
}
