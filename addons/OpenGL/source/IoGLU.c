
//metadoc GLU copyright Steve Dekorte 2002
//metadoc GLU license BSD revised
//metadoc GLU category Graphics

#include "IoGLU.h"

#include "List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoCFunction.h"
#include "IoSeq.h"
#include "IoList.h"
#include "List.h"
#include "IoGLUQuadric.h"
#include <time.h>

#define DATA(self) ((IoGLUData *)IoObject_dataPointer(self))

static const char *protoId = "GLU";

IoTag *IoGLU_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoGLU_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoGLU_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoGLU_mark);
	return tag;
}

IoGLU *IoGLU_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoGLU_newTag(state));
	IoState_registerProtoWithId_(state, self, protoId);
	IoGLU_protoInit(self);
	return self;
}

IoGLU *IoGLU_new(void *state)
{ 
	return IoState_protoWithId_(state, protoId); 
}

void IoGLU_free(IoGLU *self)
{
	/* add code to shut down GLU */
	/*free(IoObject_dataPointer(self));*/
}

void IoGLU_mark(IoGLU *self)
{
}

/* ----------------------------------------------------------- */

IoObject *IoGLU_rawClone(IoGLU *self)
{ 
	return IoState_protoWithId_(IOSTATE, protoId); 
}

/* --- GLU -------------------------------------------------------- */

void GLU_getDoubleVector(IoGLU *self, IoObject *locals, IoMessage *m, List *list, GLdouble **v, int max, char *desc)
{
	int i;

	if (max == 0)
	{
		*v=(GLdouble *)malloc(List_size(list)*sizeof(GLdouble));
	}

	for (i = 0; i < List_size(list) && ((max==0) || (i<max)); i ++)
	{
		IoNumber *num = List_at_(list, i);

		if (!ISNUMBER(num))
		{
			char fname[48];
			snprintf(fname, 48, "Io GL %s", desc);
			IoState_error_(IOSTATE, m, "%s parameter List item #%i must be a Number", fname, i);
		}

		*v[i] = IoNumber_asDouble(num);
	}
}

void GLU_getIntVector(IoGLU *self, IoObject *locals, IoMessage *m, List *list, GLint **v, int max, char *desc)
{
	int i;

	if (max==0)
	{
		*v=(GLint *)malloc(List_size(list)*sizeof(GLint));
	}

	for (i = 0; i < List_size(list) && ((max==0) || (i<max)); i ++)
	{
		IoNumber *num = List_at_(list, i);

		if (!ISNUMBER(num))
		{
			char fname[48];
			snprintf(fname, 48, "Io GL %s", desc);
			IoState_error_(IOSTATE, m, "%s parameter List item #%i must be a Number", fname, i);
		}
		*v[i] = (GLint)IoNumber_asInt(num);
	}
}

IoObject *IoGLU_gluLookAt(IoGLU *self, IoObject *locals, IoMessage *m)
{
	GLdouble eyeX    = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble eyeY    = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble eyeZ    = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLdouble centerX = IoMessage_locals_doubleArgAt_(m, locals, 3);
	GLdouble centerY = IoMessage_locals_doubleArgAt_(m, locals, 4);
	GLdouble centerZ = IoMessage_locals_doubleArgAt_(m, locals, 5);
	GLdouble upX     = IoMessage_locals_doubleArgAt_(m, locals, 6);
	GLdouble upY     = IoMessage_locals_doubleArgAt_(m, locals, 7);
	GLdouble upZ     = IoMessage_locals_doubleArgAt_(m, locals, 8);

	gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
	return self;
}

IoObject *IoGLU_gluPerspective(IoGLU *self, IoObject *locals, IoMessage *m)
{
	GLdouble fovy   = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble aspect = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble zNear  = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLdouble zFar   = IoMessage_locals_doubleArgAt_(m, locals, 3);

	gluPerspective(fovy, aspect, zNear, zFar);
	return self;
}

