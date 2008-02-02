//metadoc GLUT copyright Steve Dekorte 2002
//metadoc GLUT license BSD revised

#ifndef IOGLUT_DEFINED
#define IOGLUT_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoCoroutine.h"

#define ISGLUT(v) (IoObject_tag(v)->cloneFunc == (IoTagCloneFunc *)IoGLUT_rawClone)

#include "GLIncludes.h"

typedef IoObject IoGLUT;

typedef int (DragCallback)(void *, int, int, const char *, const unsigned char *, long);
typedef int (CopyCallback)(void *, const char *, const unsigned char *, long);

typedef struct
{
	IoCoroutine *coroutine;
	IoObject *eventTarget;
	IoMessage *displayMessage;
	IoMessage *entryMessage;
	IoMessage *joystickMessage;
	IoMessage *keyboardMessage;
	IoMessage *keyboardUpMessage;
	IoMessage *menuMessage;
	IoMessage *motionMessage;
	IoMessage *mouseMessage;
	IoMessage *passiveMotionMessage;
	IoMessage *reshapeMessage;
	IoMessage *specialMessage;
	IoMessage *specialUpMessage;
	IoMessage *timerMessage;

	// extra in
	IoMessage *acceptsDropMessage;
	IoMessage *dropMessage;
	IoMessage *pasteMessage;
	IoMessage *deleteMessage;

	// extra out
	IoMessage *dragMessage;
	IoMessage *copyMessage;

	void *callbackContext;
	DragCallback *dragCallback;
	CopyCallback *copyCallback;

	long nanoSleepPeriod;

	// joystick calibration

	IoSeq *j;
	UArray *lastJ;
	unsigned int lastJoystickButton;
} IoGLUTData;

IoObject *IoGLUT_rawClone(IoGLUT *self);
IoGLUT *IoGLUT_proto(void *state);
IoGLUT *IoGLUT_new(void *state);
IoObject *IoGLUT_perform(IoGLUT *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUT_forward(IoGLUT *self, IoObject *locals, IoMessage *m);
void IoGLUT_free(IoGLUT *self);
void IoGLUT_mark(IoGLUT *self);
IoObject *IoGLUT_glutEventTarget_(IoGLUT *self, IoObject *locals, IoMessage *m);
void IoGLUT_protoInit(IoGLUT *self);

// --- events ---------------------------------------------------------------

IoObject *IoGLUT_tryCallback(IoGLUT *self, IoMessage *m);

void IoGlutKeyboardFunc(unsigned char key, int xv, int yv);
void IoGlutSpecialFunc(int key, int xv, int yv);
void IoGlutKeyboardUpFunc(unsigned char key, int xv, int yv);
void IoGlutSpecialUpFunc(int key, int xv, int yv);

void IoGlutEntryFunc(int state);
void IoGlutMotionFunc(int xv, int yv);
void IoGlutPassiveMotionFunc(int xv, int yv);
void IoGlutMouseFunc(int button, int state, int xv, int yv);

void IoGlutDisplayFunc(void);
void IoGlutReshapeFunc(int width, int height);
void IoGlutTimerFunc(int vv);

// Menus

IoObject *IoGLUT_glutCreateMenu(IoGLUT *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUT_glutSetMenu(IoGLUT *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUT_glutGetMenu(IoGLUT *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUT_glutDestroyMenu(IoGLUT *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUT_glutAddMenuEntry(IoGLUT *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUT_glutAddSubMenu(IoGLUT *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUT_glutChangeToMenuEntry(IoGLUT *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUT_glutChangeToSubMenu(IoGLUT *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUT_glutRemoveMenuItem(IoGLUT *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUT_glutAttachMenu(IoGLUT *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUT_glutDetachMenu(IoGLUT *self, IoObject *locals, IoMessage *m);

// Joystick

//IoObject *IoGLUT_glutInitJoystick(IoGLUT *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUT_glutJoystickFunc(IoGLUT *self, IoObject *locals, IoMessage *m);


// --- extra in --------------------------------------------------------------

int IoGlutAcceptsDropFunc(
	int x,
	int y,
	const char *type,
	const unsigned char *data,
	int dataLength);

void IoGlutDropFunc(
	int x,
	int y,
	const char *type,
	const unsigned char *data,
	int dataLength);

void IoGlutPasteFunc(
	const char *type,
	const unsigned char *data,
	int dataLength);

void IoGlutDeleteFunc(void);

// --- extra out --------------------------------------------------------------

void IoGLUT_setDragCallback_(IoGLUT *self, DragCallback *func);
IoObject *IoGLUT_drag(IoGLUT *self, IoObject *locals, IoMessage *m);

void IoGLUT_setCopyCallback_(IoGLUT *self, CopyCallback *func);
IoObject *IoGLUT_copy(IoGLUT *self, IoObject *locals, IoMessage *m);

#endif
