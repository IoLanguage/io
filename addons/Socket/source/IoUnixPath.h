#ifndef IOUNIXPATH_DEFINED
#define IOUNIXPATH_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IPAddress.h"
#if !defined(_WIN32) || defined(__CYGWIN__)
#include "UnixPath.h"
#endif

#define ISUNIXPATH(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoUnixPath_rawClone)

typedef IoObject IoUnixPath;

IoUnixPath *IoUnixPath_proto(void *state);
IoUnixPath *IoUnixPath_new(void *state);
IoUnixPath *IoUnixPath_rawClone(IoUnixPath *self);
void IoUnixPath_free(IoUnixPath *self);

// path

IoObject *IoUnixPath_path(IoUnixPath *self, IoObject *locals, IoMessage *m);
IoObject *IoUnixPath_setPath(IoUnixPath *self, IoObject *locals, IoMessage *m);

#endif
