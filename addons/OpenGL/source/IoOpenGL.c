
//metadoc OpenGL copyright Steve Dekorte 2002
//metadoc OpenGL license BSD revised
//metadoc OpenGL category Graphics

#include "IoOpenGL.h"

#include "List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoCFunction.h"
#include "IoSeq.h"
#include "IoList.h"
#include "List.h"

/* --- GL -------------------------------------------------- */

void GL_getDoubleVector(IoGL *self,
					IoObject *locals,
					IoMessage *m,
					List *list,
					GLdouble **v,
					int max,
					char *desc)
{
	int i, size = List_size(list);

	if (max == 0)
	{
		*v = (GLdouble *)malloc(size * sizeof(GLdouble));
	}

	for (i = 0; i < size && ((max == 0) || (i < max)); i++)
	{
		IoNumber *num = List_at_(list, i);

		if (!ISNUMBER(num))
		{
			char fname[48];
			snprintf(fname, 48, "Io GL %s", desc);
			IoState_error_(IOSTATE, m, "%s parameter List item #%i must be a Number", fname, i);
		}

		(*v)[i] = IoNumber_asDouble(num);
	}
}

void GL_getFloatVector(IoGL *self,
					   IoObject *locals,
					   IoMessage *m,
					   List *list,
					   GLfloat **v,
					   int max,
					   char *desc)
{
	int i, size = List_size(list);

	if (max == 0)
	{
		*v = (GLfloat *)malloc(size*sizeof(GLfloat));
	}

	for (i = 0; i < size && ((max == 0) || (i < max)); i ++)
	{
		IoNumber *num = List_at_(list, i);

		if (!ISNUMBER(num))
		{
			char fname[48];
			snprintf(fname, 48, "Io GL %s", desc);
			IoState_error_(IOSTATE, m, "%s parameter List item #%i must be a Number", fname, i);
		}

		(*v)[i] = (GLfloat)IoNumber_asDouble(num);
	}
}

void GL_getIntVector(IoGL *self,
					 IoObject *locals,
					 IoMessage *m,
					 List *list,
					 GLint **v,
					 int max,
					 char *desc)
{
	int i, size = List_size(list);

	if (max == 0)
	{
		*v = (GLint *)malloc(size*sizeof(GLint));
	}

	for (i = 0; i < size && ((max == 0) || (i < max)); i++)
	{
		IoNumber *num = List_at_(list, i);

		if (!ISNUMBER(num))
		{
			char fname[48];
			snprintf(fname, 48, "Io GL %s", desc);
			IoState_error_(IOSTATE, m, "%s parameter List item #%i must be a Number", fname, i);
		}

		(*v)[i] = (GLint)IoNumber_asInt(num);
	}
}

IoObject *IoGL_glViewport(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLint x = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint y = IoMessage_locals_intArgAt_(m, locals, 1);
	GLsizei width = IoMessage_locals_intArgAt_(m, locals, 2);
	GLsizei height = IoMessage_locals_intArgAt_(m, locals, 3);
	glViewport(x, y, width, height);
	return self;
}

IoObject *IoGL_glClear(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLbitfield bitfield = IoMessage_locals_intArgAt_(m, locals, 0);
	glClear(bitfield);
	return self;
}

IoObject *IoGL_glClearDepth(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLclampd depth = IoMessage_locals_doubleArgAt_(m, locals, 0);
	glClearDepth(depth);
	return self;
}

IoObject *IoGL_glClearIndex(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLfloat c = IoMessage_locals_doubleArgAt_(m, locals, 0);
	glClearIndex(c);
	return self;
}

IoObject *IoGL_glAlphaFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum func = IoMessage_locals_intArgAt_(m, locals, 0);
	GLclampf ref = IoMessage_locals_doubleArgAt_(m, locals, 1);
	glAlphaFunc(func,ref);
	return self;
}

IoObject *IoGL_glDepthFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum func = IoMessage_locals_intArgAt_(m, locals, 0);

	glDepthFunc(func);
	return self;
}

IoObject *IoGL_glDepthMask(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLboolean flag = IoMessage_locals_intArgAt_(m, locals, 0);

	glDepthMask(flag);
	return self;
}

IoObject *IoGL_glIndexMask(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLuint mask = IoMessage_locals_intArgAt_(m, locals, 0);

	glIndexMask(mask);
	return self;
}

IoObject *IoGL_glDepthRange(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLclampd zNear = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLclampd zFar = IoMessage_locals_doubleArgAt_(m, locals, 1);

	glDepthRange(zNear,zFar);
	return self;
}

/* Stencil Functions */

IoObject *IoGL_glStencilMask(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLuint mask = IoMessage_locals_intArgAt_(m, locals, 0);

	glStencilMask(mask);
	return self;
}

IoObject *IoGL_glStencilFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum func = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint ref = IoMessage_locals_intArgAt_(m, locals, 1);
	GLuint mask = IoMessage_locals_intArgAt_(m, locals, 2);

	glStencilFunc(func,ref,mask);
	return self;
}

IoObject *IoGL_glStencilOp(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum fail = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum zfail = IoMessage_locals_intArgAt_(m, locals, 1);
	GLenum zpass = IoMessage_locals_intArgAt_(m, locals, 2);

	glStencilOp(fail,zfail,zpass);
	return self;
}

IoObject *IoGL_glClearStencil(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLint s = IoMessage_locals_intArgAt_(m, locals, 0);
	glClearStencil(s);
	return self;
}

IoObject *IoGL_glClipPlane(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum plane = IoMessage_locals_intArgAt_(m, locals, 0);
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 1));
	GLdouble equation[4] = {0.0, 0.0, 0.0, 0.0};
	GL_getDoubleVector(self, locals, m, list, (GLdouble **)&equation, 4, "glClipPlane");
	glClipPlane(plane,equation);
	return self;
}

/* --- */

IoObject *IoGL_glClearColor(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLclampf r = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLclampf g = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLclampf b = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLclampf a = IoMessage_locals_doubleArgAt_(m, locals, 3);
	glClearColor(r, g, b, a);
	return self;
}

IoObject *IoGL_glColorMask(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLboolean r = IoMessage_locals_intArgAt_(m, locals, 0);
	GLboolean g = IoMessage_locals_intArgAt_(m, locals, 1);
	GLboolean b = IoMessage_locals_intArgAt_(m, locals, 2);
	GLboolean a = IoMessage_locals_intArgAt_(m, locals, 3);
	glColorMask(r, g, b, a);
	return self;
}

IoObject *IoGL_glColor3d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble r = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble g = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble b = IoMessage_locals_doubleArgAt_(m, locals, 2);
	glColor3d(r, g, b);
	return self;
}

IoObject *IoGL_glColor3dv(IoGL *self, IoObject *locals, IoMessage *m)
{
	IoObject *o = IoMessage_locals_valueArgAt_(m, locals, 0);
	GLdouble v[3] = {0.0, 0.0, 0.0};

	if (ISVECTOR(o))
	{
		size_t i;
		UArray *vector = IoSeq_rawUArray(o);

		if(UArray_size(vector) < 3)
		{
			IoState_error_(IOSTATE, m, "Vector argument to glColor4dv must contain atleast 3 elements");
		}

		for(i = 0; i < 3; i++)
			v[i] = (double)UArray_doubleAt_(vector, i);
	}
	else if (ISLIST(o))
	{
		List *list = IoList_rawList(o);
		GL_getDoubleVector(self, locals, m, list, (GLdouble **)&v, 3, "Color3dv");
	}
	else
	{
		IoState_error_(IOSTATE, m, "argument to glColor3dv must be a List or a Vector");
	}
	glColor3dv(v);
	return self;
}

IoObject *IoGL_glColor4d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble r = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble g = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble b = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLdouble a = IoMessage_locals_doubleArgAt_(m, locals, 3);
	glColor4d(r, g, b, a);
	return self;
}

