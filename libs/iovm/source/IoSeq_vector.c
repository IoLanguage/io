/*cmetadoc Sequence description
Vector-math partition of the Sequence proto. Treats a Sequence whose
UArray itemType is float32 as a small numeric vector (typically 2, 3,
or 4 elements) for graphics-style vec2f/vec3f/vec4f operations. The
x/y/z/w accessors read and write via UArray_rawDoubleAt_ /
UArray_at_putDouble_, so any itemType that supports double coercion
works for reads; writers assume float32 storage. The vec2f/vec3f
struct bridges memcpy directly in and out of the UArray's byte buffer
because basekit guarantees float32 arrays are packed and aligned.
ISVECTOR is a function rather than a macro to avoid double-evaluating
its argument in the common ISVECTOR(expensive_call()) pattern.
*/

#include "IoSeq.h"
#include "IoNumber.h"

#define IO_ASSERT_NOT_SYMBOL(self) IoAssertNotSymbol(self, m)

/*cdoc Sequence ISVECTOR(self)
Type predicate — a "Vector" is a Sequence whose UArray itemType is
float32. Written as a function (not a macro) so the argument is
evaluated exactly once, since ISSEQ and UArray_itemType both need to
probe it.
*/
int ISVECTOR(IoObject *self) {
    // Do not have ISVECTOR as a macro because self is used twice.
    return ISSEQ(self) &&
           UArray_itemType(IoSeq_rawUArray(self)) == CTYPE_float32_t;
}

/*cdoc Sequence IoMessage_locals_vectorArgAt_(self, locals, n)
Extracts and type-checks the n-th argument as a Vector. Raises a
Vector-typed error through IoMessage_locals_numberArgAt_errorForType_
when the arg is not a float32 Sequence so bindings get the same
error format as scalar-typed coercions.
*/
void *IoMessage_locals_vectorArgAt_(IoMessage *self, void *locals, int n) {
    IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

    if (!ISVECTOR(v)) {
        IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "Vector");
    }

    return v;
}

/*cdoc Sequence IoMessage_locals_pointArgAt_(m, locals, n)
Like vectorArgAt_ but additionally asserts the Vector has at least 2
elements, so 2D graphics code can safely dereference x/y without a
separate bounds check. Raises via IOASSERT (IO-level error) if not.
*/
void *IoMessage_locals_pointArgAt_(IoMessage *m, void *locals, int n) {
    IoSeq *self = IoMessage_locals_vectorArgAt_(m, locals, n);
    IOASSERT(IoSeq_rawSize(self) > 1,
             "Vector not long enough to be used as point argument");
    return self;
}

/*cdoc Sequence IoSeq_assertIsVector(self, locals, m)
Raises an Io-level error if the receiver is not a float32-typed
Sequence. Used by vector-math methods that need the strict typing
guarantee before memcpy'ing struct-shaped buffers.
*/
void IoSeq_assertIsVector(IoSeq *self, IoObject *locals, IoMessage *m) {
    if (!(ISVECTOR(self))) {
        IoState_error_(IOSTATE, m, "Io Assertion 'Seq needs to be of type float32'");
    }
}

// ---------------------------------------------

/*cdoc Sequence IoSeq_makeFloatArrayOfSize_(self, size)
Reconfigures the receiver's UArray to hold `size` float32 elements
(zeroed) and returns a typed pointer into its bytes. Destructive —
discards existing content. Caller typically memcpy's a vec struct
into the returned pointer immediately.
*/
float *IoSeq_makeFloatArrayOfSize_(IoSeq *self, size_t size) {
    UArray *u = IoSeq_rawUArray(self);
    UArray_setItemType_(u, CTYPE_float32_t);
    UArray_setSize_(u, size);
    return (float *)IoSeq_rawBytes(self);
}

/*cdoc Sequence IoSeq_newFloatArrayOfSize_(state, size)
Convenience: allocate a Sequence and configure it as a float32 array
of the requested length in one step.
*/
IoSeq *IoSeq_newFloatArrayOfSize_(void *state, size_t size) {
    IoSeq *self = IoSeq_new(state);
    IoSeq_makeFloatArrayOfSize_(self, size);
    return self;
}

/*cdoc Sequence IoSeq_floatPointerOfLength_(self, size)
Returns a borrowed float pointer into the receiver's bytes if it
is a float32 Sequence of at least `size` elements; otherwise NULL.
Used by the vec2f/vec3f bridges to decide whether direct memcpy is
safe without tripping a bounds or type violation.
*/
float *IoSeq_floatPointerOfLength_(IoSeq *self, size_t size) {
    UArray *u = IoSeq_rawUArray(self);

    if (UArray_itemType(u) == CTYPE_float32_t && UArray_size(u) >= size) {
        return (float *)UArray_bytes(u);
    }

    return (float *)NULL;
}

