/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IOODEGEOM_DEFINED
#define IOODEGEOM_DEFINED 1

#include "IoObject.h"
#include "IoMessage.h"

#include <ode/ode.h>

typedef struct
{
	dGeomID geomId;
} IoODEGeomData;

int ISODEGEOM(IoObject *self);

IoObject *IoODEGeom_geomFromId(void *state, dGeomID id);
dGeomID IoODEGeom_rawGeomID(IoObject *self);

IoObject *IoODEGeom_collide(IoObject *self, IoObject *locals, IoMessage *m);

IoObject *IoMessage_locals_odeGeomArgAt_(IoMessage *self, void *locals, int n);
dGeomID IoMessage_locals_odeGeomIdArgAt_(IoMessage *self, void *locals, int n);


#endif
