//metadoc Socket copyright Steve Dekorte 2002
//metadoc Socket license BSD revised
/*metadoc description
See the release notes for a list of folks that helped with this code.
*/

#ifndef PORTABLESOCKET_DEFINED
#define PORTABLESOCKET_DEFINED 1

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
#include <winsock2.h>
#define ECONNRESET WSAECONNRESET

// for dog4
#ifndef IO_WINSOCK_COMPAT
#define IO_WINSOCK_COMPAT
typedef size_t socklen_t;
typedef SSIZE_T ssize_t;
#endif

#endif

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

#ifdef sun
#include <sys/file.h> /* for FASYNC */
#endif

#ifndef SOMAXCONN
#define SOMAXCONN 128
#endif

#ifdef WIN32
typedef SOCKET SOCKET_DESCRIPTOR;
#define SocketResetErrorStatus() WSASetLastError(0)
#define SocketErrorStatus() WSAGetLastError()
#else
typedef int SOCKET_DESCRIPTOR;
#define SocketResetErrorStatus() errno = 0
#define SocketErrorStatus() errno
#endif

#include "Address.h"
#include "UArray.h"
#include "Common.h"

typedef struct
{
	SOCKET_DESCRIPTOR fd;
	uint16_t af;
} Socket;

void Socket_GlobalInit(void);
void Socket_GlobalCleanup(void);
long Socket_SetDescriptorLimitToMax(void);

Socket *Socket_new(void);
void Socket_free(Socket *self);

//void Socket_setError_(Socket *self, const char *e);
//const char *Socket_error(Socket *self);

void Socket_setDescriptor_(Socket *self, SOCKET_DESCRIPTOR fd);
SOCKET_DESCRIPTOR Socket_descriptor(Socket *self);;

int Socket_isStream(Socket *self);
int Socket_isOpen(Socket *self);
int RawDescriptor_isValid(int fd);
int Socket_isValid(Socket *self);

int Socket_makeReusable(Socket *self);
int Socket_makeAsync(Socket *self);

int Socket_streamOpen(Socket *self);
int Socket_udpOpen(Socket *self);

int Socket_connectTo(Socket *self, Address *addr);
int Socket_connectToFailed(void);
int Socket_close(Socket *self);
int Socket_closeFailed(void);

int Socket_bind(Socket *self, Address *addr);
int Socket_listen(Socket *self);
int Socket_asyncFailed(void);
Socket *Socket_accept(Socket *self, Address *addr);

ssize_t Socket_streamRead(Socket *self, UArray *buffer, size_t readSize);
ssize_t Socket_streamWrite(Socket *self, UArray *buffer, size_t start, size_t writeSize);

ssize_t Socket_udpRead(Socket *self, Address *addr, UArray *buffer, size_t readSize);
ssize_t Socket_udpWrite(Socket *self, Address *addr, UArray *buffer, size_t start, size_t writeSize);

char *Socket_errorDescription(void);
#endif
