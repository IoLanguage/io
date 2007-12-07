/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IOODEBox_DEFINED
#define IOODEBox_DEFINED 1

#include "IoObject.h"
#include "IoNumber.h"
#include "geom.h"

#include <ode/ode.h>

#define ISODEBOX(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoODEBox_rawClone)

typedef IoObject IoODEBox;
typedef IoODEGeomData IoODEBoxData;

IoODEBox *IoODEBox_rawClone(IoODEBox *self);
IoODEBox *IoODEBox_proto(void *state);
IoODEBox *IoODEBox_new(void *state, dGeomID geomId);

void IoODEBox_free(IoODEBox *self);
void IoODEBox_mark(IoODEBox *self);

/* ----------------------------------------------------------- */

dGeomID IoODEBox_rawGeomId(IoODEBox *self);

/* ----------------------------------------------------------- */

IoObject *IoODEBox_geomId(IoODEBox *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBox_lengths(IoODEBox *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBox_setLengths(IoODEBox *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBox_pointDepth(IoODEBox *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBox_body(IoODEBox *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBox_setBody(IoODEBox *self, IoObject *locals, IoMessage *m);

#endif
