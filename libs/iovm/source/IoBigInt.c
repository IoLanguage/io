// IoBigInt — Arbitrary precision integer wrapping libtommath mp_int.
// Separate type from Number. No implicit coercion (matches JS BigInt semantics).

/*cmetadoc BigInt description
C implementation of arbitrary-precision integers. Each BigInt is an
IoObject whose dataPointer is an IoBigIntData that owns a libtommath
mp_int (variable-size digit array on the heap). Unlike Number, BigInt
has a real payload: rawClone allocates and mp_init's a fresh mp_int,
and the tag's freeFunc must call mp_clear before freeing the wrapper.
Operands must both be BigInts — there is no silent coercion from
Number (matching JavaScript's BigInt rules), which is enforced by the
IoBigInt_argData helper that raises an error on non-BigInt args.
Every arithmetic method follows the same mp_init/op/mp_copy-into-new-
BigInt/mp_clear pattern to keep libtommath's ownership discipline
correct even in the face of GC-driven allocation.
*/

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

/*cdoc BigInt IoBigInt_rawClone(proto)
Registered as the tag's cloneFunc. Allocates a fresh IoBigIntData and
mp_init's its mp_int so the clone owns an independent digit buffer.
The value is NOT copied from proto — callers seed the result themselves
(e.g. IoBigInt_newWithMpInt, arithmetic methods). The explicit tag set
preserves tag identity across clones when the proto chain is shared.
*/
static IoObject *IoBigInt_rawClone(IoObject *proto) {
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_tag_(self, IoObject_tag(proto));
	IoBigIntData *data = calloc(1, sizeof(IoBigIntData));
	mp_init(&data->value);
	IoObject_setDataPointer_(self, data);
	return self;
}

/*cdoc BigInt IoBigInt_free(self)
Registered as the tag's freeFunc. mp_clear releases libtommath's heap
digit buffer before the wrapper struct is freed — omitting this would
leak digits for every reclaimed BigInt.
*/
static void IoBigInt_free(IoObject *self) {
	IoBigIntData *data = DATA(self);
	if (data) {
		mp_clear(&data->value);
		free(data);
	}
}

/*cdoc BigInt IoBigInt_newTag(state)
Builds the BigInt tag with clone and free hooks. No compareFunc is
set — comparison goes through the explicit compare / <, > Io methods
so mismatched-type comparisons raise instead of silently ordering.
*/
static IoTag *IoBigInt_newTag(void *state) {
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoBigInt_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoBigInt_free);
	return tag;
}

// ---- Helpers ----

/*cdoc BigInt IoBigInt_argData(self, locals, m, argIndex)
Pulls the n-th argument and returns its IoBigIntData, or NULL after
raising an error if the argument is not a BigInt. Used at the top of
every binary operation — centralizes the "no implicit coercion" rule
so BigInt + Number, etc. fails rather than silently losing precision.
Also propagates any prior errorRaised flag.
*/
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

/*cdoc BigInt IoBigInt_newResult(state, result)
Mints a fresh BigInt and copies the caller's scratch mp_int into it.
The caller retains ownership of their temporary and must mp_clear it
after — see every arithmetic method for the canonical init/op/copy/
clear sequence. Deep copy is required because the wrapper's mp_int is
freed independently from the caller's.
*/
static IoObject *IoBigInt_newResult(void *state, mp_int *result) {
	IoObject *obj = IoBigInt_new(state);
	mp_copy(result, &DATA(obj)->value);
	return obj;
}

// ---- Arithmetic methods ----

/*cdoc BigInt IoObject_BigInt_add(self, locals, m)
Sum of two BigInts. Exemplar of the add/sub/mul shape: stack-allocated
scratch mp_int, mp_init, mp_op, copy into a fresh IoBigInt, mp_clear.
Returns nil on argument-type error raised by IoBigInt_argData.
*/
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

/*cdoc BigInt IoObject_BigInt_sub(self, locals, m)
Difference of two BigInts. Same init/op/copy/clear pattern as add.
*/
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

/*cdoc BigInt IoObject_BigInt_mul(self, locals, m)
Product of two BigInts. libtommath may allocate additional digits for
the result buffer during mp_mul; ownership transfers to the new wrapper
via mp_copy.
*/
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

/*cdoc BigInt IoObject_BigInt_div(self, locals, m)
Quotient of two BigInts (truncated toward zero, matching mp_div).
Passes NULL for the remainder out-param since only the quotient is
returned; callers needing both should use div and mod separately.
Raises on divide-by-zero before calling mp_div.
*/
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

/*cdoc BigInt IoObject_BigInt_mod(self, locals, m)
Remainder of two BigInts. Uses mp_mod, which gives a non-negative
result for positive modulus (Euclidean), distinct from C's trunc-toward-
zero fmod semantics. Raises on divide-by-zero before calling mp_mod.
*/
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

