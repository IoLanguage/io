
//metadoc GLUT copyright Steve Dekorte 2002
//metadoc GLUT license BSD revised
//metadoc GLUT category Graphics

#include "IoGLUT.h"

#include "List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoCFunction.h"
#include "IoSeq.h"
#include "IoList.h"
#include "List.h"
#include "IoDirectory.h"
#include <time.h>

#define DATA(self) ((IoGLUTData *)IoObject_dataPointer(self))

void IoGlutTimerFunc(int vv);

static IoGLUT *proto = NULL;


IoTag *IoGLUT_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("GLUT");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoGLUT_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoGLUT_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoGLUT_mark);
	return tag;
}

#define GLUTMESSAGE(name) \
IoMessage_newWithName_label_(state, IOSYMBOL(name), IOSYMBOL("[GLUT]"))

IoGLUT *IoGLUT_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	/*printf("state = %p proto = %p self = %p\n", state, proto, self); */

	proto = self;
	IoObject_tag_(self, IoGLUT_newTag(state));
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoGLUTData)));

	DATA(self)->coroutine = IoCoroutine_new(state);
	//printf("GLUT coro = %p\n", DATA(self)->coroutine);

	DATA(self)->eventTarget = NULL;
	DATA(self)->entryMessage      = GLUTMESSAGE("entry");
	DATA(self)->displayMessage    = GLUTMESSAGE("display");
	DATA(self)->keyboardMessage   = GLUTMESSAGE("keyboard");
	DATA(self)->keyboardUpMessage = GLUTMESSAGE("keyboardUp");
	DATA(self)->joystickMessage   = GLUTMESSAGE("joystick");
	DATA(self)->motionMessage     = GLUTMESSAGE("motion");
	DATA(self)->menuMessage       = GLUTMESSAGE("menu");
	DATA(self)->mouseMessage      = GLUTMESSAGE("mouse");
	DATA(self)->passiveMotionMessage = GLUTMESSAGE("passiveMotion");
	DATA(self)->reshapeMessage = GLUTMESSAGE("reshape");
	DATA(self)->specialMessage = GLUTMESSAGE("special");
	DATA(self)->specialUpMessage = GLUTMESSAGE("specialUp");
	DATA(self)->timerMessage   = GLUTMESSAGE("timer");

	DATA(self)->acceptsDropMessage = GLUTMESSAGE("acceptsDrop");
	DATA(self)->dragMessage   = GLUTMESSAGE("drag");
	DATA(self)->dropMessage   = GLUTMESSAGE("drop");
	DATA(self)->copyMessage   = GLUTMESSAGE("copy");
	DATA(self)->pasteMessage  = GLUTMESSAGE("paste");
	DATA(self)->deleteMessage = GLUTMESSAGE("delete");
	DATA(self)->nanoSleepPeriod = 1000000000/2;




	//printf("GLUT coro = %p\n", (void *)DATA(self)->coroutine);
	IoState_retain_(state, self);
	IoState_retain_(state, DATA(self)->coroutine);
	IoState_retain_(state, DATA(self)->displayMessage);
	IoState_retain_(state, DATA(self)->reshapeMessage);
	IoState_retain_(state, DATA(self)->timerMessage);



	DATA(self)->j = IoSeq_newFloatArrayOfSize_(state, 0);
	DATA(self)->lastJ = UArray_new();
	UArray_setItemType_(DATA(self)->lastJ, CTYPE_float32_t);

	IoState_registerProtoWithFunc_(state, self, IoGLUT_proto);


	//-----------------------------

	IoState_retain_(state, DATA(self)->j);

	IoState_retain_(state, DATA(self)->coroutine);
	IoState_retain_(state, DATA(self)->entryMessage);
	IoState_retain_(state, DATA(self)->displayMessage);
	IoState_retain_(state, DATA(self)->keyboardMessage);
	IoState_retain_(state, DATA(self)->keyboardUpMessage);
	IoState_retain_(state, DATA(self)->joystickMessage);
	IoState_retain_(state, DATA(self)->menuMessage);
	IoState_retain_(state, DATA(self)->mouseMessage);
	IoState_retain_(state, DATA(self)->motionMessage);
	IoState_retain_(state, DATA(self)->passiveMotionMessage);
	IoState_retain_(state, DATA(self)->reshapeMessage);
	IoState_retain_(state, DATA(self)->specialMessage);
	IoState_retain_(state, DATA(self)->specialUpMessage);
	IoState_retain_(state, DATA(self)->timerMessage);

	IoState_retain_(state, DATA(self)->acceptsDropMessage);
	IoState_retain_(state, DATA(self)->dragMessage);
	IoState_retain_(state, DATA(self)->dropMessage);
	IoState_retain_(state, DATA(self)->copyMessage);
	IoState_retain_(state, DATA(self)->pasteMessage);
	IoState_retain_(state, DATA(self)->deleteMessage);

	IoGLUT_protoInit(self);
	return self;
}

