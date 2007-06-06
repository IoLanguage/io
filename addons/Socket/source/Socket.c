/*
 docCopyright("Steve Dekorte", 2002)
 docLicense("BSD revised")
 see: http://itamarst.org/writings/win32sockets.html for windows socket gotchas
 */

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
		return -1;
	}
	
	//printf("rlp.rlim_cur = %i\n", (int)rlp.rlim_cur);
	//printf("rlp.rlim_max = %i\n", (int)rlp.rlim_max);
	
	rlp.rlim_cur = rlp.rlim_max;
	
	if (setrlimit(RLIMIT_NOFILE, &rlp) != 0)
	{
		return -2;
	}
	
     if (getrlimit(RLIMIT_NOFILE, &rlp) != 0)
	{
		return -3;
	}
	
	//printf(" max listeners = %i\n", (int)rlp.rlim_cur);
	
	return (long)(rlp.rlim_cur);
}
#endif

// -----------------------------------------------------

Socket *Socket_new(void)
{
	Socket *self = (Socket *)calloc(1, sizeof(Socket));
	self->fd = -1;
	return self;
}

void Socket_free(Socket *self)
{
	free(self);
}

void Socket_setDescriptor_(Socket *self, SOCKET_DESCRIPTOR fd) 
{ 
	self->fd = fd; 
}

SOCKET_DESCRIPTOR Socket_descriptor(Socket *self) 
{ 
	return self->fd; 
}

int Socket_streamOpen(Socket *self)
{
	SocketResetErrorStatus();
	self->fd = socket(AF_INET, SOCK_STREAM, 0);
	return self->fd;
}

int Socket_udpOpen(Socket *self)
{
	SocketResetErrorStatus();
	self->fd = socket(AF_INET, SOCK_DGRAM, 0); 
	return self->fd;
}

int Socket_openFifo(Socket *self, const char *path)
{
#ifdef WIN32
	return 1;
#else
	SocketResetErrorStatus();
	self->fd = open(path, O_RDONLY | O_NONBLOCK);
	return self->fd;
#endif
}

int Socket_openReadOnlyNonBlockingAtPath_(Socket *self, char *path)
{
#ifdef WIN32
	return -1;
#else
	self->fd = open(path, O_RDONLY | O_NONBLOCK);
	return self->fd;
#endif
}

int Socket_isOpen(Socket *self) 
{
#ifdef WIN32
	return self->fd != INVALID_SOCKET ? 1 : 0; 
#else
    return self->fd > -1 ? 1 : 0; 
#endif
}

int RawDescriptor_isValid(int fd) 
{
#ifndef WIN32
	//printf("fcntl(fd, F_GETFL, NULL) = %i\n", fcntl(fd, F_GETFL, NULL));

	if (fcntl(fd, F_GETFL, NULL) == -1)
	{
		//printf("RawDescriptor_isValid: descriptor %i returns -1 for fcntl()\n", fd);
		return 0;
	}
#else
        // ask how many bytes there are to read. Not because we
        // want to know that but just if the socket is ok. 
        u_long iToRead = 0;
        if (ioctlsocket(fd, FIONREAD, &iToRead) == WSAENOTSOCK) 
        {
                return 0;
        }
#endif        
	
	return 1;
}

int Socket_isValid(Socket *self) 
{
	int fd = self->fd;
	
	if (!RawDescriptor_isValid(fd))
	{
		//printf("RawDescriptor_isValid: file descriptor %i error: %i %s\n", fd, optval, strerror(optval));
		Socket_close(self);
		return 0;
	}
		
	//printf("RawDescriptor_isValid: file descriptor %i ok", fd);
	
	return 1;
}

int Socket_flush(Socket *self)
{
	//flush(self->fd);
	return 0;
}

int Socket_close(Socket *self)
{
	int r = 0;
#ifdef WIN32
	if (self->fd != INVALID_SOCKET)
	{
		r = closesocket(self->fd);
	}
#else
	if (self->fd != -1)
	{
		r = close(self->fd);
	}
#endif
	self->fd = -1;
	return r;
}

int Socket_makeReusable(Socket *self) 
{
	int val = 1;
	
	if (setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(int)) < 0)
	{
		return -1;
	}

