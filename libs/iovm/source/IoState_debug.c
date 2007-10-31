#include "IoState.h"
#include "IoObject.h"

void IoState_show(IoState *self)
{
	printf("--- state ----------------------------------\n");
	printf("State:\n");
	/*
	 printf("black:\n");
	 IoObjectGroup_show(self->blackGroup);
	 printf("\n");

	 printf("gray:\n");
	 IoObjectGroup_show(self->grayGroup);
	 printf("\n");

	 printf("white:\n");
	 IoObjectGroup_show(self->whiteGroup);
	 printf("\n");
	 */
	printf("stacks:\n");
	printf("\n");
}

IoObject *IoState_replacePerformFunc_with_(IoState *self,
								   IoTagPerformFunc *oldFunc,
								   IoTagPerformFunc *newFunc)
{
	PHASH_FOREACH(self->primitives, k, v,
		{
		IoObject *proto = v;
		IoTag *tag = IoObject_tag(proto);
		if (tag->performFunc == oldFunc || !tag->performFunc)
		{
			tag->performFunc = newFunc;
		}
		}
	);

	return NULL;
}

void IoState_debuggingOn(IoState *self)
{
	IoState_replacePerformFunc_with_(self,
							   (IoTagPerformFunc *)IoObject_perform,
							   (IoTagPerformFunc *)IoObject_performWithDebugger);
}

void IoState_debuggingOff(IoState *self)
{
	IoState_replacePerformFunc_with_(self,
							   (IoTagPerformFunc *)IoObject_performWithDebugger,
							   (IoTagPerformFunc *)IoObject_perform);
}

int IoState_hasDebuggingCoroutine(IoState *self)
{
	return 1; // hack awaiting decision on how to change this
}

void IoState_updateDebuggingMode(IoState *self)
{
	if (IoState_hasDebuggingCoroutine(self))
	{
		IoState_debuggingOn(self);
	}
	else
	{
		IoState_debuggingOff(self);
	}
}