IoGLUT *IoGLUT_new(void *state)
{
	return IoState_protoWithInitFunction_(state, IoGLUT_proto);
}

void IoGLUT_free(IoGLUT *self)
{
	/* add code to shut down GLUT */
	free(IoObject_dataPointer(self));
}

void IoGLUT_mark(IoGLUT *self)
{
	//printf("IoGLUT_mark\n");

	if (DATA(self)->eventTarget)
	{
		IoObject_shouldMark(DATA(self)->eventTarget);
	}

	IoObject_shouldMark(DATA(self)->j);

	IoObject_shouldMark(DATA(self)->coroutine);
	IoObject_shouldMark(DATA(self)->entryMessage);
	IoObject_shouldMark(DATA(self)->displayMessage);
	IoObject_shouldMark(DATA(self)->keyboardMessage);
	IoObject_shouldMark(DATA(self)->keyboardUpMessage);
	IoObject_shouldMark(DATA(self)->joystickMessage);
	IoObject_shouldMark(DATA(self)->menuMessage);
	IoObject_shouldMark(DATA(self)->mouseMessage);
	IoObject_shouldMark(DATA(self)->motionMessage);
	IoObject_shouldMark(DATA(self)->passiveMotionMessage);
	IoObject_shouldMark(DATA(self)->reshapeMessage);
	IoObject_shouldMark(DATA(self)->specialMessage);
	IoObject_shouldMark(DATA(self)->specialUpMessage);
	IoObject_shouldMark(DATA(self)->timerMessage);

	IoObject_shouldMark(DATA(self)->acceptsDropMessage);
	IoObject_shouldMark(DATA(self)->dragMessage);
	IoObject_shouldMark(DATA(self)->dropMessage);
	IoObject_shouldMark(DATA(self)->copyMessage);
	IoObject_shouldMark(DATA(self)->pasteMessage);
	IoObject_shouldMark(DATA(self)->deleteMessage);
}

/* ----------------------------------------------------------- */

IoObject *IoGLUT_rawClone(IoGLUT *self)
{
	return IoState_protoWithInitFunction_(IOSTATE, IoGLUT_proto);
}

/* ----------------------------------------------------------- */

IoObject *IoGLUT_glutInitDisplayMode(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int mode = IoMessage_locals_intArgAt_(m, locals, 0);
	glutInitDisplayMode(mode);
	return self;
}

IoObject *IoGLUT_glutInitWindowSize(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int w  = IoMessage_locals_intArgAt_(m, locals, 0);
	int h = IoMessage_locals_intArgAt_(m, locals, 1);
	//printf("glutInitWindowSize(%i, %i)\n", w, h);
	glutInitWindowSize(w, h);
	return self;
}

IoObject *IoGLUT_glutInitWindowPosition(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int x = IoMessage_locals_intArgAt_(m, locals, 0);
	int y = IoMessage_locals_intArgAt_(m, locals, 1);
	//printf("glutInitWindowPosition(%i, %i)\n", x, y);
	glutInitWindowPosition(x, y);
	return self;
}

void IoGlutIdleFunc(void)
{
	//IoState_yield(IoObject_state(proto));
}

static int glutHasInitialized = 0;

int IoGLUT_HasInitialized(void)
{
	return glutHasInitialized;
}

IoObject *IoGLUT_glutInit(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	IoState *state = IOSTATE;
	int argc = state->mainArgs->argc;
	UArray *ba = IoDirectory_CurrentWorkingDirectoryAsUArray();
	glutInit(&argc, (char **)(state->mainArgs->argv));
	IoDirectory_SetCurrentWorkingDirectory(UArray_asCString(ba));
	UArray_free(ba);
	//glutHasInitialized = 1;
	//glutIdleFunc(IoGlutIdleFunc);
	//glutTimerFunc((unsigned int)10, IoGlutTimerFunc, -1);
	return self;
}

IoObject *IoGLUT_glutInitDisplayString(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	IoSymbol *s = IoMessage_locals_symbolArgAt_(m, locals, 0);
	glutInitDisplayString(IoSeq_asCString(s));
	return self;
}

IoObject *IoGLUT_glutCreateWindow(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	IoSymbol *s = IoMessage_locals_symbolArgAt_(m, locals, 0);
	int windowId = glutCreateWindow(IoSeq_asCString(s));
	return IONUMBER(windowId);
}