#ifdef SO_REUSEPORT
	if (setsockopt(self->fd, SOL_SOCKET, SO_REUSEPORT, (char *)&val, sizeof(int)) < 0)
	{
		return -1;
	}
#endif

	return 0;
}

int Socket_makeAsync(Socket *self)
{
#ifdef WIN32
	unsigned long nonzero = 1;
	
	SocketResetErrorStatus();
	if (ioctlsocket(self->fd, FIONBIO, &nonzero) == -1)
#else
		if (fcntl(self->fd, F_SETFL, FASYNC | O_NONBLOCK) == -1) 
#endif
		{ 
			return -1; 
		}
			
		return 0;
}

int Socket_connectTo(Socket *self, IPAddress *address)
{
	int result;
        int eno;
	
	SocketResetErrorStatus();
	
	result = connect(self->fd, 
				  IPAddress_sockaddr(address), 
				  IPAddress_size(address));
#ifdef WIN32	
#define EINPROGRESS WSAEINPROGRESS
#define EALREADY WSAEALREADY
//note: WSAEINVAL is equivalent to WSAEWOULDBLOCK on windows
#endif
	eno = SocketErrorStatus();
	if (result == 0 || eno == EISCONN) 
	{
		//errno = 0;
		return 0;
	}
	
	if(eno == EINPROGRESS || eno == EALREADY) 
	{ 
		//errno = 0;
		return 1;
	}
	
	//Socket_checkErrno(self);
	
	return result;
}

int Socket_bind(Socket *self, IPAddress *address)
{
	int r;
	
	SocketResetErrorStatus();
	
	// returns 0 of ok, -1 on error
     r = bind(self->fd, IPAddress_sockaddr(address), IPAddress_size(address));
	//if (r == -1) Socket_checkErrno(self);
	return r;
}

int Socket_listen(Socket *self)
{
	int r;
	
	SocketResetErrorStatus();
	
	// returns 0 of ok, -1 on error
	r = listen(self->fd, SOMAXCONN);
	//if (r == -1) Socket_checkErrno(self);
	return r;
}


Socket *Socket_accept(Socket *self, IPAddress *address)
{
	socklen_t addressSize = IPAddress_size(address);
	SOCKET_DESCRIPTOR d;

	SocketResetErrorStatus();
	
	d = accept(self->fd, IPAddress_sockaddr(address), &addressSize);
	
	IPAddress_setSize_(address, addressSize);
	
	if (d != -1)
	{
		Socket *socket = Socket_new();
		Socket_setDescriptor_(socket, d);
		Socket_makeReusable(socket);
		Socket_makeAsync(socket);
		return socket;
	}
	
	//Socket_checkErrno(self);
	
	return NULL;
}

// stream ----------------------------------------

ssize_t Socket_streamRead(Socket *self, UArray *buffer, size_t readSize)
{ 
	ssize_t bytesRead;
	size_t originalSize = UArray_sizeInBytes(buffer);
		
	UArray_sizeTo_(buffer, originalSize + readSize + 1);
	
	SocketResetErrorStatus();
	
	if (!Socket_isValid(self)) { Socket_close(self); UArray_setSize_(buffer, originalSize); return 0; }
	
#ifdef WIN32
	bytesRead = (size_t)recv(self->fd, (uint8_t *)UArray_bytes(buffer) + originalSize, readSize, 0);
#else
	bytesRead = (size_t)read(self->fd, (uint8_t *)UArray_bytes(buffer) + originalSize, readSize);
#endif
	//printf("Socket streamRead bytesRead:%i errno:%i\n", bytesRead, errno);
	
	if (bytesRead > 0)
	{
		UArray_setSize_(buffer, originalSize + bytesRead);
	}
	else
	{
		UArray_setSize_(buffer, originalSize);
	}
	
	//if (bytesRead == -1) Socket_checkErrno(self);
	if ((bytesRead == 0 || SocketErrorStatus()) && 
	    (SocketErrorStatus() != EINTR && 
		SocketErrorStatus() != EAGAIN && 
		SocketErrorStatus() != EISCONN)) 
	{
		/*
		if (bytesRead == 0 && SocketErrorStatus() == 0)
		{
			printf("Socket warning: no bytesRead and no errno? closing.\n");
		}
		else
		{
			printf("Socket bytesRead: %i errno: %i '%s' detected - closeing\n", (int)bytesRead, errno, strerror(errno));
		}
		*/
		Socket_close(self);
		
	}
	
	return bytesRead;
}

