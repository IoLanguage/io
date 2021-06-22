// metadoc System category Core
// metadoc System copyright Steve Dekorte, 2002
// metadoc System license BSD revised
/*metadoc System description
Contains methods related to the IoVM.
*/

#include "IoSystem.h"
#include "IoNumber.h"
#include "IoMessage_parser.h"
#include "IoVersion.h"
#ifndef WIN32
#include "IoInstallPrefix.h"
#endif

#if defined(linux) || defined(__MINGW64__)
#include <unistd.h>
#endif

#if defined(unix) || defined(__APPLE__) || defined(__NetBSD__) ||              \
    defined(__OpenBSD__)
#include <sys/utsname.h>
#if defined(__NetBSD__) || defined(__OpenBSD__)
#include <sys/param.h>
#endif
#if defined(HAVE_SYS_SYSCTL_H) && !defined(__GLIBC__)
#include <sys/sysctl.h>
#endif
#endif

//#define WIN32
#if defined(__CYGWIN__) || defined(_WIN32)
#include <windows.h>
#endif

#ifdef WIN32
#include <windows.h>
#define _fullpath(res, path, size)                                             \
    (GetFullPathName((path), (size), (res), NULL) ? (res) : NULL)

#ifndef __CYGWIN__
static void setenv(const char *varName, const char *value, int force) {
    const char *safeValue;
    char *buf;

    if (!varName) {
        return;
    }

    if (!value) {
        safeValue = "";
    } else {
        safeValue = value;
    }

    // buffer for var and value plus '=' and the \0
    buf = (char *)io_calloc(1, strlen(varName) + strlen(safeValue) + 2);

    if (!buf) {
        return;
    }

    strcpy(buf, varName);
    strcat(buf, "=");
    strcat(buf, safeValue);

    _putenv(buf);
    io_free(buf);
}

//#define setenv(k, v, o) SetEnvironmentVariable((k), (v)) // removed by james
// burgess #define getpid GetCurrentProcessId  // removed by james burgess
//#define setenv(k, v, o) SetEnvironmentVariable((k), (v))
#endif

IO_METHOD(IoObject, installPrefix) {

    char acPath[256];
    char root[256];
    if (GetModuleFileName(NULL, acPath, 256) != 0) {
        // guaranteed file name of at least one character after path
        strcpy(strrchr(acPath, '\\'), "\\.");
        _fullpath(root, acPath, 256);
    }

    return IoState_symbolWithCString_(IOSTATE, root);
}
#endif

IoObject *IoSystem_proto(void *state) {
    IoMethodTable methodTable[] = {
#ifdef WIN32
        {"shellExecute", IoObject_shellExecute},
        {"installPrefix", IoObject_installPrefix},
#else
        {"daemon", IoObject_daemon},
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
    IoObject_setSlot_to_(self, IOSYMBOL("version"),
                         IOSYMBOL(IO_VERSION_STRING));

    // IoObject_setSlot_to_(self, IOSYMBOL("distribution"), IOSYMBOL("Io"));
    IoObject_setSlot_to_(self, IOSYMBOL("type"), IOSYMBOL("System"));
    // IoState_registerProtoWithId_((IoState *)state, self, protoId);

    /*doc System installPrefix
    Returns the root path where io was installed. The default is /usr/local.
    */

#ifndef WIN32
    IoObject_setSlot_to_(self, IOSYMBOL("installPrefix"),
                         IOSYMBOL(INSTALL_PREFIX));
#endif

    return self;
}

/*doc System args
        Returns the list of command line argument strings the program was run
   with.
*/

/*
IO_METHOD(IoObject, errorNumber)
{
        return IONUMBER(errno);
}
*/

#include <stdio.h>
#include <errno.h>

#ifdef WIN32
#include <shellapi.h>
#include "IoError.h"
#include <stdint.h>
IO_METHOD(IoObject, shellExecute) {
    LPCTSTR operation;
    LPCTSTR file;
    LPCTSTR parameters;
    LPCTSTR directory;
    int displayFlag;
    int64_t result;

    operation = CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 0));
    file = CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 1));
    parameters = IoMessage_argCount(m) > 2
                     ? CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 2))
                     : NULL;
    directory = IoMessage_argCount(m) > 3
                    ? CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 3))
                    : NULL;
    displayFlag = IoMessage_argCount(m) > 4
                      ? IoMessage_locals_intArgAt_(m, locals, 4)
                      : SW_SHOWNORMAL;

    result = (int64_t)ShellExecute(NULL, operation, file, parameters, directory,
                                   displayFlag);

    if (result > 32) {
        return self;
    } else {
        return (IoObject *)IoError_newWithMessageFormat_(
            IOSTATE, "ShellExecute Error %i", result);
    }
}
#else
#ifdef daemon
IO_METHOD(IoObject, daemon) {
    /*doc System daemon(dontChroot, dontRedirectOutputStreams)
    Become a daemon process.  If dontChroot is false, the process will change
    its directory to /.  If dontRedirectOutputStreams is false, stdout and
    stderr are redirected to /dev/null.
    */
    if (daemon(IoMessage_locals_boolArgAt_(m, locals, 0),
               IoMessage_locals_boolArgAt_(m, locals, 1))) {
        IoState_error_(IOSTATE, self,
                       "Daemonize failed. See System errorNumberDescription.");
    }

    return self;
}
#else
IO_METHOD(IoObject, daemon) {
    IoState_error_(IOSTATE, self, "no daemon function found on this platform.");
    return self;
}
#endif
#endif

