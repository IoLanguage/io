//metadoc Socket copyright Steve Dekorte 2002
//metadoc license BSD revised

#ifndef IOSOCKET_DEFINED
#define IOSOCKET_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoMessage.h"
#include "Socket.h"
#include "IoError.h"
#include "Address.h"

#define ISSOCKET(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSocket_rawClone)
#define SOCKETERROR(leadingErrorDescription) IoError_newWithMessageFormat_(IOSTATE, "%s: %s", leadingErrorDescription, Socket_errorDescription())

typedef IoObject IoSocket;

IoSocket *IoMessage_locals_socketArgAt_(IoMessage *self, IoObject *locals, int n);
IoObject *IoMessage_locals_addressArgAt_(IoMessage *self, IoObject *locals, int n);
Address *IoSocket_rawAddressFrom_(IoObject *addr);

IoSocket *IoSocket_rawClone(IoSocket *self);
IoSocket *IoSocket_proto(void *state);
IoSocket *IoSocket_new(void *state);
IoSocket *IoSocket_newWithSocket_(void *state, Socket *socket);
void IoSocket_free(IoSocket *self);

// -----------------------------------------------------------

IoObject *IoSocket_descriptorId(IoSocket *self, IoObject *locals, IoMessage *m);
SOCKET_DESCRIPTOR IoSocket_rawDescriptor(IoSocket *self);

IoObject *IoSocket_asyncStreamOpen(IoSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSocket_asyncUdpOpen(IoSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSocket_isOpen(IoSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSocket_isValid(IoSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSocket_isStream(IoSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSocket_close(IoSocket *self, IoObject *locals, IoMessage *m);

// client

IoObject *IoSocket_asyncConnect(IoSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSocket_didConnect(IoSocket *self, IoObject *locals, IoMessage *m);

// stream

IoObject *IoSocket_asyncStreamRead(IoSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSocket_asyncStreamWrite(IoSocket *self, IoObject *locals, IoMessage *m);

// udp

IoObject *IoSocket_asyncUdpRead(IoSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSocket_asyncUdpWrite(IoSocket *self, IoObject *locals, IoMessage *m);

// server

IoObject *IoSocket_asyncBind(IoSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSocket_asyncListen(IoSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSocket_asyncAccept(IoSocket *self, IoObject *locals, IoMessage *m);

//IoObject *IoSocket_sendfile(IoSocket *self, IoObject *locals, IoMessage *m);
//IoObject *IoSocket_sync(IoSocket *self, IoObject *locals, IoMessage *m);

IoObject *IoSocket_setSocketReadBufferSize(IoSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSocket_setSocketWriteBufferSize(IoSocket *self, IoObject *locals, IoMessage *m);

IoObject *IoSocket_setSocketReadLowWaterMark(IoSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSocket_setSocketWriteLowWaterMark(IoSocket *self, IoObject *locals, IoMessage *m);

IoObject *IoSocket_getSocketReadLowWaterMark(IoSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSocket_getSocketWriteLowWaterMark(IoSocket *self, IoObject *locals, IoMessage *m);

IoObject *IoSocket_setNoDelay(IoSocket *self, IoObject *locals, IoMessage *m);

IoObject *IoSocket_errorNumber(IoSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSocket_errorDescription(IoSocket *self, IoObject *locals, IoMessage *m);

IoObject *IoSocket_setAddress(IoSocket *self, IoObject *locals, IoMessage *m);

IoObject *IoSocket_fromFd(IoSocket *self, IoObject *locals, IoMessage *m);

#endif