// vec2f ---------------------------

/*cdoc Sequence IoSeq_newVec2f(state, v)
Creates a new 2-element float32 Sequence from a vec2f struct by
memcpy. Relies on vec2f being tightly packed float x, float y — any
struct padding would break the layout match.
*/
IoSeq *IoSeq_newVec2f(void *state, vec2f v) {
    IoSeq *s = IoSeq_newFloatArrayOfSize_(state, 2);
    memcpy(IoSeq_rawBytes(s), &v, sizeof(vec2f));
    return s;
}

int IoSeq_isVec2f(IoSeq *self) {
    return IoSeq_floatPointerOfLength_(self, 2) != NULL;
}

/*cdoc Sequence IoSeq_vec2f(self)
Reads the receiver's first two floats into a vec2f struct, returning
{0,0} if the receiver is not a suitable float32 Sequence. The zero
fallback keeps misuse from crashing — callers that must distinguish
present-but-zero from absent should test isVec2f first.
*/
vec2f IoSeq_vec2f(IoSeq *self) {
    float *f = IoSeq_floatPointerOfLength_(self, 2);
    if (!f) {
        vec2f v = {0, 0};
        return v;
    }
    return *((vec2f *)f);
}

/*cdoc Sequence IoSeq_setVec2f_(self, v)
Writes a vec2f into the receiver's first two floats. Silent no-op
if the receiver is not a float32 Sequence of length >= 2 — consistent
with the vec2f reader's permissive behavior.
*/
void IoSeq_setVec2f_(IoSeq *self, vec2f v) {
    float *f = IoSeq_floatPointerOfLength_(self, 2);
    if (f)
        memcpy(f, &v, sizeof(vec2f));
}

// vec3f ---------------------------

/*cdoc Sequence IoSeq_newVec3f(state, v)
3-element analogue of IoSeq_newVec2f. Same layout assumption about
vec3f being packed float x, y, z.
*/
IoSeq *IoSeq_newVec3f(void *state, vec3f v) {
    IoSeq *s = IoSeq_newFloatArrayOfSize_(state, 3);
    memcpy(IoSeq_rawBytes(s), &v, sizeof(vec3f));
    return s;
}

int IoSeq_isVec3f(IoSeq *self) {
    return IoSeq_floatPointerOfLength_(self, 3) != NULL;
}

/*cdoc Sequence IoSeq_vec3f(self)
3-element analogue of IoSeq_vec2f. Returns {0,0,0} when the receiver
is not a float32 Sequence of length >= 3.
*/
vec3f IoSeq_vec3f(IoSeq *self) {
    float *f = IoSeq_floatPointerOfLength_(self, 3);
    if (!f) {
        vec3f v = {0, 0, 0};
        return v;
    }
    return *((vec3f *)f);
}

/*cdoc Sequence IoSeq_setVec3f_(self, v)
3-element analogue of IoSeq_setVec2f_.
*/
void IoSeq_setVec3f_(IoSeq *self, vec3f v) {
    float *f = IoSeq_floatPointerOfLength_(self, 3);
    if (f)
        memcpy(f, &v, sizeof(vec3f));
}

IO_METHOD(IoSeq, x) {
    UArray *u = IoSeq_rawUArray(self);
    return IONUMBER(UArray_rawDoubleAt_(u, 0));
}

IO_METHOD(IoSeq, y) {
    UArray *u = IoSeq_rawUArray(self);
    return IONUMBER(UArray_rawDoubleAt_(u, 1));
}

IO_METHOD(IoSeq, z) {
    UArray *u = IoSeq_rawUArray(self);
    return IONUMBER(UArray_rawDoubleAt_(u, 2));
}

IO_METHOD(IoSeq, w) {
    UArray *u = IoSeq_rawUArray(self);
    return IONUMBER(UArray_rawDoubleAt_(u, 3));
}

IO_METHOD(IoSeq, setX) {
    double v = IoMessage_locals_doubleArgAt_(m, locals, 0);
    UArray *u = IoSeq_rawUArray(self);
    UArray_at_putDouble_(u, 0, v);
    return self;
}

IO_METHOD(IoSeq, setY) {
    double v = IoMessage_locals_doubleArgAt_(m, locals, 0);
    UArray *u = IoSeq_rawUArray(self);
    UArray_at_putDouble_(u, 1, v);
    return self;
}

IO_METHOD(IoSeq, setZ) {
    double v = IoMessage_locals_doubleArgAt_(m, locals, 0);
    UArray *u = IoSeq_rawUArray(self);
    UArray_at_putDouble_(u, 2, v);
    return self;
}

IO_METHOD(IoSeq, setW) {
    double v = IoMessage_locals_doubleArgAt_(m, locals, 0);
    UArray *u = IoSeq_rawUArray(self);
    UArray_at_putDouble_(u, 3, v);
    return self;
}
