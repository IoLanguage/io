/*#io
Box ioDoc(
	docCopyright("Steve Dekorte", 2002)
	docLicense("BSD revised")
*/

#include "IoBox_gl.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoVector_gl.h"
#include "IoOpenGL.h"
#include <math.h>

#define DATA(self) ((IoBoxData *)IoObject_dataPointer(self))

#define GLVERTEX2 glVertex2f
#define GLRECT glRectf

IoObject *IoBox_drawAsRect(IoBox *self, IoObject *locals, IoMessage *m)
{
	vec2f o = IoSeq_vec2f(DATA(self)->origin);
	vec2f s = IoSeq_vec2f(DATA(self)->size);
	GLRECT(o.x, o.y, o.x + s.x, o.y + s.y);
	return self;
}

IoObject *IoBox_drawAsRectOutline(IoBox *self, IoObject *locals, IoMessage *m)
{
	vec2f o = IoSeq_vec2f(DATA(self)->origin);
	vec2f s = IoSeq_vec2f(DATA(self)->size);
	glBegin(GL_LINE_LOOP);
	GLVERTEX2(o.x, o.y);
	GLVERTEX2(o.x, o.y + s.y);
	GLVERTEX2(o.x + s.x, o.y + s.y);
	GLVERTEX2(o.x + s.x, o.y);
	glEnd();
	return self;
}

IoObject *IoBox_scissor(IoBox *self, IoObject *locals, IoMessage *m)
{
	vec2f o = IoSeq_vec2f(DATA(self)->origin);
	vec2f s = IoSeq_vec2f(DATA(self)->size);
	printf("Scissor: %i %i, %i %i\n", (int)o.x, (int)o.y, (int)s.x, (int)s.y);
	glScissor((GLint)o.x, (GLint)o.y, (GLsizei)s.x, (GLsizei)s.y);
	return self;
}

/*
IoObject *IoBox_scissorToUnion(IoBox *self, IoObject *locals, IoMessage *m)
{
	IoBox *other = IoMessage_locals_boxArgAt_(m, locals, 0);
	double x1 = self->origin->x->n;
	double y1 = self->origin->y->n;
	double x2 = x1 + self->size->x->n;
	double y2 = y1 + self->size->y->n;

	double ox1 = other->origin->x->n;
	double oy1 = other->origin->y->n;
	double ox2 = ox1 + other->size->x->n;
	double oy2 = oy1 + other->size->y->n;

	double ux1 = x1 > ox1 ? x1 : ox1;
	double uy1 = y1 > oy1 ? y1 : oy1;
	double ux2 = x2 < ox2 ? x2 : ox2;
	double uy2 = y2 < oy2 ? y2 : oy2;

	double uw = ux2 - ux1;
	double uh = uy2 - uy1;
	glScissor(ux1, uy1, uw, uh);
	return self;
}

IoObject *IoBox_getScissor(IoBox *self, IoObject *locals, IoMessage *m)
{
	double v[4];
	glGetDoublev(GL_SCISSOR_BOX, v);
	self->origin->x->n = v[0];
	self->origin->y->n = v[1];
	self->size->x->n = v[2] - v[0];
	self->size->x->n = v[3] - v[1];
	return self;
}
*/

IoObject *IoBox_glProject(IoBox *self, IoObject *locals, IoMessage *m)
{
	IoSeq_glProject(DATA(self)->origin, locals, m);
	/* TODO: need to scale */
	return self;
}

IoObject *IoBox_glUnproject(IoBox *self, IoObject *locals, IoMessage *m)
{
	IoSeq_glUnproject(DATA(self)->origin, locals, m);
	/* TODO: need to scale */
	return self;
}

void IoBox_glInit(IoObject *context)
{
	IoState *state = IoObject_state(context);

	IoObject *self = IoState_protoWithInitFunction_(state, IoBox_proto);

	{
		IoMethodTable methodTable[] = {
		{"drawAsRect", IoBox_drawAsRect},
		{"drawAsRectOutline", IoBox_drawAsRectOutline},
		{"scissor", IoBox_scissor},
		//{"scissorToUnion", IoBox_scissorToUnion},
		//{"getScissor", IoBox_getScissor},
		{"glProject", IoBox_glProject},
		{"glUnproject", IoBox_glUnproject},
		{NULL, NULL},
		};

		IoObject_addMethodTable_(self, methodTable);
	}
}

