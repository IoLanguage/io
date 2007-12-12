/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IoODEContactJoint_DEFINED
#define IoODEContactJoint_DEFINED 1

#include "IoODEJoint.h"
#include "IoODEJointGroup.h"
#include <ode/ode.h>

typedef IoODEJoint IoODEContactJoint;

IoODEContactJoint *IoODEContactJoint_rawClone(IoODEContactJoint *self);
IoODEContactJoint *IoODEContactJoint_proto(void *state);
IoODEContactJoint *IoODEContactJoint_new(void *state, IoODEJointGroup *jointGroup, dJointID jointId);

void IoODEContactJoint_free(IoODEContactJoint *self);
void IoODEContactJoint_mark(IoODEContactJoint *self);

/* ----------------------------------------------------------- */

#endif
