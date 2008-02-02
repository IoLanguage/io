//metadoc GLScissor copyright Steve Dekorte 2002
//metadoc GLScissor license BSD revised

#ifndef IOGLSCISSOR_DEFINED
#define IOGLSCISSOR_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoBox.h"
#include "IoOpenGL.h"

#define ISGLScissor(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoGLScissor_rawClone)

typedef IoObject IoGLScissor;

typedef struct
{
	IoBox *rect;
	IoBox *tmpRect;
} IoGLScissorData;

IoGLScissor *IoGLScissor_rawClone(IoGLScissor *self);
IoGLScissor *IoGLScissor_proto(void *state);
IoGLScissor *IoGLScissor_new(void *state);

void IoGLScissor_free(IoGLScissor *self);
void IoGLScissor_mark(IoGLScissor *self);

/* ----------------------------------------------------------- */
IoObject *IoGLScissor_rect(IoGLScissor *self, IoObject *locals, IoMessage *m);
IoObject *IoGLScissor_setScreenRect(IoGLScissor *self, IoObject *locals, IoMessage *m);
IoObject *IoGLScissor_setViewRect(IoGLScissor *self, IoObject *locals, IoMessage *m);

void IoGLScissor_rawSync(IoGLScissor *self);
IoObject *IoGLScissor_sync(IoGLScissor *self, IoObject *locals, IoMessage *m);

IoObject *IoGLScissor_set(IoGLScissor *self, IoObject *locals, IoMessage *m);
IoObject *IoGLScissor_on(IoGLScissor *self, IoObject *locals, IoMessage *m);
IoObject *IoGLScissor_off(IoGLScissor *self, IoObject *locals, IoMessage *m);
IoObject *IoGLScissor_isOn(IoGLScissor *self, IoObject *locals, IoMessage *m);

IoObject *IoGLScissor_push(IoGLScissor *self, IoObject *locals, IoMessage *m);
IoObject *IoGLScissor_pop(IoGLScissor *self, IoObject *locals, IoMessage *m);

IoObject *IoGLScissor_unionWithViewRect(IoGLScissor *self, IoObject *locals, IoMessage *m);
IoObject *IoGLScissor_unionWithScreenRect(IoGLScissor *self, IoObject *locals, IoMessage *m);
IoObject *IoGLScissor_isVisible(IoGLScissor *self, IoObject *locals, IoMessage *m);

#endif