IoObject *IoGLUT_glutCreateSubWindow(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int win = IoMessage_locals_intArgAt_(m, locals, 0);
	int x = IoMessage_locals_intArgAt_(m, locals, 1);
	int y = IoMessage_locals_intArgAt_(m, locals, 2);
	int w = IoMessage_locals_intArgAt_(m, locals, 3);
	int h = IoMessage_locals_intArgAt_(m, locals, 4);
	return IONUMBER(glutCreateSubWindow(win, x, y, w, h));
}

IoObject *IoGLUT_glutSetWindow(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int win = IoMessage_locals_intArgAt_(m, locals, 0);
	glutSetWindow(win);
	return self;
}

IoObject *IoGLUT_glutGetWindow(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(glutGetWindow());
}

IoObject *IoGLUT_glutGet(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	GLenum n = IoMessage_locals_intArgAt_(m, locals, 0);
	return IONUMBER(glutGet(n));
}

IoObject *IoGLUT_glutDestroyWindow(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int win = IoMessage_locals_intArgAt_(m, locals, 0);
	glutDestroyWindow(win);
	return self;
}

IoObject *IoGLUT_glutPositionWindow(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int x = IoMessage_locals_intArgAt_(m, locals, 0);
	int y = IoMessage_locals_intArgAt_(m, locals, 1);
	glutPositionWindow(x, y);
	return self;
}

IoObject *IoGLUT_glutReshapeWindow(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int w = IoMessage_locals_intArgAt_(m, locals, 0);
	int h = IoMessage_locals_intArgAt_(m, locals, 1);
	glutReshapeWindow(w, h);
	return self;
}

IoObject *IoGLUT_glutPopWindow(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutPopWindow(); return self;
}

IoObject *IoGLUT_glutPushWindow(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutPushWindow();
	return self;
}

IoObject *IoGLUT_glutShowWindow(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutShowWindow();
	return self;
}

IoObject *IoGLUT_glutHideWindow(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutHideWindow();
	return self;
}

IoObject *IoGLUT_glutIconifyWindow(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutIconifyWindow();
	return self;
}

IoObject *IoGLUT_glutSetWindowTitle(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	IoSymbol *s = IoMessage_locals_symbolArgAt_(m, locals, 0);
	glutSetWindowTitle(CSTRING(s));
	return self;
}

IoObject *IoGLUT_glutSetIconTitle(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	IoSymbol *s = IoMessage_locals_symbolArgAt_(m, locals, 0);
	glutSetIconTitle(CSTRING(s));
	return self;
}

IoObject *IoGLUT_glutEventTarget_(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	DATA(proto)->eventTarget = IOREF(IoMessage_locals_valueArgAt_(m, locals, 0));
	return self;
}

// events

void IoGlutDisplayFunc(void)
{
	//printf("IoGlutDisplayFunc\n");
	IoState_pushRetainPool(IoObject_state(proto));

	IoGLUT_tryCallback(proto, DATA(proto)->displayMessage);

	IoState_popRetainPool(IoObject_state(proto));
}

IoObject *IoGLUT_glutDisplayFunc(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutDisplayFunc(IoGlutDisplayFunc);
	return self;
}

void IoGlutKeyboardFunc(unsigned char key, int xv, int yv)
{
	IoState_pushRetainPool(IoObject_state(proto));
	IoMessage_setCachedArg_toInt_(DATA(proto)->keyboardMessage, 0, (int)key);
	IoMessage_setCachedArg_toInt_(DATA(proto)->keyboardMessage, 1, xv);
	IoMessage_setCachedArg_toInt_(DATA(proto)->keyboardMessage, 2, yv);

	IoGLUT_tryCallback(proto, DATA(proto)->keyboardMessage);

	IoState_popRetainPool(IoObject_state(proto));
}

IoObject *IoGLUT_glutKeyboardFunc(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutKeyboardFunc(IoGlutKeyboardFunc);
	return self;
}

void IoGlutSpecialFunc(int key, int xv, int yv)
{
	IoState_pushRetainPool(IoObject_state(proto));
	IoMessage_setCachedArg_toInt_(DATA(proto)->specialMessage, 0, (int)key);
	IoMessage_setCachedArg_toInt_(DATA(proto)->specialMessage, 1, xv);
	IoMessage_setCachedArg_toInt_(DATA(proto)->specialMessage, 2, yv);

	IoGLUT_tryCallback(proto, DATA(proto)->specialMessage);

	IoState_popRetainPool(IoObject_state(proto));
}

IoObject *IoGLUT_glutSpecialFunc(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutSpecialFunc(IoGlutSpecialFunc);
	return self;
}

void IoGlutKeyboardUpFunc(unsigned char key, int xv, int yv)
{
	IoState_pushRetainPool(IoObject_state(proto));
	IoMessage_setCachedArg_toInt_(DATA(proto)->keyboardUpMessage, 0, (int)key);
	IoMessage_setCachedArg_toInt_(DATA(proto)->keyboardUpMessage, 1, xv);
	IoMessage_setCachedArg_toInt_(DATA(proto)->keyboardUpMessage, 2, yv);

	IoGLUT_tryCallback(proto, DATA(proto)->keyboardUpMessage);

	IoState_popRetainPool(IoObject_state(proto));
}

