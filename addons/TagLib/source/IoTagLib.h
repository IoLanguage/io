/*   copyright: Steve Dekorte, 2002
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IoTagLib_DEFINED
#define IoTagLib_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

typedef IoObject IoTagLib;

typedef struct
{

} IoTagLibData;

IoTagLib *IoTagLib_proto(void *state);
IoTagLib *IoTagLib_new(void *state);
IoTagLib *IoTagLib_rawClone(IoTagLib *self);

void IoTagLib_free(IoTagLib *self);

// -----------------------------------------------------------

IoObject *IoTagLib_load(IoTagLib *self, IoObject *locals, IoMessage *m);
IoObject *IoTagLib_save(IoTagLib *self, IoObject *locals, IoMessage *m);

#endif
