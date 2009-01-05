/*metadoc System copyright Steve Dekorte, 2002
//metadoc System license BSD revised
/*metadoc System description
Contains methods related to the IoVM.
*/
//metadoc System category Core

#include "IoSystem.h"
#include "IoNumber.h"
#include "IoMessage_parser.h"

#if defined(linux)
#include <unistd.h>
#endif

#if defined(unix) || defined(__APPLE__) || defined(__NetBSD__)
#include <sys/utsname.h>
#ifdef __NetBSD__
# include <sys/param.h>
#endif
#ifndef __CYGWIN__
# include <sys/sysctl.h>
#endif
#endif

#ifdef WIN32
#include <windows.h>

static void setenv(const char *varName, const char* value, int force)
{
	const char *safeValue;
	char *buf;

	if (!varName)
	{
		return;
	}

	if (!value)
	{
		safeValue = "";
	}
	else
	{
		safeValue = value;
	}

	// buffer for var and value plus '=' and the \0
	buf = (char*)io_calloc(1, strlen(varName) + strlen(safeValue) + 2);

	if (!buf)
	{
		return;
	}

	strcpy(buf, varName);
	strcat(buf, "=");
	strcat(buf, safeValue);

	_putenv(buf);
	io_free(buf);
}

//#define setenv(k, v, o) SetEnvironmentVariable((k), (v))
#endif

#if defined(__CYGWIN__) || defined(_WIN32)
#include <windows.h>
#endif

IoObject *IoSystem_proto(void *state)
{
	IoMethodTable methodTable[] = {
#if defined(_WIN32)
	{"shellExecute", IoObject_shellExecute},
#endif
	{"errorNumber", IoObject_errorNumberDescription},
	{"exit", IoObject_exit},
	{"getEnvironmentVariable", IoObject_getEnvironmentVariable},
	{"setEnvironmentVariable", IoObject_setEnvironmentVariable},
	{"system", IoObject_system},
	//{"memorySizeOfState", IoObject_memorySizeOfState},
	//{"compactState", IoObject_compactState},
	{"platform", IoObject_platform},
	{"platformVersion", IoObject_platformVersion},
	{"sleep", IoObject_sleep},
	{"activeCpus", IoObject_activeCpus},
	{"recycledObjectCount", IoObject_recycledObjectCount},
	{"maxRecycledObjects", IoObject_maxRecycledObjects},
	{"setMaxRecycledObjects", IoObject_setMaxRecycledObjects},
	{"symbols", IoObject_symbols},
	{"setLobby", IoObject_setLobby},
	{"thisProcessPid", IoObject_thisProcessPid},
	{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_addMethodTable_(self, methodTable);

	/*doc System version
	Returns the Io version number.
	*/
	IoObject_setSlot_to_(self, IOSYMBOL("version"), IOSYMBOL(IO_VERSION_NUMBER));

	//IoObject_setSlot_to_(self, IOSYMBOL("distribution"), IOSYMBOL("Io"));
	IoObject_setSlot_to_(self, IOSYMBOL("type"), IOSYMBOL("System"));

	#ifndef INSTALL_PREFIX
	#define INSTALL_PREFIX "/usr/local"
	#endif


	/*doc System installPrefix
	Returns the root path where io was install. The default is /usr/local.
	*/
	
	IoObject_setSlot_to_(self, IOSYMBOL("installPrefix"), IOSYMBOL(INSTALL_PREFIX));

	return self;
}

/*doc System args
	Returns the list of command line argument strings the program was run with.")
*/
	
/*
IoObject *IoObject_errorNumber(IoObject *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(errno);
}
*/

#include <stdio.h>
#include <errno.h>

#if defined(_WIN32)
#include <shellapi.h>
IoObject *IoObject_shellExecute(IoObject *self, IoObject *locals, IoMessage *m)
{
	LPCTSTR operation;
	LPCTSTR file;
	LPCTSTR parameters;
	LPCTSTR directory;
	int displayFlag;
	int result;
	
	operation = CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 0));
	file = CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 1));
	parameters = IoMessage_argCount(m) > 2 ? CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 2)) : NULL;
	directory = IoMessage_argCount(m) > 3 ? CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 3)) : NULL;
	displayFlag = IoMessage_argCount(m) > 4 ? IoMessage_locals_intArgAt_(m, locals, 4) : SW_SHOWNORMAL;
	
	result = (int)ShellExecute(NULL, operation, file, parameters, directory, displayFlag);
	
	if(result > 32)
	{
		return self;
	}
	else
	{
		return (IoObject *)IoError_newWithMessageFormat_(IOSTATE, "ShellExecute Error %i", result);
	}
}
#endif

