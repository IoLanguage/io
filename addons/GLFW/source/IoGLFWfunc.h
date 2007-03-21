/*
 *
 * GLUT Functions
 *
 */

typedef struct t_ioGLUT_funcTable {
	char *name;
	void *func;
} t_ioGLFW_funcTable;


t_ioGLFW_funcTable ioGLFW_funcTable[] = {

// GLFW initialization, termination and version querying
	{"glfwInit", IoGLFW_glfwInit},
	{"glfwTerminate", IoGLFW_glfwTerminate},
	{"glfwGetVersion", IoGLFW_glfwGetVersion},

// Window handling
	{"glfwOpenWindow", IoGLFW_glfwOpenWindow},
	{"glfwOpenWindowHint", IoGLFW_glfwOpenWindowHint},
	{"glfwCloseWindow", IoGLFW_glfwCloseWindow},
	{"glfwSetWindowTitle", IoGLFW_glfwSetWindowTitle},
	{"glfwGetWindowSize", IoGLFW_glfwGetWindowSize},
	{"glfwSetWindowSize", IoGLFW_glfwSetWindowSize},
	{"glfwSetWindowPos", IoGLFW_glfwSetWindowPos},
	{"glfwIconifyWindow", IoGLFW_glfwIconifyWindow},
	{"glfwRestoreWindow", IoGLFW_glfwRestoreWindow},
	{"glfwSwapBuffers", IoGLFW_glfwSwapBuffers},
	{"glfwSwapInterval", IoGLFW_glfwSwapInterval},
	{"glfwGetWindowParam", IoGLFW_glfwGetWindowParam},
	{"glfwSetWindowSizeCallback", IoGLFW_glfwSetWindowSizeCallback},
	{"glfwSetWindowCloseCallback", IoGLFW_glfwSetWindowCloseCallback},
	{"glfwSetWindowRefreshCallback", IoGLFW_glfwSetWindowRefreshCallback},

// Video mode functions
	{"glfwGetVideoModes", IoGLFW_glfwGetVideoModes},
	{"glfwGetDesktopMode", IoGLFW_glfwGetDesktopMode},

// Input handling
	{"glfwPollEvents", IoGLFW_glfwPollEvents},
	{"glfwWaitEvents", IoGLFW_glfwWaitEvents},
	{"glfwGetKey", IoGLFW_glfwGetKey},
	{"glfwGetMouseButton", IoGLFW_glfwGetMouseButton},
	{"glfwGetMousePos", IoGLFW_glfwGetMousePos},
	{"glfwSetMousePos", IoGLFW_glfwSetMousePos},
	{"glfwGetMouseWheel", IoGLFW_glfwGetMouseWheel},
	{"glfwSetMouseWheel", IoGLFW_glfwSetMouseWheel},
	{"glfwSetKeyCallback", IoGLFW_glfwSetKeyCallback},
	{"glfwSetCharCallback", IoGLFW_glfwSetCharCallback},
	{"glfwSetMouseButtonCallback", IoGLFW_glfwSetMouseButtonCallback},
	{"glfwSetMousePosCallback", IoGLFW_glfwSetMousePosCallback},
	{"glfwSetMouseWheelCallback", IoGLFW_glfwSetMouseWheelCallback},

// Joystick input
	{"glfwGetJoystickParam", IoGLFW_glfwGetJoystickParam},
	{"glfwGetJoystickPos", IoGLFW_glfwGetJoystickPos},
	{"glfwGetJoystickButtons", IoGLFW_glfwGetJoystickButtons},

// Time
	{"glfwGetTime", IoGLFW_glfwGetTime},
	{"glfwSetTime", IoGLFW_glfwSetTime},
	{"glfwSleep", IoGLFW_glfwSleep},

// Extension support
/*
	{"glfwExtensionSupported", IoGLFW_glfwExtensionSupported},
	{"glfwGetProcAddress", IoGLFW_glfwGetProcAddress},
	{"glfwGetGLVersion", IoGLFW_glfwGetGLVersion},
*/

// Threading support
/*
	{"glfwCreateThread", IoGLFW_glfwCreateThread},
	{"glfwDestroyThread", IoGLFW_glfwDestroyThread},
	{"glfwWaitThread", IoGLFW_glfwWaitThread},
	{"glfwGetThreadID", IoGLFW_glfwGetThreadID},
	{"glfwCreateMutex", IoGLFW_glfwCreateMutex},
	{"glfwDestroyMutex", IoGLFW_glfwDestroyMutex},
	{"glfwLockMutex", IoGLFW_glfwLockMutex},
	{"glfwUnlockMutex", IoGLFW_glfwUnlockMutex},
	{"glfwCreateCond", IoGLFW_glfwCreateCond},
	{"glfwDestroyCond", IoGLFW_glfwDestroyCond},
	{"glfwWaitCond", IoGLFW_glfwWaitCond},
	{"glfwSignalCond", IoGLFW_glfwSignalCond},
	{"glfwBroadcastCond", IoGLFW_glfwBroadcastCond},
	{"glfwGetNumberOfProcessors", IoGLFW_glfwGetNumberOfProcessors},
*/

// Enable/disable functions
	{"glfwEnable", IoGLFW_glfwEnable},
	{"glfwDisable", IoGLFW_glfwDisable},

// Image/texture I/O support
/*
	{"glfwReadImage", IoGLFW_glfwReadImage},
	{"glfwFreeImage", IoGLFW_glfwFreeImage},
	{"glfwLoadTexture", IoGLFW_glfwLoadTexture},
*/

	/* The end */
	{NULL,NULL}
};