IoObject *IoGLUT_glutKeyboardUpFunc(IoGLUT *self, IoObject *locals, IoMessage *m)
{
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 13)
	glutKeyboardUpFunc(IoGlutKeyboardUpFunc);
	return self;
#endif
}

void IoGlutSpecialUpFunc(int key, int xv, int yv)
{
	IoState_pushRetainPool(IoObject_state(proto));
	IoMessage_setCachedArg_toInt_(DATA(proto)->specialUpMessage, 0, (int)key);
	IoMessage_setCachedArg_toInt_(DATA(proto)->specialUpMessage, 1, xv);
	IoMessage_setCachedArg_toInt_(DATA(proto)->specialUpMessage, 2, yv);

	IoGLUT_tryCallback(proto, DATA(proto)->specialUpMessage);

	IoState_popRetainPool(IoObject_state(proto));
}

IoObject *IoGLUT_glutSpecialUpFunc(IoGLUT *self, IoObject *locals, IoMessage *m)
{
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 13)
	glutSpecialUpFunc(IoGlutSpecialUpFunc);
	return self;
#endif
}

IoObject *IoGLUT_glutGetModifiers(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(glutGetModifiers());
}

void IoGlutEntryFunc(int state)
{
	IoState_pushRetainPool(IoObject_state(proto));
	IoMessage_setCachedArg_toInt_(DATA(proto)->entryMessage, 0, state);

	IoGLUT_tryCallback(proto, DATA(proto)->entryMessage);

	IoState_popRetainPool(IoObject_state(proto));
}

IoObject *IoGLUT_glutEntryFunc(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutEntryFunc(IoGlutEntryFunc); return self;
}

void IoGlutMotionFunc(int xv, int yv)
{
	IoState_pushRetainPool(IoObject_state(proto));

	{
		IoMessage_setCachedArg_toInt_(DATA(proto)->motionMessage, 0, xv);
		IoMessage_setCachedArg_toInt_(DATA(proto)->motionMessage, 1, yv);

		IoGLUT_tryCallback(proto, DATA(proto)->motionMessage);
	}

	IoState_popRetainPool(IoObject_state(proto));
}

IoObject *IoGLUT_glutMotionFunc(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutMotionFunc(IoGlutMotionFunc);
	return self;
}

void IoGlutPassiveMotionFunc(int xv, int yv)
{
	IoState_pushRetainPool(IoObject_state(proto));

	{
		IoMessage_setCachedArg_toInt_(DATA(proto)->passiveMotionMessage, 0, xv);
		IoMessage_setCachedArg_toInt_(DATA(proto)->passiveMotionMessage, 1, yv);

		IoGLUT_tryCallback(proto, DATA(proto)->passiveMotionMessage);
	}

	IoState_popRetainPool(IoObject_state(proto));
}

IoObject *IoGLUT_glutPassiveMotionFunc(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutPassiveMotionFunc(IoGlutPassiveMotionFunc);
	return self;
}

void IoGlutMouseFunc(int button, int state, int xv, int yv)
{
	IoState_pushRetainPool(IoObject_state(proto));

	{
		IoMessage_setCachedArg_toInt_(DATA(proto)->mouseMessage, 0, button);
		IoMessage_setCachedArg_toInt_(DATA(proto)->mouseMessage, 1, state);
		IoMessage_setCachedArg_toInt_(DATA(proto)->mouseMessage, 2, xv);
		IoMessage_setCachedArg_toInt_(DATA(proto)->mouseMessage, 3, yv);

		IoGLUT_tryCallback(proto, DATA(proto)->mouseMessage);
	}

	IoState_popRetainPool(IoObject_state(proto));
}

IoObject *IoGLUT_glutMouseFunc(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutMouseFunc(IoGlutMouseFunc);
	return self;
}

void IoGlutReshapeFunc(int width, int height)
{
	//printf("IoGlutReshapeFunc\n");
	IoState_pushRetainPool(IoObject_state(proto));
	{
		IoMessage_setCachedArg_toInt_(DATA(proto)->reshapeMessage, 0, width?width:1);
		IoMessage_setCachedArg_toInt_(DATA(proto)->reshapeMessage, 1, height?height:1);

		IoGLUT_tryCallback(proto, DATA(proto)->reshapeMessage);
	}

	IoState_popRetainPool(IoObject_state(proto));
}

IoObject *IoGLUT_glutReshapeFunc(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutReshapeFunc(IoGlutReshapeFunc);
	return self;
}

