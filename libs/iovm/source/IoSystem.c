// metadoc System category Core
// metadoc System copyright Steve Dekorte, 2002
// metadoc System license BSD revised
/*metadoc System description
Contains methods related to the IoVM.
*/

/*cmetadoc System description
C implementation of Io's System object — a slot-only object (no
dedicated data pointer or tag) that exposes VM- and environment-level
hooks: process exit, errno string, env var get/set, platform name,
sleep, symbol table, lobby root, and GC recycler tuning. Under WASI
the host-provided surface is minimal: daemon and system() raise
errors, thisProcessPid is always 1, activeCpus is 1, and platform/
platformVersion return "wasm"/"wasi-0.1". IO_VERSION_STRING and
INSTALL_PREFIX are stamped onto System as slots in IoSystem_proto so
Io code can query the build identity.
*/

#include "IoSystem.h"
#include "IoNumber.h"
#include "IoMessage_parser.h"
#include "IoVersion.h"

#include "IoInstallPrefix.h"

#include <unistd.h>

/*cdoc System IoSystem_proto(state)
Constructs the singleton System object: installs the method table and
stamps version / installPrefix / type slots derived from compile-time
macros (IO_VERSION_STRING, INSTALL_PREFIX). There is no proto
registration — System is accessed by name from the lobby rather than
via IoState_protoWithId_, and there is nothing to clone.
*/
IoObject *IoSystem_proto(void *state) {
    IoMethodTable methodTable[] = {
        {"daemon", IoObject_daemon},
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

    IoObject_setSlot_to_(self, IOSYMBOL("installPrefix"),
                         IOSYMBOL(INSTALL_PREFIX));

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

/*cdoc System IoObject_daemon(self, locals, m)
Stub for the historical daemonize call. WASI has no fork/setsid so
the method always raises an Io exception. Kept for source
compatibility with the standard library.
*/
IO_METHOD(IoObject, daemon) {
    IoState_error_(IOSTATE, self, "daemon is not supported on WASM.");
    return self;
}

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

/*cdoc System IoObject_system(self, locals, m)
Stub for the historical system(3) shell bridge. WASI has no process
creation so the method always raises an Io exception — preserved as
a method so calling Io code can catch a consistent error rather than
a slot-not-found.
*/
IO_METHOD(IoObject, system) {
    /*doc System system(aString)
    Makes a system call and returns a Number for the return value.
    */

    IoState_error_(IOSTATE, self, "System system is not supported on WASM");
    return IONIL(self);
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

    return IoState_symbolWithCString_(IOSTATE, "wasm");
}

IO_METHOD(IoObject, platformVersion) {
    char platformVersion[256];

    /*doc System platformVersion
    Returns the version id of the OS.
    */

    snprintf(platformVersion, sizeof(platformVersion), "wasi-0.1");

    return IoState_symbolWithCString_(IOSTATE, platformVersion);
}

IO_METHOD(IoObject, activeCpus) {
    /*doc System activeCpus
    Returns the number of active CPUs.
    */

    return IONUMBER(1);
}

#include "PortableUsleep.h"

/*cdoc System IoObject_sleep(self, locals, m)
Blocks the current thread for the requested number of seconds by
chunking into sub-second usleep calls (usleep's POSIX argument must
be less than 1,000,000). Under WASM this is a real synchronous pause,
since there is no scheduler to yield to — long sleeps stall the whole
runtime.
*/
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

    return IONUMBER(1);
}

/*doc System version
        Returns a version number for Io.
*/

/*doc System distribution
        Returns the Io distribution name as a string.
*/
