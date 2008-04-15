//metadoc copyright Steve Dekorte 2002
//metadoc license BSD revised

#ifndef IOPYTHON_DEFINED
#define IOPYTHON_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "PythonData.h"

#define ISPYTHON(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoPython_rawClone)

typedef IoObject IoPython;

IoPython *IoPython_rawClone(IoPython *self);
IoPython *IoPython_proto(void *state);
IoPython *IoPython_new(void *state);
void IoPython_free(IoPython *self);

/* ----------------------------------------------------------- */
IoObject *IoPython_credits(IoPython *self, IoObject *locals, IoMessage *m);
IoObject *IoPython_forward(IoPython *self, IoObject *locals, IoMessage *m);

IoObject *IoPython_import(IoPython *self, IoObject *locals, IoMessage *m);
IoObject *IoPython_call(IoPython *self, IoObject *locals, IoMessage *m);
IoObject *IoPython_print(IoPython *self, IoObject *locals, IoMessage *m);
#endif