IoObject *IoObject_errorNumberDescription(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc System errorNumber
	Returns the C errno string.
	*/
	return errno ? IOSYMBOL(strerror(errno)) : IONIL(self);
}

IoObject *IoObject_exit(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc System exit(optionalReturnCodeNumber)
	Shutdown the IoState (io_free all objects) and return
control to the calling program (if any). 
*/

	int returnCode = 0;

	if (IoMessage_argCount(m))
	{
		returnCode = IoMessage_locals_intArgAt_(m, locals, 0);
	}

	IoState_exit(IOSTATE, returnCode);
	return self;
}

IoObject *IoObject_getEnvironmentVariable(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc System getEnvironmentVariable(nameString)
	Returns a string with the value of the environment 
	variable whose name is specified by nameString.
	*/

	IoSymbol *key = IoMessage_locals_symbolArgAt_(m, locals, 0);
	char *s = getenv(CSTRING(key));

	if (!s)
	{
		return ((IoState *)IOSTATE)->ioNil;
	}

	return IoState_symbolWithCString_(IOSTATE, s);
}

IoObject *IoObject_system(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc System system(aString)
	Makes a system call and returns a Number for the return value.
	*/

	IoSymbol *s = IoMessage_locals_symbolArgAt_(m, locals, 0);
	int result = system(CSTRING(s))/ 256;
	//printf("system result = %i\n", result);
	return IONUMBER(result);
}

IoObject *IoObject_memorySizeOfState(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Object memorySizeOfState
	Returns the number of bytes in the IoState
	(this may not include memory allocated by C libraries).
	*/

	return IONUMBER(0);
	//return IONUMBER(IoState_memorySize(IOSTATE));
}

IoObject *IoObject_compactState(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Object compactState
	Attempt to compact the memory of the IoState if possible.
	*/

	//IoState_compact(IOSTATE);
	return self;
}

IoObject *IoObject_setEnvironmentVariable(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc System setEnvironmentVariable(keyString, valueString)
	Sets the environment variable keyString to the value valueString.
*/

	// setenv() takes different args in different implementations
	IoSymbol *key = IoMessage_locals_symbolArgAt_(m, locals, 0);
	IoSymbol *value = IoMessage_locals_symbolArgAt_(m, locals, 1);
	setenv(CSTRING(key), CSTRING(value), 1);
	return self;
}

IoObject *IoObject_platform(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc System platform
	Returns a string description of the platform.
	*/

	char *platform = "Unknown";

#if defined(__CYGWIN__)

	platform = "cygwin";

#elif defined(__MINGW32__)

	platform = "mingw";

#elif defined(_WIN32)

	OSVERSIONINFO os;

	os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	GetVersionEx(&os);

	switch(os.dwPlatformId)
	{
		case VER_PLATFORM_WIN32_WINDOWS:
			switch(os.dwMinorVersion)
			{
				case 0:
					platform = "Windows 95";
					break;
				case 10:
					platform = "Windows 98";
					break;
				case 90:
					platform = "Windows ME";
					break;
				default:
					platform = "Windows 9X";
					break;
			}
			break;

		case VER_PLATFORM_WIN32_NT:
			if (os.dwMajorVersion == 3 || os.dwMajorVersion == 4)
			{
				platform = "Windows NT";
			}
			else if (os.dwMajorVersion == 5)
			{
				switch(os.dwMinorVersion)
				{
					case 0:
						platform = "Windows 2000";
						break;
					case 1:
						platform = "Windows XP";
						break;
					default:
						platform = "Windows";
						break;
				}
			}
			else
			{
				platform = "Windows";
			}
			break;

		default: platform = "Windows";
	}

#elif defined(unix) || defined(__APPLE__) || defined(__NetBSD__)
	/* Why Apple and NetBSD don't define 'unix' I'll never know. */
	struct utsname os;
	int ret = uname(&os);

	if (ret == 0)
	{
		platform = os.sysname;
	}
#endif

	return IoState_symbolWithCString_(IOSTATE, platform);
}

