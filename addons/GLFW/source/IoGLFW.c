//metadoc GLFW copyright Joel de Vahl, 2006
//metadoc GLFW license BSD revised
//metadoc GLFW category Graphics

#include "IoGLFW.h"

#include <GL/glfw.h>

#if defined(__APPLE_CC__)
#include <Carbon/Carbon.h>
#endif

#include "IoState.h"
#include "IoNumber.h"
#include "IoCFunction.h"
#include "IoList.h"

#define DATA(self) ((IoGLFWData *)IoObject_dataPointer(self))

static IoGLFW *proto = NULL;

// Callbacks
IoObject *IoGLFW_tryCallback(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	IoState *state = IOSTATE;
	IoObject *tryCoro = DATA(self)->coroutine;

	IoCoroutine_try(tryCoro, locals, locals, m);

	if(IoCoroutine_rawException(tryCoro) != state->ioNil)
	{
		IoState_exception_(state, tryCoro);
	}

	IoCoroutine_clearStack(tryCoro);
	return IoCoroutine_rawResult(tryCoro);
}

int IoGLFWCloseCallback(){
	IoState_pushRetainPool(IoObject_state(proto));

	IoNumber* res = IoGLFW_tryCallback(proto, DATA(proto)->closeTarget, DATA(proto)->closeMessage);

	IoState_popRetainPool(IoObject_state(proto));

	return IoNumber_asInt(res);
}

void IoGLFWSizeCallback(int width, int height){
	IoState_pushRetainPool(IoObject_state(proto));

	IoMessage_setCachedArg_toInt_(DATA(proto)->sizeMessage, 0, width);
	IoMessage_setCachedArg_toInt_(DATA(proto)->sizeMessage, 1, height);
	IoGLFW_tryCallback(proto, DATA(proto)->sizeTarget, DATA(proto)->sizeMessage);

	IoState_popRetainPool(IoObject_state(proto));

	return;
}

void IoGLFWRefreshCallback(){
	IoState_pushRetainPool(IoObject_state(proto));

	IoGLFW_tryCallback(proto, DATA(proto)->refreshTarget, DATA(proto)->refreshMessage);

	IoState_popRetainPool(IoObject_state(proto));

	return;
}

void IoGLFWKeyCallback(int key, int action){
	IoState_pushRetainPool(IoObject_state(proto));

	IoMessage_setCachedArg_toInt_(DATA(proto)->keyMessage, 0, key);
	IoMessage_setCachedArg_toInt_(DATA(proto)->keyMessage, 1, action);
	IoGLFW_tryCallback(proto, DATA(proto)->keyTarget, DATA(proto)->keyMessage);

	IoState_popRetainPool(IoObject_state(proto));

	return;
}

void IoGLFWCharCallback(int character, int action){
	IoState_pushRetainPool(IoObject_state(proto));

	IoMessage_setCachedArg_toInt_(DATA(proto)->charMessage, 0, character);
	IoMessage_setCachedArg_toInt_(DATA(proto)->charMessage, 1, action);
	IoGLFW_tryCallback(proto, DATA(proto)->charTarget, DATA(proto)->charMessage);

	IoState_popRetainPool(IoObject_state(proto));

	return;
}

void IoGLFWMouseButtonCallback(int button, int action){
	IoState_pushRetainPool(IoObject_state(proto));

	IoMessage_setCachedArg_toInt_(DATA(proto)->mousebuttonMessage, 0, button);
	IoMessage_setCachedArg_toInt_(DATA(proto)->mousebuttonMessage, 1, action);
	IoGLFW_tryCallback(proto, DATA(proto)->mousebuttonTarget, DATA(proto)->mousebuttonMessage);

	IoState_popRetainPool(IoObject_state(proto));

	return;
}

void IoGLFWMousePosCallback(int x, int y){
	IoState_pushRetainPool(IoObject_state(proto));

	IoMessage_setCachedArg_toInt_(DATA(proto)->mouseposMessage, 0, x);
	IoMessage_setCachedArg_toInt_(DATA(proto)->mouseposMessage, 1, y);
	IoGLFW_tryCallback(proto, DATA(proto)->mouseposTarget, DATA(proto)->mouseposMessage);

	IoState_popRetainPool(IoObject_state(proto));

	return;
}

