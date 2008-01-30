/*   copyright: Rich Collins, 2007
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IONETWORKADAPTER_DEFINED
#define IONETWORKADAPTER_DEFINED 1

#ifdef __cplusplus
extern "C" {
#endif

#include "IoObject.h"

#define ISNETWORKADAPTER(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoNetworkAdapter_rawClone)

typedef IoObject IoNetworkAdapter;

IoNetworkAdapter *IoNetworkAdapter_rawClone(IoNetworkAdapter *self);
IoNetworkAdapter *IoNetworkAdapter_proto(void *state);
IoNetworkAdapter *IoNetworkAdapter_new(void *state);

void IoNetworkAdapter_free(IoNetworkAdapter *self);
void IoNetworkAdapter_mark(IoNetworkAdapter *self);

/* ----------------------------------------------------------- */

IoObject *IoNetworkAdapter_macAddress(IoNetworkAdapter *self, IoObject *locals, IoMessage *m);

#ifdef __cplusplus
}
#endif
#endif