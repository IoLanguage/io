#include "IPAddress.h"
#include "UnixPath.h"
#include "Address.h"
#include <stdlib.h>
#include <string.h>

Address *Address_new(void)
{
	Address *self = io_calloc(1, sizeof(Address));
	memset(self, 0, sizeof(Address));
	return self;
}

Address *Address_newWithIPAddress(void *ip)
{
	Address *self = Address_new();

	self->addr = (void *)ip;
	self->addrSockaddrFunc = (Address_sockaddr_func *)IPAddress_sockaddr;
	self->addrSizeFunc = (Address_size_func *)IPAddress_size;
	self->addrSetSizeFunc = (Address_setSize_func *)IPAddress_setSize_;
	self->addrFamilyFunc = (Address_family_func *)IPAddress_family;
	
	return self;
}

#if !defined(_WIN32) || defined(__CYGWIN__)
Address *Address_newWithUnixPath(void *unixpath)
{
	Address *self = Address_new();

	self->addr = (void *)unixpath;
	self->addrSockaddrFunc = (Address_sockaddr_func *)UnixPath_sockaddr;
	self->addrSizeFunc = (Address_size_func *)UnixPath_size;
	self->addrSetSizeFunc = (Address_setSize_func *)UnixPath_setSize_;
	self->addrFamilyFunc = (Address_family_func *)UnixPath_family;

	return self;
}
#endif

void Address_free(Address *self)
{
	io_free(self);
}



// access

struct sockaddr *Address_sockaddr(Address *self)
{
	return (struct sockaddr *)(self->addrSockaddrFunc(self->addr));
}

socklen_t Address_size(Address *self)
{
	return self->addrSizeFunc(self->addr);
}

void Address_setSize_(Address *self, socklen_t size)
{
	self->addrSetSizeFunc(self->addr, size);
}

int Address_family(Address *self)
{
	self->addrFamilyFunc(self->addr);
}


