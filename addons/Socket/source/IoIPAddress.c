//metadoc IPAddress copyright Steve Dekorte, 2004
//metadoc IPAddress license BSD revised
//metadoc IPAddress description Object representation of an Internet Protocol Address.
//metadoc IPAddress category Networking

/*doc IPAddress setHostName(hostName)
	Translates hostName to an IP using asynchronous DNS and 
	sets the host attribute. Returns self.
*/

#include "IoIPAddress.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"

#define IPADDRESS(self) ((IPAddress *)IoObject_dataPointer(self))

IoIPAddress *IoMessage_locals_ipAddressArgAt_(IoMessage *self, IoObject *locals, int n)
{
	IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

	if (!ISIPADDRESS(v))
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "IPAddress");
	}

	return v;
}

IPAddress *IoMessage_locals_rawIPAddressArgAt_(IoMessage *self, IoObject *locals, int n)
{
	return IoIPAddress_rawIPAddress(IoMessage_locals_ipAddressArgAt_(self, locals, n));
}

IPAddress *IoIPAddress_rawIPAddress(IoIPAddress *self)
{
	return IPADDRESS(self);
}

IoTag *IoIPAddress_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("IPAddress");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoIPAddress_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoIPAddress_free);
	return tag;
}

IoIPAddress *IoIPAddress_proto(void *state)
{
	IoObject *self = IoObject_new(state);

	IoObject_tag_(self, IoIPAddress_newTag(state));
	IoObject_setDataPointer_(self, IPAddress_new());

	IoState_registerProtoWithFunc_((IoState *)state, self, IoIPAddress_proto);

	{
		IoMethodTable methodTable[] = {
		{"setIp", IoIPAddress_setIp},
		{"ip", IoIPAddress_ip},

		{"setPort", IoIPAddress_setPort},
		{"port", IoIPAddress_port},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoIPAddress *IoIPAddress_rawClone(IoIPAddress *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, IPAddress_new());
	return self;
}

IoIPAddress *IoIPAddress_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoIPAddress_proto);
	return IOCLONE(proto);
}

void IoIPAddress_free(IoIPAddress *self)
{
	IPAddress_free(IPADDRESS(self));
}

// -----------------------------------------------------------

/*
IoIPAddress *IoIPAddress_newWithIPAddress_size_(void *state,
								  struct sockaddr *address,
								  size_t size)
{
	IoIPAddress *self = IoIPAddress_new(state);
	io_free(DATA(self)->address);
	DATA(self)->address = io_malloc(size);
	DATA(self)->size = size;
	memcpy(DATA(self)->address, address, size);
	return self;
}
*/

// ip

IoObject *IoIPAddress_setIp(IoIPAddress *self, IoObject *locals, IoMessage *m)
{
	IoSeq *ip = IoMessage_locals_seqArgAt_(m, locals, 0);
	char *ipString = IoSeq_asCString(ip);
	IPAddress_setIp_(IPADDRESS(self), ipString);
	return self;
}

IoObject *IoIPAddress_ip(IoIPAddress *self, IoObject *locals, IoMessage *m)
{
	char *ip = IPAddress_ip(IPADDRESS(self));
	return IOSYMBOL(ip);
}

// port

IoObject *IoIPAddress_setPort(IoIPAddress *self, IoObject *locals, IoMessage *m)
{
	uint16_t port = IoMessage_locals_intArgAt_(m, locals, 0);
	IPAddress_setPort_(IPADDRESS(self), port);
	return self;
}

IoObject *IoIPAddress_port(IoIPAddress *self, IoObject *locals, IoMessage *m)
{
	int port = IPAddress_port(IPADDRESS(self));
	return IONUMBER(port);
}
