#ifndef UNIXPATH_DEFINED
#define UNIXPATH_DEFINED 1

#include "Common.h"
#include "Address.h"

#if !defined(_WIN32) || defined(__CYGWIN__)
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#else
#include <ws2tcpip.h>
#include <winsock2.h>
#endif

#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX 108
#endif

typedef struct
{
	socklen_t size;
	struct sockaddr_un *sockaddr;
	Address *addr;
} UnixPath;

UnixPath *UnixPath_new(void);
UnixPath *UnixPath_setUnixPath_size_(UnixPath *self, struct sockaddr *sockaddr, size_t size);
void UnixPath_free(UnixPath *self);

struct sockaddr *UnixPath_sockaddr(UnixPath *self);
socklen_t UnixPath_size(UnixPath *self);
void UnixPath_setSize_(UnixPath *self, socklen_t size);

uint16_t UnixPath_family(UnixPath *self);

void UnixPath_setPath_(UnixPath *self, char *path);
char *UnixPath_path(UnixPath *self);

#endif
