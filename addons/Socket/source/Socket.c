
//metadoc Socket copyright Steve Dekorte 2002
//metadoc Socket license BSD revised
//metadoc Socket category Networking

#include "Socket.h"
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#endif

/*
#ifdef WIN32
#define EINPROGRESS 36
#define EDQUOT 69
#endif
*/

/*
int
getpeername(int s, struct sockaddr *name, int *namelen);
getsockname
*/

void Socket_ignoreSignal(int sig)
{
	//printf("Socket_ignoreSignal(%i)\n", sig);
}

void Socket_GlobalInit(void)
{
/*
#ifndef WIN32
	signal(SIGPIPE, Socket_ignoreSignal);
#endif
*/

#ifdef WIN32
	{
		WSADATA wsd;
		WSAStartup(MAKEWORD(2,0), &wsd);
	}
#else
	signal(SIGPIPE, Socket_ignoreSignal);
#endif
}

void Socket_GlobalCleanup(void)
{
#ifdef WIN32
	WSACleanup();
#endif
}


#ifdef WIN32
long Socket_SetDescriptorLimitToMax(void)
{
	return -1;
}
#else

#include <sys/resource.h>

long Socket_SetDescriptorLimitToMax(void)
{
	struct rlimit rlp;
	
	// if were root, we could also do:
	// system("sysctl -w kern.maxfiles=1000001");
	// system("sysctl -w kern.maxfilesperproc=1000000");

	if (getrlimit(RLIMIT_NOFILE, &rlp) != 0)
	{
		printf("getrlimit(RLIMIT_NOFILE, &rlp) != 0\n");
		return -1;
	}

	//printf("rlp.rlim_cur = %i\n", (int)rlp.rlim_cur);
	//printf("rlp.rlim_max = %i\n", (int)rlp.rlim_max);

	rlp.rlim_cur = 256; //rlp.rlim_max;

	do
	{
		rlp.rlim_cur *=2; 

		if (setrlimit(RLIMIT_NOFILE, &rlp) != 0)
		{
			break;
		}

		if (getrlimit(RLIMIT_NOFILE, &rlp) != 0)
		{
			break;
		}
		
	} while(rlp.rlim_cur < 64000);

	//printf(" Socket: max file descriptors = %i\n", (int)rlp.rlim_cur);

	return (long)(rlp.rlim_cur);
}
#endif

// -----------------------------------------------------

Socket *Socket_new(void)
{
	Socket *self = (Socket *)io_calloc(1, sizeof(Socket));
#ifdef WIN32
	self->fd = INVALID_SOCKET;
#else
	self->fd = -1;
#endif

	self->af = 0;
	
	return self;
}

void Socket_free(Socket *self)
{
	Socket_close(self);
	io_free(self);
}

void Socket_setDescriptor_(Socket *self, SOCKET_DESCRIPTOR fd)
{
	self->fd = fd;
}

SOCKET_DESCRIPTOR Socket_descriptor(Socket *self)
{
	return self->fd;
}

// ----------------------------------------

int Socket_makeReusable(Socket *self)
{
	int val = 1;

	SocketResetErrorStatus();
	if (setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(int)) < 0)
	{
		return 0;
	}

#ifdef SO_REUSEPORT
	SocketResetErrorStatus();
	if (setsockopt(self->fd, SOL_SOCKET, SO_REUSEPORT, (char *)&val, sizeof(int)) < 0)
	{
		return 0;
	}
#endif

	return 1;
}

int Socket_makeAsync(Socket *self)
{
#ifdef WIN32
	unsigned long nonzero = 1;

	SocketResetErrorStatus();
	if (ioctlsocket(self->fd, FIONBIO, &nonzero) == -1)
#else
	SocketResetErrorStatus();
	if (fcntl(self->fd, F_SETFL, FASYNC | O_NONBLOCK) == -1)
#endif
	{
		return 0;
	}

	return 1;
}

// ----------------------------------------