IoObject *IoGLU_gluOrtho2D(IoGLU *self, IoObject *locals, IoMessage *m)
{
	GLdouble l = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble r = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble b = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLdouble t = IoMessage_locals_doubleArgAt_(m, locals, 3);

	gluOrtho2D(l, r, b, t);
	return self;
}

IoObject *IoGLU_gluBuild1DMipmaps(IoGLU *self, IoObject *locals, IoMessage *m)
{
	GLenum target        = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint internalFormat = IoMessage_locals_intArgAt_(m, locals, 1);
	GLsizei width        = IoMessage_locals_intArgAt_(m, locals, 2);
	GLenum format        = IoMessage_locals_intArgAt_(m, locals, 3);
	GLenum type          = IoMessage_locals_intArgAt_(m, locals, 4);
	IoSeq *data       = IoMessage_locals_seqArgAt_(m, locals, 5);
	GLvoid  *pointer     = (GLvoid *)IoSeq_rawBytes(data);
	GLint r;

	r = gluBuild1DMipmaps(target,internalFormat,width,format,type,pointer);
	return IONUMBER(r);
}

IoObject *IoGLU_gluBuild2DMipmaps(IoGLU *self, IoObject *locals, IoMessage *m)
{
	GLenum target        = IoMessage_locals_intArgAt_(m, locals, 0);
	GLint internalFormat = IoMessage_locals_intArgAt_(m, locals, 1);
	GLsizei width        = IoMessage_locals_intArgAt_(m, locals, 2);
	GLsizei height       = IoMessage_locals_intArgAt_(m, locals, 3);
	GLenum format        = IoMessage_locals_intArgAt_(m, locals, 4);
	GLenum type          = IoMessage_locals_intArgAt_(m, locals, 5);
	IoSeq *data       = IoMessage_locals_seqArgAt_(m, locals, 6);
	GLvoid  *pointer     = (GLvoid *)IoSeq_rawBytes(data);
	GLint r;

	r = gluBuild2DMipmaps(target,internalFormat,width,height,format,type,pointer);
	return IONUMBER(r);
}

IoObject *IoGLU_gluErrorString(IoGLU *self, IoObject *locals, IoMessage *m)
{
	GLenum name = IoMessage_locals_intArgAt_(m, locals, 0);
	GLubyte *r;

	r = (GLubyte *)gluErrorString(name);
	return IOSYMBOL(r);
}

IoObject *IoGLU_gluGetString(IoGLU *self, IoObject *locals, IoMessage *m)
{
	GLenum name = IoMessage_locals_intArgAt_(m, locals, 0);
	GLubyte *r;

	r = (GLubyte *)gluGetString(name);
	return IOSYMBOL(r);
}

IoObject *IoGLU_gluPickMatrix(IoGLU *self, IoObject *locals, IoMessage *m)
{
	GLdouble x     = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble y     = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble delX  = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLdouble delY  = IoMessage_locals_doubleArgAt_(m, locals, 3);
	GLint viewport = IoMessage_locals_intArgAt_(m, locals, 4);

	gluPickMatrix(x,y,delX,delY,&viewport);
	return self;
}

IoObject *IoGLU_gluProject(IoGLU *self, IoObject *locals, IoMessage *m)
{
	GLdouble objX = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble objY = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble objZ = IoMessage_locals_doubleArgAt_(m, locals, 2);
	List *list1 = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 3));
	List *list2 = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 4));
	List *list3 = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 5));
	IoList *list4 = IoMessage_locals_listArgAt_(m, locals, 6);
	IoList *list5 = IoMessage_locals_listArgAt_(m, locals, 7);
	IoList *list6 = IoMessage_locals_listArgAt_(m, locals, 8);
	GLdouble model[16] = {
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
	};
	GLdouble proj[16] = {
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
	};
	GLint view[1]={0};
	GLdouble winX[1]={0.0};
	GLdouble winY[1]={0.0};
	GLdouble winZ[1]={0.0};
	GLint r;

	GLU_getDoubleVector(self,locals,m,list1,(GLdouble **)&model,16,"gluProject");
	GLU_getDoubleVector(self,locals,m,list2,(GLdouble **)&proj,16,"gluProject");
	GLU_getIntVector(self,locals,m,list3,(GLint **)&view,1,"gluProject");
	r = gluProject(objX,objY,objZ,model,proj,view,winX,winY,winZ);
	IoList_rawAt_put_(list4,0,IONUMBER(winX[0]));
	IoList_rawAt_put_(list5,0,IONUMBER(winY[0]));
	IoList_rawAt_put_(list6,0,IONUMBER(winZ[0]));
	return IONUMBER(r);
}

