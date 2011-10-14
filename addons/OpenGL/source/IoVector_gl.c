//metadoc Vector copyright Steve Dekorte 2002
//metadoc Vector license BSD revised

#include "IoVector_gl.h"
#include "List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoOpenGL.h"
#include <math.h>

#include "GLIncludes.h"

void IoVector_glInit(IoObject *context)
{
	IoState *state = IoObject_state(context);
	IoObject *self = IoState_protoWithName_(state, "Sequence");

	{
		IoMethodTable methodTable[] = {
		{"glNormal", IoSeq_glNormal},
		{"glTranslate", IoSeq_glTranslate},
		{"glTranslatei", IoSeq_glTranslatei},
		{"glRotate", IoSeq_glRotate},
		{"glScale", IoSeq_glScale},
		{"glUnproject", IoSeq_glUnproject},
		{"glProject", IoSeq_glProject},

		{"glVertex", IoSeq_glVertex},
		{"glColor", IoSeq_glColor},
		{"glClearColor", IoSeq_glClearColor},
		{"drawQuadTo", IoSeq_drawQuadTo},
		{"drawLineLoop", IoSeq_drawLineLoop},
		{"drawLineLoopi", IoSeq_drawLineLoopi},
		{"drawQuad", IoSeq_drawQuad},

		{"red", IoSeq_x},
		{"green", IoSeq_y},
		{"blue", IoSeq_z},
		{"alpha", IoSeq_w},

		{"setRed", IoSeq_setX},
		{"setGreen", IoSeq_setY},
		{"setBlue", IoSeq_setZ},
		{"setAlpha", IoSeq_setW},

		{"drawAsLine", IoSeq_drawAsLine},
		{"drawFilled", IoSeq_drawFilled},

		/*
		{"drawLineTo", IoSeq_drawLineTo},
		{"drawLineLoopTo", IoSeq_drawLineLoopTo},
		*/
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
}

IoObject *IoSeq_glNormal(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
	float *f = IoSeq_floatPointerOfLength_(self, 3);
	glNormal3f(f[0], f[1], f[2]);
	}
	return self;
}

IoObject *IoSeq_glTranslate(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
	float *f = IoSeq_floatPointerOfLength_(self, 3);
	glTranslatef(f[0], f[1], f[2]);
	}
	return self;
}

IoObject *IoSeq_glTranslatei(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
	float *f = IoSeq_floatPointerOfLength_(self, 3);
	glTranslated(
				(GLdouble)(GLint)(f[0]),
				(GLdouble)(GLint)(f[1]),
				(GLdouble)(GLint)(f[2]));
	}
	return self;
}

IoObject *IoSeq_glRotate(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
	float angle = IoMessage_locals_doubleArgAt_(m, locals, 0);
	float *f = IoSeq_floatPointerOfLength_(self, 3);
	glRotatef(angle, f[0], f[1], f[2]);
	}
	return self;
}

IoObject *IoSeq_glScale(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
	float *f = IoSeq_floatPointerOfLength_(self, 3);
	glScalef(f[0], f[1], f[2]);
	}
	return self;
}

IoObject *IoSeq_glVertex(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
	float *f = IoSeq_floatPointerOfLength_(self, 3);
	glVertex3f(f[0], f[1], f[2]);
	}
	return self;
}

IoObject *IoSeq_glColor(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
	float *f = IoSeq_floatPointerOfLength_(self, 4);
	glColor4f(f[0], f[1], f[2], f[3]);
	}
	return self;
}

IoObject *IoSeq_glClearColor(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
	float *f = IoSeq_floatPointerOfLength_(self, 4);
	glClearColor(f[0], f[1], f[2], f[3]);
	}
	return self;
}

IoObject *IoSeq_glUnproject(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
		float *f = IoSeq_floatPointerOfLength_(self, 3);
		GLint viewport[4];
		GLdouble mvmatrix[16];
		GLdouble projmatrix[16];
		GLdouble wx, wy, wz;

		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

		gluUnProject((GLdouble)(f[0]),
					 (GLdouble)(f[1]),
					 (GLdouble)(f[2]),
					 mvmatrix, projmatrix, viewport,
					 &wx, &wy, &wz);
		if (wz == 1) wz = 0; /* hack for orthographic projections? */

		f[0] = wx;
		f[1] = wy;
		f[2] = wz;
	}
	return self;
}

IoObject *IoSeq_glProject(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
		float *f = IoSeq_floatPointerOfLength_(self, 3);
		/* Project the receiver from object to window coordinates. */
		GLint viewport[4];
		GLdouble mvmatrix[16];
		GLdouble projmatrix[16];
		GLdouble objx, objy, objz;

		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

		gluProject((GLdouble)(f[0]),
				   (GLdouble)(f[1]),
				   (GLdouble)(f[2]),
				   mvmatrix, projmatrix, viewport,
				   &objx, &objy, &objz);

		f[0] = objx;
		f[1] = objy;
		f[2] = objz;
	}
	return self;
}

