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

#include <signal.h>

static IoState *stateToReceiveControlC = NULL;
static int multipleIoStates = 0;

void IoState_UserInterruptHandler(int sig) 
{
	printf("\nIo received user interrupt. Calling System userInterruptHandler.\n");
	
	if(multipleIoStates)
	{
		// what could we do here to tell which IoState we are in?
		// maybe look 
		printf("Unable to print stack trace since multiple IoStates are in use.\n");
	}
	else
	{
		IoState *self = stateToReceiveControlC;
		IoObject *system = IoState_doCString_(self, "System");
		IoMessage *m = IoMessage_newWithName_(self, SIOSYMBOL("userInterruptHandler"));
		IoMessage_locals_performOn_(m, system, system); 
	}
	
	//printf("\nIo exiting.\n");
}

void IoState_setupUserInterruptHandler(IoState *self)
{
	if(stateToReceiveControlC) 
	{ 
		multipleIoStates = 1;
	}
	
	stateToReceiveControlC = self;
	signal(SIGINT, IoState_UserInterruptHandler);
}
