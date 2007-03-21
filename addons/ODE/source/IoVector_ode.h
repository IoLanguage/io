/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IOVector_ode_DEFINED
#define IOVector_ode_DEFINED 1

#include <ode/ode.h>

void IoVector_odeInit(IoState *state, IoObject *context);

IoVector *IoVector_newWithODEPoint(IoState *state, const dReal *point);
IoVector *IoVector_newWithODEVector4(IoState *state, const dReal *point);

#endif