IoObject *IoGL_glColor4dv(IoGL *self, IoObject *locals, IoMessage *m)
{
	IoObject *o = IoMessage_locals_valueArgAt_(m, locals, 0);
	GLdouble v[4] = {0.0, 0.0, 0.0, 0.0};

	if (ISVECTOR(o))
	{
		size_t i;
		UArray *vector = IoSeq_rawUArray(o);
		if(UArray_size(vector) < 4)
		{
			IoState_error_(IOSTATE, m, "Vector argument to glColor4dv must contain atleast 4 elements");
		}
		for(i = 0; i < 4; i++)
			v[i] = (double)UArray_rawDoubleAt_(vector, i);
	}
	else if (ISLIST(o))
	{
		List *list = IoList_rawList(o);
		GL_getDoubleVector(self,locals,m,list,(GLdouble **)&v,4,"Color4dv");
	}
	else
	{
		IoState_error_(IOSTATE, m, "argument to glColor4dv must be a List or a Vector");
	}

	glColor4dv(v);
	return self;
}

IoObject *IoGL_glMap1d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLdouble u1 = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble u2 = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLint stride = IoMessage_locals_intArgAt_(m, locals, 3);
	GLint order = IoMessage_locals_intArgAt_(m, locals, 4);
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 5));
	GLdouble *points = NULL;

	GL_getDoubleVector(self,locals,m,list,(GLdouble **)&points,0,"glMap1d");
	if (points) {
		glMap1d(target,u1,u2,stride,order,points);
		free(points);
	}
	return self;
}

IoObject *IoGL_glMap2d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLdouble u1 = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble u2 = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLint ustride = IoMessage_locals_intArgAt_(m, locals, 3);
	GLint uorder = IoMessage_locals_intArgAt_(m, locals, 4);
	GLdouble v1 = IoMessage_locals_doubleArgAt_(m, locals, 5);
	GLdouble v2 = IoMessage_locals_doubleArgAt_(m, locals, 6);
	GLint vstride = IoMessage_locals_intArgAt_(m, locals, 7);
	GLint vorder = IoMessage_locals_intArgAt_(m, locals, 8);
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 9));
	GLdouble *points = NULL;

	GL_getDoubleVector(self,locals,m,list,(GLdouble **)&points,0,"glMap2d");
	if (points) {
		glMap2d(target,u1,u2,ustride,uorder,v1,v2,vstride,vorder,points);
		free(points);
	}
	return self;
}

IoObject *IoGL_glMapGrid1d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLint un = IoMessage_locals_intArgAt_(m, locals, 0);
	GLdouble u1 = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble u2 = IoMessage_locals_doubleArgAt_(m, locals, 2);

	glMapGrid1d(un,u1,u2);
	return self;
}

IoObject *IoGL_glMapGrid2d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLint un = IoMessage_locals_intArgAt_(m, locals, 0);
	GLdouble u1 = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble u2 = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLint vn = IoMessage_locals_intArgAt_(m, locals, 3);
	GLdouble v1 = IoMessage_locals_doubleArgAt_(m, locals, 4);
	GLdouble v2 = IoMessage_locals_doubleArgAt_(m, locals, 5);

	glMapGrid2d(un,u1,u2,vn,v1,v2);
	return self;
}

IoObject *IoGL_glColorPointer(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLint size = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum type = IoMessage_locals_intArgAt_(m, locals, 1);
	GLsizei stride = IoMessage_locals_intArgAt_(m, locals, 2);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 3);
	GLvoid  *pointer = (GLvoid *)IoSeq_rawBytes(data);

	glColorPointer(size,type,stride,pointer);
	return self;
}

IoObject *IoGL_glIndexPointer(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum type = IoMessage_locals_intArgAt_(m, locals, 0);
	GLsizei stride = IoMessage_locals_intArgAt_(m, locals, 1);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 2);
	GLvoid  *pointer = (GLvoid *)IoSeq_rawBytes(data);

	glIndexPointer(type,stride,pointer);
	return self;
}

IoObject *IoGL_glNormalPointer(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum type = IoMessage_locals_intArgAt_(m, locals, 0);
	GLsizei stride = IoMessage_locals_intArgAt_(m, locals, 1);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 2);
	GLvoid  *pointer = (GLvoid *)IoSeq_rawBytes(data);

	glNormalPointer(type,stride,pointer);
	return self;
}

IoObject *IoGL_glTexCoordPointer(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLint size = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum type = IoMessage_locals_intArgAt_(m, locals, 1);
	GLsizei stride = IoMessage_locals_intArgAt_(m, locals, 2);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 3);
	GLvoid  *pointer = (GLvoid *)IoSeq_rawBytes(data);

	glTexCoordPointer(size,type,stride,pointer);
	return self;
}

IoObject *IoGL_glVertexPointer(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLint size = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum type = IoMessage_locals_intArgAt_(m, locals, 1);
	GLsizei stride = IoMessage_locals_intArgAt_(m, locals, 2);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 3);
	GLvoid  *pointer = (GLvoid *)IoSeq_rawBytes(data);

	glVertexPointer(size,type,stride,pointer);
	return self;
}

IoObject *IoGL_glGetString(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum name = IoMessage_locals_intArgAt_(m, locals, 0);
	GLubyte *r = (GLubyte *)glGetString(name);
	return IOSYMBOL(r);
}

IoObject *IoGL_glGetPolygonStipple(IoGL *self, IoObject *locals, IoMessage *m)
{
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 0);
	GLubyte  *mask = (GLubyte *)IoSeq_rawBytes(data);

	glGetPolygonStipple(mask);
	return self;
}

IoObject *IoGL_glGetPixelMapfv(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum map = IoMessage_locals_intArgAt_(m, locals, 0);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 1);
	GLvoid  *values = (GLvoid *)IoSeq_rawBytes(data);
	glGetPixelMapfv(map,values);
	return self;
}

IoObject *IoGL_glGetPixelMapuiv(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum map = IoMessage_locals_intArgAt_(m, locals, 0);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 1);
	GLvoid  *values = (GLvoid *)IoSeq_rawBytes(data);
	glGetPixelMapuiv(map,values);
	return self;
}

IoObject *IoGL_glGetPixelMapusv(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum map = IoMessage_locals_intArgAt_(m, locals, 0);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 1);
	GLvoid  *values = (GLvoid *)IoSeq_rawBytes(data);
	glGetPixelMapusv(map,values);
	return self;
}

/* Color index */

IoObject *IoGL_glIndexd(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble c = IoMessage_locals_doubleArgAt_(m, locals, 0);

	glIndexd(c);
	return self;
}

IoObject *IoGL_glIndexdv(IoGL *self, IoObject *locals, IoMessage *m)
{
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	GLdouble c[1] = {0.0};

	GL_getDoubleVector(self,locals,m,list,(GLdouble **)&c,1,"Indexdv");
	glIndexd(c[0]);
	return self;
}

/* Facing */

IoObject *IoGL_glCullFace(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum mode = IoMessage_locals_intArgAt_(m, locals, 0);

	glCullFace(mode);
	return self;
}

IoObject *IoGL_glFrontFace(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum mode = IoMessage_locals_intArgAt_(m, locals, 0);

	glFrontFace(mode);
	return self;
}

IoObject *IoGL_glGetDoublev(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum pname = IoMessage_locals_intArgAt_(m, locals, 0);
	IoList *list = IoMessage_locals_listArgAt_(m, locals, 1);
	GLdouble params[16] = {
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
	};
	int i,n;

	glGetDoublev(pname, params);
	switch (pname) {
		case GL_MODELVIEW_MATRIX:
		case GL_PROJECTION_MATRIX:
		case GL_TEXTURE_MATRIX:
			n=16;
			break;

		case GL_ACCUM_CLEAR_VALUE:
		case GL_COLOR_CLEAR_VALUE:
		case GL_COLOR_WRITEMASK:
		case GL_CURRENT_COLOR:
		case GL_CURRENT_RASTER_COLOR:
		case GL_CURRENT_RASTER_POSITION:
		case GL_CURRENT_RASTER_TEXTURE_COORDS:
		case GL_CURRENT_TEXTURE_COORDS:
		case GL_FOG_COLOR:
		case GL_LIGHT_MODEL_AMBIENT:
		case GL_MAP2_GRID_DOMAIN:
		case GL_SCISSOR_BOX:
		case GL_VIEWPORT:
			n=4;
			break;

		case GL_CURRENT_NORMAL:
			n=3;
			break;

		case GL_DEPTH_RANGE:
		case GL_LINE_WIDTH_RANGE:
		case GL_MAP2_GRID_SEGMENTS:
		case GL_MAX_VIEWPORT_DIMS:
		case GL_POINT_SIZE_RANGE:
		case GL_POLYGON_MODE:
			n=2;
			break;

		default:
			n=1;
			break;
	}
	/* Transfer doubles into list */
	for (i=0; i<n; i++) {
		IoList_rawAt_put_(list,i,IONUMBER(params[i]));
	}

	return self;
}

