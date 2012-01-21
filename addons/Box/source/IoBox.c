//metadoc Box copyright Steve Dekorte 2002
//metadoc Box license BSD revised
//metadoc Box category Math
/*metadoc Box description
A primitive for fast operations on rectangles.
*/

#include "IoBox.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include <math.h>

static const char *protoId = "Box";

#define DATA(self) ((IoBoxData *)IoObject_dataPointer(self))

const char *IoBox_protoId(void)
{
	return protoId;
}

void *IoMessage_locals_boxArgAt_(IoMessage *self, void *locals, int n)
{
	IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

	if (!ISBOX(v))
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "Box");
	}

	return v;
}

IoTag *IoBox_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoBox_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoBox_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoBox_mark);
	return tag;
}


IoBox *IoBox_rawClone(IoBox *proto)
{
	IoBox *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoBoxData)));

	DATA(self)->origin = IOCLONE(DATA(proto)->origin);
	DATA(self)->size   = IOCLONE(DATA(proto)->size);
	return self;
}

IoBox *IoBox_new(void *state)
{
	IoBox *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

/* ----------------------------------------------------------- */

void IoBox_rawCopy(IoBox *self, IoBox *other)
{
	IoSeq_rawCopy_(DATA(self)->origin, DATA(other)->origin);
	IoSeq_rawCopy_(DATA(self)->size, DATA(other)->size);
}

void IoBox_rawSet(IoBox *self,
				  NUM_TYPE x,
				  NUM_TYPE y,
				  NUM_TYPE z,
				  NUM_TYPE w,
				  NUM_TYPE h,
				  NUM_TYPE d)
{
	vec3f xyz = { x, y, z};
	vec3f whd = { w, h, d};
	IoSeq_setVec3f_(DATA(self)->origin, xyz);
	IoSeq_setVec3f_(DATA(self)->size,   whd);
}

IoBox *IoBox_newSet(void *state,
				NUM_TYPE x,
				NUM_TYPE y,
				NUM_TYPE z,
				NUM_TYPE w,
				NUM_TYPE h,
				NUM_TYPE d)
{
	vec3f xyz = { x, y, z};
	vec3f whd = { w, h, d};
	IoBox *self = IoBox_new(state);
	IoSeq_setVec3f_(DATA(self)->origin, xyz);
	IoSeq_setVec3f_(DATA(self)->size,   whd);
	return self;
}

void IoBox_free(IoBox *self)
{
	free(IoObject_dataPointer(self));
}

void IoBox_mark(IoBox *self)
{
	IoObject_shouldMark((IoObject *)DATA(self)->origin);
	IoObject_shouldMark((IoObject *)DATA(self)->size);
}

IoSeq *IoBox_rawOrigin(IoBox *self)
{
	return DATA(self)->origin;
}

IoSeq *IoBox_rawSize(IoBox *self)
{
	return DATA(self)->size;
}

/* ----------------------------------------------------------- */

IoObject *IoBox_origin(IoBox *self, IoObject *locals, IoMessage *m)
{
	/*doc Box origin
	Returns the point object for the origin of the box.
	*/

	return DATA(self)->origin;
}

IoObject *IoBox_size(IoBox *self, IoObject *locals, IoMessage *m)
{
	/*doc Box size
	Returns the point object for the size of the box.
	*/

	return DATA(self)->size;
}

IoObject *IoBox_width(IoBox *self, IoObject *locals, IoMessage *m)
{
	/*doc Box width
	Same as; size x
	*/

	return IoSeq_x(DATA(self)->size, locals, m);
}

IoObject *IoBox_height(IoBox *self, IoObject *locals, IoMessage *m)
{
	/*doc Box height
	Same as; size y
	*/

	return IoSeq_y(DATA(self)->size, locals, m);
}

IoObject *IoBox_depth(IoBox *self, IoObject *locals, IoMessage *m)
{
	/*doc Box depth 
	Same as; size z
	*/

	return IoSeq_z(DATA(self)->size, locals, m);
}

IoObject *IoBox_set(IoBox *self, IoObject *locals, IoMessage *m)
{
	/*doc Box set(origin, size)
	Copies the values in origin and size to set the box's origin and size.
	*/

	IoSeq_rawCopy_(DATA(self)->origin, IoMessage_locals_pointArgAt_(m, locals, 0));
	IoSeq_rawCopy_(DATA(self)->size,   IoMessage_locals_pointArgAt_(m, locals, 1));
	return self;
}

IoObject *IoBox_setOrigin(IoBox *self, IoObject *locals, IoMessage *m)
{
	/*doc Box setOrigin(aPoint)
	Copies the values in aPoint to the box's origin point.
	*/

	IoSeq_rawCopy_(DATA(self)->origin, IoMessage_locals_pointArgAt_(m, locals, 0));
	return self;
}

IoObject *IoBox_setSize(IoBox *self, IoObject *locals, IoMessage *m)
{
	/*doc Box setSize(aPoint)
	Copies the values in aPoint to the box's size point.
	*/

	IoSeq_rawCopy_(DATA(self)->size, IoMessage_locals_pointArgAt_(m, locals, 0));
	return self;
}

IoObject *IoBox_copy(IoBox *self, IoObject *locals, IoMessage *m)
{
	/*doc Box copy(aBox)
	Copies the values of aBox to the receiver.
	*/

	IoBox *other = IoMessage_locals_boxArgAt_(m, locals, 0);
	IoSeq_rawCopy_(DATA(self)->origin, DATA(other)->origin);
	IoSeq_rawCopy_(DATA(self)->size,   DATA(other)->size);
	return self;
}

IoObject *IoBox_print(IoBox *self, IoObject *locals, IoMessage *m)
{
	/*doc Box print
	Prints a string representation of the receiver to the standard output.
	*/

	IoState_print_(IOSTATE, "Box clone set(");
	IoSeq_print(DATA(self)->origin, locals, m);
	IoState_print_(IOSTATE, ", ");
	IoSeq_print(DATA(self)->size, locals, m);
	IoState_print_(IOSTATE, ")");
	return self;
}

IoObject *IoBox_Union(IoBox *self, IoObject *locals, IoMessage *m)
{
	/*doc Box Union(aBox)
	Returns a new box containing the 2d union of the receiver and aBox.
	*/

	IoBox *other = IoMessage_locals_boxArgAt_(m, locals, 0);
	IoBox_rawUnion(self, other);
	return self;
}

void IoBox_rawUnion(IoBox *self, IoBox *other)
{
	vec2f o1, o2;
	vec2f v1 = IoSeq_vec2f(DATA(self)->origin);
	vec2f v2 = IoSeq_vec2f(DATA(self)->size);

	v2.x += v1.x;
	v2.y += v1.y;

	o1 = IoSeq_vec2f(DATA(other)->origin);
	o2 = IoSeq_vec2f(DATA(other)->size);

	o2.x += o1.x;
	o2.y += o1.y;

	{
		vec2f u1, u2, us;
		NUM_TYPE uw;
		NUM_TYPE uh;

		u1.x = v1.x > o1.x ? v1.x : o1.x;
		u1.y = v1.y > o1.y ? v1.y : o1.y;
		u2.x = v2.x < o2.x ? v2.x : o2.x;
		u2.y = v2.y < o2.y ? v2.y : o2.y;

		uw = u2.x - u1.x;
		uh = u2.y - u1.y;

		IoSeq_setVec2f_(DATA(self)->origin, u1);
		us.x = uw > 0 ? uw:0;
		us.y = uh > 0 ? uh:0;
		IoSeq_setVec2f_(DATA(self)->size, us);
	}
}

int IoBox_rawContains3dPoint(IoBox *self, IoVector *otherPoint)
{
	// should really do this with stack allocated Vectors or something

	vec3f p = IoSeq_vec3f(otherPoint);
	vec3f v = IoSeq_vec3f(DATA(self)->origin);
	vec3f s = IoSeq_vec3f(DATA(self)->size);

	NUM_TYPE px = p.x, py = p.y, pz = p.z;
	NUM_TYPE x = v.x, y = v.y, z = v.z;
	NUM_TYPE w = s.x, h = s.y, d = s.z;

	// Fix to allow Boxes with negative w, h, or d to contain Points. -jk
	if (w < 0) { w = -w; x = -x; px = -px; }
	if (h < 0) { h = -h; y = -y; py = -py; }
	if (d < 0) { d = -d; x = -x; px = -px; }

	return (px >= x) &&
		   (py >= y) &&
		   (pz >= z) &&

		   (px <= x + w) &&
		   (py <= y + h) &&
		   (pz <= z + d);
}


IoObject *IoBox_containsPoint(IoBox *self, IoObject *locals, IoMessage *m)
{
	/*doc Box containsPoint(aPoint)
	Returns true if aPoint is within the receiver's bounds, false otherwise.
	*/

	int result;

	IoVector *otherPoint = IoMessage_locals_pointArgAt_(m, locals, 0);

	UArray *bo = IoSeq_rawUArray(IoBox_rawOrigin(self));
	UArray *bs = IoSeq_rawUArray(IoBox_rawSize(self));
	UArray *p  = IoSeq_rawUArray(otherPoint);

	// do a malloc since the vectors might be large

	UArray *b1 = UArray_clone(bo);
	UArray *b2 = UArray_clone(bs);

	// make bo2 the box endpoint

	UArray_add_(b2, b1);

	// ensure bo1 is on the left bottom and bo2 is on the top right

	UArray_Min(b1, b2);
	UArray_Max(b2, bo);

	result = UArray_greaterThanOrEqualTo_(p, b1) && UArray_greaterThanOrEqualTo_(b2, p);

	UArray_free(b1);
	UArray_free(b2);

	return IOBOOL(self, result);
}

IoObject *IoBox_intersectsBox(IoBox *self, IoObject *locals, IoMessage *m)
{
	/*doc Box intersectsBox(aBox)
	Returns true if aBox is within the receiver's bounds, false otherwise.
	*/

	int result = 0;
	return IOBOOL(self, result);
}


/*
IoObject *IoBox_Min(IoBox *self, IoObject *locals, IoMessage *m)
{
	IoBox *other = IoMessage_locals_pointArgAt_(m, locals, 0);
	if (self->x->n > DATA(other)->x->n) self->x->n = DATA(other)->x->n;
	if (self->y->n > DATA(other)->y->n) self->y->n = DATA(other)->y->n;
	if (self->z->n > DATA(other)->z->n) self->z->n = DATA(other)->z->n;
	return self;
}

IoObject *IoBox_Max(IoBox *self, IoObject *locals, IoMessage *m)
{
	IoBox *other = IoMessage_locals_pointArgAt_(m, locals, 0);
	if (self->x->n < DATA(other)->x->n) self->x->n = DATA(other)->x->n;
	if (self->y->n < DATA(other)->y->n) self->y->n = DATA(other)->y->n;
	if (self->z->n < DATA(other)->z->n) self->z->n = DATA(other)->z->n;
	return self;
}
*/

// --- view resizing ------------------------

static double resizeXFunc(int id, double dx, double x)
{
		switch(id)
		{
			// 1 := fixed, 0 := spring
			case 0: return x + (dx/2);
			case 1: return x - dx;
			case 10: return x + (dx/2);
			case 11:  return x + dx;
			// 110, 111 nop
		}

		return x;
}

static double resizeWFunc(int id, double dx, double w)
{
		switch(id)
		{
			// 1 = fixed, 0 = spring
			case 0: return w + (dx/2);
			case 1: return w + dx;
			case 100: return w + (dx/2);
			case 101: return w + dx;
			// 110, 111 nop
		}

		return w;
}

#ifndef WIN32
#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)
#endif

static double UArray_x(UArray *self) { return UArray_doubleAt_(self, 0); }
static double UArray_y(UArray *self) { return UArray_doubleAt_(self, 1); }

static void UArray_setXY(UArray *self, double x, double y)
{
	UArray_at_putDouble_(self, 0, x);
	UArray_at_putDouble_(self, 1, y);
}

UArray *IoBox_rawResizeBy(IoBox *self,
	UArray *d,
	int resizeWidth, int resizeHeight,
	UArray *minSize, UArray *maxSize)
{
	double x, w, y, h;
	UArray *position = IoSeq_rawUArray(IoBox_rawOrigin(self));
	UArray *size     = IoSeq_rawUArray(IoBox_rawSize(self));
	UArray *outd     = UArray_new();

	UArray_setItemType_(outd, CTYPE_float32_t);
	UArray_setSize_(outd, 2);

	x = resizeXFunc(resizeWidth, UArray_x(d), UArray_x(position));
	w = resizeWFunc(resizeWidth, UArray_x(d), UArray_x(size));

	y = resizeXFunc(resizeHeight, UArray_y(d), UArray_y(position));
	h = resizeWFunc(resizeHeight, UArray_y(d), UArray_y(size));

	if (minSize)
	{
		w = max(w, UArray_x(minSize));
		h = max(h, UArray_y(minSize));
	}

	if (maxSize)
	{
		w = min(w, UArray_x(maxSize));
		h = min(h, UArray_y(maxSize));
	}

	UArray_setXY(outd, w - UArray_x(size), h - UArray_y(size));
	UArray_setXY(position, x, y);
	UArray_setXY(size, w, h);

	UArray_round(position);
	UArray_round(size);

	return outd;
}

IoObject *IoBox_resizeBy(IoBox *self, IoObject *locals, IoMessage *m)
{
	IoSeq *d         = IoMessage_locals_pointArgAt_(m, locals, 0);
	int resizeWidth  = IoMessage_locals_intArgAt_(m, locals, 1);
	int resizeHeight = IoMessage_locals_intArgAt_(m, locals, 2);
	IoSeq *minSize   = IoMessage_locals_valueArgAt_(m, locals, 3);
	IoSeq *maxSize   = IoMessage_locals_valueArgAt_(m, locals, 4);

	UArray *mins = ISNIL(minSize) ? 0x0 : IoSeq_rawUArray(minSize);
	UArray *maxs = ISNIL(maxSize) ? 0x0 : IoSeq_rawUArray(maxSize);

	UArray *outd = IoBox_rawResizeBy(self,
	IoSeq_rawUArray(d),
	resizeWidth, resizeHeight,
	mins, maxs);

	IoSeq *out = IoSeq_newWithUArray_copy_(IOSTATE, outd, 0);

	return out;
}

IoBox *IoBox_proto(void *state)
{
	vec3f o = {0, 0, 0};
	IoBox *self = IoObject_new(state);
	IoObject_tag_(self, IoBox_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoBoxData)));

	DATA(self)->origin = IoSeq_newVec3f(state, o);
	DATA(self)->size   = IoSeq_newVec3f(state, o);

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"set", IoBox_set},
		{"origin", IoBox_origin},
		{"size", IoBox_size},

		{"width", IoBox_width},
		{"height", IoBox_height},
		{"depth", IoBox_depth},

		{"setOrigin", IoBox_setOrigin},
		{"setSize", IoBox_setSize},
		{"Union", IoBox_Union},

		{"print", IoBox_print},
		{"containsPoint", IoBox_containsPoint},
		{"intersectsBox", IoBox_intersectsBox},
		/*
		{"asString", IoBox_asString},
		{"Min", IoBox_Min},
		{"Max", IoBox_Max},
		*/

		{"resizeBy", IoBox_resizeBy},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}