IO_METHOD(IoObject, errorNumberDescription) {
    /*doc System errorNumber
    Returns the C errno string.
    */
    return errno ? IOSYMBOL(strerror(errno)) : IONIL(self);
}

IO_METHOD(IoObject, exit) {
    /*doc System exit(optionalReturnCodeNumber)
    Shutdown the IoState (io_free all objects) and return
control to the calling program (if any).
*/

    int returnCode = 0;

    if (IoMessage_argCount(m)) {
        returnCode = IoMessage_locals_intArgAt_(m, locals, 0);
    }

    IoState_exit(IOSTATE, returnCode);
    return self;
}

IO_METHOD(IoObject, getEnvironmentVariable) {
    /*doc System getEnvironmentVariable(nameString)
    Returns a string with the value of the environment
    variable whose name is specified by nameString.
    */

    IoSymbol *key = IoMessage_locals_symbolArgAt_(m, locals, 0);
    char *s = getenv(CSTRING(key));

    if (!s) {
        return ((IoState *)IOSTATE)->ioNil;
    }

    return IoState_symbolWithCString_(IOSTATE, s);
}

IO_METHOD(IoObject, system) {
    /*doc System system(aString)
    Makes a system call and returns a Number for the return value.
    */

    IoSymbol *s = IoMessage_locals_symbolArgAt_(m, locals, 0);

    char *buf = NULL;
    int result = 0;
    buf = (char *)getcwd(buf, 1024);

    // printf("CURDIR: [%s]\n", buf);
    // printf("SYSTEM: [%s]\n", CSTRING(s));
    result = system(CSTRING(s));
    // printf("system result = %i\n", result);
#if !defined(_WIN32) || defined(__CYGWIN__)
    result /= 256;
#endif

    return IONUMBER(result);
}

IO_METHOD(IoObject, memorySizeOfState) {
    /*doc Object memorySizeOfState
    Returns the number of bytes in the IoState
    (this may not include memory allocated by C libraries).
    */

    return IONUMBER(0);
    // return IONUMBER(IoState_memorySize(IOSTATE));
}

IO_METHOD(IoObject, compactState) {
    /*doc Object compactState
    Attempt to compact the memory of the IoState if possible.
    */

    // IoState_compact(IOSTATE);
    return self;
}