void IoGlutTimerFunc(int vv)
{
	IoState *state = IoObject_state(proto);

	//printf("IoGlutTimerFunc\n");

	IoState_pushRetainPool(state);

	if (vv == -1)
	{
		//IoState_yield(IoObject_state(proto));
		glutTimerFunc((unsigned int)100, IoGlutTimerFunc, -1);
	}
	else
	{
		IoObject *m = DATA(proto)->timerMessage;
		IoMessage_setCachedArg_toInt_(m, 0, vv);
		IoGLUT_tryCallback(proto, m);
	}
	IoState_popRetainPool(state);
}

IoObject *IoGLUT_glutTimerFunc(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	unsigned int msecs = IoMessage_locals_longArgAt_(m, locals, 0);
	int v = IoMessage_locals_intArgAt_(m, locals, 1);
	//printf("IoGLUT_glutTimerFunc msecs %i v %i\n", msecs, v);
	glutTimerFunc((unsigned int)msecs, IoGlutTimerFunc, v);
	return self;
}

IoObject *IoGLUT_tryCallback(IoGLUT *self, IoMessage *m)
{
	IoState *state = IoObject_state(proto);
	IoObject *tryCoro = DATA(self)->coroutine;
	IoObject *t = DATA(proto)->eventTarget;
	IoObject *result = state->ioNil;

	//printf("IoGLUT_tryCallback(self, %p)\n", (void *)m);

	if (t)
	{
		//result = IoState_tryToPerform(state, t, t, m);


		//IoCoroutine_try(tryCoro, t, t, m);
		IoMessage_locals_performOn_(m, t, t);

		if (IoCoroutine_rawException(tryCoro) != state->ioNil)
		{
			IoState_exception_(state, tryCoro);
		}

		IoCoroutine_clearStack(tryCoro);
		return IoCoroutine_rawResult(tryCoro);
	}

	return result;
}

#ifdef GLUT_KEY_REPEAT_ON
IoObject *IoGLUT_glutIgnoreKeyRepeat(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int v = IoMessage_locals_intArgAt_(m, locals, 0);
	glutIgnoreKeyRepeat(v);
	return self;
}
#endif

IoObject *IoGLUT_glutMainLoop(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutHasInitialized = 1;
	glutMainLoop();
	return self;
}

IoObject *IoGLUT_glutPostRedisplay(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutPostRedisplay();
	return self;
}

IoObject *IoGLUT_glutSwapBuffers(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutSwapBuffers();
	return self;
}

IoObject *IoGLUT_glutSolidCone(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	GLdouble radius = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble height = IoMessage_locals_intArgAt_(m, locals, 1);
	GLint slices = IoMessage_locals_intArgAt_(m, locals, 2);
	GLint stacks = IoMessage_locals_intArgAt_(m, locals, 3);
	glutSolidCone(radius, height, slices, stacks);
	return self;
}

IoObject *IoGLUT_glutWireCone(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	GLdouble radius = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble height = IoMessage_locals_intArgAt_(m, locals, 1);
	GLint slices = IoMessage_locals_intArgAt_(m, locals, 2);
	GLint stacks = IoMessage_locals_intArgAt_(m, locals, 3);
	glutWireCone(radius, height, slices, stacks);
	return self;
}

IoObject *IoGLUT_glutSolidSphere(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	GLdouble radius = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLint slices = IoMessage_locals_intArgAt_(m, locals, 1);
	GLint stacks = IoMessage_locals_intArgAt_(m, locals, 2);
	glutSolidSphere(radius, slices, stacks);
	return self;
}

IoObject *IoGLUT_glutWireSphere(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	GLdouble radius = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLint slices = IoMessage_locals_intArgAt_(m, locals, 1);
	GLint stacks = IoMessage_locals_intArgAt_(m, locals, 2);
	glutWireSphere(radius, slices, stacks);
	return self;
}

IoObject *IoGLUT_glutSolidTorus(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	GLdouble innerRadius = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble outerRadius = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLint nsides = IoMessage_locals_intArgAt_(m, locals, 2);
	GLint rings = IoMessage_locals_intArgAt_(m, locals, 3);
	glutSolidTorus(innerRadius, outerRadius, nsides, rings);
	return self;
}

IoObject *IoGLUT_glutWireTorus(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	GLdouble innerRadius = IoMessage_locals_doubleArgAt_(m, locals, 0);
	GLdouble outerRadius = IoMessage_locals_doubleArgAt_(m, locals, 1);
	GLint nsides = IoMessage_locals_intArgAt_(m, locals, 2);
	GLint rings = IoMessage_locals_intArgAt_(m, locals, 3);
	glutWireTorus(innerRadius, outerRadius, nsides, rings);
	return self;
}

IoObject *IoGLUT_glutSolidDodecahedron(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutSolidDodecahedron();
	return self;
}