IoObject *IoGL_glGetTexEnvfv(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum pname = IoMessage_locals_intArgAt_(m, locals, 1);
	IoList *list = IoMessage_locals_listArgAt_(m, locals, 2);
	GLfloat params[4] = {
		0.0,0.0,0.0,0.0,
	};
	int i,n;

	glGetTexEnvfv(target, pname, params);

	switch (pname)
	{
		case GL_TEXTURE_ENV_COLOR:
			n=4;
			break;

		default:
			n=1;
			break;
	}

	/* Transfer floats into list */

	for (i = 0; i < n; i ++)
	{
		IoList_rawAt_put_(list,i,IONUMBER(params[i]));
	}

	return self;
}

IoObject *IoGL_glGetTexGendv(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum coord = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum pname = IoMessage_locals_intArgAt_(m, locals, 1);
	IoList *list = IoMessage_locals_listArgAt_(m, locals, 2);
	GLdouble params[4] = {
		0.0,0.0,0.0,0.0,
	};
	int i,n;

	glGetTexGendv(coord, pname, params);

	switch (pname)
	{
		case GL_TEXTURE_GEN_MODE:
			n=1;
			break;

		default:
			n=4;
			break;
	}

	/* Transfer doubles into list*/

	for (i = 0; i < n; i ++)
	{
		IoList_rawAt_put_(list,i,IONUMBER(params[i]));
	}

	return self;
}

IoObject *IoGL_glGetTexImage(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint level = IoMessage_locals_intArgAt_(m, locals, 1);
	GLenum format = IoMessage_locals_intArgAt_(m, locals, 2);
	GLenum type = IoMessage_locals_intArgAt_(m, locals, 3);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 4);
	GLvoid  *pixels = (GLvoid *)IoSeq_rawBytes(data);

	glGetTexImage(target,level,format,type,pixels);
	return self;
}

IoObject *IoGL_glGetTexLevelParameterfv(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint level = IoMessage_locals_intArgAt_(m, locals, 1);
	GLenum pname = IoMessage_locals_intArgAt_(m, locals, 2);
	IoList *list = IoMessage_locals_listArgAt_(m, locals, 3);
	GLfloat params[1] = {0.0};
	int i;

	glGetTexLevelParameterfv(target,level,pname,params);
	/* Transfer float into list */
	for (i = 0; i < 1; i ++)
	{
		IoList_rawAt_put_(list,i,IONUMBER(params[i]));
	}

	return self;
}

IoObject *IoGL_glGetTexParameterfv(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum pname = IoMessage_locals_intArgAt_(m, locals, 1);
	IoList *list = IoMessage_locals_listArgAt_(m, locals, 2);
	GLfloat params[4] = {
		0.0,0.0,0.0,0.0,
	};
	int i,n;

	glGetTexParameterfv(target, pname, params);
	switch (pname)
	{
		case GL_TEXTURE_BORDER_COLOR:
			n=4;
			break;

		default:
			n=1;
			break;
	}
	/* Transfer floats into list */
	for (i = 0; i < n; i ++)
	{
		IoList_rawAt_put_(list, i, IONUMBER(params[i]));
	}

	return self;
}

IoObject *IoGL_glGetClipPlane(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum plane = IoMessage_locals_intArgAt_(m, locals, 0);
	IoList *list = IoMessage_locals_listArgAt_(m, locals, 1);
	GLdouble equation[4] = {0.0,0.0,0.0,0.0};
	int i;

	glGetClipPlane(plane, equation);
	/* Transfer doubles into list */
	for (i = 0; i < 4; i ++)
	{
		IoList_rawAt_put_(list, i, IONUMBER(equation[i]));
	}
	return self;
}

IoObject *IoGL_glGetLightfv(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum light = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum pname = IoMessage_locals_intArgAt_(m, locals, 1);
	IoList *list = IoMessage_locals_listArgAt_(m, locals, 2);
	GLfloat params[4] = {0.0,0.0,0.0,0.0};
	int i,n;

	glGetLightfv(light,pname,params);

	switch (pname)
	{
		case GL_SPOT_DIRECTION:
			n=3;
			break;
		case GL_SPOT_EXPONENT:
		case GL_SPOT_CUTOFF:
		case GL_CONSTANT_ATTENUATION:
		case GL_LINEAR_ATTENUATION:
		case GL_QUADRATIC_ATTENUATION:
			n=1;
			break;
		default:
			n=4;
			break;
	}

	/* Transfer floats into list */
	for (i = 0; i < n; i ++)
	{
		IoList_rawAt_put_(list,i,IONUMBER(params[i]));
	}

	return self;
}

IoObject *IoGL_glGetMaterialfv(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum face = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum pname = IoMessage_locals_intArgAt_(m, locals, 1);
	IoList *list = IoMessage_locals_listArgAt_(m, locals, 2);
	GLfloat params[4] = {0.0,0.0,0.0,0.0};
	int i,n;

	glGetMaterialfv(face,pname,params);
	switch (pname)
	{
		case GL_COLOR_INDEXES: n = 3; break;
		case GL_SHININESS:     n = 1; break;
		default: n = 4; break;
	}

	/* Transfer floats into list */
	for (i = 0; i < n; i ++)
	{
		IoList_rawAt_put_(list,i,IONUMBER(params[i]));
	}

	return self;
}

IoObject *IoGL_glGetMapdv(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum query  = IoMessage_locals_intArgAt_(m, locals, 1);
	IoList *list  = IoMessage_locals_listArgAt_(m, locals, 2);
	GLdouble v[16] = {
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
	};
	int i;

	glGetMapdv(target,query,v);
	/* Transfer doubles into list */
	for (i=0; i<16; i++) {
		IoList_rawAt_put_(list,i,IONUMBER(v[i]));
	}
	return self;
}

IoObject *IoGL_glGetError(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum r = glGetError();
	return IONUMBER(r);
}

IoObject *IoGL_glBegin(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLint v = IoMessage_locals_intArgAt_(m, locals, 0);
	glBegin(v);
	return self;
}

IoObject *IoGL_glNormal3d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble z = IoMessage_locals_doubleArgAt_(m, locals, 2);
	glNormal3d(x, y, z);
	return self;
}

IoObject *IoGL_glNormal3dv(IoGL *self, IoObject *locals, IoMessage *m)
{
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	GLdouble v[3] = {0.0, 0.0, 0.0};
	GL_getDoubleVector(self,locals,m,list,(GLdouble **)&v,3,"Normal3dv");
	glNormal3dv(v);
	return self;
}

IoObject *IoGL_glVertex2d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	glVertex2d(x, y);
	return self;
}

IoObject *IoGL_glVertex2i(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLint x = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint y = IoMessage_locals_intArgAt_(m, locals, 1);
	glVertex2i(x, y);
	return self;
}

IoObject *IoGL_glVertex2dv(IoGL *self, IoObject *locals, IoMessage *m)
{
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	GLdouble v[2] = {0.0, 0.0};
	GL_getDoubleVector(self,locals,m,list,(GLdouble **)&v,2,"Vertex2dv");
	glVertex2dv(v);
	return self;
}

IoObject *IoGL_glVertex3d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble z = IoMessage_locals_doubleArgAt_(m, locals, 2);
	glVertex3d(x, y, z);
	return self;
}

IoObject *IoGL_glVertex3dv(IoGL *self, IoObject *locals, IoMessage *m)
{
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	GLdouble v[3] = {0.0, 0.0, 0.0};
	GL_getDoubleVector(self, locals, m, list, (GLdouble **)&v, 3, "Vertex3dv");
	glVertex3dv(v);
	return self;
}

IoObject *IoGL_glVertex4d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble z = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLdouble w = IoMessage_locals_doubleArgAt_(m, locals, 3);
	glVertex4d(x, y, z, w);
	return self;
}

IoObject *IoGL_glVertex4dv(IoGL *self, IoObject *locals, IoMessage *m)
{
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	GLdouble v[4] = {0.0, 0.0, 0.0, 0.0};
	GL_getDoubleVector(self,locals,m,list,(GLdouble **)&v,4,"Vertex4dv");
	glVertex4dv(v);
	return self;
}

