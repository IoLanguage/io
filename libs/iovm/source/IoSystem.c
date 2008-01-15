/*#io
System ioDoc(
	docCopyright("Steve Dekorte", 2002)
	docLicense("BSD revised")
	docObject("System")
	docDescription("Contains methods related to the IoVM.")
	docCategory("Core")
*/

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

static void setEnvironmentVariable(const char *varName, const char* value, int force)
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
	{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_addMethodTable_(self, methodTable);

	/*#io
		docSlot("version", "Returns the Io version number.")
	*/
	IoObject_setSlot_to_(self, IOSYMBOL("version"), IONUMBER(IO_VERSION_NUMBER));

	//IoObject_setSlot_to_(self, IOSYMBOL("distribution"), IOSYMBOL("Io"));
	IoObject_setSlot_to_(self, IOSYMBOL("type"), IOSYMBOL("System"));

	#ifndef INSTALL_PREFIX
	#define INSTALL_PREFIX "/usr/local"
	#endif


	/*#io
		docSlot("installPrefix", "Returns the root path where io was install. The default is /usr/local.")
	*/
	IoObject_setSlot_to_(self, IOSYMBOL("installPrefix"), IOSYMBOL(INSTALL_PREFIX));

	return self;
}

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
	return IONUMBER((int) ShellExecute(NULL, "open", CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 0)), NULL, NULL, SW_SHOWNORMAL));
}
#endif

IoObject *IoObject_errorNumberDescription(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("errorNumber", "Returns the C errno string.")
	*/
	return errno ? IOSYMBOL(strerror(errno)) : IONIL(self);
}

IoObject *IoObject_exit(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("exit(optionalReturnCodeNumber)",
			"Shutdown the IoState (io_free all objects) and return
control to the calling program (if any). ")
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
	/*#io
	docSlot("getEnvironmentVariable(nameString)",
		   "Returns a string with the value of the environment variable whose name is specified by nameString.")
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
	/*#io
	docSlot("system(aString)",
			"Makes a system call and returns a Number for the return value.")
	*/

	IoSymbol *s = IoMessage_locals_symbolArgAt_(m, locals, 0);
	int result = system(CSTRING(s))/ 256;
	//printf("system result = %i\n", result);
	return IONUMBER(result);
}

IoObject *IoObject_memorySizeOfState(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*
	docSlot("memorySizeOfState",
			"Returns the number of bytes in the IoState
	(this may not include memory allocated by C libraries).")
	*/

	return IONUMBER(0);
	//return IONUMBER(IoState_memorySize(IOSTATE));
}

IoObject *IoObject_compactState(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*
	docSlot("compactState",
			"Attempt to compact the memory of the IoState if possible.")
	*/

	//IoState_compact(IOSTATE);
	return self;
}

IoObject *IoObject_setEnvironmentVariable(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setEnvironmentVariable(keyString, valueString)", "Sets the environment variable keyString to the value valueString.")
	*/

	// setenv() takes different args in different implementations
	IoSymbol *key = IoMessage_locals_symbolArgAt_(m, locals, 0);
	IoSymbol *value = IoMessage_locals_symbolArgAt_(m, locals, 1);
	setenv(CSTRING(key), CSTRING(value), 1);
	return self;
}

IoObject *IoObject_platform(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("platform", "Returns a string description of the platform.")
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
			break;
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

	/*#io
	docSlot("platformVersion", "Returns the version id of the OS.")
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
	/*#io
	docSlot("activeCpus", "Returns the number of active CPUs.")
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
	/*#io
	docSlot("sleep(secondsNumber)", "Performs a *blocking* sleep call for specified number of seconds.")
	*/

	double seconds = IoMessage_locals_doubleArgAt_(m, locals, 0);
	unsigned int microseconds = (seconds * 1000000);
	usleep(microseconds);
	return self;
}

IoObject *IoObject_maxRecycledObjects(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("maxRecycledObjects", "Returns the max number of recycled objects used.")
	*/

	return IONUMBER(IOSTATE->maxRecycledObjects);
}

IoObject *IoObject_setMaxRecycledObjects(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setMaxRecycledObjects(aNumber)", "Sets the max number of recycled objects used.")
	*/

	size_t max = IoMessage_locals_sizetArgAt_(m, locals, 0);
	IOSTATE->maxRecycledObjects = max;
	return self;
}

IoObject *IoObject_recycledObjectCount(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("recycledObjectCount", "Returns the current number of objects being held for recycling.")
	*/

	return IONUMBER(List_size(IOSTATE->recycledObjects));
}

#include "IoList.h"

IoObject *IoObject_symbols(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("symbols", "Returns a List containing all Symbols currently in the system.")
	*/
	IoList *list = IoList_new(IOSTATE);
	SHASH_FOREACH(IOSTATE->symbols, i, v, IoList_rawAppend_(list, v));
	return list;
}

IoObject *IoObject_setLobby(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setLobby", "Sets the root object of the garbage collector.")
	*/

	IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
	IoState_setLobby_(IOSTATE, v);
	return self;
}

/*#io
docSlot("version", "Returns a version number for Io.")
*/

/*#io
docSlot("distribution", "Returns the Io distribution name as a string.")
*/

// save ----------------------

/*
IoObject *IoObject_setStateFile(IoObject *self, IoObject *locals, IoMessage *m)
{
	IoSeq *fileName = IoMessage_locals_seqArgAt_(m, locals, 0);
}

IoObject *IoObject_saveState(IoObject *self, IoObject *locals, IoMessage *m)
{
	IoState_saveState(IOSTATE);
}

int IoState_saveState(IoState *self)
{
	Collector *collector = IOSTATE->collector;
	self->store->open();
	Collector_collect(collect);
	COLLECTOR_FOREACH(collector, v, IoState_storeObject_(state, v));
	self->store->close();
	return 1;
}

int IoState_syncState(IoState *self)
{
	Collector *collector = IOSTATE->collector;
	self->store->open();
	Collector_collect(collect);
	COLLECTOR_FOREACH(collector, v,
		if (!IoObject_persistentId(v) || IoObject_isDirty(v))
		IoState_storeObject_(state, v)
	);
	self->store->close();
	return 1;
}

PID_TYPE IoObject_pid(IoObject *self)
{
	if (!IoObject_persistentId(self))
	{
		IoObject_persistentId_(self, IoState_newPid(IOSTATE));
	}

	return IoObject_persistentId(self);
}

PID_TYPE IoState_newPid(IoState *self)
{
	return self->store->newPid();
}

int IoState_storeObject_(IoState *self, IoObject *v)
{
	// IoObject_pid will request new pid from the state if needed

	UArray *u = IoObject_asStorable(v);
	self->store->atPut(state, IoObject_pid(v), UArray_bytes(u), UArray_sizeInBytes(u));
	return 1;
}

// load ----------------------

int IoState_loadState(IoState *self, IoObject *v)
{
	IoObject *lobby;
	self->store->open();
	Datum *d = self->store->at(state, 1); // pid of lobby
	lobby = IoObject_fromDatum(self, d)
	IoState_setLobby_(self, PObject_newWithPid(state, 1));
	return 1;
}
*/
