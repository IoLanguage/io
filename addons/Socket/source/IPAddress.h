
//metadoc IPAddress copyright Steve Dekorte 2002
//metadoc IPAddress license BSD revised

#ifndef IPADDRESS_DEFINED
#define IPADDRESS_DEFINED 1

//#include "Socket.h"
#include "Common.h"
#include "Address.h"

#include <sys/types.h>
#ifndef WIN32
#include <sys/uio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#else
#include <ws2tcpip.h>
#include <winsock2.h>

// for dog4
#ifndef IO_WINSOCK_COMPAT
#define IO_WINSOCK_COMPAT
typedef size_t socklen_t;
typedef SSIZE_T ssize_t;
#endif

#ifndef EISCONN
#define EISCONN WSAEISCONN
#define ENOTCONN WSAENOTCONN
#endif
#endif

typedef struct
{
	socklen_t size;
	struct sockaddr_in *sockaddr;
	Address *addr;
} IPAddress;

IPAddress *IPAddress_new(void);
IPAddress *IPAddress_setIPAddress_size_(IPAddress *self, struct sockaddr *sockaddr, size_t size);
void IPAddress_free(IPAddress *self);

struct sockaddr *IPAddress_sockaddr(IPAddress *self);
socklen_t IPAddress_size(IPAddress *self);
void IPAddress_setSize_(IPAddress *self, socklen_t size);

uint16_t IPAddress_family(IPAddress *self);

// ip

void IPAddress_setIp_(IPAddress *self, char *ip);
char *IPAddress_ip(IPAddress *self);

// port

void IPAddress_setPort_(IPAddress *self, uint16_t port);
uint16_t IPAddress_port(IPAddress *self);

#endif
