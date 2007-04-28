/*#io
Socket ioDoc(
		   docCopyright("Steve Dekorte", 2004)
		   docLicense("BSD revised")
		   docDependsOn("SocketManager")
		   docDescription("""Interface to network communication. 
Sockets will auto yields to other coroutines while waiting on a request. 
All blocking operations use the timeout settings of the socket. 
Reads are appended to the socket's read buffer which can be accessed using the readBuffer method. 
Example:

<pre>
socket := Socket clone setHost("www.yahoo.com") setPort(80) connect 
if(socket error) then( write(socket error, "\n"); exit) 

socket write("GET /\n\n")

while(socket read, Nop)
if(socket error) then(write(socket error, "\n"); exit) 

write("read ", socket readBuffer length, " bytes\n")
</pre>""")
		   docCategory("Networking")
*/


/*#io
docSlot("setHost(hostName)", 
	   "Translates hostName to an IP using asynchronous DNS and sets the host attribute. Returns self.")
*/

#include "IoSocket.h"
#include "IoIPAddress.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include "UArray.h"
#include "List.h"

IoSocket *IoMessage_locals_socketArgAt_(IoMessage *self, IoObject *locals, int n)
{
	IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);
	
	if (!ISSOCKET(v)) 
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "Socket");
	}
	
	return v;
}

#define SOCKET(self) ((Socket *)IoObject_dataPointer(self))

void IoSocket_tagCleanup(IoTag *self)
{  
	Socket_GlobalCleanup();
}

IoTag *IoSocket_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Socket");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSocket_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSocket_rawClone);
	IoTag_cleanupFunc_(tag, (IoTagCleanupFunc *)IoSocket_tagCleanup);
	Socket_GlobalInit();
	return tag;
}

IoSocket *IoSocket_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	
	IoObject_tag_(self, IoSocket_newTag(state));
	IoObject_setDataPointer_(self, Socket_new());
	
	IoState_registerProtoWithFunc_((IoState *)state, self, IoSocket_proto);
	
	{
		IoMethodTable methodTable[] = {
		//{"openFifo", IoSocket_openFifo_},
		
		{"asyncStreamOpen", IoSocket_asyncStreamOpen},
		{"asyncUdpOpen", IoSocket_asyncUdpOpen},
		{"isOpen", IoSocket_isOpen},
		{"isValid", IoSocket_isValid},
		{"isStream", IoSocket_isStream},
		
		{"asyncBind", IoSocket_asyncBind},
		{"asyncListen", IoSocket_asyncListen},
		{"asyncAccept", IoSocket_asyncAccept},
		
		{"asyncConnect", IoSocket_connectTo},
			
		{"asyncStreamRead", IoSocket_asyncStreamRead},
		{"asyncStreamWrite", IoSocket_asyncStreamWrite},
			
		{"asyncUdpRead", IoSocket_udpRead},
		{"asyncUdpWrite", IoSocket_udpWrite},
			
		{"close", IoSocket_close},
		{"descriptorId", IoSocket_descriptorId},
		
		//{"sendfile", IoSocket_sendfile},
		//{"sync", IoSocket_sync},
		
		{"setSocketReadBufferSize", IoSocket_setSocketReadBufferSize},
		{"setSocketWriteBufferSize", IoSocket_setSocketWriteBufferSize},
		
		{"getSocketReadLowWaterMark", IoSocket_getSocketReadLowWaterMark},
		{"getSocketWriteLowWaterMark", IoSocket_getSocketWriteLowWaterMark},

		{"setSocketReadLowWaterMark", IoSocket_setSocketReadLowWaterMark},
		{"setSocketWriteLowWaterMark", IoSocket_setSocketWriteLowWaterMark},
		
		{"setNoDelay", IoSocket_setNoDelay},
                {"errno", IoSocket_errnoDescription},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	
	return self;
}

IoSocket *IoSocket_rawClone(IoSocket *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, Socket_new());	
	return self; 
}

IoSocket *IoSocket_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoSocket_proto);
	return IOCLONE(proto);
}

// ----------------------------------------------------------- 

IoSocket *IoSocket_newWithSocket_(void *state, Socket *socket)
{
	IoSocket *self = IoSocket_new(state);
	Socket_free(SOCKET(self));
	IoObject_setDataPointer_(self, socket);
	return self;
}

void IoSocket_free(IoSocket *self) 
{
	Socket_free(SOCKET(self));
}

// ----------------------------------------

IoObject *IoSocket_descriptorId(IoSocket *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(Socket_descriptor(SOCKET(self)));
}

SOCKET_DESCRIPTOR IoSocket_rawDescriptor(IoSocket *self)
{
	return Socket_descriptor(SOCKET(self));
}

// ----------------------------------------

