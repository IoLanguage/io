/*
docCopyright("Daniel Rosengren", 2007)
*/

#ifndef IOCAIRO_DEFINED
#define IOCAIRO_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

typedef IoObject IoCairo;

IoCairo *IoCairo_rawClone(IoCairo *self);
IoCairo *IoCairo_proto(void *state);
void IoCairo_free(IoCairo *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairo_version(IoCairo *self, IoObject *locals, IoMessage *m);

#endif
