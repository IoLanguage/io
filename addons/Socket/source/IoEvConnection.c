//metadoc EvRequest copyright Steve Dekorte, 2004
//metadoc EvRequest license BSD revised
//metadoc EvRequest description Networking Event.
//metadoc EvRequest category Networking

#include "IoEvConnection.h"
#include "IoEventManager.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"


#define CONN(self) ((struct evhttp_connection *)IoObject_dataPointer(self))

IoTag *IoEvConnection_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("EvRequest");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoEvConnection_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoEvConnection_free);
	return tag;
}

IoEvConnection *IoEvConnection_proto(void *state)
{
	IoObject *self = IoObject_new(state);

	IoObject_tag_(self, IoEvConnection_newTag(state));
	IoObject_setDataPointer_(self, (struct event *)calloc(1, sizeof(struct event)));

	IoState_registerProtoWithFunc_((IoState *)state, self, IoEvConnection_proto);

	{
		IoMethodTable methodTable[] = {
		{"connect", IoEvConnection_connect},
		{"disconnect", IoEvConnection_disconnect},
		{"setRetries", IoEvConnection_setRetries_},
		{"setTimeout", IoEvConnection_setTimeout_},
		{"setLocalAddress", IoEvConnection_setLocalAddress_},
		{NULL, NULL},
		};

		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoEvConnection *IoEvConnection_rawClone(IoEvConnection *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, 0x0);
	return self;
}

IoEvConnection *IoEvConnection_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoEvConnection_proto);
	return IOCLONE(proto);
}

void IoEvConnection_free(IoEvConnection *self)
{
	//printf("IoEvConnection_free\n");
	
	if (CONN(self))
	{
		//printf("IoEvConnection_free skipping free?\n");
		//evhttp_connection_free(CONN(self));
		IoObject_setDataPointer_(self, 0x0);
	}
}

struct evhttp_connection *IoEvConnection_rawConnection(IoEvConnection *self)
{
	return CONN(self);
}

IoObject *IoEvConnection_setTimeout_(IoEvConnection *self, IoObject *locals, IoMessage *m)
{
	int timeoutInSeconds = IoMessage_locals_intArgAt_(m, locals, 0);
	evhttp_connection_set_timeout(CONN(self), timeoutInSeconds);
	return self;
}

IoObject *IoEvConnection_setRetries_(IoEvConnection *self, IoObject *locals, IoMessage *m)
{
	int retry_max = IoMessage_locals_intArgAt_(m, locals, 0);
	evhttp_connection_set_retries(CONN(self), retry_max);
	return self;
}

IoObject *IoEvConnection_setLocalAddress_(IoEvConnection *self, IoObject *locals, IoMessage *m)
{
	IoSymbol *localAddress = IoMessage_locals_symbolArgAt_(m, locals, 0);
	evhttp_connection_set_local_address(CONN(self), CSTRING(localAddress));
	return self;
}

void IoEvConnection_ConnectionCloseCallback(struct evhttp_connection *con, void *arg)
{
	IoObject *self = arg;
	//printf("IoEvConnection_ConnectionCloseCallback\n");
	IoEvConnection_free(self);
}

IoObject *IoEvConnection_connect(IoEvConnection *self, IoObject *locals, IoMessage *m)
{
	IoEventManager *em = IoObject_getSlot_(self, IOSYMBOL("eventManager"));
	IoSeq *address = IoObject_seqGetSlot_(self, IOSYMBOL("address"));
	int port = IoObject_doubleGetSlot_(self, IOSYMBOL("port"));

	IOASSERT(CONN(self) == 0x0, "already have connection");
	IOASSERT(ISEEVENTMANAGER(em), "eventManager slot not set properly");

	//printf("IoEventManager_rawBase(em) = %p\n", (void *)IoEventManager_rawBase(em));

	IoObject_setDataPointer_(self, evhttp_connection_new(CSTRING(address), port));
	evhttp_connection_set_base(CONN(self), IoEventManager_rawBase(em));
	evhttp_connection_set_closecb(CONN(self), IoEvConnection_ConnectionCloseCallback, self);
	return self;
}

IoObject *IoEvConnection_disconnect(IoEvConnection *self, IoObject *locals, IoMessage *m)
{
	IoEvConnection_free(self);
	return self;
}