IoObject *IoGL_glEnd(IoGL *self, IoObject *locals, IoMessage *m)
{
	glEnd();
	return self;
}

IoObject *IoGL_glPointSize(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble size = IoMessage_locals_doubleArgAt_(m, locals, 0);

	glPointSize(size);
	return self;
}

IoObject *IoGL_glLineWidth(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble w = IoMessage_locals_doubleArgAt_(m, locals, 0);

	glLineWidth(w);
	return self;
}

IoObject *IoGL_glListBase(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLuint base = IoMessage_locals_doubleArgAt_(m, locals, 0);

	glListBase(base);
	return self;
}

IoObject *IoGL_glLineStipple(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLint factor = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLushort pattern = IoMessage_locals_intArgAt_(m, locals, 1);

	glLineStipple(factor,pattern);
	return self;
}

IoObject *IoGL_glPolygonMode(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum face = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum mode = IoMessage_locals_intArgAt_(m, locals, 1);

	glPolygonMode(face, mode);
	return self;
}

IoObject *IoGL_glEdgeFlag(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLboolean flag = IoMessage_locals_intArgAt_(m, locals, 0);

	glEdgeFlag(flag);
	return self;
}

IoObject *IoGL_glEdgeFlagPointer(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLsizei stride = IoMessage_locals_intArgAt_(m, locals, 0);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 1);
	GLvoid  *pointer = (GLvoid *)IoSeq_rawBytes(data);

	glEdgeFlagPointer(stride,pointer);
	return self;
}

IoObject *IoGL_glPolygonOffset(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLfloat factor = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLfloat units = IoMessage_locals_doubleArgAt_(m, locals, 1);

	glPolygonOffset(factor, units);
	return self;
}

IoObject *IoGL_glTranslated(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble z = IoMessage_locals_doubleArgAt_(m, locals, 2);
	glTranslated(x, y, z);
	return self;
}

IoObject *IoGL_glTranslatei(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLint x = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint y = IoMessage_locals_intArgAt_(m, locals, 1);
	GLint z = IoMessage_locals_intArgAt_(m, locals, 2);
	glTranslated((GLdouble)x, (GLdouble)y, (GLdouble)z);
	return self;
}

IoObject *IoGL_glScaled(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble z = IoMessage_locals_doubleArgAt_(m, locals, 2);
	glScaled(x, y, z);
	return self;
}

IoObject *IoGL_glRotated(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble a = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble x = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble y = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLdouble z = IoMessage_locals_doubleArgAt_(m, locals, 3);
	glRotated(a, x, y, z);
	return self;
}

IoObject *IoGL_glLoadIdentity(IoGL *self, IoObject *locals, IoMessage *m)
{
	glLoadIdentity();
	return self;
}

IoObject *IoGL_glLogicOp(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum opcode = IoMessage_locals_intArgAt_(m, locals, 0);

	glLogicOp(opcode);
	return self;
}

IoObject *IoGL_glPushMatrix(IoGL *self, IoObject *locals, IoMessage *m)
{
	glPushMatrix();
	return self;
}

IoObject *IoGL_glPopMatrix(IoGL *self, IoObject *locals, IoMessage *m)
{
	glPopMatrix();
	return self;
}

IoObject *IoGL_glInitNames(IoGL *self, IoObject *locals, IoMessage *m)
{
	glInitNames();
	return self;
}

IoObject *IoGL_glPushName(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLuint name = IoMessage_locals_intArgAt_(m, locals, 0);
	glPushName(name);
	return self;
}

IoObject *IoGL_glLoadName(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLuint name = IoMessage_locals_intArgAt_(m, locals, 0);
	glLoadName(name);
	return self;
}

IoObject *IoGL_glPopName(IoGL *self, IoObject *locals, IoMessage *m)
{
	glPopName();
	return self;
}

IoObject *IoGL_glArrayElement(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLint i = IoMessage_locals_intArgAt_(m, locals, 0);
	glArrayElement(i);
	return self;
}

IoObject *IoGL_glDrawArrays(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum mode = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint first = IoMessage_locals_intArgAt_(m, locals, 1);
	GLsizei count = IoMessage_locals_intArgAt_(m, locals, 2);

	glDrawArrays(mode,first,count);
	return self;
}

IoObject *IoGL_glDrawBuffer(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum mode = IoMessage_locals_intArgAt_(m, locals, 0);

	glDrawBuffer(mode);
	return self;
}

IoObject *IoGL_glReadBuffer(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum mode = IoMessage_locals_intArgAt_(m, locals, 0);

	glReadBuffer(mode);
	return self;
}

IoObject *IoGL_glSelectBuffer(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLsizei size = IoMessage_locals_intArgAt_(m, locals, 0);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 1);
	GLvoid  *buffer = (GLvoid *)IoSeq_rawBytes(data);

	glSelectBuffer(size,buffer);
	return self;
}

IoObject *IoGL_glDrawElements(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum mode = IoMessage_locals_intArgAt_(m, locals, 0);
	GLsizei count = IoMessage_locals_intArgAt_(m, locals, 1);
	GLenum type = IoMessage_locals_intArgAt_(m, locals, 2);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 3);
	GLvoid  *indices = (GLvoid *)IoSeq_rawBytes(data);

	glDrawElements(mode,count,type,indices);
	return self;
}

IoObject *IoGL_glInterleavedArrays(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum format = IoMessage_locals_intArgAt_(m, locals, 0);
	GLsizei first = IoMessage_locals_intArgAt_(m, locals, 1);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 2);
	GLvoid  *pointer = (GLvoid *)IoSeq_rawBytes(data);

	glInterleavedArrays(format,first,pointer);
	return self;
}

IoObject *IoGL_glLoadMatrixd(IoGL *self, IoObject *locals, IoMessage *m)
{
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	GLdouble v[16] =
	{
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0
	};
	GLdouble *vp = v;

	GL_getDoubleVector(self, locals, m, list, &vp, 16, "glLoadMatrixd");
	glLoadMatrixd(v);
	return self;
}

IoObject *IoGL_glMultMatrixd(IoGL *self, IoObject *locals, IoMessage *m)
{
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	GLdouble v[16] =
	{
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0
	};
	GLdouble *vp = v;

	GL_getDoubleVector(self, locals, m, list, &vp, 16, "glMultMatrixd");
	glMultMatrixd(v);
	return self;
}

IoObject *IoGL_glFrustum(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble left = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble right = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble bottom = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLdouble top = IoMessage_locals_doubleArgAt_(m, locals, 3);
	GLdouble zNear = IoMessage_locals_doubleArgAt_(m, locals, 4);
	GLdouble zFar = IoMessage_locals_doubleArgAt_(m, locals, 5);
	glFrustum(left,right,bottom,top,zNear,zFar);
	return self;
}

IoObject *IoGL_glOrtho(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble left = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble right = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble bottom = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLdouble top = IoMessage_locals_doubleArgAt_(m, locals, 3);
	GLdouble zNear = IoMessage_locals_doubleArgAt_(m, locals, 4);
	GLdouble zFar = IoMessage_locals_doubleArgAt_(m, locals, 5);
	glOrtho(left,right,bottom,top,zNear,zFar);
	return self;
}

IoObject *IoGL_glMatrixMode(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum mode = IoMessage_locals_intArgAt_(m, locals, 0);

	glMatrixMode(mode);
	return self;
}

IoObject *IoGL_glFogf(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum pname = IoMessage_locals_intArgAt_(m, locals, 0);
	GLfloat param = IoMessage_locals_doubleArgAt_(m, locals, 1);

	glFogf(pname,param);
	return self;
}

IoObject *IoGL_glFogfv(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum pname = IoMessage_locals_intArgAt_(m, locals, 0);
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 1));
	GLfloat *params = NULL;

	GL_getFloatVector(self,locals,m,list,(GLfloat **)&params,0,"glFogfv");
	if (params) {
		glFogfv(pname,params);
		free(params);
	}
	return self;
}

IoObject *IoGL_glFlush(IoGL *self, IoObject *locals, IoMessage *m)
{
	glFlush();
	return self;
}

IoObject *IoGL_glFinish(IoGL *self, IoObject *locals, IoMessage *m)
{
	glFinish();
	return self;
}