IoObject *IoGLUT_glutWireDodecahedron(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutWireDodecahedron();
	return self;
}

IoObject *IoGLUT_glutSolidOctahedron(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutSolidOctahedron();
	return self;
}

IoObject *IoGLUT_glutWireOctahedron(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutWireOctahedron();
	return self;
}

IoObject *IoGLUT_glutSolidTetrahedron(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutSolidTetrahedron();
	return self;
}

IoObject *IoGLUT_glutWireTetrahedron(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutWireTetrahedron();
	return self;
}

IoObject *IoGLUT_glutSolidIcosahedron(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutSolidIcosahedron();
	return self;
}

IoObject *IoGLUT_glutWireIcosahedron(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutWireIcosahedron();
	return self;
}

IoObject *IoGLUT_glutSolidTeapot(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	GLdouble r = IoMessage_locals_doubleArgAt_(m, locals, 0);
	glutSolidTeapot(r);
	return self;
}

IoObject *IoGLUT_glutWireTeapot(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	GLdouble r = IoMessage_locals_doubleArgAt_(m, locals, 0);
	glutWireTeapot(r);
	return self;
}

IoObject *IoGLUT_glutSolidCube(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	GLdouble radius = IoMessage_locals_doubleArgAt_(m, locals, 0);
	glutSolidCube(radius);
	return self;
}

IoObject *IoGLUT_glutWireCube(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	GLdouble radius = IoMessage_locals_doubleArgAt_(m, locals, 0);
	glutWireCube(radius);
	return self;
}

IoObject *IoGLUT_glutStrokeCharacter(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int fontNum = IoMessage_locals_intArgAt_(m, locals, 0);
	char c = IoMessage_locals_intArgAt_(m, locals, 1);
	void *font = GLUT_STROKE_ROMAN;
	if (fontNum) font = GLUT_STROKE_MONO_ROMAN;
	glutStrokeCharacter(font, c);
	return self;
}

IoObject *IoGLUT_glutStrokeString(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int fontNum = IoMessage_locals_intArgAt_(m, locals, 0);
	IoSymbol *string = IoMessage_locals_seqArgAt_(m, locals, 1);
	char *s = CSTRING(string);
	void *font = GLUT_STROKE_ROMAN;
	if (fontNum) font = GLUT_STROKE_MONO_ROMAN;

	while (*s)
	{
		glutStrokeCharacter(font, *s);
		s++;
	}

	return self;
}

/* --- GLUT game mode ----------------------------------------------------- */

IoObject *IoGLUT_glutFullScreen(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutFullScreen();
	return self;
}

#ifdef GLUT_GAME_MODE_ACTIVE

IoObject *IoGLUT_glutEnterGameMode(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutEnterGameMode();
	return self;
}

IoObject *IoGLUT_glutLeaveGameMode(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	glutLeaveGameMode();
	return self;
}

IoObject *IoGLUT_glutWarpPointer(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int x = IoMessage_locals_intArgAt_(m, locals, 0);
	int y = IoMessage_locals_intArgAt_(m, locals, 1);
	glutWarpPointer(x, y);
	return self;
}

IoObject *IoGLUT_glutGameModeString(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	IoSymbol *s = IoMessage_locals_symbolArgAt_(m, locals, 0);
	glutGameModeString(IoSeq_asCString(s));
	return self;
}

#endif

IoObject *IoGLUT_glutSetCursor(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int x = IoMessage_locals_intArgAt_(m, locals, 0);
	glutSetCursor(x);
	return self;
}

// Menus --------------------------------------------------------------------

void IoGlutMenuFunc(int menuId)
{
	IoState_pushRetainPool(IoObject_state(proto));

	{
		IoMessage_setCachedArg_toInt_(DATA(proto)->menuMessage, 0, menuId);
		IoGLUT_tryCallback(proto, DATA(proto)->menuMessage);
	}

	IoState_popRetainPool(IoObject_state(proto));
}

IoObject *IoGLUT_glutCreateMenu(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int menuId = glutCreateMenu(IoGlutMenuFunc);
	return IONUMBER(menuId);
}

IoObject *IoGLUT_glutSetMenu(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int menuId = IoMessage_locals_intArgAt_(m, locals, 0);
	glutSetMenu(menuId);
	return self;
}

IoObject *IoGLUT_glutGetMenu(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(glutGetMenu());
}

IoObject *IoGLUT_glutDestroyMenu(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int menuId = IoMessage_locals_intArgAt_(m, locals, 0);
	glutDestroyMenu(menuId);
	return self;
}

IoObject *IoGLUT_glutAddMenuEntry(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	IoSymbol *name = IoMessage_locals_symbolArgAt_(m, locals, 0);
	int entryId = IoMessage_locals_intArgAt_(m, locals, 1);
	glutAddMenuEntry(CSTRING(name), entryId);
	return self;
}

