//metadoc Event copyright Steve Dekorte, 2004
//metadoc Event license BSD revised
//metadoc Event description Networking Event.
//metadoc Event category Networking

#include "IoEvent.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"

#define EVENT(self) ((struct event *)IoObject_dataPointer(self))

IoEvent *IoMessage_locals_eventArgAt_(IoMessage *self, IoObject *locals, int n)
{
	IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

	if (!ISEVENT(v))
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "Event");
	}

	return v;
}

IoTag *IoEvent_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Event");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoEvent_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoEvent_free);
	return tag;
}

IoEvent *IoEvent_proto(void *state)
{
	IoObject *self = IoObject_new(state);

	IoObject_tag_(self, IoEvent_newTag(state));
	IoObject_setDataPointer_(self, (struct event *)calloc(1, sizeof(struct event)));

	IoState_registerProtoWithFunc_((IoState *)state, self, IoEvent_proto);

	{
		IoMethodTable methodTable[] = {
		{"isPending", IoEvent_isPending},
		{"isValid", IoEvent_isValid},
		{NULL, NULL},
		};

		IoObject_addMethodTable_(self, methodTable);
	}

	IoObject_setSlot_to_(self, IOSYMBOL("EV_READ"), IONUMBER(EV_READ));
	IoObject_setSlot_to_(self, IOSYMBOL("EV_WRITE"), IONUMBER(EV_WRITE));
	IoObject_setSlot_to_(self, IOSYMBOL("EV_SIGNAL"), IONUMBER(EV_SIGNAL));
	//IoObject_setSlot_to_(self, IOSYMBOL("EV_TIMEOUT"), IONUMBER(EV_TIMEOUT));
	//IoObject_setSlot_to_(self, IOSYMBOL("EV_PERSIST"), IONUMBER(EV_PERSIST));

	return self;
}

IoEvent *IoEvent_rawClone(IoEvent *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, (struct event *)calloc(1, sizeof(struct event)));
	return self;
}

IoEvent *IoEvent_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoEvent_proto);
	return IOCLONE(proto);
}

void IoEvent_free(IoEvent *self)
{
	// this check ensures that libevent is never holding a referenced
	// to an IoEvent that has been collected

	if (event_initialized(EVENT(self)) && event_pending(EVENT(self), 0, NULL))
	{
		event_del(EVENT(self));
	}

	free(EVENT(self));
}

struct event *IoEvent_rawEvent(IoEvent *self)
{
	return EVENT(self);
}

IoObject *IoEvent_isPending(IoEvent *self, IoObject *locals, IoMessage *m)
{
	int result = event_pending(EVENT(self), 0, NULL);
	return IOBOOL(self, result);
}

IoObject *IoEvent_isValid(IoEvent *self, IoObject *locals, IoMessage *m)
{
	return RawDescriptor_isValid(EVENT(self)->ev_fd) ? IOTRUE(self) : IOFALSE(self);
}
