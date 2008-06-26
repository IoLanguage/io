/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IOSeq_ode_DEFINED
#define IOSeq_ode_DEFINED 1

#include <ode/ode.h>
#include "IoState.h"

void IoSeq_odeInit(IoState *state, IoObject *context);

IoSeq *IoSeq_newWithODEPoint(IoState *state, const dReal *point);
IoSeq *IoSeq_newWithODEVector4(IoState *state, const dReal *point);

#endif