IoObject *IoGLUT_glutAddSubMenu(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	IoSymbol *name = IoMessage_locals_symbolArgAt_(m, locals, 0);
	int entryId = IoMessage_locals_intArgAt_(m, locals, 1);
	glutAddSubMenu(CSTRING(name), entryId);
	return self;
}

IoObject *IoGLUT_glutChangeToMenuEntry(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int entryId = IoMessage_locals_intArgAt_(m, locals, 0);
	IoSymbol *name = IoMessage_locals_symbolArgAt_(m, locals, 1);
	int value = IoMessage_locals_intArgAt_(m, locals, 2);
	glutChangeToMenuEntry(entryId, CSTRING(name), value);
	return self;
}

// GLUT_MENU_NUM_ITEMS

IoObject *IoGLUT_glutChangeToSubMenu(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int entryId = IoMessage_locals_intArgAt_(m, locals, 0);
	IoSymbol *name = IoMessage_locals_symbolArgAt_(m, locals, 1);
	int value = IoMessage_locals_intArgAt_(m, locals, 2);
	glutChangeToSubMenu(entryId, CSTRING(name), value);
	return self;
}

IoObject *IoGLUT_glutRemoveMenuItem(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int entry = IoMessage_locals_intArgAt_(m, locals, 0);
	glutRemoveMenuItem(entry);
	return self;
}

IoObject *IoGLUT_glutAttachMenu(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int button = IoMessage_locals_intArgAt_(m, locals, 0);
	glutAttachMenu(button);
	return self;
}

IoObject *IoGLUT_glutDetachMenu(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int button = IoMessage_locals_intArgAt_(m, locals, 0);
	glutAttachMenu(button);
	return self;
}

// Joystick --------------------------------------------------------------------

void IoGlutJoystickFunc(unsigned int buttonMask, int x, int y, int z)
{
	UArray *j = IoSeq_rawUArray(DATA(proto)->j);
	UArray *lastJ = DATA(proto)->lastJ;
	vec3f v = {x, y, z};

	IoSeq_setVec3f_(DATA(proto)->j, v);
	UArray_subtract_(lastJ, j);

	if (DATA(proto)->lastJoystickButton != buttonMask || UArray_sumAsDouble(lastJ) != 0.0) // only send callback if a change occurs
	{
		//printf("sum %f\n", (float)Vector_sum(lastJ));
		//printf("b %i %i\n", (int)buttonMask, (int)DATA(proto)->lastJoystickButton);
		IoState_pushRetainPool(IoObject_state(proto));

		{
			IoMessage_setCachedArg_toInt_(DATA(proto)->joystickMessage, 0, (int)buttonMask);
			IoMessage_setCachedArg_to_(DATA(proto)->joystickMessage, 1, DATA(proto)->j);
			IoGLUT_tryCallback(proto, DATA(proto)->joystickMessage);
		}

		DATA(proto)->lastJoystickButton = buttonMask;

		IoState_popRetainPool(IoObject_state(proto));
	}

	UArray_copy_(lastJ, j);
}

/*
IoObject *IoGLUT_glutInitJoystick(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	IoSymbol *deviceName = IoMessage_locals_symbolArgAt_(m, locals, 0);
	// deviceName something like /dev/input/js0
	glutInitJoystick(IoGlutJoystickFunc, CSTRING(deviceName));
	return self;
}
*/
IoObject *IoGLUT_glutJoystickFunc(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int pollInterval = IoMessage_locals_intArgAt_(m, locals, 0);
	glutJoystickFunc(IoGlutJoystickFunc, pollInterval);
	return self;
}


/* -------------------------------------------------------------------*/

/* can use NULL tag since the methods don't access DATA(self) - they use DATA(proto) instead */
#define IOCFUNCTION_GLUT(func) IOCFUNCTION(func, NULL)

#include "IoGLUTconst.h"
#include "IoGLUTfunc.h"

void IoGLUT_protoInit(IoGLUT *self)
{
	IoObject_setSlot_to_(self,IOSYMBOL("clone"), IOCFUNCTION_GLUT(IoObject_self));

	/* GLUT Constants */
	{
		t_ioGLUT_constTable *curpos=ioGLUT_constTable;
		while (curpos->name)
		{
			IoObject_setSlot_to_(self,
								 IOSYMBOL(curpos->name),
								 IONUMBER(curpos->value));
			curpos++;
		}
	}

	/* GLUT Functions */
	{
		t_ioGLUT_funcTable *curpos=ioGLUT_funcTable;

		while (curpos->name)
		{
			IoCFunction *f = IoCFunction_newWithFunctionPointer_tag_name_(IOSTATE, curpos->func, NULL, curpos->name);
			IoObject_setSlot_to_(self, IOSYMBOL(curpos->name), f);
			curpos++;
		}
	}
}