IoObject *IoGLU_gluUnProject(IoGLU *self, IoObject *locals, IoMessage *m)
{
	GLdouble winX = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble winY = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble winZ = IoMessage_locals_doubleArgAt_(m, locals, 2);
	List *list1 = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 3));
	List *list2 = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 4));
	List *list3 = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 5));
	IoList *list4 = IoMessage_locals_listArgAt_(m, locals, 6);
	IoList *list5 = IoMessage_locals_listArgAt_(m, locals, 7);
	IoList *list6 = IoMessage_locals_listArgAt_(m, locals, 8);
	GLdouble model[16] = {
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
	};
	GLdouble proj[16] = {
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,
	};
	GLint view[1]={0};
	GLdouble objX[1]={0.0};
	GLdouble objY[1]={0.0};
	GLdouble objZ[1]={0.0};
	GLint r;

	GLU_getDoubleVector(self,locals,m,list1,(GLdouble **)&model,16,"gluUnproject");
	GLU_getDoubleVector(self,locals,m,list2,(GLdouble **)&proj,16,"gluUnproject");
	GLU_getIntVector(self,locals,m,list3,(GLint **)&view,1,"gluUnproject");
	r = gluUnProject(winX,winY,winZ,model,proj,view,objX,objY,objZ);
	IoList_rawAt_put_(list4,0,IONUMBER(objX[0]));
	IoList_rawAt_put_(list5,0,IONUMBER(objY[0]));
	IoList_rawAt_put_(list6,0,IONUMBER(objZ[0]));
	return IONUMBER(r);
}

IoObject *IoGLU_gluUnProjectOrigin(IoGLU *self, IoObject *locals, IoMessage *m)
{
	GLdouble winX = 0;
	GLdouble winY = 0;
	GLdouble winZ = 0;

	GLdouble model[16];
	GLdouble proj[16];
	GLint view[4];

	GLdouble objX, objY, objZ;
	GLint r;

	glGetDoublev(GL_MODELVIEW_MATRIX, model);
	glGetDoublev(GL_PROJECTION_MATRIX, proj);
	glGetIntegerv(GL_VIEWPORT, view);

	r = gluUnProject(winX, winY, winZ,
					model, proj, view,
					&objX, &objY, &objZ);

	{
		UArray *rv = UArray_new();
		UArray_setSize_(rv, 3);
		UArray_setEncoding_(rv, CENCODING_NUMBER);
		UArray_setItemType_(rv, CTYPE_float64_t);
		UArray_at_putDouble_(rv, 0, objX);
		UArray_at_putDouble_(rv, 1, objY);
		UArray_at_putDouble_(rv, 2, objZ);
		return IoSeq_newWithUArray_copy_(IOSTATE, rv, 0);
	}
}

