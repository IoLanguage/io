/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IOODESimpleSpace_DEFINED
#define IOODESimpleSpace_DEFINED 1

#include "IoObject.h"
#include "IoNumber.h"

#include <ode/ode.h>

typedef IoObject IoODESimpleSpace;

typedef struct
{
	dSpaceID spaceId;
} IoODESimpleSpaceData;

IoODESimpleSpace *IoODESimpleSpace_rawClone(IoODESimpleSpace *self);
IoODESimpleSpace *IoODESimpleSpace_proto(void *state);
IoODESimpleSpace *IoODESimpleSpace_new(void *state);

void IoODESimpleSpace_free(IoODESimpleSpace *self);
void IoODESimpleSpace_mark(IoODESimpleSpace *self);

/* ----------------------------------------------------------- */

dSpaceID IoODESimpleSpace_rawSimpleSpaceId(IoODESimpleSpace *self);

/* ----------------------------------------------------------- */

IoObject *IoODESimpleSpace_spaceId(IoODESimpleSpace *self, IoObject *locals, IoMessage *m);
IoObject *IoODESimpleSpace_plane(IoODESimpleSpace *self, IoObject *locals, IoMessage *m);
IoObject *IoODESimpleSpace_box(IoODESimpleSpace *self, IoObject *locals, IoMessage *m);
IoObject *IoODESimpleSpace_collide(IoODESimpleSpace *self, IoObject *locals, IoMessage *m);

#endif
