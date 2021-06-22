
// metadoc Compiler copyright Steve Dekorte 2002
// metadoc Compiler license BSD revised

#ifndef IoCompiler_DEFINED
#define IoCompiler_DEFINED 1

#include "IoObject.h"

#ifdef __cplusplus
extern "C" {
#endif

IoObject *IoCompiler_proto(void *state);

IO_METHOD(IoObject, tokensForString);
IO_METHOD(IoObject, messageForTokens);
IO_METHOD(IoObject, messageForString);
IO_METHOD(IoObject, messageForString2);

#ifdef __cplusplus
}
#endif
#endif
