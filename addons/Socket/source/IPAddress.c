#include "IPAddress.h"
#include <stdlib.h>
#include <string.h>

IPAddress *IPAddress_new(void)
{
	IPAddress *self = calloc(1, sizeof(IPAddress));
	self->sockaddr = calloc(1, sizeof(struct sockaddr_in));
	self->size = sizeof(struct sockaddr_in);
	return self;
}

IPAddress *IPAddress_setIPAddress_size_(IPAddress *self,
							   struct sockaddr *address,
							   size_t size)
{
	self->sockaddr = realloc(self->sockaddr, size);
	memcpy(self->sockaddr, address, size);
	self->size = size;
	return self;
}

void IPAddress_free(IPAddress *self)
{
	free(self->sockaddr);
	 free(self);
}

// access

struct sockaddr *IPAddress_sockaddr(IPAddress *self)
{
	return (struct sockaddr *)(self->sockaddr);
}

socklen_t IPAddress_size(IPAddress *self)
{
	return self->size;
}

void IPAddress_setSize_(IPAddress *self, socklen_t size)
{
	self->size = size;
}

// ip

void IPAddress_setIp_(IPAddress *self, char *ip)
{
	self->sockaddr->sin_family = AF_INET;
	self->sockaddr->sin_addr.s_addr = inet_addr(ip);
}

char *IPAddress_ip(IPAddress *self)
{
	return inet_ntoa(self->sockaddr->sin_addr);
}

// port

void IPAddress_setPort_(IPAddress *self, uint16_t port)
{
	self->sockaddr->sin_port = htons(port);
}

uint16_t IPAddress_port(IPAddress *self)
{
	return ntohs(self->sockaddr->sin_port);
}
