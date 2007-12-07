/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IoODEHinge_DEFINED
#define IoODEHinge_DEFINED 1

#include "IoODEJoint.h"
#include "IoODEJointGroup.h"

typedef IoODEJoint IoODEHinge;

IoODEHinge *IoODEHinge_rawClone(IoODEHinge *self);
IoODEHinge *IoODEHinge_proto(void *state);
IoODEHinge *IoODEHinge_newProto(void *state, IoODEJointGroup *jointGroup);

void IoODEHinge_free(IoODEHinge *self);
void IoODEHinge_mark(IoODEHinge *self);

/* ----------------------------------------------------------- */

IoObject *IoODEHinge_setAnchor(IoODEHinge *self, IoObject *locals, IoMessage *m);
//IoObject *IoODEHinge_setAnchor2(IoODEHinge *self, IoObject *locals, IoMessage *m);

IoObject *IoODEHinge_getParam(IoODEHinge *self, IoObject *locals, IoMessage *m, int parameter);
IoObject *IoODEHinge_setParam(IoODEHinge *self, IoObject *locals, IoMessage *m, int parameter);

#define PARAMS \
	PARAM(dParamLoStop, lowStop, setLowStop) \
	PARAM(dParamHiStop, highStop, setHighStop) \
	PARAM(dParamVel, velocity, setVelocity) \
	PARAM(dParamFMax, maxForce, setMaxForce) \
	PARAM(dParamFudgeFactor, fudgeFactor, setFudgeFactor) \
	PARAM(dParamBounce, bounce, setBounce) \
	PARAM(dParamCFM, cfm, setCfm) \
	PARAM(dParamStopERP, stopErp, setStopErp) \
	PARAM(dParamStopCFM, stopCfm, setStopCfm)

#define PARAM(X, _N, _SETN) \
IoObject *IoODEHinge_##_N(IoODEHinge *self, IoObject *locals, IoMessage *m); \
IoObject *IoODEHinge_##_SETN(IoODEHinge *self, IoObject *locals, IoMessage *m);
PARAMS
#undef PARAM

IoObject *IoODEHinge_anchor(IoODEHinge *self, IoObject *locals, IoMessage *m);
IoObject *IoODEHinge_anchor2(IoODEHinge *self, IoObject *locals, IoMessage *m);
IoObject *IoODEHinge_axis(IoODEHinge *self, IoObject *locals, IoMessage *m);
IoObject *IoODEHinge_angle(IoODEHinge *self, IoObject *locals, IoMessage *m);
IoObject *IoODEHinge_angleRate(IoODEHinge *self, IoObject *locals, IoMessage *m);

IoObject *IoODEHinge_setAnchor(IoODEHinge *self, IoObject *locals, IoMessage *m);
IoObject *IoODEHinge_setAxis(IoODEHinge *self, IoObject *locals, IoMessage *m);
IoObject *IoODEHinge_addTorque(IoODEHinge *self, IoObject *locals, IoMessage *m);

#endif
