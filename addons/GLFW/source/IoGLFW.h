/*
docCopyright("Joel de Vahl", 2006)
docLicense("BSD revised")
*/

#ifndef IOGLFW_DEFINED
#define IOGLFW_DEFINED 1

#include "IoObject.h"

#define ISGLFW(v) IoObject_tag(((IoObject *)v)->name == (IoTagCloneFunc *)IoGLFW_rawClone)

#include "GLIncludes.h"

typedef IoObject IoGLFW;

typedef struct
{
	IoCoroutine *coroutine;

	IoMessage *closeMessage;
	IoObject *closeTarget;
	IoMessage *sizeMessage;
	IoObject *sizeTarget;
	IoMessage *refreshMessage;
	IoObject *refreshTarget;
	IoMessage *keyMessage;
	IoObject *keyTarget;
	IoMessage *charMessage;
	IoObject *charTarget;
	IoMessage *mousebuttonMessage;
	IoObject *mousebuttonTarget;
	IoMessage *mouseposMessage;
	IoObject *mouseposTarget;
	IoMessage *mousewheelMessage;
	IoObject *mousewheelTarget;
} IoGLFWData;

IoObject *IoGLFW_rawClone(IoGLFW *self);
IoGLFW *IoGLFW_proto(void *state);
IoGLFW *IoGLFW_new(void *state);
IoObject *IoGLFW_perform(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_forward(IoGLFW *self, IoObject *locals, IoMessage *m);
void IoGLFW_free(IoGLFW *self);
void IoGLFW_mark(IoGLFW *self);
void IoGLFW_protoInit(IoGLFW *self);

// GLFW initialization, termination and version querying
IoObject *IoGLFW_glfwInit(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwTerminate(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwGetVersion(IoGLFW *self, IoObject *locals, IoMessage *m);

// Window handling
IoObject *IoGLFW_glfwOpenWindow(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwOpenWindowHint(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwCloseWindow(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSetWindowTitle(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwGetWindowSize(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSetWindowSize(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSetWindowPos(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwIconifyWindow(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwRestoreWindow(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSwapBuffers(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSwapInterval(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwGetWindowParam(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSetWindowSizeCallback(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSetWindowCloseCallback(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSetWindowRefreshCallback(IoGLFW *self, IoObject *locals, IoMessage *m);

// Video mode functions
IoObject *IoGLFW_glfwGetVideoModes(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwGetDesktopMode(IoGLFW *self, IoObject *locals, IoMessage *m);

// Input handling
IoObject *IoGLFW_glfwPollEvents(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwWaitEvents(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwGetKey(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwGetMouseButton(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwGetMousePos(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSetMousePos(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwGetMouseWheel(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSetMouseWheel(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSetKeyCallback(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSetCharCallback(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSetMouseButtonCallback(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSetMousePosCallback(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSetMouseWheelCallback(IoGLFW *self, IoObject *locals, IoMessage *m);

// Joystick input
IoObject *IoGLFW_glfwGetJoystickParam(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwGetJoystickPos(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwGetJoystickButtons(IoGLFW *self, IoObject *locals, IoMessage *m);

// Time
IoObject *IoGLFW_glfwGetTime(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSetTime(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSleep(IoGLFW *self, IoObject *locals, IoMessage *m);

// Extension support
/* Disabled... use GLFW instead
IoObject *IoGLFW_glfwExtensionSupported(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwGetProcAddress(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwGetGLVersion(IoGLFW *self, IoObject *locals, IoMessage *m);
*/

// Threading support
/* Disabled... use Thread instead
IoObject *IoGLFW_glfwCreateThread(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwDestroyThread(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwWaitThread(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwGetThreadID(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwCreateMutex(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwDestroyMutex(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwLockMutex(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwUnlockMutex(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwCreateCond(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwDestroyCond(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwWaitCond(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwSignalCond(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwBroadcastCond(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwGetNumberOfProcessors(IoGLFW *self, IoObject *locals, IoMessage *m);
*/

// Enable/disable functions
IoObject *IoGLFW_glfwEnable(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwDisable(IoGLFW *self, IoObject *locals, IoMessage *m);

// Image/texture I/O support
/* Disabled... use Image instead
IoObject *IoGLFW_glfwReadImage(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwFreeImage(IoGLFW *self, IoObject *locals, IoMessage *m);
IoObject *IoGLFW_glfwLoadTexture(IoGLFW *self, IoObject *locals, IoMessage *m);
*/

#endif
