/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IOODEPlane_DEFINED
#define IOODEPlane_DEFINED 1

#include "IoObject.h"
#include "IoNumber.h"
#include "geom.h"

#include <ode/ode.h>

#define ISODEPLANE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoODEPlane_rawClone)

typedef IoObject IoODEPlane;
typedef IoODEGeomData IoODEPlaneData;

IoODEPlane *IoODEPlane_rawClone(IoODEPlane *self);
IoODEPlane *IoODEPlane_proto(void *state);
IoODEPlane *IoODEPlane_new(void *state, dGeomID geomId);

void IoODEPlane_free(IoODEPlane *self);
void IoODEPlane_mark(IoODEPlane *self);

/* ----------------------------------------------------------- */

dGeomID IoODEPlane_rawGeomId(IoODEPlane *self);

/* ----------------------------------------------------------- */

IoObject *IoODEPlane_geomId(IoODEPlane *self, IoObject *locals, IoMessage *m);
IoObject *IoODEPlane_params(IoODEPlane *self, IoObject *locals, IoMessage *m);
IoObject *IoODEPlane_setParams(IoODEPlane *self, IoObject *locals, IoMessage *m);

#endif
