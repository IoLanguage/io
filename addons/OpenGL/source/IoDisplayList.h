/*   Copyright (c) 2004, Steve Dekorte
docLicense("BSD revised")
 */

#ifndef IODISPLAYLIST_DEFINED
#define IODISPLAYLIST_DEFINED 1

#include "IoObject.h"
#include "IoState.h"
#include "GLIncludes.h"

#define ISDISPLAYLIST(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoDisplayList_rawClone)

typedef IoObject IoDisplayList;

IoDisplayList *IoDisplayList_proto(void *state);
IoDisplayList *IoDisplayList_new(void *state);
IoDisplayList *IoDisplayList_rawClone(IoDisplayList *self);

void IoDisplayList_free(IoDisplayList *self);
void IoDisplayList_mark(IoDisplayList *self);

/* ----------------------------------------------------------- */

void IoDisplayList_number_(IoDisplayList *self, GLuint n);
GLuint IoDisplayList_number(IoDisplayList *self);

IoObject *IoDisplayList_begin(IoDisplayList *self, IoObject *locals, IoMessage *m);
IoObject *IoDisplayList_end(IoDisplayList *self, IoObject *locals, IoMessage *m);
IoObject *IoDisplayList_call(IoDisplayList *self, IoObject *locals, IoMessage *m);
IoObject *IoDisplayList_id(IoDisplayList *self, IoObject *locals, IoMessage *m);


#endif