IoObject *IoGL_glNewList(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLuint list = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum mode = IoMessage_locals_intArgAt_(m, locals, 1);

	glNewList(list, mode);
	return self;
}

IoObject *IoGL_glEndList(IoGL *self, IoObject *locals, IoMessage *m)
{
	glEndList();
	return self;
}

IoObject *IoGL_glGenLists(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLsizei range = IoMessage_locals_intArgAt_(m, locals, 0);
	GLuint r = glGenLists(range);
	return IONUMBER(r);
}


IoObject *IoGL_glDeleteLists(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLuint list = IoMessage_locals_intArgAt_(m, locals, 0);
	GLsizei range = IoMessage_locals_intArgAt_(m, locals, 1);

	glDeleteLists(list, range);
	return self;
}

IoObject *IoGL_glCallList(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLuint n = IoMessage_locals_intArgAt_(m, locals, 0);
	glCallList(n);
	return self;
}

IoObject *IoGL_glHint(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum mode = IoMessage_locals_intArgAt_(m, locals, 1);

	glHint(target, mode);
	return self;
}

IoObject *IoGL_glEnable(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum n = IoMessage_locals_intArgAt_(m, locals, 0);

	glEnable(n);
	return self;
}

IoObject *IoGL_glIsEnabled(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum cap = IoMessage_locals_intArgAt_(m, locals, 0);

	GLboolean r = glIsEnabled(cap);
	IoNumber *result = IONUMBER(r);
	return result;
}

IoObject *IoGL_glIsList(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLuint list = IoMessage_locals_intArgAt_(m, locals, 0);

	GLboolean r = glIsList(list);
	IoNumber *result = IONUMBER(r);
	return result;
}

IoObject *IoGL_glIsTexture(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLuint texture = IoMessage_locals_intArgAt_(m, locals, 0);

	GLboolean r = glIsTexture(texture);
	IoNumber *result = IONUMBER(r);
	return result;
}

IoObject *IoGL_glPrioritizeTextures(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLsizei n = IoMessage_locals_intArgAt_(m, locals, 0);
	List *list1 = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 1));
	List *list2 = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 2));
	GLuint *textures = NULL;
	GLclampf *priorities = NULL;

	GL_getIntVector(self,locals,m,list1,(GLint **)&textures,0,"glPrioritizeTextures");
	if (textures) {
		GL_getFloatVector(self,locals,m,list2,(GLfloat **)&priorities,0,"glPrioritizeTextures");
		if (priorities) {
			glPrioritizeTextures(n,textures,priorities);
			free(priorities);
		}
		free(textures);
	}
	return self;
}

IoObject *IoGL_glDisable(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum n = IoMessage_locals_intArgAt_(m, locals, 0);

	glDisable(n);
	return self;
}

IoObject *IoGL_glRenderMode(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum mode = IoMessage_locals_intArgAt_(m, locals, 0);

	glRenderMode(mode);
	return self;
}

IoObject *IoGL_glFeedbackBuffer(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLsizei size = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum type = IoMessage_locals_intArgAt_(m, locals, 1);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 2);
	GLfloat *buffer = (GLvoid *)IoSeq_rawBytes(data);

	glFeedbackBuffer(size,type,buffer);
	return self;
}

IoObject *IoGL_glPassThrough(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLfloat token = IoMessage_locals_doubleArgAt_(m, locals, 0);

	glPassThrough(token);
	return self;
}

IoObject *IoGL_glBlendFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum s = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum d = IoMessage_locals_intArgAt_(m, locals, 1);
	glBlendFunc(s, d);
	return self;
}

IoObject *IoGL_glAccum(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum op = IoMessage_locals_intArgAt_(m, locals, 0);
	GLfloat value = (GLfloat)IoMessage_locals_doubleArgAt_(m, locals, 1);
	glAccum(op, value);
	return self;
}

IoObject *IoGL_glClearAccum(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLfloat r = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLfloat g = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLfloat b = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLfloat a = IoMessage_locals_doubleArgAt_(m, locals, 3);
	glClearAccum(r, g, b, a);
	return self;
}

IoObject *IoGL_glScissor(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLint x   = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint y   = IoMessage_locals_intArgAt_(m, locals, 1);
	GLsizei w = IoMessage_locals_intArgAt_(m, locals, 2);
	GLsizei h = IoMessage_locals_intArgAt_(m, locals, 3);

	glScissor(x, y, w, h);
	return self;
}

IoObject *IoGL_glShadeModel(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum model = IoMessage_locals_intArgAt_(m, locals, 0);

	glShadeModel(model);
	return self;
}

IoObject *IoGL_glRectd(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble x1 = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble y1 = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble x2 = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLdouble y2 = IoMessage_locals_doubleArgAt_(m, locals, 3);

	glRectd(x1, y1, x2, y2);
	return self;
}

IoObject *IoGL_glRectdv(IoGL *self, IoObject *locals, IoMessage *m)
{
	List *list1 = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	List *list2 = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 1));
	GLdouble v1[2] = {0.0,0.0};
	GLdouble v2[2] = {0.0,0.0};

	GL_getDoubleVector(self, locals, m, list1, (GLdouble **)&v1, 2, "glRectdv");
	GL_getDoubleVector(self, locals, m, list2, (GLdouble **)&v2, 2, "glRectdv");
	glRectdv(v1, v2);
	return self;
}

IoObject *IoGL_glEvalCoord1d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble u = IoMessage_locals_doubleArgAt_(m, locals, 0);

	glEvalCoord1d(u);
	return self;
}

IoObject *IoGL_glEvalCoord1dv(IoGL *self, IoObject *locals, IoMessage *m)
{
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	GLdouble u[1] = {0.0};

	GL_getDoubleVector(self, locals, m, list, (GLdouble **)&u, 1, "glEvalCoord1dv");
	glEvalCoord1dv(u);
	return self;
}

IoObject *IoGL_glEvalCoord2d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble u = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble v = IoMessage_locals_doubleArgAt_(m, locals, 1);

	glEvalCoord2d(u, v);
	return self;
}

IoObject *IoGL_glEvalCoord2dv(IoGL *self, IoObject *locals, IoMessage *m)
{
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	GLdouble u[2] = {0.0, 0.0};

	GL_getDoubleVector(self, locals, m, list, (GLdouble **)&u, 2, "glEvalCoord2dv");
	glEvalCoord2dv(u);
	return self;
}

IoObject *IoGL_glEvalMesh1(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum mode = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint i1 = IoMessage_locals_intArgAt_(m, locals, 1);
	GLint i2 = IoMessage_locals_intArgAt_(m, locals, 2);

	glEvalMesh1(mode,i1,i2);
	return self;
}

IoObject *IoGL_glEvalMesh2(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum mode = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint i1 = IoMessage_locals_intArgAt_(m, locals, 1);
	GLint i2 = IoMessage_locals_intArgAt_(m, locals, 2);
	GLint j1 = IoMessage_locals_intArgAt_(m, locals, 3);
	GLint j2 = IoMessage_locals_intArgAt_(m, locals, 4);

	glEvalMesh2(mode,i1,i2,j1,j2);
	return self;
}

IoObject *IoGL_glEvalPoint1(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLint i = IoMessage_locals_intArgAt_(m, locals, 0);

	glEvalPoint1(i);
	return self;
}

IoObject *IoGL_glEvalPoint2(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLint i = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint j = IoMessage_locals_intArgAt_(m, locals, 1);

	glEvalPoint2(i,j);
	return self;
}

IoObject *IoGL_glBindTexture(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLuint texture = IoMessage_locals_intArgAt_(m, locals, 1);
	glBindTexture(target, texture);
	return self;
}

IoObject *IoGL_glTexImage1D(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint level  = IoMessage_locals_intArgAt_(m, locals, 1);
	GLint internalFormat = IoMessage_locals_intArgAt_(m, locals, 2);
	GLsizei width  = IoMessage_locals_intArgAt_(m, locals, 3);
	GLint border = IoMessage_locals_intArgAt_(m, locals, 4);
	GLenum sourceFormat = IoMessage_locals_intArgAt_(m, locals, 5);
	GLenum type = IoMessage_locals_intArgAt_(m, locals, 6);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 7);

	glTexImage1D(target, level, internalFormat, width, border,
				 sourceFormat, type, (GLvoid*)IoSeq_rawBytes(data)
				);
	return self;
}