IO_METHOD(IoObject, setEnvironmentVariable) {
    /*doc System setEnvironmentVariable(keyString, valueString)
    Sets the environment variable keyString to the value valueString.
*/

    // setenv() takes different args in different implementations
    IoSymbol *key = IoMessage_locals_symbolArgAt_(m, locals, 0);
    IoSymbol *value = IoMessage_locals_symbolArgAt_(m, locals, 1);
    setenv(CSTRING(key), CSTRING(value), 1);
    return self;
}

IO_METHOD(IoObject, platform) {
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

    os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&os);

    switch (os.dwPlatformId) {
    case VER_PLATFORM_WIN32_WINDOWS:
        switch (os.dwMinorVersion) {
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
        if (os.dwMajorVersion == 3 || os.dwMajorVersion == 4) {
            platform = "Windows NT";
        } else if (os.dwMajorVersion == 5) {
            switch (os.dwMinorVersion) {
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
        } else {
            platform = "Windows";
        }
        break;

    default:
        platform = "Windows";
    }

#elif defined(unix) || defined(__APPLE__) || defined(__NetBSD__) ||            \
    defined(__OpenBSD__)
    /* Why Apple and NetBSD don't define 'unix' I'll never know. */
    struct utsname os;
    int ret = uname(&os);

    if (ret == 0) {
        platform = os.sysname;
    }
#endif

    return IoState_symbolWithCString_(IOSTATE, platform);
}

IO_METHOD(IoObject, platformVersion) {
    char platformVersion[256];

    /*doc System platformVersion
    Returns the version id of the OS.
    */

#if defined(_WIN32)

    OSVERSIONINFO os;

    os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&os);

    snprintf(platformVersion, sizeof(platformVersion) - 1, "%d.%d",
             os.dwMajorVersion, os.dwMinorVersion);

#elif defined(unix) || defined(__APPLE__) || defined(__NetBSD__) ||            \
    defined(__OpenBSD__)
    /* Why Apple and NetBSD don't define 'unix' I'll never know. */
    struct utsname os;
    int ret = uname(&os);

    if (ret == 0) {
        snprintf(platformVersion, 256, "%s.%s", os.version, os.release);
        // sprintf(platformVersion, (size_t)sizeof(platformVersion) - 1, (const
        // char *)os.release);
    }
#endif

    return IoState_symbolWithCString_(IOSTATE, platformVersion);
}

IO_METHOD(IoObject, activeCpus) {
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

IO_METHOD(IoObject, sleep) {
    /*doc System sleep(secondsNumber)
    Performs a *blocking* sleep call for specified number of seconds.
    */

    double seconds = IoMessage_locals_doubleArgAt_(m, locals, 0);
    unsigned int microseconds = (seconds * 1000000);
    while (microseconds > 999999) {
        usleep(999999);
        microseconds -= 999999;
    }
    usleep(microseconds);
    return self;
}

IO_METHOD(IoObject, maxRecycledObjects) {
    /*doc System maxRecycledObjects
    Returns the max number of recycled objects used.
    */

    return IONUMBER(IOSTATE->maxRecycledObjects);
}

IO_METHOD(IoObject, setMaxRecycledObjects) {
    /*doc System setMaxRecycledObjects(aNumber)
    Sets the max number of recycled objects used.
    */

    size_t max = IoMessage_locals_sizetArgAt_(m, locals, 0);
    IOSTATE->maxRecycledObjects = max;
    return self;
}

IO_METHOD(IoObject, recycledObjectCount) {
    /*doc System recycledObjectCount
    Returns the current number of objects being held for recycling.
    */

    return IONUMBER(List_size(IOSTATE->recycledObjects));
}

#include "IoList.h"

IO_METHOD(IoObject, symbols) {
    /*doc System symbols
    Returns a List containing all Symbols currently in the system.
    */

    IoList *list = IoList_new(IOSTATE);
    CHASH_FOREACH(IOSTATE->symbols, i, v, IoList_rawAppend_(list, v));
    return list;
}

IO_METHOD(IoObject, setLobby) {
    /*doc System setLobby(anObject)
    Sets the root object of the garbage collector.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    IoState_setLobby_(IOSTATE, v);
    return self;
}

IO_METHOD(IoObject, thisProcessPid) {
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
