#ifndef IO_JS_BRIDGE_H
#define IO_JS_BRIDGE_H

#include "IoObject.h"

IoObject *IoJSObject_proto(void *state);
IoObject *IoJSObject_newWithHandle_(void *state, int handle);
void IoJSBridge_markIoHandles(void);

#endif