ssize_t Socket_streamWrite(Socket *self, 
					  UArray *buffer, 
					  size_t start, 
					  size_t writeSize)
{
	size_t bufferSize = UArray_sizeInBytes(buffer);
	ssize_t bytesSent;
	
	if (start > bufferSize) 
	{
		return 0;
	}
	
	if (start + writeSize > bufferSize) 
	{
		writeSize = bufferSize - start;
	}
		
	SocketResetErrorStatus();

	if (!Socket_isValid(self)) { Socket_close(self); return 0; }

	/*
	In Unix, socket.send(buf) will buffer as much of buf as it has space for, 
	and then return how much it accepted. This could be 0 or up to something around 128K. 
	If you send some data and then some more, it will append to the previous buffer.

	In Windows, socket.send(buf) will either accept the entire buffer or raise ENOBUFS. 
	Testing indicates that it will internally buffer any amount up to 50MB 
	(this seems to be the total for either the process or the OS, I'm not sure). 
	However, it will not incrementally accept more data to append to a socket's 
	buffer until the big buffer has been completely emptied 
	(seemingly down to the SO_SNDBUF length, which is 8192), but rather raises WSAEWOULDBLOCK instead.
	*/

	//printf("write %i -> %i of %i\n", start, start + writeSize, bufferSize);
#ifdef WIN32
	bytesSent =  send(self->fd, UArray_bytes(buffer) + start, writeSize, 0);
#else
	bytesSent = write(self->fd, UArray_bytes(buffer) + start, writeSize);
#endif
	
	if (bytesSent < 0 || SocketErrorStatus()) 
	{
		//printf("Socket_streamWrite() bytesSent %i errno %i - closing\n", bytesSent, errno);
		Socket_close(self);
		//Socket_checkErrno(self);
	}
	
	return bytesSent;
}

ssize_t Socket_udpRead(Socket *self, 
				   IPAddress *address, 
				   UArray *buffer, 
				   size_t readSize)
{
	socklen_t addressSize = IPAddress_size(address);
	size_t oldSize = UArray_sizeInBytes(buffer);
	ssize_t bytesRead;
	
	UArray_setItemType_(buffer, CTYPE_uint8_t);
	
	UArray_sizeTo_(buffer, oldSize + readSize);
	
	SocketResetErrorStatus();
	
	bytesRead = recvfrom(self->fd, 
					 (uint8_t *)UArray_bytes(buffer), 
					 readSize, 
					 0, 
					 IPAddress_sockaddr(address),
					 &addressSize);
	//printf("Socket_udpRead bytesRead = %i\n", bytesRead);
	
	if (bytesRead > 0)
	{
		UArray_setSize_(buffer, oldSize + bytesRead);
		IPAddress_setSize_(address, addressSize);
	}
	else
	{
		UArray_setSize_(buffer, oldSize);
	}
	
	return bytesRead;
}

ssize_t Socket_udpWrite(Socket *self, 
				   IPAddress *address, 
				   UArray *buffer,
				   size_t start, 
				   size_t writeSize)
{
	size_t bufferSize = UArray_sizeInBytes(buffer);
	ssize_t bytesWritten;
	
	if (start > bufferSize) return 0;
	
	if (start + writeSize > bufferSize) 
	{
		writeSize = bufferSize - start;
	}
	
	SocketResetErrorStatus();

	bytesWritten = sendto(self->fd, 
			    UArray_bytes(buffer), 
			    writeSize, 
			    0, 
			    IPAddress_sockaddr(address), 
			    IPAddress_size(address));
	
	//if (bytesWritten < 0) Socket_checkErrno(self);
	
	return bytesWritten;
}

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

/*
size_t Socket_sendfile(Socket *self, int fd)
{
	off_t bytesSent;
	
	sendfile(fd, self->fd, 0, 0, NULL, &bytesSent, 0);
	
	return bytesSent;
}
*/
