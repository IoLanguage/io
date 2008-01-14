/*#io
Sandbox ioDoc(
	docCopyright("Steve Dekorte", 2002)
	docLicense("BSD revised")
	docObject("Sandbox")
	docDescription("Sandbox can be used to run separate instances of Io within the same process.")
	docCategory("Core")
*/

#include "IoSandbox.h"
#include "IoSeq.h"
#include "IoState.h"
#include "IoCFunction.h"
#include "IoObject.h"
#include "IoList.h"
#include "IoSeq.h"
#include "UArray.h"
#include "PortableTruncate.h"
#include <errno.h>
#include <stdio.h>

#define DATA(self) ((IoState *)IoObject_dataPointer(self))

IoTag *IoSandbox_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Sandbox");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSandbox_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSandbox_free);
	return tag;
}

IoSandbox *IoSandbox_proto(void *state)
{
	IoMethodTable methodTable[] = {
	{"messageCount", IoSandbox_messageCount},
	{"setMessageCount", IoSandbox_setMessageCount},
	{"timeLimit", IoSandbox_timeLimit},
	{"setTimeLimit", IoSandbox_setTimeLimit},
	{"doSandboxString", IoSandbox_doSandboxString},
	{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoSandbox_newTag(state));

	IoState_registerProtoWithFunc_((IoState *)state, self, IoSandbox_proto);

	IoObject_addMethodTable_(self, methodTable);

	return self;
}

IoState *IoSandbox_boxState(IoSandbox *self)
{
	if (!DATA(self))
	{
		IoObject_setDataPointer_(self, IoState_new());
		IoSandbox_addPrintCallback(self);
	}

	return DATA(self);
}

IoSandbox *IoSandbox_rawClone(IoSandbox *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	return self;
}

void IoSandbox_addPrintCallback(IoSandbox *self)
{
	IoState *boxState = IoSandbox_boxState(self);
	IoState_callbackContext_(boxState, self);
	IoState_printCallback_(boxState, IoSandbox_printCallback);
}

void IoSandbox_printCallback(void *voidSelf, const UArray *ba)
{
	IoSandbox *self = voidSelf;

	IoState *state = IOSTATE;
	IoSeq *buf = IoSeq_newWithUArray_copy_(IOSTATE, (UArray *)ba, 1);
	IoMessage *m = IoMessage_newWithName_(state, IOSYMBOL("printCallback"));
	IoMessage *arg = IoMessage_newWithName_returnsValue_(state, IOSYMBOL("buffer"), buf);
	IoMessage_addArg_(m, arg);
	IoMessage_locals_performOn_(m, state->lobby, self);
}

IoSandbox *IoSandbox_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoSandbox_proto);
	return IOCLONE(proto);
}

void IoSandbox_free(IoSandbox *self)
{
	if (IoObject_dataPointer(self))
	{
		IoState_free(IoSandbox_boxState(self));
	}
}

/* ----------------------------------------------------------- */

IoNumber *IoSandbox_messageCount(IoSandbox *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("messageCount",
			"Returns a number containing the messageCount limit of the Sandbox. ")
	*/

	IoState *boxState = IoSandbox_boxState(self);
	return IONUMBER(boxState->messageCountLimit);
}

IoObject *IoSandbox_setMessageCount(IoSandbox *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setMessageCount(anInteger)",
			"Sets the messageCount limit of the receiver. ")
	*/

	IoState *boxState = IoSandbox_boxState(self);
	boxState->messageCountLimit = IoMessage_locals_intArgAt_(m, locals, 0);
	return self;
}

IoNumber *IoSandbox_timeLimit(IoSandbox *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("timeLimit",
			"Returns a number containing the time limit of calls made to the Sandbox. ")
	*/

	IoState *boxState = IoSandbox_boxState(self);
	return IONUMBER(boxState->timeLimit);
}

IoObject *IoSandbox_setTimeLimit(IoSandbox *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setTimeLimit(aDouble)",
			"Sets the time limit of the Sandbox. ")
	*/

	IoState *boxState = IoSandbox_boxState(self);
	boxState->timeLimit = IoMessage_locals_doubleArgAt_(m, locals, 0);
	return self;
}

IoObject *IoSandbox_doSandboxString(IoSandbox *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("doSandboxString(aString)",
			"Evaluate aString instead the Sandbox. ")
	*/

	IoState *boxState = IoSandbox_boxState(self);
	char *s = IoMessage_locals_cStringArgAt_(m, locals, 0);

	IoObject *result = IoState_doSandboxCString_(boxState, s);

	if (ISSYMBOL(result))
	{
		return IOSYMBOL(CSTRING(result));
	}

	if (ISSEQ(result))
	{
		return IOSEQ(IOSEQ_BYTES(result), IOSEQ_LENGTH(result));
	}

	if (ISNUMBER(result))
	{
		return IONUMBER(CNUMBER(result));
	}

	return IONIL(self);
}