IoObject *IoSocket_asyncStreamOpen(IoSocket *self, IoObject *locals, IoMessage *m)
{
	Socket *socket = SOCKET(self);
	SocketResetErrorStatus();
	
	if (Socket_streamOpen(socket) == -1) return IOFALSE(self);
	if (Socket_isOpen(socket) != 1) return IOFALSE(self);
	if (Socket_makeReusable(socket) != 0) return IOFALSE(self);
	if (Socket_makeAsync(socket) != 0) return IOFALSE(self);
		
	return self;
}

IoObject *IoSocket_asyncUdpOpen(IoSocket *self, IoObject *locals, IoMessage *m)
{
	Socket *socket = SOCKET(self);
	SocketResetErrorStatus();
	
	if (Socket_udpOpen(socket) == -1) return IOFALSE(self);
	if (Socket_isOpen(socket) != 1) return IOFALSE(self);
	if (Socket_makeReusable(socket) != 0) return IOFALSE(self);
	if (Socket_makeAsync(socket) != 0) return IOFALSE(self);
	
	return self;
}

// ----------------------------------------

IoObject *IoSocket_isStream(IoSocket *self, IoObject *locals, IoMessage *m)
{
	return IOBOOL(self, Socket_isStream(SOCKET(self)));
}

IoObject *IoSocket_isOpen(IoSocket *self, IoObject *locals, IoMessage *m)
{
	return IOBOOL(self, Socket_isOpen(SOCKET(self)));
}

IoObject *IoSocket_isValid(IoSocket *self, IoObject *locals, IoMessage *m)
{
	return IOBOOL(self, Socket_isValid(SOCKET(self)));
}

IoObject *IoSocket_connectTo(IoSocket *self, IoObject *locals, IoMessage *m)
{	
	IPAddress *address = IoMessage_locals_rawIPAddressArgAt_(m, locals, 0);
	int r = Socket_connectTo(SOCKET(self), address);
	return IOBOOL(self, r == 0);
}

IoObject *IoSocket_close(IoSocket *self, IoObject *locals, IoMessage *m)
{ 
	Socket_close(SOCKET(self)); 
	return IOTRUE(self);
}

// server -------------------------------

IoObject *IoSocket_asyncBind(IoSocket *self, IoObject *locals, IoMessage *m)
{	
	IPAddress *address = IoMessage_locals_rawIPAddressArgAt_(m, locals, 0);
	return IOBOOL(self, Socket_bind(SOCKET(self), address) == 0);
}	

IoObject *IoSocket_asyncListen(IoSocket *self, IoObject *locals, IoMessage *m)
{
	return IOBOOL(self, Socket_listen(SOCKET(self)) == 0);
}

IoObject *IoSocket_asyncAccept(IoSocket *self, IoObject *locals, IoMessage *m)
{
	IPAddress *address = IoMessage_locals_rawIPAddressArgAt_(m, locals, 0);
	Socket *socket = Socket_accept(SOCKET(self), address);
	return (socket) ? IoSocket_newWithSocket_(IOSTATE, socket) : IONIL(self);
}

// stream -------------------------------

IoObject *IoSocket_asyncStreamRead(IoSocket *self, IoObject *locals, IoMessage *m)
{
	IoSeq *bufferSeq = IoMessage_locals_mutableSeqArgAt_(m, locals, 0);
	UArray *buffer = IoSeq_rawUArray(bufferSeq);
	size_t readSize = IoMessage_locals_intArgAt_(m, locals, 1);
	ssize_t bytesRead = Socket_streamRead(SOCKET(self), buffer, readSize);
	int eno = SocketErrorStatus();

	//printf("bytesRead = %i\n", bytesRead);
	//printf("eno == EAGAIN = %i\n", eno == EAGAIN);
	if (bytesRead == -1 && (eno == EAGAIN || eno == EINTR)) 
	{
		SocketResetErrorStatus();
		return IOFALSE(self);
	}
	
	if (bytesRead == 0)
	{
		Socket_close(SOCKET(self));
		return IOFALSE(self);
	}

	return IOBOOL(self, bytesRead > 0);
}

IoObject *IoSocket_asyncStreamWrite(IoSocket *self, IoObject *locals, IoMessage *m)
{
	IoSeq *bufferSeq = IoMessage_locals_seqArgAt_(m, locals, 0);
	UArray *buffer = IoSeq_rawUArray(bufferSeq);
	//size_t bufSize = UArray_size(buffer);
	size_t start = IoMessage_locals_intArgAt_(m, locals, 1);
	size_t writeSize = IoMessage_locals_intArgAt_(m, locals, 2);
	ssize_t bytesWritten;

	bytesWritten = Socket_streamWrite(SOCKET(self), buffer, start, writeSize);
	
	return IOBOOL(self, bytesWritten != 0);
	//return IOBOOL(self, bytesWritten == writeSize);
	//return IONUMBER(bytesWritten);
}

// udp ------------------------------