int Socket_isStream(Socket *self)
{
	int ok, optval;
	socklen_t optlen;

	SocketResetErrorStatus();

	ok = getsockopt(self->fd, SOL_SOCKET, SO_TYPE, (void *)(&optval), &optlen);

	if (ok != 0)
	{
		return 0;
	}

	return optval == SOCK_STREAM;
}

int Socket_isOpen(Socket *self)
{
#ifdef WIN32
	return self->fd != INVALID_SOCKET;
#else
	return self->fd != -1;
#endif
}

int RawDescriptor_isValid(int fd)
{
#ifdef WIN32
	// ask how many bytes there are to read. Not because we
	// want to know that but just if the socket is ok.
	u_long iToRead = 0;
	if (ioctlsocket((SOCKET_DESCRIPTOR) fd, FIONREAD, &iToRead) == SOCKET_ERROR && SocketErrorStatus() == WSAENOTSOCK)
#else
	if (fcntl(fd, F_GETFL, NULL) == -1)
#endif
	{
		return 0;
	}
	return 1;
}

int Socket_isValid(Socket *self)
{
	return RawDescriptor_isValid(self->fd);
}

// ----------------------------------------

int Socket_streamOpen(Socket *self)
{
	SocketResetErrorStatus();
	self->fd = socket(self->af, SOCK_STREAM, 0);

	return Socket_isOpen(self);
}

int Socket_udpOpen(Socket *self)
{
	SocketResetErrorStatus();
	self->fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	return Socket_isOpen(self);
}

// ----------------------------------------

int Socket_connectTo(Socket *self, Address *addr)
{
	int result;

	SocketResetErrorStatus();
	result = connect(self->fd, Address_sockaddr(addr), Address_size(addr));

#ifdef WIN32	
	return result == 0 || SocketErrorStatus() == WSAEISCONN;
#else
	return result == 0 || SocketErrorStatus() == EISCONN;
#endif
}

int Socket_connectToFailed(void)
{
	int errorNumber = SocketErrorStatus();
#ifdef WIN32
	return errorNumber && errorNumber != WSAEINPROGRESS && errorNumber != WSAEALREADY && errorNumber != WSAEWOULDBLOCK && errorNumber != WSAEINVAL;
#else
	return errorNumber && errorNumber != EINPROGRESS && errorNumber != EALREADY && errorNumber != EINTR;
#endif
}

int Socket_close(Socket *self)
{
	int result = 0;
#ifdef WIN32
	if (self->fd != INVALID_SOCKET)
	{
		result = closesocket(self->fd);
	}
	self->fd = INVALID_SOCKET;
#else
	if (self->fd != -1)
	{
		result = close(self->fd);
	}
	self->fd = -1;
#endif
	return result == 0;
}

int Socket_closeFailed(void)
{
	int errorNumber = SocketErrorStatus();
#ifdef WIN32
	return errorNumber;
#else
	return errorNumber && errorNumber != EINTR;
#endif
}

int Socket_bind(Socket *self, Address *addr)
{
	int result;

	SocketResetErrorStatus();
	result = bind(self->fd, Address_sockaddr(addr), Address_size(addr));

	return result == 0;
}

int Socket_listen(Socket *self)
{
	int result;

	SocketResetErrorStatus();

	// returns 0 of ok, -1 on error
	result = listen(self->fd, SOMAXCONN);
	//if (r == -1) Socket_checkErrorNumber(self);
	return result == 0;
}

int Socket_asyncFailed(void)
{
	int errorNumber = SocketErrorStatus();
#ifdef WIN32
	return errorNumber && errorNumber != WSAEWOULDBLOCK;
#else
	return errorNumber && errorNumber != EWOULDBLOCK && errorNumber != EAGAIN && errorNumber != EINTR;
#endif
}

Socket *Socket_accept(Socket *self, Address *addr)
{
	socklen_t addressSize = Address_size(addr);
	SOCKET_DESCRIPTOR d;

	SocketResetErrorStatus();

	d = accept(self->fd, Address_sockaddr(addr), &addressSize);

	Address_setSize_(addr, addressSize);

	if (d != -1)
	{
		Socket *socket = Socket_new();
		Socket_setDescriptor_(socket, d);
		if (Socket_makeReusable(socket) && Socket_makeAsync(socket))
			return socket;
		else
			return NULL;
	}
	
	return NULL;
}