void IoGLFWMouseWheelCallback(int pos){
	IoState_pushRetainPool(IoObject_state(proto));

	IoMessage_setCachedArg_toInt_(DATA(proto)->mousewheelMessage, 0, pos);
	IoGLFW_tryCallback(proto, DATA(proto)->mousewheelTarget, DATA(proto)->mousewheelMessage);

	IoState_popRetainPool(IoObject_state(proto));

	return;
}

// Initialization
IoTag *IoGLFW_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("GLFW");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoGLFW_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoGLFW_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoGLFW_mark);
	return tag;
}

#define GLFWMESSAGE(name) \
IoMessage_newWithName_label_(state, IOSYMBOL(name), IOSYMBOL("[GLFW]"))

#define GLFWMARK(name) if(DATA(self)->name) { IoObject_shouldMark(DATA(self)->name); }

IoGLFW *IoGLFW_proto(void *state)
{
	IoObject *self = IoObject_new(state);

	proto = self;
	IoObject_tag_(self, IoGLFW_newTag(state));
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoGLFWData)));

	DATA(self)->coroutine = IoCoroutine_new(state);

	IoState_registerProtoWithFunc_(state, self, IoGLFW_proto);

	IoGLFW_protoInit(self);
	return self;
}

IoGLFW *IoGLFW_new(void *state)
{
	return IoState_protoWithInitFunction_(state, IoGLFW_proto);
}

void IoGLFW_free(IoGLFW *self)
{
	free(IoObject_dataPointer(self));
}

void IoGLFW_mark(IoGLFW *self)
{
	GLFWMARK(coroutine);

	GLFWMARK(closeMessage);
	GLFWMARK(closeTarget);
	GLFWMARK(sizeMessage);
	GLFWMARK(sizeTarget);
	GLFWMARK(refreshMessage);
	GLFWMARK(refreshTarget);
	GLFWMARK(keyMessage);
	GLFWMARK(keyTarget);
	GLFWMARK(charMessage);
	GLFWMARK(charTarget);
	GLFWMARK(mousebuttonMessage);
	GLFWMARK(mousebuttonTarget);
	GLFWMARK(mouseposMessage);
	GLFWMARK(mouseposTarget);
	GLFWMARK(mousewheelMessage);
	GLFWMARK(mousewheelTarget);
}

IoObject *IoGLFW_rawClone(IoGLFW *self)
{
	return IoState_protoWithInitFunction_(IOSTATE, IoGLFW_proto);
}


// GLFW initialization, termination and version querying
IoObject *IoGLFW_glfwInit(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	glfwInit();
	return self;
}

IoObject *IoGLFW_glfwTerminate(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	glfwTerminate();
	return self;
}

IoObject *IoGLFW_glfwGetVersion(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int major;
	int minor;
	int rev;

	glfwGetVersion(&major, &minor, &rev);

	IoList *list = IOREF(IoList_new(IOSTATE));
	IoList_rawAppend_(list, IONUMBER(major));
	IoList_rawAppend_(list, IONUMBER(minor));
	IoList_rawAppend_(list, IONUMBER(rev));

	return list;
}

// Window handling
IoObject *IoGLFW_glfwOpenWindow(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int width = IoMessage_locals_intArgAt_(m, locals, 0);
	int height = IoMessage_locals_intArgAt_(m, locals, 1);
	int redbits = IoMessage_locals_intArgAt_(m, locals, 2);
	int greenbits = IoMessage_locals_intArgAt_(m, locals, 3);
	int bluebits = IoMessage_locals_intArgAt_(m, locals, 4);
	int alphabits = IoMessage_locals_intArgAt_(m, locals, 5);
	int depthbits = IoMessage_locals_intArgAt_(m, locals, 6);
	int stencilbits = IoMessage_locals_intArgAt_(m, locals, 7);
	int mode = IoMessage_locals_intArgAt_(m, locals, 8);

	glfwOpenWindow(width, height, redbits, greenbits, bluebits, alphabits, depthbits, stencilbits, mode);

	return self;
}

IoObject *IoGLFW_glfwOpenWindowHint(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int target = IoMessage_locals_intArgAt_(m, locals, 0);
	int hint = IoMessage_locals_intArgAt_(m, locals, 1);

	glfwOpenWindowHint(target, hint);

	return self;
}

IoObject *IoGLFW_glfwCloseWindow(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	glfwCloseWindow();

	return self;
}

