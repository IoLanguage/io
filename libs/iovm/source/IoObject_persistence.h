
//metadoc Object copyright Steve Dekorte 2002
//metadoc Object license BSD revised

#ifndef IOOBJECT_PERSISTENCE_DEFINED
#define IOOBJECT_PERSISTENCE_DEFINED 1

#include "IoObject.h"

#ifdef __cplusplus
extern "C" {
#endif

void IoObject_writeToStream_(IoObject *self, BStream *stream);
IoObject *IoObject_allocFromStream_(IoObject *self, BStream *stream);
void IoObject_writeStream_(IoObject *self, BStream *stream);

#ifdef __cplusplus
}
#endif
#endif