// stream ----------------------------------------

ssize_t Socket_streamRead(Socket *self, UArray *buffer, size_t readSize)
{
	ssize_t bytesRead;
	size_t originalSize = UArray_sizeInBytes(buffer);

	UArray_sizeTo_(buffer, originalSize + readSize + 1);

	SocketResetErrorStatus();
	
#ifdef WIN32
	bytesRead = (size_t)recv(self->fd, (uint8_t *)UArray_bytes(buffer) + originalSize, readSize, 0);
#else
	bytesRead = (size_t)read(self->fd, (uint8_t *)UArray_bytes(buffer) + originalSize, readSize);
#endif
	
	if (bytesRead > 0)
	{
		UArray_setSize_(buffer, originalSize + bytesRead);
	}
	else
	{
		UArray_setSize_(buffer, originalSize);
		// Maintain truthy return value semantics
		bytesRead = 0;
	}

	return bytesRead;
}

ssize_t Socket_streamWrite(Socket *self, UArray *buffer, size_t start, size_t writeSize)
{
	size_t bufferSize = UArray_sizeInBytes(buffer);
	ssize_t bytesWritten;

	if (start > bufferSize)
	{
		return 0;
	}

	if (start + writeSize > bufferSize)
	{
		writeSize = bufferSize - start;
	}

	SocketResetErrorStatus();

#ifdef WIN32
	bytesWritten = send(self->fd, UArray_bytes(buffer) + start, writeSize, 0);
#else
	bytesWritten = write(self->fd, UArray_bytes(buffer) + start, writeSize);
#endif

	// Maintain truthy return value semantics
	bytesWritten = (bytesWritten < 0) ? 0 : bytesWritten;
	return bytesWritten;
}

ssize_t Socket_udpRead(Socket *self, Address *addr, UArray *buffer, size_t readSize)
{
	socklen_t addressSize = Address_size(addr);
	size_t oldSize = UArray_sizeInBytes(buffer);
	ssize_t bytesRead;

	UArray_setItemType_(buffer, CTYPE_uint8_t);

	UArray_sizeTo_(buffer, oldSize + readSize);

	SocketResetErrorStatus();

	bytesRead = recvfrom(self->fd, (uint8_t *)UArray_bytes(buffer), readSize, 0, Address_sockaddr(addr), &addressSize);

	if (bytesRead > 0)
	{
		UArray_setSize_(buffer, oldSize + bytesRead);
		Address_setSize_(addr, addressSize);
	}
	else
	{
		UArray_setSize_(buffer, oldSize);
		// Maintain truthy return value semantics
		bytesRead = 0;
	}

	return bytesRead;
}

ssize_t Socket_udpWrite(Socket *self, Address *addr, UArray *buffer, size_t start, size_t writeSize)
{
	size_t bufferSize = UArray_sizeInBytes(buffer);
	ssize_t bytesWritten;

	if (start > bufferSize) return 0;

	if (start + writeSize > bufferSize)
	{
		writeSize = bufferSize - start;
	}

	SocketResetErrorStatus();

	bytesWritten = sendto(self->fd, UArray_bytes(buffer), writeSize, 0, Address_sockaddr(addr), Address_size(addr));

	// Maintain truthy return value semantics
	bytesWritten = (bytesWritten < 0) ? 0 : bytesWritten;
	return bytesWritten;
}

// ----------------------------------------

char *Socket_errorDescription(void)
{
	int err = SocketErrorStatus();
#ifdef WIN32
	if (err)
	{
		char *tmpBuffer;
		char *messageBuffer;

		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, err, 0, &tmpBuffer, 0, NULL);
		messageBuffer = calloc(strlen(tmpBuffer) + 1, 1);
		strcpy(messageBuffer, tmpBuffer);
		LocalFree(tmpBuffer);
		return messageBuffer;
	}
	else 
	{
		return "";
	}
#else
	return err ? strerror(err) : "";
#endif
}