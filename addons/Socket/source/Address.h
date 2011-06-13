#ifndef ADDRESS_DEFINED
#define ADDRESS_DEFINED 1

#include "Common.h"

#if !defined(_WIN32) || defined(__CYGWIN__)
#include <unistd.h>
#include <sys/socket.h>
#else
#include <ws2tcpip.h>
#endif

typedef struct sockaddr * (Address_sockaddr_func) (void *self);
typedef socklen_t (Address_size_func)(void *self);
typedef void (Address_setSize_func)(void *self, socklen_t size);
typedef int (Address_family_func)(void *self);

typedef struct
{
	void *addr;
	Address_sockaddr_func *addrSockaddrFunc;
	Address_size_func *addrSizeFunc;
	Address_setSize_func *addrSetSizeFunc;
	Address_family_func *addrFamilyFunc;
} Address;

Address *Address_new(void);
void Address_free(Address *self);
Address *Address_newWithIPAddress(void *ip);
#if !defined(_WIN32) || defined(__CYGWIN__)
Address *Address_newWithUnixPath(void *unixpath);
#endif

struct sockaddr *Address_sockaddr(Address *self);
socklen_t Address_size(Address *self);
void Address_setSize_(Address *self, socklen_t size);
int Address_family(Address *self);

#endif
