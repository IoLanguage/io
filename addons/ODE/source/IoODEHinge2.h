/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IoODEHinge2_DEFINED
#define IoODEHinge2_DEFINED 1

#include "IoODEJoint.h"
#include "IoODEJointGroup.h"

typedef IoODEJoint IoODEHinge2;

IoODEHinge2 *IoODEHinge2_rawClone(IoODEHinge2 *self);
IoODEHinge2 *IoODEHinge2_proto(void *state);
IoODEHinge2 *IoODEHinge2_newProto(void *state, IoODEJointGroup *jointGroup);

void IoODEHinge2_free(IoODEHinge2 *self);
void IoODEHinge2_mark(IoODEHinge2 *self);

/* ----------------------------------------------------------- */

IoObject *IoODEHinge2_setAnchor(IoODEHinge2 *self, IoObject *locals, IoMessage *m);
//IoObject *IoODEHinge2_setAnchor2(IoODEHinge2 *self, IoObject *locals, IoMessage *m);

IoObject *IoODEHinge2_getParam(IoODEHinge2 *self, IoObject *locals, IoMessage *m, int parameter);
IoObject *IoODEHinge2_setParam(IoODEHinge2 *self, IoObject *locals, IoMessage *m, int parameter);

#define PARAMS \
	PARAM(dParamLoStop, lowStop, setLowStop) \
	PARAM(dParamHiStop, highStop, setHighStop) \
	PARAM(dParamVel, velocity, setVelocity) \
	PARAM(dParamFMax, maxForce, setMaxForce) \
	PARAM(dParamFudgeFactor, fudgeFactor, setFudgeFactor) \
	PARAM(dParamBounce, bounce, setBounce) \
	PARAM(dParamCFM, cfm, setCfm) \
	PARAM(dParamStopERP, stopErp, setStopErp) \
	PARAM(dParamStopCFM, stopCfm, setStopCfm) \
	PARAM(dParamSuspensionERP, suspensionErp, setSuspensionErp) \
	PARAM(dParamSuspensionCFM, suspensionCfm, setSuspensionCfm)

#define PARAM(X, _N, _SETN) \
IoObject *IoODEHinge2_##_N(IoODEHinge2 *self, IoObject *locals, IoMessage *m); \
IoObject *IoODEHinge2_##_SETN(IoODEHinge2 *self, IoObject *locals, IoMessage *m);
PARAMS
#undef PARAM

IoObject *IoODEHinge2_anchor(IoODEHinge2 *self, IoObject *locals, IoMessage *m);
IoObject *IoODEHinge2_anchor2(IoODEHinge2 *self, IoObject *locals, IoMessage *m);
IoObject *IoODEHinge2_axis1(IoODEHinge2 *self, IoObject *locals, IoMessage *m);
IoObject *IoODEHinge2_axis2(IoODEHinge2 *self, IoObject *locals, IoMessage *m);
IoObject *IoODEHinge2_angle1(IoODEHinge2 *self, IoObject *locals, IoMessage *m);
IoObject *IoODEHinge2_angle1Rate(IoODEHinge2 *self, IoObject *locals, IoMessage *m);
IoObject *IoODEHinge2_angle2Rate(IoODEHinge2 *self, IoObject *locals, IoMessage *m);

IoObject *IoODEHinge2_setAnchor(IoODEHinge2 *self, IoObject *locals, IoMessage *m);
IoObject *IoODEHinge2_setAxis1(IoODEHinge2 *self, IoObject *locals, IoMessage *m);
IoObject *IoODEHinge2_setAxis2(IoODEHinge2 *self, IoObject *locals, IoMessage *m);
IoObject *IoODEHinge2_addTorques(IoODEHinge2 *self, IoObject *locals, IoMessage *m);

#endif
