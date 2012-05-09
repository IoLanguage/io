//metadoc EvDNSRequest copyright Steve Dekorte, 2004
//metadoc EvDNSRequest license BSD revised
//metadoc EvDNSRequest description libevent DNS support 
//metadoc EvDNSRequest category Networking

#include "IoEvDNSRequest.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoMap.h"
#include "IoSeq.h"
#include "EvDNSRequest.h" 

#define REQUEST(self) ((struct evdns_request *)IoObject_dataPointer(self))

EvDNSRequest_resolv_conf_parse(DNS_OPTIONS_ALL, "/etc/resolv.conf");

static const char *protoId = "EvDNSRequest";

IoTag *IoEvDNSRequest_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoEvDNSRequest_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoEvDNSRequest_free);
	return tag;
}

IoEvDNSRequest *IoEvDNSRequest_proto(void *state)
{
	IoObject *self = IoObject_new(state);

	IoObject_tag_(self, IoEvDNSRequest_newTag(state));
	IoObject_setDataPointer_(self, 0x0);

	IoState_registerProtoWithFunc_((IoState *)state, self, IoEvDNSRequest_proto);

	{
		IoMethodTable methodTable[] = {
		{"resolveIPv4", IoEvDNSRequest_resolveIPv4},
		/*
		{"resolveIPv6", IoEvDNSRequest_resolveIPv6},
		{"resolveReverseIPv4", IoEvDNSRequest_resolveReverseIPv4},
		{"resolveReverseIPv6", IoEvDNSRequest_resolveReverseIPv6},
		*/
		{NULL, NULL},
		};

		IoObject_addMethodTable_(self, methodTable);
	} 


	return self;
}

IoEvDNSRequest *IoEvDNSRequest_rawClone(IoEvDNSRequest *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, 0x0);
	return self;
}

IoEvDNSRequest *IoEvDNSRequest_new(void *state)
{
	IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
	return IOCLONE(proto);
}

void IoEvDNSRequest_free(IoEvDNSRequest *self)
{
	if (REQUEST(self))
	{	
		// looks like evhttpd is responsible for freeing the request
		IoObject_setDataPointer_(self, 0x0);
	}
}

static int get32(unsigned char *p)
{
	return ((p[0] * 256 + p[1]) * 256 + p[2]) * 256 + p[3];
}

void IoEvDNSRequest_callback(int result, char type, int count, int ttl, void *addresses, void *arg)
{
	IoEvDNSRequest *self = arg;
	//type is either DNS_IPv4_A or DNS_PTR or DNS_IPv6_AAAA

	IoObject_setSlot_to_(self, IOSYMBOL("ttl"), IONUMBER(ttl));

	if(result == DNS_ERR_NONE)
	{
		IoList *ioAddresses = IoList_new(IOSTATE);
		IoObject_setSlot_to_(self, IOSYMBOL("addresses"), ioAddresses);
		
		int i;
		for (i = 0; i < count; i ++)
		{
			//addresses needs to be cast according to type
			uint32_t a = ((uint32_t *)addresses)[i];
			struct in_addr addr;
			char *ip;
			addr.s_addr = htonl(get32(rr->rdata));
			ip = (char *)inet_ntoa(addr);
			IoList_rawAppend_(ioAddresses, IOSYMBOL(ip));
		}
	}
	else 
	{
		IoObject_setSlot_to_(self, IOSYMBOL("error"), IOSYMBOL("fail"));
	}

	
	IoMessage *m = IoMessage_newWithName_label_(IOSTATE, IOSYMBOL("handleResponse"), IOSYMBOL("IoEvDNSRequest"));
	IoMessage_locals_performOn_(m, self, self);
}


IoObject *IoEvDNSRequest_resolveIPv4(IoEvDNSRequest *self, IoObject *locals, IoMessage *m)
{	
	IoSeq *host = IoObject_seqGetSlot_(self, IOSYMBOL("host"));
	IoEvDNS *evDns = IoObject_getSlot_(self, IOSYMBOL("evDns"));
	IOASSERT(ISEVDNS(evDns), "evDns slot not set properly");
	
	struct evdns_base *dnsBase = IoObject_dataPointer(evDns);
	struct evdns_request *req = evdns_base_resolve_ipv4(dnsBase, 
		CSTRING(host),
		DNS_QUERY_NO_SEARCH,
		(EvDNSRequest_callback_type)EvDNSRequest_callback,
		(void *)self 
	);
	
	IoObject_setDataPointer_(self, req);
	
	return self;
}

