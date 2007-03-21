/*#io
Compiler ioDoc(
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IoCompiler_DEFINED 
#define IoCompiler_DEFINED 1

#include "IoObject.h"

#ifdef __cplusplus
extern "C" {
#endif

IoObject *IoCompiler_proto(void *state);

IoObject *IoObject_tokensForString(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_messageForTokens(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_messageForString(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_messageForString2(IoObject *self, IoObject *locals, IoMessage *m);

#ifdef __cplusplus
}
#endif
#endif
