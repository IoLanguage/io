
#include "IoSeq.h"
#include "IoNumber.h"

#define IO_ASSERT_NOT_SYMBOL(self) IoAssertNotSymbol(self, m)

int ISVECTOR(IoObject *self)
{
	// Don't have ISVECTOR as a macro because self is used twice.
	return ISSEQ(self) && UArray_itemType(IoSeq_rawUArray(self)) == CTYPE_float32_t;
}

void *IoMessage_locals_vectorArgAt_(IoMessage *self, void *locals, int n)
{
	IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

	if (!ISVECTOR(v))
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "Vector");
	}

	return v;
}

void *IoMessage_locals_pointArgAt_(IoMessage *m, void *locals, int n)
{
	IoSeq *self = IoMessage_locals_vectorArgAt_(m, locals, n);
	IOASSERT(IoSeq_rawSize(self) > 1, "Vector not long enough to be used as point argument");
	return self;
}

void IoSeq_assertIsVector(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IOASSERT(ISVECTOR(self), "Seq needs to be of type float32");
}

// ---------------------------------------------

float *IoSeq_makeFloatArrayOfSize_(IoSeq *self, size_t size)
{
	UArray *u = IoSeq_rawUArray(self);
	UArray_setItemType_(u, CTYPE_float32_t);
	UArray_setSize_(u, size);
	return (float *)IoSeq_rawBytes(self);
}

IoSeq *IoSeq_newFloatArrayOfSize_(void *state, size_t size)
{
	IoSeq *self = IoSeq_new(state);
	IoSeq_makeFloatArrayOfSize_(self, size);
	return self;
}

float *IoSeq_floatPointerOfLength_(IoSeq *self, size_t size)
{
	UArray *u = IoSeq_rawUArray(self);

	if(UArray_itemType(u) == CTYPE_float32_t && UArray_size(u) >= size)
	{
		return (float *)UArray_bytes(u);
	}

	return (float *)NULL;
}

// vec2f ---------------------------

IoSeq *IoSeq_newVec2f(void *state, vec2f v)
{
	IoSeq *s = IoSeq_newFloatArrayOfSize_(state, 2);
	memcpy(IoSeq_rawBytes(s), &v, sizeof(vec2f));
	return s;
}

int IoSeq_isVec2f(IoSeq *self)
{
	return IoSeq_floatPointerOfLength_(self, 2) != NULL;
}

vec2f IoSeq_vec2f(IoSeq *self)
{
	float *f = IoSeq_floatPointerOfLength_(self, 2);
	if (!f) { vec2f v = {0,0}; return v; }
	return *((vec2f *)f);
}

void IoSeq_setVec2f_(IoSeq *self, vec2f v)
{
	float *f = IoSeq_floatPointerOfLength_(self, 2);
	if (f) memcpy(f, &v, sizeof(vec2f));
}

// vec3f ---------------------------

IoSeq *IoSeq_newVec3f(void *state, vec3f v)
{
	IoSeq *s = IoSeq_newFloatArrayOfSize_(state, 3);
	memcpy(IoSeq_rawBytes(s), &v, sizeof(vec3f));
	return s;
}

int IoSeq_isVec3f(IoSeq *self)
{
	return IoSeq_floatPointerOfLength_(self, 3) != NULL;
}

vec3f IoSeq_vec3f(IoSeq *self)
{
	float *f = IoSeq_floatPointerOfLength_(self, 3);
	if (!f) { vec3f v = {0,0,0}; return v; }
	return *((vec3f *)f);
}

void IoSeq_setVec3f_(IoSeq *self, vec3f v)
{
	float *f = IoSeq_floatPointerOfLength_(self, 3);
	if (f) memcpy(f, &v, sizeof(vec3f));
}


IoObject *IoSeq_x(IoSeq *self, IoObject *locals, IoMessage *m)
{
	UArray *u = IoSeq_rawUArray(self);
	return IONUMBER(UArray_rawDoubleAt_(u, 0));
}

IoObject *IoSeq_y(IoSeq *self, IoObject *locals, IoMessage *m)
{
	UArray *u = IoSeq_rawUArray(self);
	return IONUMBER(UArray_rawDoubleAt_(u, 1));
}

IoObject *IoSeq_z(IoSeq *self, IoObject *locals, IoMessage *m)
{
	UArray *u = IoSeq_rawUArray(self);
	return IONUMBER(UArray_rawDoubleAt_(u, 2));
}

IoObject *IoSeq_w(IoSeq *self, IoObject *locals, IoMessage *m)
{
	UArray *u = IoSeq_rawUArray(self);
	return IONUMBER(UArray_rawDoubleAt_(u, 3));
}

IoObject *IoSeq_setX(IoSeq *self, IoObject *locals, IoMessage *m)
{
	double v = IoMessage_locals_doubleArgAt_(self, locals, 0);
	UArray *u = IoSeq_rawUArray(self);
	UArray_at_putDouble_(u, 0, v);
	return self;
}

IoObject *IoSeq_setY(IoSeq *self, IoObject *locals, IoMessage *m)
{
	double v = IoMessage_locals_doubleArgAt_(self, locals, 0);
	UArray *u = IoSeq_rawUArray(self);
	UArray_at_putDouble_(u, 1, v);
	return self;
}

IoObject *IoSeq_setZ(IoSeq *self, IoObject *locals, IoMessage *m)
{
	double v = IoMessage_locals_doubleArgAt_(self, locals, 0);
	UArray *u = IoSeq_rawUArray(self);
	UArray_at_putDouble_(u, 2, v);
	return self;
}

IoObject *IoSeq_setW(IoSeq *self, IoObject *locals, IoMessage *m)
{
	double v = IoMessage_locals_doubleArgAt_(self, locals, 0);
	UArray *u = IoSeq_rawUArray(self);
	UArray_at_putDouble_(u, 3, v);
	return self;
}