IoObject *IoGLFW_glfwSetWindowTitle(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	IoSymbol *s = IoMessage_locals_symbolArgAt_(m, locals, 0);

	glfwSetWindowTitle(CSTRING(s));

	return self;
}

IoObject *IoGLFW_glfwGetWindowSize(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int width;
	int height;
	vec2f v;
	glfwGetWindowSize(&width, &height);

	v.x = width;
	v.y = height;

	return IoSeq_newVec2f(IOSTATE, v);
}

IoObject *IoGLFW_glfwSetWindowSize(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int width = IoMessage_locals_intArgAt_(m, locals, 0);
	int height = IoMessage_locals_intArgAt_(m, locals, 1);

	glfwSetWindowSize(width, height);

	return self;
}

IoObject *IoGLFW_glfwSetWindowPos(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int x = IoMessage_locals_intArgAt_(m, locals, 0);
	int y = IoMessage_locals_intArgAt_(m, locals, 1);

	glfwSetWindowPos(x, y);

	return self;
}

IoObject *IoGLFW_glfwIconifyWindow(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	glfwIconifyWindow();

	return self;
}

IoObject *IoGLFW_glfwRestoreWindow(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	glfwRestoreWindow();

	return self;
}

IoObject *IoGLFW_glfwSwapBuffers(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	glfwSwapBuffers();
	return self;
}

IoObject *IoGLFW_glfwSwapInterval(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int interval = IoMessage_locals_intArgAt_(m, locals, 0);

	glfwSwapInterval(interval);

	return self;
}

IoObject *IoGLFW_glfwGetWindowParam(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int param = IoMessage_locals_intArgAt_(m, locals, 0);
	return IONUMBER(glfwGetWindowParam(param));
}

IoObject *IoGLFW_glfwSetWindowSizeCallback(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	List* args = IoMessage_rawArgList(m);

	DATA(proto)->sizeMessage = List_at_(args, 0);
	DATA(proto)->sizeTarget = self;

	glfwSetWindowSizeCallback(IoGLFWSizeCallback);

	return self;
}

IoObject *IoGLFW_glfwSetWindowCloseCallback(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	List* args = IoMessage_rawArgList(m);

	DATA(proto)->closeMessage = List_at_(args, 0);
	DATA(proto)->closeTarget = self;

	glfwSetWindowCloseCallback(IoGLFWCloseCallback);

	return self;
}

IoObject *IoGLFW_glfwSetWindowRefreshCallback(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	List* args = IoMessage_rawArgList(m);

	DATA(proto)->sizeMessage = List_at_(args, 0);
	DATA(proto)->sizeTarget = self;

	glfwSetWindowRefreshCallback(IoGLFWRefreshCallback);

	return self;
}

// Video mode functions
IoObject *IoGLFW_glfwGetVideoModes(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	IoList* list = IOREF(IoList_new(IOSTATE));
	IoObject* mode;
	GLFWvidmode modes[32];
	int num = glfwGetVideoModes(modes, 32);
	int i = 0;

	while(i < num){
		mode = IOREF(IoObject_new(IOSTATE));
		IoObject_inlineSetSlot_to_(mode, IoState_symbolWithCString_(IOSTATE, "width"), IONUMBER(modes[i].Width));
		IoObject_inlineSetSlot_to_(mode, IoState_symbolWithCString_(IOSTATE, "height"), IONUMBER(modes[i].Height));
		IoObject_inlineSetSlot_to_(mode, IoState_symbolWithCString_(IOSTATE, "redBits"), IONUMBER(modes[i].RedBits));
		IoObject_inlineSetSlot_to_(mode, IoState_symbolWithCString_(IOSTATE, "greenBits"), IONUMBER(modes[i].GreenBits));
		IoObject_inlineSetSlot_to_(mode, IoState_symbolWithCString_(IOSTATE, "blueBits"), IONUMBER(modes[i].BlueBits));
		IoList_rawAppend_(list, mode);
		i++;
	}

	return list;
}

IoObject *IoGLFW_glfwGetDesktopMode(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	IoObject* ret = IOREF(IoObject_new(IOSTATE));
	GLFWvidmode mode;
	glfwGetDesktopMode(&mode);

	IoObject_inlineSetSlot_to_(ret, IoState_symbolWithCString_(IOSTATE, "width"), IONUMBER(mode.Width));
	IoObject_inlineSetSlot_to_(ret, IoState_symbolWithCString_(IOSTATE, "height"), IONUMBER(mode.Height));
	IoObject_inlineSetSlot_to_(ret, IoState_symbolWithCString_(IOSTATE, "redBits"), IONUMBER(mode.RedBits));
	IoObject_inlineSetSlot_to_(ret, IoState_symbolWithCString_(IOSTATE, "greenBits"), IONUMBER(mode.GreenBits));
	IoObject_inlineSetSlot_to_(ret, IoState_symbolWithCString_(IOSTATE, "blueBits"), IONUMBER(mode.BlueBits));

	return ret;
}

