/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IoODEFixed_DEFINED
#define IoODEFixed_DEFINED 1

#include "IoODEJoint.h"
#include "IoODEJointGroup.h"

typedef IoODEJoint IoODEFixed;

IoODEFixed *IoODEFixed_rawClone(IoODEFixed *self);
IoODEFixed *IoODEFixed_proto(void *state);
IoODEFixed *IoODEFixed_newProto(void *state, IoODEJointGroup *jointGroup);

void IoODEFixed_free(IoODEFixed *self);
void IoODEFixed_mark(IoODEFixed *self);

/* ----------------------------------------------------------- */

IoObject *IoODEFixed_fix(IoODEFixed *self, IoObject *locals, IoMessage *m);

#endif