size_t IoGL_AcceptedPixelForFormat_(GLenum sourceFormat)
{
	switch (sourceFormat)
	{
		case GL_ALPHA: return 8;
		case GL_ALPHA8: return 8;
		case GL_LUMINANCE: return 8;
		case GL_LUMINANCE8: return 8;
		case GL_LUMINANCE16: return 16;
		case GL_LUMINANCE_ALPHA: return 8+8;
		case GL_LUMINANCE8_ALPHA8: return 8+8;
		case GL_LUMINANCE16_ALPHA16: return 16+16;
		case GL_INTENSITY: return 8;
		case GL_INTENSITY8: return 8;
		case GL_INTENSITY16: return 16;
		case GL_RGB: return 8+8+8;
		case GL_RGB8: return 8+8+8;
		case GL_RGB16: return 16+16+16;
		case GL_RGBA: return 8+8+8;
		case GL_RGBA8: return 8+8+8+8;
		case GL_RGBA16: return 16+16+16+16;
	}
	return 0;
}

size_t IoGL_BitsPerPixelForFormat_(GLenum sourceFormat)
{
	switch (sourceFormat)
	{
		case GL_ALPHA: return 8;
		case GL_ALPHA4: return 4;
		case GL_ALPHA8: return 8;
		case GL_ALPHA12: return 12;
		case GL_ALPHA16: return 16;
		case GL_LUMINANCE: return 8;
		case GL_LUMINANCE4: return 4;
		case GL_LUMINANCE8: return 8;
		case GL_LUMINANCE12: return 12;
		case GL_LUMINANCE16: return 16;
		case GL_LUMINANCE_ALPHA: return 8+8;
		case GL_LUMINANCE4_ALPHA4: return 4+4;
		case GL_LUMINANCE6_ALPHA2: return 6+2;
		case GL_LUMINANCE8_ALPHA8: return 8+8;
		case GL_LUMINANCE12_ALPHA4: return 12+14;
		case GL_LUMINANCE12_ALPHA12: return 12+12;
		case GL_LUMINANCE16_ALPHA16: return 16+16;
		case GL_INTENSITY: return 8;
		case GL_INTENSITY4: return 4;
		case GL_INTENSITY8: return 8;
		case GL_INTENSITY12: return 12;
		case GL_INTENSITY16: return 16;
		case GL_R3_G3_B2: return 3+3+2;
		case GL_RGB: return 8+8+8;
		case GL_RGB4: return 4+4+4;
		case GL_RGB5: return 5+5+5;
		case GL_RGB8: return 8+8+8;
		case GL_RGB10: return 10+10+10;
		case GL_RGB12: return 12+12+12;
		case GL_RGB16: return 16+16+16;
		case GL_RGBA: return 8+8+8+8;
		case GL_RGBA2: return 2+2+2+2;
		case GL_RGBA4: return 4+4+4+4;
		case GL_RGB5_A1: return 5+5+5+1;
		case GL_RGBA8: return 8+8+8+8;
		case GL_RGB10_A2: return 10+10+10+2;
		case GL_RGBA12: return 12+12+12+12;
		case GL_RGBA16: return 16+16+16+16;
	}
	return 0;
}

#include "IoGLUT.h"

#define CHECKGLINIT() IOASSERT(IoGLUT_HasInitialized(), "OpenGL context not set up yet");

IoObject *IoGL_glTexImage2D(IoGL *self, IoObject *locals, IoMessage *m)
{
	CHECKGLINIT();
	{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint level  = IoMessage_locals_intArgAt_(m, locals, 1);
	GLint internalFormat = IoMessage_locals_intArgAt_(m, locals, 2);
	GLsizei width  = IoMessage_locals_intArgAt_(m, locals, 3);
	GLsizei height = IoMessage_locals_intArgAt_(m, locals, 4);
	GLint border = IoMessage_locals_intArgAt_(m, locals, 5);
	GLenum sourceFormat = IoMessage_locals_intArgAt_(m, locals, 6);
	GLenum type = IoMessage_locals_intArgAt_(m, locals, 7);
	IoObject *data = IoMessage_locals_valueArgAt_(m, locals, 8);
	GLvoid *bytes = IoSeq_rawBytes(data);


	IOASSERT(ISBUFFER(data) || ISNIL(data), "data must be a Buffer or Nil");
	IOASSERT(IoGL_AcceptedPixelForFormat_(sourceFormat), "unacceptable pixel format");

	{
		size_t requiredSize = (width * height * IoGL_BitsPerPixelForFormat_(sourceFormat)) / 8;
		/*
		printf("sourceFormat = %i GL_RGB = %i GL_RGBA = %i\n", sourceFormat, GL_RGB, GL_RGBA);
		printf("bytes per pixel = %i\n", (int)IoGL_BitsPerPixelForFormat_(sourceFormat)/8);
		printf("w %i h %i\n", width, height);
		printf("size = %i\n", (int)requiredSize);
		*/
		IOASSERT(IoSeq_rawSize(data) == requiredSize, "data not of correct size for width, height and depth");
		//bytes = malloc(requiredSize);
	}

/*
	printf("--- glTexImage2D(target %i, level %i, internalFormat %i, width %i, height %i, border %i, sourceFormat %i, type %i, bytes %p)\n",
		target,
		level,
		internalFormat,
		width, height,
		border,
		sourceFormat,
		type,
		(GLvoid*) bytes
	);
	*/
	
/*	
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGB,
				 width, height,
				 0,
				 GL_RGB,
				 GL_UNSIGNED_BYTE,
				 (GLvoid*) bytes);
*/
	glTexImage2D(target,
				 level,
				 internalFormat,
				 width, height,
				 border,
				 sourceFormat,
				 type,
				 (GLvoid*) bytes);
	}
	//printf("glTexImage2D() done\n");
	return self;
}

IoObject *IoGL_glTexParameterf(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum pname  = IoMessage_locals_intArgAt_(m, locals, 1);
	GLfloat param  = IoMessage_locals_intArgAt_(m, locals, 2);

	glTexParameterf(target, pname, param);
	return self;
}

IoObject *IoGL_glTexParameterfv(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum pname  = IoMessage_locals_intArgAt_(m, locals, 1);
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 2));
	GLfloat *v = NULL;

	GL_getFloatVector(self,locals,m,list,(GLfloat **)&v,0,"glTexParameterfv");

	if (v)
	{
		glTexParameterfv(target, pname, v);
		free(v);
	}

	return self;
}

IoObject *IoGL_glTexEnvf(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum pname  = IoMessage_locals_intArgAt_(m, locals, 1);
	GLfloat param  = IoMessage_locals_doubleArgAt_(m, locals, 2);

	glTexEnvf(target, pname, param);
	return self;
}

IoObject *IoGL_glTexEnvfv(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum pname  = IoMessage_locals_intArgAt_(m, locals, 1);
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 2));
	GLfloat *v = NULL;
	GL_getFloatVector(self,locals,m,list,(GLfloat **)&v,0,"glTexParameterfv");

	if (v)
	{
		glTexEnvfv(target, pname, v);
		free(v);
	}

	return self;
}

IoObject *IoGL_glTexCoord1d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble s = IoMessage_locals_doubleArgAt_(m, locals, 0);
	glTexCoord1d(s);
	return self;
}

IoObject *IoGL_glTexCoord1dv(IoGL *self, IoObject *locals, IoMessage *m)
{
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	GLdouble v[1] = {0.0};

	GL_getDoubleVector(self, locals, m, list, (GLdouble **)&v, 1, "glTexCoord1dv");
	glTexCoord1dv(v);
	return self;
}

IoObject *IoGL_glTexCoord2d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble s = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble t = IoMessage_locals_doubleArgAt_(m, locals, 1);

	glTexCoord2d(s, t);
	return self;
}

IoObject *IoGL_glTexCoord2dv(IoGL *self, IoObject *locals, IoMessage *m)
{
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	GLdouble v[2] = {0.0, 0.0};

	GL_getDoubleVector(self, locals, m, list, (GLdouble **)&v, 2, "glTexCoord2dv");
	glTexCoord2dv(v);
	return self;
}

IoObject *IoGL_glTexCoord3d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble s = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble t = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble r = IoMessage_locals_doubleArgAt_(m, locals, 2);
	glTexCoord3d(s, t, r);
	return self;
}

