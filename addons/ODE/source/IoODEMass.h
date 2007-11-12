/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IOODEMass_DEFINED
#define IOODEMass_DEFINED 1

#include "IoObject.h"
#include "IoNumber.h"

#include <ode/ode.h>

#define ISODEMASS(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoODEMass_rawClone)

typedef IoObject IoODEMass;

typedef dMass IoODEMassData;

IoODEMass *IoODEMass_rawClone(IoODEMass *self);
IoODEMass *IoODEMass_proto(void *state);
IoODEMass *IoODEMass_new(void *state);

void IoODEMass_free(IoODEMass *self);
void IoODEMass_mark(IoODEMass *self);

/* ----------------------------------------------------------- */

IoODEMass *IoMessage_locals_odeMassArgAt_(IoMessage *self, void *locals, int n);
dMass *IoMessage_locals_odeMassStructArgAt_(IoMessage *self, void *locals, int n);

dMass *IoODEMass_dMassStruct(IoODEMass *self);

/* ----------------------------------------------------------- */

IoObject *IoODEMass_reset(IoODEMass *self, IoObject *locals, IoMessage *m);
IoObject *IoODEMass_mass(IoODEMass *self, IoObject *locals, IoMessage *m);
IoObject *IoODEMass_setMass(IoODEMass *self, IoObject *locals, IoMessage *m);
IoObject *IoODEMass_centerOfGravity(IoODEMass *self, IoObject *locals, IoMessage *m);
IoObject *IoODEMass_setCenterOfGravity(IoODEMass *self, IoObject *locals, IoMessage *m);
IoObject *IoODEMass_inertiaTensor(IoODEMass *self, IoObject *locals, IoMessage *m);
IoObject *IoODEMass_parameters(IoODEMass *self, IoObject *locals, IoMessage *m);
IoObject *IoODEMass_setParameters(IoODEMass *self, IoObject *locals, IoMessage *m);

IoObject *IoODEMass_setSphereDensity(IoODEMass *self, IoObject *locals, IoMessage *m);
IoObject *IoODEMass_setSphereMass(IoODEMass *self, IoObject *locals, IoMessage *m);
IoObject *IoODEMass_setCappedCylinderDensity(IoODEMass *self, IoObject *locals, IoMessage *m);
IoObject *IoODEMass_setCappedCylinderMass(IoODEMass *self, IoObject *locals, IoMessage *m);
IoObject *IoODEMass_setBoxDensity(IoODEMass *self, IoObject *locals, IoMessage *m);
IoObject *IoODEMass_setBoxMass(IoODEMass *self, IoObject *locals, IoMessage *m);

#endif