// Input handling
IoObject *IoGLFW_glfwPollEvents(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	glfwPollEvents();

	return self;
}

IoObject *IoGLFW_glfwWaitEvents(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	glfwWaitEvents();

	return self;
}

IoObject *IoGLFW_glfwGetKey(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int key = IoMessage_locals_intArgAt_(m, locals, 0);

	return IONUMBER(glfwGetKey(key));
}

IoObject *IoGLFW_glfwGetMouseButton(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int button = IoMessage_locals_intArgAt_(m, locals, 0);

	return IONUMBER(glfwGetMouseButton(button));
}

IoObject *IoGLFW_glfwGetMousePos(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int xpos;
	int ypos;
	vec2f v;

	glfwGetMousePos(&xpos, &ypos);

	v.x = xpos;
	v.y = ypos;

	return IoSeq_newVec2f(IOSTATE, v);
}

IoObject *IoGLFW_glfwSetMousePos(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int xpos = IoMessage_locals_intArgAt_(m, locals, 0);
	int ypos = IoMessage_locals_intArgAt_(m, locals, 1);

	glfwSetMousePos(xpos, ypos);

	return self;
}

IoObject *IoGLFW_glfwGetMouseWheel(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(glfwGetMouseWheel());
}

IoObject *IoGLFW_glfwSetMouseWheel(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int pos = IoMessage_locals_intArgAt_(m, locals, 0);

	glfwSetMouseWheel(pos);

	return self;
}

IoObject *IoGLFW_glfwSetKeyCallback(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	List* args = IoMessage_rawArgList(m);

	DATA(proto)->keyMessage = List_at_(args, 0);
	DATA(proto)->keyTarget = self;

	glfwSetKeyCallback(IoGLFWKeyCallback);

	return self;
}

IoObject *IoGLFW_glfwSetCharCallback(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	List* args = IoMessage_rawArgList(m);

	DATA(proto)->charMessage = List_at_(args, 0);
	DATA(proto)->charTarget = self;

	glfwSetCharCallback(IoGLFWCharCallback);

	return self;
}

IoObject *IoGLFW_glfwSetMouseButtonCallback(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	List* args = IoMessage_rawArgList(m);

	DATA(proto)->mousebuttonMessage = List_at_(args, 0);
	DATA(proto)->mousebuttonTarget = self;

	glfwSetMouseButtonCallback(IoGLFWMouseButtonCallback);

	return self;
}

IoObject *IoGLFW_glfwSetMousePosCallback(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	List* args = IoMessage_rawArgList(m);

	DATA(proto)->mouseposMessage = List_at_(args, 0);
	DATA(proto)->mouseposTarget = self;

	glfwSetMousePosCallback(IoGLFWMousePosCallback);

	return self;
}

IoObject *IoGLFW_glfwSetMouseWheelCallback(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	List* args = IoMessage_rawArgList(m);

	DATA(proto)->mousewheelMessage = List_at_(args, 0);
	DATA(proto)->mousewheelTarget = self;

	glfwSetMouseWheelCallback(IoGLFWMouseWheelCallback);

	return self;
}


// Joystick input
IoObject *IoGLFW_glfwGetJoystickParam(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int joy = IoMessage_locals_intArgAt_(m, locals, 0);
	int param = IoMessage_locals_intArgAt_(m, locals, 1);

	glfwGetJoystickParam(joy, param);

	return self;
}

IoObject *IoGLFW_glfwGetJoystickPos(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int joy = IoMessage_locals_intArgAt_(m, locals, 0);
	int numaxes = IoMessage_locals_intArgAt_(m, locals, 1);
	float* pos = malloc(sizeof(float) * numaxes);

	glfwGetJoystickPos(joy, pos, numaxes);

	IoList *list = IOREF(IoList_new(IOSTATE));
	int i = 0;
	while(i < numaxes)
		IoList_rawAppend_(list, IONUMBER(pos[i++]));

	free(pos);
	return list;
}