// --- Extra In --------------------------------------------------------------

int IoGlutAcceptsDropFunc(
						  int x,
						  int y,
						  const char *type,
						  const unsigned char *data,
						  int dataLength)
{
	int result = 0;
	IoState *state = IoObject_state(proto);
	IoState_pushRetainPool(state);

	{
		IoMessage *m = DATA(proto)->acceptsDropMessage;
		IoSymbol *typeString = IoState_symbolWithCString_(state, (char *)type);
		IoSeq *dataBuffer = IoSeq_newWithData_length_(state, (unsigned char *)data, dataLength);

		IoMessage_setCachedArg_toInt_(m, 0, x);
		IoMessage_setCachedArg_toInt_(m, 1, y);
		IoMessage_setCachedArg_to_(m, 2, typeString);
		IoMessage_setCachedArg_to_(m, 3, dataBuffer);

		if (DATA(proto)->eventTarget)
		{
			IoObject *r = IoGLUT_tryCallback(proto, m);
;
			if (r && ISNUMBER(r))
			{
				result = CNUMBER(r);
			}
		}
	}

	IoState_popRetainPool(state);
	return result;
}

void IoGlutDropFunc(
				int x,
				int y,
				const char *type,
				const unsigned char *data,
				int dataLength)
{
	IoState *state = IoObject_state(proto);
	IoState_pushRetainPool(state);

	{
		IoMessage *m = DATA(proto)->dropMessage;
		IoSymbol *typeString = IoState_symbolWithCString_(state, (char *)type);
		IoSeq *dataBuffer = IoSeq_newWithData_length_(state, (unsigned char *)data, dataLength);

		IoMessage_setCachedArg_toInt_(m, 0, x);
		IoMessage_setCachedArg_toInt_(m, 1, y);
		IoMessage_setCachedArg_to_(m, 2, typeString);
		IoMessage_setCachedArg_to_(m, 3, dataBuffer);

		IoGLUT_tryCallback(proto, m);
	}

	IoState_popRetainPool(state);
}

void IoGlutPasteFunc(
					 const char *type,
					 const unsigned char *data,
					 int dataLength)
{
	IoState *state = IoObject_state(proto);
	IoState_pushRetainPool(state);

	{
		IoMessage *m = DATA(proto)->pasteMessage;
		IoSymbol *typeString = IoState_symbolWithCString_(state, (char *)type);
		IoSeq *dataBuffer = IoSeq_newWithData_length_(state, (unsigned char *)data, dataLength);

		IoMessage_setCachedArg_to_(m, 0, typeString);
		IoMessage_setCachedArg_to_(m, 1, dataBuffer);

		IoGLUT_tryCallback(proto, m);
	}

	IoState_popRetainPool(IoObject_state(proto));
}

void IoGlutDeleteFunc(void)
{
	IoState *state = IoObject_state(proto);
	IoState_pushRetainPool(state);
	IoGLUT_tryCallback(proto, DATA(proto)->deleteMessage);
	IoState_popRetainPool(state);
}


// --- Extra Out --------------------------------------------------------------

void IoGLUT_setDragCallback_(IoGLUT *self, DragCallback *func)
{
	DATA(self)->dragCallback = func;
}

IoObject *IoGLUT_drag(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	int x = IoMessage_locals_intArgAt_(m, locals, 0);
	int y = IoMessage_locals_intArgAt_(m, locals, 1);
	IoSymbol *type = IoMessage_locals_symbolArgAt_(m, locals, 2);
	IoSeq *buffer = IoMessage_locals_mutableSeqArgAt_(m, locals, 3);

	if (DATA(self)->dragCallback)
	{
		(DATA(self)->dragCallback)(DATA(self)->callbackContext,
								   x,
								   y,
								   CSTRING(type),
								   IoSeq_rawBytes(buffer),
								   IoSeq_rawSize(buffer));
	}

	return IONIL(self);
}

void IoGLUT_setCopyCallback_(IoGLUT *self, CopyCallback *func)
{
	DATA(self)->copyCallback = func;
}

IoObject *IoGLUT_copy(IoGLUT *self, IoObject *locals, IoMessage *m)
{
	IoSymbol *type = IoMessage_locals_symbolArgAt_(m, locals, 0);
	IoSeq *buffer = IoMessage_locals_mutableSeqArgAt_(m, locals, 1);

	if (DATA(self)->copyCallback)
	{
		(DATA(self)->copyCallback)(DATA(self)->callbackContext,
								   CSTRING(type),
								   IoSeq_rawBytes(buffer),
								   IoSeq_rawSize(buffer));
	}

	return IONIL(self);
}