IoObject *IoGLU_gluProjectOrigin(IoGLU *self, IoObject *locals, IoMessage *m)
{
	GLdouble winX = 0;
	GLdouble winY = 0;
	GLdouble winZ = 0;

	GLdouble model[16];
	GLdouble proj[16];
	GLint view[4];

	GLdouble objX, objY, objZ;
	GLint r;

	glGetDoublev(GL_MODELVIEW_MATRIX, model);
	glGetDoublev(GL_PROJECTION_MATRIX, proj);
	glGetIntegerv(GL_VIEWPORT, view);

	r = gluProject(winX, winY, winZ,
					model, proj, view,
					&objX, &objY, &objZ);

	{
		UArray *rv = UArray_new();
		UArray_setSize_(rv, 3);
		UArray_setEncoding_(rv, CENCODING_NUMBER);
		UArray_setItemType_(rv, CTYPE_float64_t);
		UArray_at_putDouble_(rv, 0, objX);
		UArray_at_putDouble_(rv, 1, objY);
		UArray_at_putDouble_(rv, 2, objZ);
		return IoSeq_newWithUArray_copy_(IOSTATE, rv, 0);
	}
}

IoObject *IoGLU_gluScaleImage(IoGLU *self, IoObject *locals, IoMessage *m)
{
	/*doc GLU gluScaleImage(formatIn, widthIn, heightIn, typeIn, dataSeqIn, widthOut, heightOut, typeOut, dataSeqOut)
	Scales the image data in dataSeqIn and outputs the result to dataSeqOut.
	Returns error code as a Number.
	*/
	GLenum format = IoMessage_locals_intArgAt_(m, locals, 0);
	GLsizei wIn = IoMessage_locals_intArgAt_(m, locals, 1);
	GLsizei hIn = IoMessage_locals_intArgAt_(m, locals, 2);
	GLenum typeIn = IoMessage_locals_intArgAt_(m, locals, 3);
	IoSeq *dataIn = IoMessage_locals_seqArgAt_(m, locals, 4);
	GLsizei wOut = IoMessage_locals_intArgAt_(m, locals, 5);
	GLsizei hOut = IoMessage_locals_intArgAt_(m, locals, 6);
	GLenum typeOut = IoMessage_locals_intArgAt_(m, locals, 7);
	IoSeq *dataOut = IoMessage_locals_seqArgAt_(m, locals, 8);
	GLvoid  *pointerIn = (GLvoid *)IoSeq_rawBytes(dataIn);
	GLvoid  *pointerOut;
	int componentCount = (format == GL_RGB) ? 3 : 4;
	GLint r;
	
	printf("componentCount = %i\n", componentCount);
	
	UArray_setSize_(IoSeq_rawUArray(dataOut), wOut*hOut*componentCount);
	pointerOut = (GLvoid *)IoSeq_rawBytes(dataOut);

	r = gluScaleImage(format, wIn, hIn, typeIn, pointerIn, wOut, hOut, typeOut, pointerOut);
	
	return IONUMBER(r);
}

/* --- Quadrics ----------------------------------------------------------------*/

IoObject *IoGLU_gluNewQuadric(IoGLU *self, IoObject *locals, IoMessage *m)
{
	IoObject *obj = IoGLUQuadric_new(IOSTATE);
	IoGLUQuadric_quadric(obj);
	return obj;
}

IoObject *IoGLU_gluDeleteQuadric(IoGLU *self, IoObject *locals, IoMessage *m)
{
	/* this gets done automatically when it's garbage collected */
	return self;
}

IoObject *IoGLU_gluDisk(IoGLU *self, IoObject *locals, IoMessage *m)
{
	IoGLUQuadric *q = IoMessage_locals_gluQuadricArgAt_(m, locals, 0);
	GLdouble inner = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble outer = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLint slices = IoMessage_locals_intArgAt_(m, locals, 3);
	GLint stacks = IoMessage_locals_intArgAt_(m, locals, 4);

	gluDisk(IoGLUQuadric_quadric(q), inner, outer, slices, stacks);
	return self;
}

IoObject *IoGLU_gluPartialDisk(IoGLU *self, IoObject *locals, IoMessage *m)
{
	IoGLUQuadric *q = IoMessage_locals_gluQuadricArgAt_(m, locals, 0);
	GLdouble inner = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble outer = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLint slices = IoMessage_locals_intArgAt_(m, locals, 3);
	GLint stacks = IoMessage_locals_intArgAt_(m, locals, 4);
	GLdouble start = IoMessage_locals_doubleArgAt_(m, locals, 5);
	GLdouble sweep = IoMessage_locals_doubleArgAt_(m, locals, 6);

	gluPartialDisk(IoGLUQuadric_quadric(q), inner, outer, slices, stacks, start, sweep);
	return self;
}

