/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IOODEContact_DEFINED
#define IOODEContact_DEFINED 1

#include "IoObject.h"
#include "IoNumber.h"

#include <ode/ode.h>

#define ISODECONTACT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoODEContact_rawClone)

typedef IoObject IoODEContact;

typedef dContact IoODEContactData;

IoODEContact *IoODEContact_rawClone(IoODEContact *self);
IoODEContact *IoODEContact_proto(void *state);
IoODEContact *IoODEContact_new(void *state);
IoODEContact *IoODEContact_newContactGeom(void *state, dContactGeom *contact);

void IoODEContact_free(IoODEContact *self);
void IoODEContact_mark(IoODEContact *self);

/* ----------------------------------------------------------- */

IoODEContact *IoMessage_locals_odeContactArgAt_(IoMessage *self, void *locals, int n);
dContact *IoMessage_locals_odeContactStructArgAt_(IoMessage *self, void *locals, int n);

dContact *IoODEContact_dContactStruct(IoODEContact *self);

/* ----------------------------------------------------------- */

IoObject *IoODEContact_position(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setPosition(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_normal(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setNormal(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_depth(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setDepth(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_geom1(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setGeom1(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_geom2(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setGeom2(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_side1(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setSide1(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_side2(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setSide2(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_mu(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setMu(IoODEContact *self, IoObject *locals, IoMessage *m);

IoObject *IoODEContact_frictionDirection(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setFrictionDirection(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_frictionDirectionEnabled(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_disableFrictionDirection(IoODEContact *self, IoObject *locals, IoMessage *m);

IoObject *IoODEContact_mu2(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setMu2(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_mu2Enabled(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_disableMu2(IoODEContact *self, IoObject *locals, IoMessage *m);

IoObject *IoODEContact_bounce(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setBounce(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_bounceEnabled(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_disableBounce(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_bounceVelocity(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setBounceVelocity(IoODEContact *self, IoObject *locals, IoMessage *m);

IoObject *IoODEContact_softErp(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setSoftErp(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_softErpEnabled(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_disableSoftErp(IoODEContact *self, IoObject *locals, IoMessage *m);

IoObject *IoODEContact_softCfm(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setSoftCfm(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_softCfmEnabled(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_disableSoftCfm(IoODEContact *self, IoObject *locals, IoMessage *m);

IoObject *IoODEContact_motion1(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setMotion1(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_motion1Enabled(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_disableMotion1(IoODEContact *self, IoObject *locals, IoMessage *m);

IoObject *IoODEContact_motion2(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setMotion2(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_motion2Enabled(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_disableMotion2(IoODEContact *self, IoObject *locals, IoMessage *m);

IoObject *IoODEContact_slip1(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setSlip1(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_slip1Enabled(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_disableSlip1(IoODEContact *self, IoObject *locals, IoMessage *m);

IoObject *IoODEContact_slip2(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_setSlip2(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_slip2Enabled(IoODEContact *self, IoObject *locals, IoMessage *m);
IoObject *IoODEContact_disableSlip2(IoODEContact *self, IoObject *locals, IoMessage *m);

#endif
