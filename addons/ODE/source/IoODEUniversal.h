/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IoODEUniversal_DEFINED
#define IoODEUniversal_DEFINED 1

#include "IoODEJoint.h"
#include "IoODEJointGroup.h"

typedef IoODEJoint IoODEUniversal;

IoODEUniversal *IoODEUniversal_rawClone(IoODEUniversal *self);
IoODEUniversal *IoODEUniversal_proto(void *state);
IoODEUniversal *IoODEUniversal_newProto(void *state, IoODEJointGroup *jointGroup);

void IoODEUniversal_free(IoODEUniversal *self);
void IoODEUniversal_mark(IoODEUniversal *self);

/* ----------------------------------------------------------- */

IoObject *IoODEUniversal_setAnchor(IoODEUniversal *self, IoObject *locals, IoMessage *m);
//IoObject *IoODEUniversal_setAnchor2(IoODEUniversal *self, IoObject *locals, IoMessage *m);

IoObject *IoODEUniversal_getParam(IoODEUniversal *self, IoObject *locals, IoMessage *m, int parameter);
IoObject *IoODEUniversal_setParam(IoODEUniversal *self, IoObject *locals, IoMessage *m, int parameter);

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
IoObject *IoODEUniversal_##_N(IoODEUniversal *self, IoObject *locals, IoMessage *m); \
IoObject *IoODEUniversal_##_SETN(IoODEUniversal *self, IoObject *locals, IoMessage *m);
PARAMS
#undef PARAM

IoObject *IoODEUniversal_anchor(IoODEUniversal *self, IoObject *locals, IoMessage *m);
IoObject *IoODEUniversal_anchor2(IoODEUniversal *self, IoObject *locals, IoMessage *m);
IoObject *IoODEUniversal_axis1(IoODEUniversal *self, IoObject *locals, IoMessage *m);
IoObject *IoODEUniversal_axis2(IoODEUniversal *self, IoObject *locals, IoMessage *m);
IoObject *IoODEUniversal_angle1(IoODEUniversal *self, IoObject *locals, IoMessage *m);
IoObject *IoODEUniversal_angle2(IoODEUniversal *self, IoObject *locals, IoMessage *m);
IoObject *IoODEUniversal_angle1Rate(IoODEUniversal *self, IoObject *locals, IoMessage *m);
IoObject *IoODEUniversal_angle2Rate(IoODEUniversal *self, IoObject *locals, IoMessage *m);

IoObject *IoODEUniversal_setAnchor(IoODEUniversal *self, IoObject *locals, IoMessage *m);
IoObject *IoODEUniversal_setAxis1(IoODEUniversal *self, IoObject *locals, IoMessage *m);
IoObject *IoODEUniversal_setAxis2(IoODEUniversal *self, IoObject *locals, IoMessage *m);
IoObject *IoODEUniversal_addTorques(IoODEUniversal *self, IoObject *locals, IoMessage *m);

#endif