IoObject *IoSeq_drawLineTo(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
	IoSeq *other = IoMessage_locals_pointArgAt_(m, locals, 0);
	vec3f p  = IoSeq_vec3f(self);
	vec3f p2 = IoSeq_vec3f(other);
	glBegin(GL_LINES);
	glVertex3f((GLdouble)(p.x),  (GLdouble)(p.y),  (GLdouble)(p.z));
	glVertex3f((GLdouble)(p2.x), (GLdouble)(p2.y), (GLdouble)(p2.z));
	glEnd();
	}
	return self;
}

IoObject *IoSeq_drawLineLoopTo(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
	IoSeq *other = IoMessage_locals_pointArgAt_(m, locals, 0);
	vec3f p  = IoSeq_vec3f(self);
	vec3f p2 = IoSeq_vec3f(other);
	glBegin(GL_LINE_LOOP);
	glVertex3d((GLdouble)(p.x),  (GLdouble)(p.y), 0);
	glVertex3d((GLdouble)(p2.x), (GLdouble)(p.y), 0);
	glVertex3d((GLdouble)(p2.x), (GLdouble)(p2.y), 0);
	glVertex3d((GLdouble)(p.x),  (GLdouble)(p2.y), 0);
	glEnd();
	}
	return self;
}

IoObject *IoSeq_drawLineLoop(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
	vec2f p  = IoSeq_vec2f(self);
	GLdouble x = (p.x);
	GLdouble y = (p.y);

	glBegin(GL_LINES);
	glVertex2d(x, y);
	glVertex2d(0, y);

	glVertex2d(0, y);
	glVertex2d(0, 0);

	glVertex2d(0, 0);
	glVertex2d(x, 0);

	glVertex2d(x, 0);
	glVertex2d(x, y);
	glEnd();
	}
	return self;
}

IoObject *IoSeq_drawLineLoopi(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
	vec2f p  = IoSeq_vec2f(self);
	GLint x = (p.x);
	GLint y = (p.y);
	GLint s = 0;

	if (IoMessage_argCount(m))
	{
		s = (GLint)IoMessage_locals_doubleArgAt_(m, locals, 0);
	}

	glBegin(GL_LINES);
	glVertex2i(x-s, y-s);
	glVertex2i(s, y-s);

	glVertex2i(s, y-s);
	glVertex2i(s, s);

	glVertex2i(s, s);
	glVertex2i(x-s, s);

	glVertex2i(x-s, s);
	glVertex2i(x-s, y-s);
	glEnd();
	}
	return self;
}

IoObject *IoSeq_drawQuadTo(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
	IoSeq *other = IoMessage_locals_pointArgAt_(m, locals, 0);
	vec2f p  = IoSeq_vec2f(self);
	vec2f p2 = IoSeq_vec2f(other);
	double x1, y1, x2, y2;
	double s = 0;

	if (IoMessage_argCount(m) > 1)
	{
		s = IoMessage_locals_doubleArgAt_(m, locals, 1);
	}

	x1 = (p.x) + s;
	y1 = (p.y) + s;
	x2 = (p2.x) - s;
	y2 = (p2.y) - s;

	glBegin(GL_QUADS);
	glVertex2d(x1, y1);
	glVertex2d(x2, y1);
	glVertex2d(x2, y2);
	glVertex2d(x1, y2);
	glEnd();
	}
	return self;
}

IoObject *IoSeq_drawQuad(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
	vec2f p = IoSeq_vec2f(self);
	double x, y, w, h;
	double s = 0;

	if (IoMessage_argCount(m) > 1)
	{
		s = -IoMessage_locals_doubleArgAt_(m, locals, 1);
	}

	x = s;
	y = s;
	w = (p.x) - (s*2);
	h = (p.y) - (s*2);

	glBegin(GL_QUADS);
	glVertex2d(w, h);
	glVertex2d(x, h);
	glVertex2d(x, y);
	glVertex2d(w, y);
	glEnd();
	}
	return self;
}

IoObject *IoSeq_drawAsLine(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
	size_t i, size = IoSeq_rawSize(self);
	float *values = (float *)IoSeq_rawBytes(self);

	glBegin(GL_LINE_STRIP);

	for (i = 0; i < size; i ++)
	{
		glVertex2f((float)i, values[i]);
	}

	glEnd();
	}
	return self;
}

IoObject *IoSeq_drawFilled(IoSeq *self, IoObject *locals, IoMessage *m)
{
	IoSeq_assertIsVector(self, locals, m);
	{
	size_t i, size = IoSeq_rawSize(self);
	float *values = (float *)IoSeq_rawBytes(self);

	glBegin(GL_TRIANGLE_STRIP);

	for (i = 0; i < size; i ++)
	{
		float x = i;
		float y = values[i];

		glVertex2f(x, 0);
		glVertex2f(x, y);
	}

	glEnd();
	}
	return self;
}