IoObject *IoGLU_gluCylinder(IoGLU *self, IoObject *locals, IoMessage *m)
{
	IoGLUQuadric *q = IoMessage_locals_gluQuadricArgAt_(m, locals, 0);
	GLdouble base = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble top = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLdouble height = IoMessage_locals_doubleArgAt_(m, locals, 3);
	GLint slices = IoMessage_locals_intArgAt_(m, locals, 4);
	GLint stacks = IoMessage_locals_intArgAt_(m, locals, 5);

	gluCylinder(IoGLUQuadric_quadric(q), base, top, height, slices, stacks);
	return self;
}

IoObject *IoGLU_gluQuadricDrawStyle(IoGLU *self, IoObject *locals, IoMessage *m)
{
	IoGLUQuadric *q = IoMessage_locals_gluQuadricArgAt_(m, locals, 0);
	GLenum draw = IoMessage_locals_intArgAt_(m, locals, 1);

	gluQuadricDrawStyle(IoGLUQuadric_quadric(q), draw);
	return self;
}

IoObject *IoGLU_gluQuadricNormals(IoGLU *self, IoObject *locals, IoMessage *m)
{
	IoGLUQuadric *q = IoMessage_locals_gluQuadricArgAt_(m, locals, 0);
	GLenum draw = IoMessage_locals_intArgAt_(m, locals, 1);

	gluQuadricNormals(IoGLUQuadric_quadric(q), draw);
	return self;
}

IoObject *IoGLU_gluQuadricOrientation(IoGLU *self, IoObject *locals, IoMessage *m)
{
	IoGLUQuadric *q = IoMessage_locals_gluQuadricArgAt_(m, locals, 0);
	GLenum or = IoMessage_locals_intArgAt_(m, locals, 1);

	gluQuadricOrientation(IoGLUQuadric_quadric(q), or);
	return self;
}

IoObject *IoGLU_gluQuadricTexture(IoGLU *self, IoObject *locals, IoMessage *m)
{
	IoGLUQuadric *q = IoMessage_locals_gluQuadricArgAt_(m, locals, 0);
	GLboolean t = IoMessage_locals_intArgAt_(m, locals, 1);

	gluQuadricTexture(IoGLUQuadric_quadric(q), t);
	return self;
}

IoObject *IoGLU_gluSphere(IoGLU *self, IoObject *locals, IoMessage *m)
{
	IoGLUQuadric *q = IoMessage_locals_gluQuadricArgAt_(m, locals, 0);
	GLdouble radius = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLint slices = IoMessage_locals_intArgAt_(m, locals, 2);
	GLint stacks = IoMessage_locals_intArgAt_(m, locals, 3);

	gluSphere(IoGLUQuadric_quadric(q), radius, slices, stacks);
	return self;
}

void gluRoundedBox(GLUquadricObj *quadric, GLdouble w, GLdouble h, GLdouble r, GLint slices)
{
	if (r * 2.0 > w) r = w / 2.0;
	if (r * 2.0 > h) r = h / 2.0;

	glRectd(r, r,  w - r, h - r); // middle
	glRectd(0, r,  r, h - r); // left
	glRectd(w - r, r,  w, h - r); // right
	glRectd(r, h-r,  w - r, h); // top
	glRectd(r, 0, w-r, r); // bottom

	gluQuadricDrawStyle(quadric, GLU_FILL);

	glPushMatrix();
	glTranslated(r, r, 0);
	gluPartialDisk(quadric, 0, r, slices, 1, 180, 90);
	glPopMatrix();

	glPushMatrix();
	glTranslated(r, h-r, 0);
	gluPartialDisk(quadric, 0, r, slices, 1, 0, -90);
	glPopMatrix();

	glPushMatrix();
	glTranslated(w-r, h-r, 0);
	gluPartialDisk(quadric, 0, r, slices, 1, 0, 90);
	glPopMatrix();

	glPushMatrix();
	glTranslated(w-r, r, 0);
	gluPartialDisk(quadric, 0, r, slices, 1, 90, 90);
	glPopMatrix();
}