IoObject *IoGLFW_glfwGetJoystickButtons(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int joy = IoMessage_locals_intArgAt_(m, locals, 0);
	int numbuttons = IoMessage_locals_intArgAt_(m, locals, 1);
	unsigned char* buttons = malloc(sizeof(unsigned char) * numbuttons);

	glfwGetJoystickButtons(joy, buttons, numbuttons);

	IoList *list = IOREF(IoList_new(IOSTATE));
	int i = 0;
	while(i < numbuttons)
		IoList_rawAppend_(list, IONUMBER(buttons[i++]));

	free(buttons);
	return list;
}

// Time
IoObject *IoGLFW_glfwGetTime(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(glfwGetTime());
}

IoObject *IoGLFW_glfwSetTime(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	double time = IoMessage_locals_doubleArgAt_(m, locals, 0);

	glfwSetTime(time);

	return self;
}

IoObject *IoGLFW_glfwSleep(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	double time = IoMessage_locals_doubleArgAt_(m, locals, 0);

	glfwSleep(time);

	return self;
}

// Extension support
/* Disabled... use GLEW for extension support
IoObject *IoGLFW_glfwExtensionSupported(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
IoObject *IoGLFW_glfwGetProcAddress(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
IoObject *IoGLFW_glfwGetGLVersion(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
*/

// Threading support
/* Disabled... use Thread instead
IoObject *IoGLFW_glfwCreateThread(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
IoObject *IoGLFW_glfwDestroyThread(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
IoObject *IoGLFW_glfwWaitThread(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
IoObject *IoGLFW_glfwGetThreadID(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
IoObject *IoGLFW_glfwCreateMutex(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
IoObject *IoGLFW_glfwDestroyMutex(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
IoObject *IoGLFW_glfwLockMutex(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
IoObject *IoGLFW_glfwUnlockMutex(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
IoObject *IoGLFW_glfwCreateCond(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
IoObject *IoGLFW_glfwDestroyCond(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
IoObject *IoGLFW_glfwWaitCond(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
IoObject *IoGLFW_glfwSignalCond(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
IoObject *IoGLFW_glfwBroadcastCond(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
IoObject *IoGLFW_glfwGetNumberOfProcessors(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
*/

// Enable/disable functions
IoObject *IoGLFW_glfwEnable(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int token = IoMessage_locals_intArgAt_(m, locals, 0);
	glfwEnable(token);
	return self;
}

IoObject *IoGLFW_glfwDisable(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	int token = IoMessage_locals_intArgAt_(m, locals, 0);
	glfwDisable(token);
	return self;
}

// Image/texture I/O support
/* Disabled... use Image instead
IoObject *IoGLFW_glfwReadImage(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
IoObject *IoGLFW_glfwFreeImage(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
IoObject *IoGLFW_glfwLoadTexture(IoGLFW *self, IoObject *locals, IoMessage *m)
{
	return self;
}
*/

#define IOCFUNCTION_GLFW(func) IOCFUNCTION(func, NULL)

#include "IoGLFWconst.h"
#include "IoGLFWfunc.h"

void IoGLFW_protoInit(IoGLFW *self)
{
#if defined(__APPLE_CC__)
	/* HACK */
	extern int32_t CPSEnableForegroundOperation(ProcessSerialNumber * PSN);
	ProcessSerialNumber psn;
	GetCurrentProcess(&psn);
	CPSEnableForegroundOperation(&psn);
	SetFrontProcess(&psn);
#endif

	IoObject_setSlot_to_(self,IOSYMBOL("clone"), IOCFUNCTION_GLFW(IoObject_self));

	/* GLFW Constants */
	{
		t_ioGLFW_constTable *curpos = ioGLFW_constTable;
		while(curpos->name)
		{
			IoObject_setSlot_to_(self,
								 IOSYMBOL(curpos->name),
								 IONUMBER(curpos->value));
			curpos++;
		}
	}

	/* GLFW Functions */
	{
		t_ioGLFW_funcTable *curpos=ioGLFW_funcTable;

		while(curpos->name)
		{
			IoCFunction *f = IoCFunction_newWithFunctionPointer_tag_name_(IOSTATE, curpos->func, NULL, curpos->name);
			IoObject_setSlot_to_(self, IOSYMBOL(curpos->name), f);
			curpos++;
		}
	}
}