IoObject *IoObject_platformVersion(IoObject *self, IoObject *locals, IoMessage *m)
{
	char platformVersion[256];

	/*doc System platformVersion
	Returns the version id of the OS.
	*/

#if defined(_WIN32)

	OSVERSIONINFO os;

	os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	GetVersionEx(&os);

	snprintf(platformVersion, sizeof(platformVersion) - 1, "%d.%d",
		os.dwMajorVersion, os.dwMinorVersion);

#elif defined(unix) || defined(__APPLE__) || defined(__NetBSD__)
	/* Why Apple and NetBSD don't define 'unix' I'll never know. */
	struct utsname os;
	int ret = uname(&os);

	if (ret == 0)
	{
		snprintf(platformVersion, sizeof(platformVersion) - 1, os.release);
	}
#endif

	return IoState_symbolWithCString_(IOSTATE, platformVersion);
}

IoObject *IoObject_activeCpus(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc System activeCpus
	Returns the number of active CPUs.
	*/
	
	int cpus = 1;
#if defined(CTL_HW)
	int mib[2];
	size_t len = sizeof(cpus);
	mib[0] = CTL_HW;
#if defined(HW_AVAILCPU)
	mib[1] = HW_AVAILCPU;
#elif defined(HW_NCPU)
	mib[1] = HW_NCPU;
#else
#error
#endif
	sysctl(mib, 2, &cpus, &len, NULL, 0);
#elif defined(_SC_NPROCESSORS_ONLN)
	cpus = sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(_SC_NPROC_ONLN)
	cpus = sysconf(_SC_NPROC_ONLN);
#elif defined(WIN32)
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	cpus = si.dwNumberOfProcessors;
#else
#error
#endif
	return IONUMBER(cpus);
}

#include "PortableUsleep.h"

IoObject *IoObject_sleep(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc System sleep(secondsNumber)
	Performs a *blocking* sleep call for specified number of seconds.
	*/

	double seconds = IoMessage_locals_doubleArgAt_(m, locals, 0);
	unsigned int microseconds = (seconds * 1000000);
	usleep(microseconds);
	return self;
}

IoObject *IoObject_maxRecycledObjects(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc System maxRecycledObjects
	Returns the max number of recycled objects used.
	*/

	return IONUMBER(IOSTATE->maxRecycledObjects);
}

IoObject *IoObject_setMaxRecycledObjects(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc System setMaxRecycledObjects(aNumber)
	Sets the max number of recycled objects used.
	*/

	size_t max = IoMessage_locals_sizetArgAt_(m, locals, 0);
	IOSTATE->maxRecycledObjects = max;
	return self;
}

IoObject *IoObject_recycledObjectCount(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc System recycledObjectCount
	Returns the current number of objects being held for recycling.
	*/

	return IONUMBER(List_size(IOSTATE->recycledObjects));
}

#include "IoList.h"

IoObject *IoObject_symbols(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc System symbols
	Returns a List containing all Symbols currently in the system.
	*/
	
	IoList *list = IoList_new(IOSTATE);
	SHASH_FOREACH(IOSTATE->symbols, i, v, IoList_rawAppend_(list, v));
	return list;
}

IoObject *IoObject_setLobby(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc System setLobby(anObject)
	Sets the root object of the garbage collector.
	*/

	IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
	IoState_setLobby_(IOSTATE, v);
	return self;
}

IoObject *IoObject_thisProcessPid(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc System thisProcessPid()
	Return the process id (pid) for this Io process.
	*/
	
	return IONUMBER(getpid());
}

/*doc System version
	Returns a version number for Io.
*/

/*doc System distribution
	Returns the Io distribution name as a string.
*/

