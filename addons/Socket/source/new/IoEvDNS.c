//metadoc EvDNS copyright Steve Dekorte, 2004
//metadoc EvDNS license BSD revised
//metadoc EvDNS description libevent DNS support 
//metadoc EvDNS category Networking

#include "IoEvDNS.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoMap.h"
#include "IoSeq.h"
#include "evdns.h" 

#define DNS(self) ((struct evdns_base *)IoObject_dataPointer(self))

IoTag *IoEvDNS_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("EvDNS");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoEvDNS_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoEvDNS_free);
	return tag;
}

IoEvDNS *IoEvDNS_proto(void *state)
{
	IoObject *self = IoObject_new(state);

	IoObject_tag_(self, IoEvDNS_newTag(state));
	IoObject_setDataPointer_(self, 0x0);

	IoState_registerProtoWithFunc_((IoState *)state, self, IoEvDNS_proto);

	{
		IoMethodTable methodTable[] = {
		{"newRequest", IoEvDNS_newRequest},
		{NULL, NULL},
		};

		IoObject_addMethodTable_(self, methodTable);
	} 


	return self;
}

IoEvDNS *IoEvDNS_rawClone(IoEvDNS *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, 0x0);
	return self;
}

IoEvDNS *IoEvDNS_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoEvDNS_proto);
	return self; // singleton
}

void IoEvDNS_free(IoEvDNS *self)
{
	if (DNS(self))
	{	
		int fail_requests = 0;
		evdns_base_free(DNS(self), fail_requests);	
		IoObject_setDataPointer_(self, 0x0);
	}
}

void IoEvDNS_rawInitIfNeeded(IoEvDNS *self)
{
	if (!DNS(self))
	{
		IoEventManager *em = IoObject_getSlot_(self, IOSYMBOL("eventManager"));
		IOASSERT(ISEEVENTMANAGER(em), "eventManager slot not set properly");
		
		struct event_base *base = evhttp_new(IoEventManager_rawBase(em);
		int initialize_nameservers = 1;
		struct evdns_base *dnsBase = evdns_base_new(base, initialize_nameservers);	
		IoObject_setDataPointer_(self, dnsBase);
	}
}

	
IoObject *IoEvDNS_newRequest(IoEvDNS *self, IoObject *locals, IoMessage *m)
{
	IoEvDNS_rawInitIfNeeded(self);
	
	IoEvDNSRequest *r = IoEvDNSRequest_new(IOSTATE);
	
	return r;
}

