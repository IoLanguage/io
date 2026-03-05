#include "IoState.h"
#include <stdio.h>

void IoAddonsInit(IoObject *context);

//#define IOBINDINGS

int main(int argc, const char *argv[])
{
	int exitResult;
	IoState *self;

	self = IoState_new();
#ifdef IOBINDINGS
	IoState_setBindingsInitCallback(self, (IoStateBindingsInitCallback *)IoAddonsInit);
#endif
	IoState_init(self);
	IoState_argc_argv_(self, argc, argv);
	//IoState_doCString_(self, "some test code...");
	IoState_runCLI(self);
	exitResult = IoState_exitResult(self);

	IoState_free(self);

	//printf("exitResult = %i\n", exitResult);
	return exitResult;
}
