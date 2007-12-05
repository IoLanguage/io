/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/
#include "IoVMApi.h"

IOVM_API void IoState_fatalError_(IoState *self, char *error);

IOVM_API void IoState_error_(IoState *self, IoMessage *m, const char *format, ...);