/*cdoc BigInt IoObject_BigInt_pow(self, locals, m)
Integer power. Exponent comes as a Number (not a BigInt) because
mp_expt_n takes a C int; negative or non-integral exponents are
rejected up front since BigInt has no fractional representation.
*/
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

/*cdoc BigInt IoObject_BigInt_compare(self, locals, m)
Three-way compare returning -1/0/1 as a Number. Translates libtommath's
MP_LT/MP_EQ/MP_GT enum into the Io-standard integer ordering used by
sort predicates.
*/
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

/*cdoc BigInt IoObject_BigInt_asString(self, locals, m)
Decimal string form. Queries libtommath for the needed buffer size via
mp_radix_size, allocates, renders with mp_to_radix, and interns the
result as a Symbol so repeated large values share storage.
*/
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

/*cdoc BigInt IoObject_BigInt_asNumber(self, locals, m)
Lossy conversion to a Number. Values beyond 2^53 or below -2^53 lose
integer precision through the double; Infinity is returned for
out-of-range magnitudes per mp_get_double.
*/
IO_METHOD(IoObject, BigInt_asNumber) {
	(void)locals; (void)m;
	return IONUMBER(mp_get_double(&DATA(self)->value));
}

IO_METHOD(IoObject, BigInt_asBigInt) {
	(void)locals; (void)m;
	return self;
}

// ---- Unary methods ----

/*cdoc BigInt IoObject_BigInt_negate(self, locals, m)
Unary minus. Same init/op/copy/clear pattern as the binary ops, with
mp_neg writing into a scratch mp_int.
*/
IO_METHOD(IoObject, BigInt_negate) {
	(void)locals; (void)m;
	mp_int result;
	mp_init(&result);
	mp_neg(&DATA(self)->value, &result);
	IoObject *r = IoBigInt_newResult(IOSTATE, &result);
	mp_clear(&result);
	return r;
}

/*cdoc BigInt IoObject_BigInt_abs(self, locals, m)
Absolute value via mp_abs. Always allocates a fresh BigInt even when
self is already non-negative — BigInts are immutable from Io.
*/
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

/*cdoc BigInt IoObject_BigInt_from(self, locals, m)
Construction entry point accepting a Number, a Sequence (parsed as
decimal), or another BigInt (copy). Number path uses mp_set_double,
which truncates the fractional component; Sequence path routes errors
from mp_read_radix back through IoState_error_ so malformed literals
surface as an Io exception.
*/
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

/*cdoc BigInt IoBigInt_proto(state)
Creates the BigInt proto. Allocates an IoBigIntData payload initialized
to zero, attaches the tag, registers the proto on the state, and
installs the full arithmetic/comparison/conversion method table. All
BigInts are clones of this proto.
*/
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

/*cdoc BigInt IoBigInt_new(state)
Convenience constructor: look up the proto and IOCLONE. rawClone
provides a zeroed mp_int, so the caller typically follows up with
mp_copy or mp_set_double to seed the value.
*/
IoObject *IoBigInt_new(void *state) {
	IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
	return IOCLONE(proto);
}

/*cdoc BigInt IoBigInt_newWithMpInt(state, src)
Constructs a BigInt from an existing libtommath mp_int. Deep-copies via
mp_copy so the caller's mp_int can be cleared independently — this is
the safe bridge from C code that already owns an mp_int.
*/
IoObject *IoBigInt_newWithMpInt(void *state, mp_int *src) {
	IoObject *self = IoBigInt_new(state);
	mp_copy(src, &DATA(self)->value);
	return self;
}

/*cdoc BigInt IoBigInt_newFromCString(state, str)
Parses a C string as a base-10 integer. Unlike the Io-level from
method, this silently ignores parse errors — intended for callers
that have already validated the input.
*/
IoObject *IoBigInt_newFromCString(void *state, const char *str) {
	IoObject *self = IoBigInt_new(state);
	mp_read_radix(&DATA(self)->value, str, 10);
	return self;
}

/*cdoc BigInt IoBigInt_isBigInt(self)
Type check by tag-name comparison. Used by argData and by external
bindings that need to distinguish BigInt from Number before calling
IoBigInt_mp. Name comparison (rather than tag pointer equality) is
chosen so the check keeps working across VM state restarts.
*/
int IoBigInt_isBigInt(IoObject *self) {
	return IoObject_tag(self)->name &&
	       strcmp(IoObject_tag(self)->name, protoId) == 0;
}

/*cdoc BigInt IoBigInt_mp(self)
Returns a borrowed pointer to the internal mp_int. Caller must not
mp_clear it — the wrapper still owns the digits. Used by helpers
that want to call libtommath functions directly without going back
through the Io method dispatch path.
*/
mp_int *IoBigInt_mp(IoObject *self) {
	return &DATA(self)->value;
}
