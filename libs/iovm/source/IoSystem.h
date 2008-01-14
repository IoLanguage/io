/*#io
System ioDoc(
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
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
IoObject *IoObject_errno(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_errnoDescription(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_exit(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_compileString(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_getenv(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_setenv(IoObject *self, IoObject *locals, IoMessage *m);
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

#ifdef __cplusplus
}
#endif
#endif