void gluRoundedBoxOutline(GLUquadricObj *quadric, GLdouble w, GLdouble h, GLdouble r, GLint slices)
{
	if (r * 2.0 > w) r = w / 2.0;
	if (r * 2.0 > h) r = h / 2.0;

	glBegin(GL_LINES);

	// left
	glVertex2d(0, r);
	glVertex2d(0, h - r);

	// right
	glVertex2d(w, r);
	glVertex2d(w, h - r);

	// top
	glVertex2d(r, h);
	glVertex2d(w-r, h);

	// bottom
	glVertex2d(r, 0);
	glVertex2d(w-r, 0);

	glEnd();

	gluQuadricDrawStyle(quadric, GLU_LINE);

	glPushMatrix();
	glTranslated(r, r, 0);
	gluPartialDisk(quadric, r, r, slices, 1, 180, 90);
	glPopMatrix();

	glPushMatrix();
	glTranslated(r, h-r, 0);
	gluPartialDisk(quadric, r, r, slices, 1, 0, -90);
	glPopMatrix();

	glPushMatrix();
	glTranslated(w-r, h-r, 0);
	gluPartialDisk(quadric, r, r, slices, 1, 0, 90);
	glPopMatrix();

	glPushMatrix();
	glTranslated(w-r, r, 0);
	gluPartialDisk(quadric, r, r, slices, 1, 90, 90);
	glPopMatrix();
}

IoObject *IoGLU_gluRoundedBox(IoGLU *self, IoObject *locals, IoMessage *m)
{
	GLUquadricObj *q = IoGLUQuadric_quadric(IoMessage_locals_gluQuadricArgAt_(m, locals, 0));
	GLdouble w = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble h = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLdouble r = IoMessage_locals_doubleArgAt_(m, locals, 3);
	GLint slices = IoMessage_locals_intArgAt_(m, locals, 4);

	gluRoundedBox(q, w, h, r, slices);

	return self;
}


IoObject *IoGLU_gluRoundedBoxOutline(IoGLU *self, IoObject *locals, IoMessage *m)
{
	GLUquadricObj *q = IoGLUQuadric_quadric(IoMessage_locals_gluQuadricArgAt_(m, locals, 0));
	GLdouble w = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLdouble h = IoMessage_locals_doubleArgAt_(m, locals, 2);
	GLdouble r = IoMessage_locals_doubleArgAt_(m, locals, 3);
	GLint slices = IoMessage_locals_intArgAt_(m, locals, 4);

	gluRoundedBoxOutline(q, w, h, r, slices);

	return self;
}


/* -------------------------------------------------------------------*/

#include "IoGLUconst.h"
#include "IoGLUfunc.h"

void IoGLU_protoInit(IoGLU *self)
{
	IoObject_setSlot_to_(self, IOSYMBOL("clone"), IOCFUNCTION(IoObject_self, NULL));

	/* GLU Constants */
	{
		t_ioGLU_constTable *curpos=ioGLU_constTable;

		while (curpos->name)
		{
			IoObject_setSlot_to_(self, IOSYMBOL(curpos->name), IONUMBER(curpos->value));
			curpos++;
		}
	}

	/* GLU Functions */
	{
		t_ioGLU_funcTable *curpos=ioGLU_funcTable;

		while (curpos->name)
		{
			IoCFunction *f = IoCFunction_newWithFunctionPointer_tag_name_(IOSTATE, curpos->func, NULL, curpos->name);

			IoObject_setSlot_to_(self, IOSYMBOL(curpos->name), f);
			curpos++;
		}
	}
}

