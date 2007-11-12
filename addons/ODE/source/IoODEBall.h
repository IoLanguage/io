/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IoODEBall_DEFINED
#define IoODEBall_DEFINED 1

#include "IoODEJoint.h"
#include "IoODEJointGroup.h"

typedef IoODEJoint IoODEBall;

IoODEBall *IoODEBall_rawClone(IoODEBall *self);
IoODEBall *IoODEBall_proto(void *state);
IoODEBall *IoODEBall_newProto(void *state, IoODEJointGroup *jointGroup);

void IoODEBall_free(IoODEBall *self);
void IoODEBall_mark(IoODEBall *self);

/* ----------------------------------------------------------- */

IoObject *IoODEBall_setAnchor(IoODEBall *self, IoObject *locals, IoMessage *m);
//IoObject *IoODEBall_setAnchor2(IoODEBall *self, IoObject *locals, IoMessage *m);

IoObject *IoODEBall_anchor(IoODEBall *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBall_anchor2(IoODEBall *self, IoObject *locals, IoMessage *m);

#endif