IoObject *IoGL_glTexCoord3dv(IoGL *self, IoObject *locals, IoMessage *m)
{
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	GLdouble v[3] = {0.0, 0.0, 0.0};

	GL_getDoubleVector(self, locals, m, list, (GLdouble **)&v, 3, "glTexCoord3dv");
	glTexCoord3dv(v);
	return self;
}

IoObject *IoGL_glTexCoord4d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble s = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble t = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble r = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLdouble q = IoMessage_locals_doubleArgAt_(m, locals, 3);
	glTexCoord4d(s, t, r, q);
	return self;
}

IoObject *IoGL_glTexCoord4dv(IoGL *self, IoObject *locals, IoMessage *m)
{
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	GLdouble v[4] = {0.0, 0.0, 0.0, 0.0};

	GL_getDoubleVector(self, locals, m, list, (GLdouble **)&v, 4, "glTexCoord4dv");
	glTexCoord4dv(v);
	return self;
}

IoObject *IoGL_glRasterPos2d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	glRasterPos2d(x , y);
	return self;
}

IoObject *IoGL_glRasterPos2dv(IoGL *self, IoObject *locals, IoMessage *m)
{
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	GLdouble v[2] = {0.0, 0.0};

	GL_getDoubleVector(self, locals, m, list, (GLdouble **)&v, 2, "glRasterPos2dv");
	glRasterPos2dv(v);
	return self;
}


IoObject *IoGL_glRasterPos3d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble z = IoMessage_locals_doubleArgAt_(m, locals, 2);
	glRasterPos3d(x , y, z);
	return self;
}

IoObject *IoGL_glRasterPos3dv(IoGL *self, IoObject *locals, IoMessage *m)
{
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	GLdouble v[3] = {0.0, 0.0, 0.0};

	GL_getDoubleVector(self, locals, m, list, (GLdouble **)&v, 3, "glRasterPos3dv");
	glRasterPos3dv(v);
	return self;
}

IoObject *IoGL_glRasterPos4d(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLdouble x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble z = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLdouble w = IoMessage_locals_doubleArgAt_(m, locals, 3);
	glRasterPos4d(x , y, z, w);
	return self;
}

IoObject *IoGL_glRasterPos4dv(IoGL *self, IoObject *locals, IoMessage *m)
{
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	GLdouble v[4] = {0.0, 0.0, 0.0, 0.0};

	GL_getDoubleVector(self, locals, m, list, (GLdouble **)&v, 4, "glRasterPos4dv");
	glRasterPos4dv(v);
	return self;
}

IoObject *IoGL_glPushAttrib(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLbitfield mask = IoMessage_locals_intArgAt_(m, locals, 0);
	glPushAttrib(mask);
	return self;
}

IoObject *IoGL_glPopAttrib(IoGL *self, IoObject *locals, IoMessage *m)
{
	glPopAttrib();
	return self;
}

IoObject *IoGL_glPushClientAttrib(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLbitfield mask = IoMessage_locals_intArgAt_(m, locals, 0);
	glPushClientAttrib(mask);
	return self;
}

IoObject *IoGL_glPopClientAttrib(IoGL *self, IoObject *locals, IoMessage *m)
{
	glPopClientAttrib();
	return self;
}

IoObject *IoGL_glGenTextures(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLsizei n = IoMessage_locals_intArgAt_(m, locals, 0);
	IoList *list = IoMessage_locals_listArgAt_(m, locals, 1);
	GLuint *textures = NULL;
	int i;

	if ((textures=(GLuint *)malloc(n*sizeof(GLuint))))
	{
		glGenTextures(n, textures);
		/* Transfer integers into list */
		for (i = 0; i < n; i ++)
		{
			IoList_rawAt_put_(list,i,IONUMBER(textures[i]));
		}
		free(textures);
	}
	return self;
}

IoObject *IoGL_glDeleteTextures(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLsizei n = IoMessage_locals_intArgAt_(m, locals, 0);
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 1));
	GLuint *textures = NULL;

	GL_getIntVector(self,locals,m,list,(GLint **)&textures,0,"glDeleteTextures");

	if (textures)
	{
		glDeleteTextures(n,textures);
		free(textures);
	}
	return self;
}

IoObject *IoGL_glAreTexturesResident(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLsizei n = IoMessage_locals_intArgAt_(m, locals, 0);
	List *list1 = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 1));
	IoList *list2 = IoMessage_locals_listArgAt_(m, locals, 2);
	GLuint *textures = NULL;
	GLboolean *residences = NULL;
	int i;

	GL_getIntVector(self,locals,m,list1,(GLint **)&textures,0,"glAreTexturesResident");
	if (textures)
	{
		if ((residences=(GLboolean *)malloc(n*sizeof(GLboolean))))
		{
			glAreTexturesResident(n,textures,residences);
			/* Transfer booleans into list */
			for (i = 0; i < n; i ++)
			{
				IoList_rawAt_put_(list2,i,IONUMBER(residences[i]));
			}
			free(residences);
		}
		free(textures);
	}
	return self;
}

/* Pixel Functions */

IoObject *IoGL_glPixelStoref(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum pname = IoMessage_locals_intArgAt_(m, locals, 0);
	GLfloat param = IoMessage_locals_doubleArgAt_(m, locals, 1);

	glPixelStoref(pname,param);
	return self;
}

IoObject *IoGL_glPixelTransferi(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum pname = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint  param = IoMessage_locals_intArgAt_(m, locals, 1);
	glPixelTransferi(pname,param);
	return self;
}

IoObject *IoGL_glPixelTransferf(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum pname = IoMessage_locals_intArgAt_(m, locals, 0);
	GLfloat param = IoMessage_locals_doubleArgAt_(m, locals, 1);
	glPixelTransferf(pname,param);
	return self;
}

IoObject *IoGL_glPixelZoom(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLfloat xfactor = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLfloat yfactor = IoMessage_locals_doubleArgAt_(m, locals, 1);
	glPixelZoom(xfactor,yfactor);
	return self;
}

IoObject *IoGL_glPixelMapfv(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum map = IoMessage_locals_intArgAt_(m, locals, 0);
	GLsizei mapsize = IoMessage_locals_intArgAt_(m, locals, 1);
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 2));
	GLfloat *values = NULL;

	GL_getFloatVector(self,locals,m,list,(GLfloat **)&values,0,"glPixelMapfv");

	if (values)
	{
		glPixelMapfv(map,mapsize,values);
		free(values);
	}

	return self;
}

IoObject *IoGL_glReadPixels(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLint x = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint y = IoMessage_locals_intArgAt_(m, locals, 1);
	GLsizei width = IoMessage_locals_intArgAt_(m, locals, 2);
	GLsizei height = IoMessage_locals_intArgAt_(m, locals, 3);
	GLenum format = IoMessage_locals_intArgAt_(m, locals, 4);
	GLenum type = IoMessage_locals_intArgAt_(m, locals, 5);
	IoSeq *pixels = IoMessage_locals_seqArgAt_(m, locals, 6);
	
	IOASSERT(IoGL_AcceptedPixelForFormat_(format), "unacceptable pixel format");

	{
		size_t requiredSize = (width * height * IoGL_BitsPerPixelForFormat_(format)) / 8;
		UArray *u = IoSeq_rawUArray(pixels);
		UArray_setSize_(u, requiredSize);
	}
	
	glReadPixels(x, y, width, height, format, type, (GLvoid*)IoSeq_rawBytes(pixels));
	return self;
}

IoObject *IoGL_glDrawPixels(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLsizei width = IoMessage_locals_intArgAt_(m, locals, 0);
	GLsizei height = IoMessage_locals_intArgAt_(m, locals, 1);
	GLenum format = IoMessage_locals_intArgAt_(m, locals, 2);
	GLenum type = IoMessage_locals_intArgAt_(m, locals, 3);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 4);
	GLvoid  *pixels = (GLvoid *)IoSeq_rawBytes(data);

	IOASSERT(IoGL_AcceptedPixelForFormat_(format), "unacceptable pixel format");
	
	{
		size_t requiredSize = (width * height * IoGL_BitsPerPixelForFormat_(format)) / 8;
		IOASSERT(IoSeq_rawSize(pixels) == requiredSize, "pixels data not correct size");
	}
		
	glDrawPixels(width,height,format,type,pixels);
	return self;
}

