//metadoc Io2Objc copyright 2003, Steve Dekorte
//metadoc Io2Objc license BSD revised
/*metadoc Io2Objc description
An Io proxy to an Objective-C object or class
*/

#ifndef IOOBJECTIVEC_DEFINED
#define IOOBJECTIVEC_DEFINED 1

#include "IoState.h"
#include "IoObject.h"
#include "IoNumber.h"
#include "List.h"
#include <ctype.h>
#include "Runtime.h"
#include "IoObject.h"
#include "IoObject.h"
#include "IoObjcBridge.h"

#define ISIO2OBJC(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)Io2Objc_rawClone)

typedef IoObject Io2Objc;

typedef struct
{
	IoObjcBridge *bridge;
	id object; /* object that this instance is talking to */
	unsigned char *returnBuffer;
	int returnBufferSize;
} Io2ObjcData;

Io2Objc *Io2Objc_rawClone(Io2Objc *self);
Io2Objc *Io2Objc_proto(void *state);
Io2Objc *Io2Objc_new(void *state);

void Io2Objc_free(Io2Objc *self);
void Io2Objc_nullObjcBridge(Io2Objc *self);

void Io2Objc_mark(Io2Objc *self);
void Io2Objc_setBridge(Io2Objc *self, void *bridge);
void Io2Objc_setObject(Io2Objc *self, void *object);
void *Io2Objc_object(Io2Objc *self);

/* ----------------------------------------------------------------- */
IoObject *Io2Objc_perform(Io2Objc *self, IoObject *locals, IoMessage *m);
Io2Objc *Io2Objc_newSubclassNamed(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *Io2Objc_metaclass(Io2Objc *self, IoObject *locals, IoMessage *m);
IoObject *Io2Objc_setSlot(Io2Objc *self, IoObject *locals, IoMessage *m);
IoObject *Io2Objc_updateSlot(Io2Objc *self, IoObject *locals, IoMessage *m);
IoObject *Io2Objc_super(Io2Objc *self, IoObject *locals, IoMessage *m);
//IoObject *Io2Objc_print(Io2Objc *self, IoObject *locals, IoMessage *m);
//IoObject *Io2Objc_slotSummary(Io2Objc *self, IoObject *locals, IoMessage *m);
IoObject *Io2Objc_isIo2Objc(Io2Objc *self, IoObject *locals, IoMessage *m);
IoObject *Io2Objc_io2ObjcType(Io2Objc *self, IoObject *locals, IoMessage *m);


#endif
