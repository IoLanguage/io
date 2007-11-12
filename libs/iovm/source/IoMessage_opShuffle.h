/*
docCopyright("Jonathan Wright", 2006)
docLicense("BSD revised")
*/

#ifndef IOMESSAGE_OPSHUFFLE_DEFINED
#define IOMESSAGE_OPSHUFFLE_DEFINED 1

#include "IoMessage.h"

#ifdef __cplusplus
extern "C" {
#endif

IoMessage *IoMessage_opShuffle(IoMessage *self, IoObject *locals, IoMessage *m);
void IoMessage_opShuffle_(IoMessage *self);

#ifdef __cplusplus
}
#endif
#endif