IoObject *IoGL_glBitmap(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLsizei width = IoMessage_locals_intArgAt_(m, locals, 0);
	GLsizei height = IoMessage_locals_intArgAt_(m, locals, 1);
	GLfloat xorig = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLfloat yorig = IoMessage_locals_doubleArgAt_(m, locals, 3);
	GLfloat xmove = IoMessage_locals_doubleArgAt_(m, locals, 4);
	GLfloat ymove = IoMessage_locals_doubleArgAt_(m, locals, 5);
	IoSeq *bitmap = IoMessage_locals_seqArgAt_(m, locals, 6);
	glBitmap(width,height,xorig,yorig,xmove,ymove, (GLubyte *)IoSeq_rawBytes(bitmap));
	return self;
}

IoObject *IoGL_glCopyPixels(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLint x = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint y = IoMessage_locals_intArgAt_(m, locals, 1);
	GLsizei width = IoMessage_locals_intArgAt_(m, locals, 2);
	GLsizei height = IoMessage_locals_intArgAt_(m, locals, 3);
	GLenum type = IoMessage_locals_intArgAt_(m, locals, 4);
	glCopyPixels(x,y,width,height,type);
	return self;
}

IoObject *IoGL_glLightf(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum light = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum pname = IoMessage_locals_intArgAt_(m, locals, 1);
	GLenum param = IoMessage_locals_doubleArgAt_(m, locals, 2);
	glLightf(light, pname, param);
	return self;
}

IoObject *IoGL_glLightfv(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum light = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum pname = IoMessage_locals_intArgAt_(m, locals, 1);
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 2));
	GLfloat *params = NULL;

	GL_getFloatVector(self,locals,m,list,(GLfloat **)&params,0,"glLightfv");

	if (params)
	{
		glLightfv(light, pname, params);
		free(params);
	}

	return self;
}

IoObject *IoGL_glLightModelf(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum pname = IoMessage_locals_intArgAt_(m, locals, 0);
	GLfloat param = IoMessage_locals_doubleArgAt_(m, locals, 1);
	glLightModelf(pname, param);
	return self;
}

IoObject *IoGL_glMaterialf(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum face = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum pname = IoMessage_locals_intArgAt_(m, locals, 1);
	GLenum param = IoMessage_locals_doubleArgAt_(m, locals, 2);
	glMaterialf(face, pname, param);
	return self;
}

IoObject *IoGL_glMaterialfv(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum face = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum pname = IoMessage_locals_intArgAt_(m, locals, 1);
	List *list = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 2));
	GLfloat *params = NULL;

	GL_getFloatVector(self,locals,m,list,(GLfloat **)&params,0,"glMaterialfv");

	if (params)
	{
		glMaterialfv(face, pname, params);
		free(params);
	}

	return self;
}

IoObject *IoGL_glColorMaterial(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum face = IoMessage_locals_intArgAt_(m, locals, 0);
	GLenum mode = IoMessage_locals_intArgAt_(m, locals, 1);
	glColorMaterial(face, mode);
	return self;
}

IoObject *IoGL_glTexSubImage1D(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint level = IoMessage_locals_intArgAt_(m, locals, 1);
	GLint xoffset = IoMessage_locals_intArgAt_(m, locals, 2);
	GLsizei width = IoMessage_locals_intArgAt_(m, locals, 3);
	GLenum format = IoMessage_locals_intArgAt_(m, locals, 4);
	GLenum type = IoMessage_locals_intArgAt_(m, locals, 5);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 6);
	GLvoid  *pixels = (GLvoid *)IoSeq_rawBytes(data);

	glTexSubImage1D(target,level,xoffset,width,format,type,pixels);
	return self;
}

IoObject *IoGL_glTexSubImage2D(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint level = IoMessage_locals_intArgAt_(m, locals, 1);
	GLint xoffset = IoMessage_locals_intArgAt_(m, locals, 2);
	GLint yoffset = IoMessage_locals_intArgAt_(m, locals, 3);
	GLsizei width = IoMessage_locals_intArgAt_(m, locals, 4);
	GLsizei height = IoMessage_locals_intArgAt_(m, locals, 5);
	GLenum format = IoMessage_locals_intArgAt_(m, locals, 6);
	GLenum type = IoMessage_locals_intArgAt_(m, locals, 7);
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 8);
	GLvoid  *pixels = (GLvoid *)IoSeq_rawBytes(data);

	glTexSubImage2D(target,level,xoffset,yoffset,width,height,format,type,pixels);
	return self;
}

IoObject *IoGL_glCopyTexImage1D(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint level = IoMessage_locals_intArgAt_(m, locals, 1);
	GLenum internalformat = IoMessage_locals_intArgAt_(m, locals, 2);
	GLint x = IoMessage_locals_intArgAt_(m, locals, 3);
	GLint y = IoMessage_locals_intArgAt_(m, locals, 4);
	GLsizei width = IoMessage_locals_intArgAt_(m, locals, 5);
	GLint border = IoMessage_locals_intArgAt_(m, locals, 6);

	glCopyTexImage1D(target,level,internalformat,x,y,width,border);
	return self;
}

IoObject *IoGL_glCopyTexImage2D(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint level = IoMessage_locals_intArgAt_(m, locals, 1);
	GLenum internalformat = IoMessage_locals_intArgAt_(m, locals, 2);
	GLint x = IoMessage_locals_intArgAt_(m, locals, 3);
	GLint y = IoMessage_locals_intArgAt_(m, locals, 4);
	GLsizei width = IoMessage_locals_intArgAt_(m, locals, 5);
	GLsizei height = IoMessage_locals_intArgAt_(m, locals, 6);
	GLint border = IoMessage_locals_intArgAt_(m, locals, 7);

	glCopyTexImage2D(target,level,internalformat,x,y,width,height,border);
	return self;
}

IoObject *IoGL_glCopyTexSubImage1D(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint level = IoMessage_locals_intArgAt_(m, locals, 1);
	GLint xoffset = IoMessage_locals_intArgAt_(m, locals, 2);
	GLint x = IoMessage_locals_intArgAt_(m, locals, 3);
	GLint y = IoMessage_locals_intArgAt_(m, locals, 4);
	GLsizei width = IoMessage_locals_intArgAt_(m, locals, 5);

	glCopyTexSubImage1D(target, level, xoffset, x, y, width);
	return self;
}

IoObject *IoGL_glCopyTexSubImage2D(IoGL *self, IoObject *locals, IoMessage *m)
{
	GLenum target = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint level = IoMessage_locals_intArgAt_(m, locals, 1);
	GLint xoffset = IoMessage_locals_intArgAt_(m, locals, 2);
	GLint yoffset = IoMessage_locals_intArgAt_(m, locals, 3);
	GLint x = IoMessage_locals_intArgAt_(m, locals, 4);
	GLint y = IoMessage_locals_intArgAt_(m, locals, 5);
	GLsizei width = IoMessage_locals_intArgAt_(m, locals, 6);
	GLsizei height = IoMessage_locals_intArgAt_(m, locals, 7);

	glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
	return self;
}

/* -------------------------------------------------------------------*/

#include "IoGLconst.h"
#include "IoGLfunc.h"

IoObject *IoOpenGL_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	unsigned int slotNumber = 0;

	IoObject_setSlot_to_(self, IOSYMBOL("type"), IOSYMBOL("OpenGL"));
	/* too many slots for one object, so break them up into a proto chain of objects */

	/* GL Constants */
	{
		t_ioGL_constTable *curpos = ioGL_constTable;
		while (curpos->name)
		{
			IoObject_setSlot_to_(self, IOSYMBOL(curpos->name), IONUMBER(curpos->value));
			curpos++;
			slotNumber ++;

			if (slotNumber > 128)
			{
				self = IOCLONE(self);
				slotNumber = 0;
			}
		}
	}

	/* GL Functions */
	{
		t_ioGL_funcTable *curpos=ioGL_funcTable;

		while (curpos->name)
		{
			IoCFunction *f = IoCFunction_newWithFunctionPointer_tag_name_(IOSTATE, curpos->func, NULL, curpos->name);

			IoObject_setSlot_to_(self, IOSYMBOL(curpos->name), f);
			curpos++;
			slotNumber ++;

			if (slotNumber > 128)
			{
				self = IOCLONE(self);
				slotNumber = 0;
			}
		}
	}

	return self;
}
