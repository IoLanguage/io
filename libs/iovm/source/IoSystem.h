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

#if defined(_WIN32)
IoObject *IoObject_shellExecute(IoObject *self, IoObject *locals, IoMessage *m);
#endif
IoObject *IoObject_errorNumber(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_errorNumberDescription(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_exit(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_compileString(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_getEnvironmentVariable(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_setEnvironmentVariable(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_system(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_memorySizeOfState(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_compactState(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_platform(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_platformVersion(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_sleep(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_activeCpus(IoObject *self, IoObject *locals, IoMessage *m);

IoObject *IoObject_maxRecycledObjects(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_setMaxRecycledObjects(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_recycledObjectCount(IoObject *self, IoObject *locals, IoMessage *m);

IoObject *IoObject_symbols(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_setLobby(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_thisProcessPid(IoObject *self, IoObject *locals, IoMessage *m);

#ifdef __cplusplus
}
#endif
#endif
