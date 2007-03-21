/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IoODESlider_DEFINED
#define IoODESlider_DEFINED 1

#include "IoODEJoint.h"
#include "IoODEJointGroup.h"

typedef IoODEJoint IoODESlider;

IoODESlider *IoODESlider_rawClone(IoODESlider *self);
IoODESlider *IoODESlider_proto(void *state);
IoODESlider *IoODESlider_newProto(void *state, IoODEJointGroup *jointGroup);

void IoODESlider_free(IoODESlider *self);
void IoODESlider_mark(IoODESlider *self);

/* ----------------------------------------------------------- */

IoObject *IoODESlider_setAnchor(IoODESlider *self, IoObject *locals, IoMessage *m);
//IoObject *IoODESlider_setAnchor2(IoODESlider *self, IoObject *locals, IoMessage *m);

IoObject *IoODESlider_getParam(IoODESlider *self, IoObject *locals, IoMessage *m, int parameter);
IoObject *IoODESlider_setParam(IoODESlider *self, IoObject *locals, IoMessage *m, int parameter);

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
IoObject *IoODESlider_##_N(IoODESlider *self, IoObject *locals, IoMessage *m); \
IoObject *IoODESlider_##_SETN(IoODESlider *self, IoObject *locals, IoMessage *m);
PARAMS
#undef PARAM

IoObject *IoODESlider_axis(IoODESlider *self, IoObject *locals, IoMessage *m);
IoObject *IoODESlider_position(IoODESlider *self, IoObject *locals, IoMessage *m);
IoObject *IoODESlider_positionRate(IoODESlider *self, IoObject *locals, IoMessage *m);

IoObject *IoODESlider_setAxis(IoODESlider *self, IoObject *locals, IoMessage *m);
IoObject *IoODESlider_addForce(IoODESlider *self, IoObject *locals, IoMessage *m);

#endif
