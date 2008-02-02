/*metadoc Socket copyright
	Steve Dekorte, 2004
*/

/*metadoc Socket license
	BSD revised
*/

/*metadoc Socket dependsOn
	SocketManager
*/

/*metadoc Socket description
Interface to network communication.
Sockets will auto yield to other coroutines while waiting on a request.
All blocking operations use the timeout settings of the socket.
Reads are appended to the socket's read buffer which can 
be accessed using the readBuffer method.

Example:

<code>	
socket := Socket clone setHost("www.yahoo.com") setPort(80) connect
if(socket error) then( write(socket error, "\n"); exit)

socket write("GET /\n\n")

while(socket read, Nop)
if(socket error) then(write(socket error, "\n"); exit)

write("read ", socket readBuffer length, " bytes\n")
</code>	""")
//metadoc category Networking")
*/


/*doc Socket setHost(hostName)
Translates hostName to an IP using asynchronous DNS and sets 
the host attribute. Returns self.
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

		{"setSocketReadBufferSize", IoSocket_setSocketReadBufferSize},
		{"setSocketWriteBufferSize", IoSocket_setSocketWriteBufferSize},

		{"getSocketReadLowWaterMark", IoSocket_getSocketReadLowWaterMark},
		{"getSocketWriteLowWaterMark", IoSocket_getSocketWriteLowWaterMark},

		{"setSocketReadLowWaterMark", IoSocket_setSocketReadLowWaterMark},
		{"setSocketWriteLowWaterMark", IoSocket_setSocketWriteLowWaterMark},

		{"setNoDelay", IoSocket_setNoDelay},
		
		{"errorNumber", IoSocket_errorNumber},
		{"errorDescription", IoSocket_errorDescription},

		{NULL, NULL}
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

IoSocket *IoSocket_rawSetupEvent_(IoSocket *self, IoObject *locals, IoMessage *m, char *eventSlotName)
{
	IoObject *event = IoObject_rawGetSlot_(self, IOSYMBOL(eventSlotName));
	if(!event || ISNIL(event))
	{
		IoState_error_(IOSTATE, m, "Expected %s slot to be set!", eventSlotName);
		return IONIL(self);
	}
	else
	{
		IoObject_setSlot_to_(event, IOSYMBOL("descriptorId"), IoSocket_descriptorId(self, locals, m));
		return self;
	}
}

IoSocket *IoSocket_rawSetupEvents(IoSocket *self, IoObject *locals, IoMessage *m)
{
	IoSocket_rawSetupEvent_(self, locals, m, "readEvent");
	IoSocket_rawSetupEvent_(self, locals, m, "writeEvent");
	return self;
}

IoObject *IoSocket_descriptorId(IoSocket *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER((int) Socket_descriptor(SOCKET(self)));
}

SOCKET_DESCRIPTOR IoSocket_rawDescriptor(IoSocket *self)
{
	return Socket_descriptor(SOCKET(self));
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
	int isValid = Socket_isValid(SOCKET(self));

	if (!isValid)
		IoSocket_close(self, locals, m);

	return IOBOOL(self, isValid);
}

// ----------------------------------------

IoObject *IoSocket_asyncStreamOpen(IoSocket *self, IoObject *locals, IoMessage *m)
{
	Socket *socket = SOCKET(self);
	SocketResetErrorStatus();

	if (Socket_streamOpen(socket) && Socket_isOpen(socket) && Socket_makeReusable(socket) && Socket_makeAsync(socket))
	{
		IoSocket_rawSetupEvents(self, locals, m);
		return self;
	}
	else
	{
		return SOCKETERROR("Failed to create stream socket");
	}
}

IoObject *IoSocket_asyncUdpOpen(IoSocket *self, IoObject *locals, IoMessage *m)
{
	Socket *socket = SOCKET(self);

	if (Socket_udpOpen(socket) && Socket_isOpen(socket) && Socket_makeReusable(socket) && Socket_makeAsync(socket))
	{
		IoSocket_rawSetupEvents(self, locals, m);
		return self;
	}
	else
	{
		return SOCKETERROR("Failed to create udp socket");
	}
}

// ----------------------------------------

IoObject *IoSocket_connectTo(IoSocket *self, IoObject *locals, IoMessage *m)
{
	IPAddress *address = IoMessage_locals_rawIPAddressArgAt_(m, locals, 0);
	if (Socket_connectTo(SOCKET(self), address))
		return self;
	else
	{
		if (Socket_connectToFailed())
			return SOCKETERROR("Socket connect failed");
		else
			return IONIL(self);
	}
}

IoObject *IoSocket_close(IoSocket *self, IoObject *locals, IoMessage *m)
{
	if (Socket_close(SOCKET(self)))
	{
		IoSocket_rawSetupEvents(self, locals, m);
		return self;
	}
	else
	{
		if (Socket_closeFailed())
			return SOCKETERROR("Failed to close socket");
		else
			return IONIL(self);
	}
}

// server -------------------------------

IoObject *IoSocket_asyncBind(IoSocket *self, IoObject *locals, IoMessage *m)
{
	IPAddress *address = IoMessage_locals_rawIPAddressArgAt_(m, locals, 0);
	if (Socket_bind(SOCKET(self), address))
		return self;
	else
		return SOCKETERROR("Failed to bind socket");
}

IoObject *IoSocket_asyncListen(IoSocket *self, IoObject *locals, IoMessage *m)
{
	if (Socket_listen(SOCKET(self)))
		return self;
	else
		return SOCKETERROR("Socket listen failed");
}

IoObject *IoSocket_asyncAccept(IoSocket *self, IoObject *locals, IoMessage *m)
{
	IPAddress *address = IoMessage_locals_rawIPAddressArgAt_(m, locals, 0);
	Socket *socket = Socket_accept(SOCKET(self), address);
	if (socket)
	{
		IoObject *newSocket = IoSocket_newWithSocket_(IOSTATE, socket);
		newSocket = IoObject_initClone_(self, locals, m, newSocket);
		return IoSocket_rawSetupEvents(newSocket, locals, m);
	}
	else
	{
		if (Socket_asyncFailed())
			return SOCKETERROR("Socket accept failed");
		else
			return IONIL(self);
	}
}

// stream -------------------------------

IoObject *IoSocket_asyncStreamRead(IoSocket *self, IoObject *locals, IoMessage *m)
{
	IoSeq *bufferSeq = IoMessage_locals_mutableSeqArgAt_(m, locals, 0);
	UArray *buffer = IoSeq_rawUArray(bufferSeq);
	size_t readSize = IoMessage_locals_intArgAt_(m, locals, 1);

	if (Socket_streamRead(SOCKET(self), buffer, readSize))
	{
		return self;
	}
	else
	{
		if (Socket_asyncFailed())
		{
			IoSocket_close(self, locals, m);
			return SOCKETERROR("Socket stream read failed");
		}
		else
		{
			if (SocketErrorStatus() == 0)
			// 0 bytes means the other end disconnected
				IoSocket_close(self, locals, m);
			return IONIL(self);
		}
	}
}

IoObject *IoSocket_asyncStreamWrite(IoSocket *self, IoObject *locals, IoMessage *m)
{
	IoSeq *bufferSeq = IoMessage_locals_seqArgAt_(m, locals, 0);
	UArray *buffer = IoSeq_rawUArray(bufferSeq);
	size_t start = IoMessage_locals_intArgAt_(m, locals, 1);
	size_t writeSize = IoMessage_locals_intArgAt_(m, locals, 2);
	size_t bytesWritten = Socket_streamWrite(SOCKET(self), buffer, start, writeSize);
	
	if (bytesWritten)
	{
		UArray_removeRange(buffer, start, bytesWritten);
		return self;
	}
	else
	{
		if (Socket_asyncFailed())
		{
			IoSocket_close(self, locals, m);
			return SOCKETERROR("Socket stream write failed");
		}
		else
		{
			if (IoSocket_errorNumber(self, locals, m) == 0)
			// 0 bytes means the other end disconnected
				IoSocket_close(self, locals, m);
			return IONIL(self);
		}
	}
}

// udp ------------------------------

IoObject *IoSocket_udpRead(IoSocket *self, IoObject *locals, IoMessage *m)
{
	IPAddress *address = IoMessage_locals_rawIPAddressArgAt_(m, locals, 0);
	UArray *buffer = IoSeq_rawUArray(IoMessage_locals_mutableSeqArgAt_(m, locals, 1));
	size_t readSize = IoMessage_locals_sizetArgAt_(m, locals, 2);
	
	if (Socket_udpRead(SOCKET(self), address, buffer, readSize))
	{
		return self;
	}
	else
	{
		if (Socket_asyncFailed())
			return SOCKETERROR("Socket udp read failed");
		else
			return IONIL(self);
	}
}

IoObject *IoSocket_udpWrite(IoSocket *self, IoObject *locals, IoMessage *m)
{
	IPAddress *address = IoMessage_locals_rawIPAddressArgAt_(m, locals, 0);
	UArray *buffer = IoSeq_rawUArray(IoMessage_locals_seqArgAt_(m, locals, 1));
	size_t start = IoMessage_locals_intArgAt_(m, locals, 2);
	size_t writeSize = IoMessage_locals_intArgAt_(m, locals, 3);
	size_t bytesWritten = Socket_udpWrite(SOCKET(self), address, buffer, start, writeSize);
	
	if (bytesWritten)
	{
		if (bytesWritten < writeSize)
		{
			return SOCKETERROR("Socket udp write failed");
		}
		else
		{
			UArray_removeRange(buffer, start, bytesWritten);
			return self;
		}
	}
	else
	{
		if (Socket_asyncFailed())
			return SOCKETERROR("Socket udp write failed");
		else
			return IONIL(self);
	}
}

// ----------------------------------

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

IoObject *IoSocket_errorNumber(IoSocket *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(SocketErrorStatus());
}

IoObject *IoSocket_errorDescription(IoSocket *self, IoObject *locals, IoMessage *m)
{
	return IOSYMBOL(Socket_errorDescription());
}