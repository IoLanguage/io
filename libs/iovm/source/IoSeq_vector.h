
typedef float NUM_TYPE;

//typedef IoSeq IoVector;
#define IoVector IoSeq
int ISVECTOR(IoObject *self);

// ---------------------------------------

void *IoMessage_locals_vectorArgAt_(IoMessage *self, void *locals, int n);
void *IoMessage_locals_pointArgAt_(IoMessage *m, void *locals, int n);

void IoSeq_assertIsVector(IoObject *self, IoObject *locals, IoMessage *m);
IoSeq *IoSeq_newFloatArrayOfSize_(void *state, size_t size);
float *IoSeq_floatPointerOfLength_(IoSeq *self, size_t size);

// vec2f ---------------------------

typedef struct
{
	NUM_TYPE x;
	NUM_TYPE y;
} vec2f;

IoSeq *IoSeq_newVec2f(void *state, vec2f v);
int IoSeq_isVec2f(IoSeq *self);
vec2f IoSeq_vec2f(IoSeq *self);
void IoSeq_setVec2f_(IoSeq *self, vec2f v);

// vec3f ---------------------------

typedef struct
{
	NUM_TYPE x;
	NUM_TYPE y;
	NUM_TYPE z;
} vec3f;

IoSeq *IoSeq_newVec3f(void *state, vec3f v);
int IoSeq_isVec3f(IoSeq *self);
vec3f IoSeq_vec3f(IoSeq *self);
void IoSeq_setVec3f_(IoSeq *self, vec3f v);

// --------------

IoObject *IoSeq_x(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_y(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_z(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_w(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_setX(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_setY(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_setZ(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_setW(IoSeq *self, IoObject *locals, IoMessage *m);