IoObject *IoSocket_udpRead(IoSocket *self, IoObject *locals, IoMessage *m)
{
	IPAddress *address = IoMessage_locals_rawIPAddressArgAt_(m, locals, 0);
	UArray *buffer = IoSeq_rawUArray(IoMessage_locals_mutableSeqArgAt_(m, locals, 1));
	size_t readSize = IoMessage_locals_sizetArgAt_(m, locals, 2);
	ssize_t bytesRead = Socket_udpRead(SOCKET(self), address, buffer, readSize);
	return IOBOOL(self, bytesRead > 0);
	//return IONUMBER(bytesRead);
}

IoObject *IoSocket_udpWrite(IoSocket *self, IoObject *locals, IoMessage *m)
{
	IPAddress *address = IoMessage_locals_rawIPAddressArgAt_(m, locals, 0);
	UArray *buffer = IoSeq_rawUArray(IoMessage_locals_mutableSeqArgAt_(m, locals, 1));
	size_t start = IoMessage_locals_intArgAt_(m, locals, 2);
	size_t writeSize = IoMessage_locals_intArgAt_(m, locals, 3);
	ssize_t bytesWritten = Socket_udpWrite(SOCKET(self), address, buffer, start, writeSize);
	//return IOBOOL(self, bytesWritten == UArray_size(buffer));
	return IONUMBER(bytesWritten);
}

/*
IoObject *IoSocket_sendfile(IoSocket *self, IoObject *locals, IoMessage *m)
{
	int fd = IoMessage_locals_intArgAt_(m, locals, 0);
	size_t bytesSent = Socket_sendfile(SOCKET(self), fd);
	return IONUMBER(bytesSent);
}
*/
/*
IoObject *IoSocket_sync(IoSocket *self, IoObject *locals, IoMessage *m)
{
	int r = fcntl(SOCKET(self)->fd, F_FULLFSYNC, 0);
	//fsync(SOCKET(self)->fd);
	return IONUMBER(r);
}
*/

IoObject *IoSocket_setSocketReadBufferSize(IoSocket *self, IoObject *locals, IoMessage *m)
{
	int size = IoMessage_locals_intArgAt_(m, locals, 0);
	int r = setsockopt(SOCKET(self)->fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(int));
	return IONUMBER(r);
}

IoObject *IoSocket_setSocketWriteBufferSize(IoSocket *self, IoObject *locals, IoMessage *m)
{
	int size = IoMessage_locals_intArgAt_(m, locals, 0);
	int r = setsockopt(SOCKET(self)->fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(int));
	return IONUMBER(r);
}

IoObject *IoSocket_setSocketReadLowWaterMark(IoSocket *self, IoObject *locals, IoMessage *m)
{
	int size = IoMessage_locals_intArgAt_(m, locals, 0);
	int r = setsockopt(SOCKET(self)->fd, SOL_SOCKET, SO_RCVLOWAT, &size, sizeof(int));
	return IONUMBER(r);
}

IoObject *IoSocket_setSocketWriteLowWaterMark(IoSocket *self, IoObject *locals, IoMessage *m)
{
	int size = IoMessage_locals_intArgAt_(m, locals, 0);
	int r = setsockopt(SOCKET(self)->fd, SOL_SOCKET, SO_SNDLOWAT, &size, sizeof(int));
	return IONUMBER(r);
}

IoObject *IoSocket_getSocketReadLowWaterMark(IoSocket *self, IoObject *locals, IoMessage *m)
{
	int size = 0;
	socklen_t length = sizeof(int);
	//int r = 
	getsockopt(SOCKET(self)->fd, SOL_SOCKET, SO_RCVLOWAT, &size, &length);
	
	return IONUMBER(size);
}

IoObject *IoSocket_getSocketWriteLowWaterMark(IoSocket *self, IoObject *locals, IoMessage *m)
{
	int size = 0;
	socklen_t length = sizeof(int);
	//int r = 
	getsockopt(SOCKET(self)->fd, SOL_SOCKET, SO_SNDLOWAT, &size, &length);
	return IONUMBER(size);
}

#ifndef WIN32
#include <sys/socket.h>
#endif

IoObject *IoSocket_setNoDelay(IoSocket *self, IoObject *locals, IoMessage *m)
{
	int r = -1;
	#ifdef TCP_NODELAY
	int flag = 1;
	r = setsockopt(SOCKET(self)->fd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
	#endif
	return IONUMBER(r);
}								 


IoObject *IoSocket_errnoDescription(IoSocket *self, IoObject *locals, IoMessage *m)
{
     int err = SocketErrorStatus();
#ifdef WIN32
     if (err) {
         char buf[128];
         sprintf(buf, "WSA Error %d", err);
         return IOSYMBOL(buf);
     } else {
         return IONIL(self);
     }
#else
   return err ? IOSYMBOL(strerror(err)) : IONIL(self);
#endif
}

