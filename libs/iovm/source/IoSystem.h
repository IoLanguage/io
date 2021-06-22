/*metadoc System copyright
        Steve Dekorte 2002
*/

/*metadoc System license
        BSD revised
*/

#ifndef IoSystem_DEFINED
#define IoSystem_DEFINED 1

#include "IoObject.h"

#ifdef __cplusplus
extern "C" {
#endif

IoObject *IoSystem_proto(void *state);

#if defined(__CYGWIN__) || defined(_WIN32)
IO_METHOD(IoObject, shellExecute);
#else
IO_METHOD(IoObject, daemon);
#endif
IO_METHOD(IoObject, errorNumber);
IO_METHOD(IoObject, errorNumberDescription);
IO_METHOD(IoObject, exit);
IO_METHOD(IoObject, compileString);
IO_METHOD(IoObject, getEnvironmentVariable);
IO_METHOD(IoObject, setEnvironmentVariable);
IO_METHOD(IoObject, system);
IO_METHOD(IoObject, memorySizeOfState);
IO_METHOD(IoObject, compactState);
IO_METHOD(IoObject, platform);
IO_METHOD(IoObject, platformVersion);
IO_METHOD(IoObject, sleep);
IO_METHOD(IoObject, activeCpus);

IO_METHOD(IoObject, maxRecycledObjects);
IO_METHOD(IoObject, setMaxRecycledObjects);
IO_METHOD(IoObject, recycledObjectCount);

IO_METHOD(IoObject, symbols);
IO_METHOD(IoObject, setLobby);
IO_METHOD(IoObject, thisProcessPid);

#ifdef __cplusplus
}
#endif
#endif
