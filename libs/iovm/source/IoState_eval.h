/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/
#include "IoVMApi.h"

IOVM_API IoObject *IoState_tryToPerform(IoState *self,
							  IoObject *target,
							  IoObject *locals,
							  IoMessage *m);

IOVM_API IoObject *IoState_rawOn_doCString_withLabel_(IoState *self,
									IoObject *target,
									const char *s,
									const char *label);

IOVM_API IoObject *IoState_doCString_(IoState *self, const char *s);

IOVM_API IoObject *IoState_on_doCString_withLabel_(IoState *self,
								  IoObject *target,
								  const char *s,
								  const char *label);

//IoObject *IoState_on_doPackedCString_withLabel_(IoState *self, IoObject *target, const char *s, const char *label);

// sandbox

IOVM_API void IoState_zeroSandboxCounts(IoState *self);
IOVM_API void IoState_resetSandboxCounts(IoState *self);
IOVM_API IoObject *IoState_doSandboxCString_(IoState *self, const char *s);
IOVM_API double IoState_endTime(IoState *self);

// file

IOVM_API IoObject *IoState_doFile_(IoState *self, const char *path);

