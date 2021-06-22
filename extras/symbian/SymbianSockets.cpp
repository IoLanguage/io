#include <e32base.h>
#include <f32file.h>
#include <in_sock.h>

extern "C"
{
#include "IoState.h"
#include "IoNIL.h"
#include "IoMessage.h"
#include "IoNumber.h"
}

#include "SymbianMain.h"
#include "SymbianSockets.h"

#define min(a,b) ((a) < (b) ? (a) : (b))
#define SOCKET_BUFFER_SIZE 2048

class InternalSocket
{
public:
	RSocket socket;
	char buffer[SOCKET_BUFFER_SIZE];
	int bufferTop;
	int bufferBottom;

	InternalSocket() :
		bufferTop(0),
		bufferBottom(0)
	{
		memset(buffer, 0, sizeof(buffer));
	}

	void ResetBuffer()
	{
		bufferTop = 0;
		bufferBottom = 0;
		memset(buffer, 0, sizeof(buffer));
	}

	TInt ReadIntoBuffer()
	{
		TRequestStatus status;
		TPtr8 ptr8((TUint8*)buffer, sizeof(buffer), sizeof(buffer));
		TSockXfrLength tlen;
		socket.RecvOneOrMore(ptr8, 0, status, tlen);	
		User::WaitForRequest(status);
		bufferTop = tlen();
		bufferBottom = 0;
		return status.Int();
	}

	TInt ReadChar(char* ch)
	{
		TInt status = KErrNone;
		if(bufferTop == bufferBottom)
		{
			status = ReadIntoBuffer();
		}

		if(status == KErrNone)
		{
			*ch = buffer[bufferBottom++];
		}

		return status;
	}

	TInt ReadAllChars(char* ch, int length)
	{
		TInt status = KErrNone;

		while(length > 0)
		{
			if(bufferTop == bufferBottom)
			{
				status = ReadIntoBuffer();
				if(status != KErrNone)
					break;
			}

			if(bufferTop - bufferBottom >= length)
			{
				memcpy(ch, buffer + bufferBottom, length);
				bufferBottom += length;
				length = 0;
			}
			else
			{
				int l = min(length, bufferTop - bufferBottom);
				memcpy(ch, buffer + bufferBottom, l);
				length -= l;
				ch += l;
				bufferBottom += l;
			}
		}

		return status;
	}

	TInt ReadLine(char* ch, int length)
	{
		TInt status = KErrNone;
		int state = 0;

		while(length > 0)
		{
			if(bufferTop == bufferBottom)
			{
				status = ReadIntoBuffer();
				if(status != KErrNone)
					break;
			}

			if(bufferTop - bufferBottom >= length)
			{
				char* start = buffer + bufferBottom;
				char* end = buffer + length;

				while(start != end)
				{
					char c = *start++;
					if(c == 13)
					{
						state = 1;
					}
					else if(c == 10)
					{
						if(state == 1)
						{
							state = 2;
							bufferBottom = start - buffer;
							length = 0;
							return status;
							break;
						}
						else
						{
							*ch++ = c;
						}
					}
					else
					{
						if(state == 1)
						{
							*ch++ = 13;
						}
						else
						{
							*ch++ = c;
						}
						state = 0;
					}
				}
				bufferBottom += length;
				length = 0;
			}
			else
			{
				int l = min(length, bufferTop - bufferBottom);
				char* start = buffer + bufferBottom;
				char* end = buffer + l;
				while(state != 2 && start != end)
				{
					char c = *start++;
					if(c == 13)
					{
						state = 1;
					}
					else if(c == 10)
					{
						if(state == 1)
						{
							state = 2;
							bufferBottom = start - buffer;
							length = 0;
							return status;
							break;
						}
						else
						{
							*ch++ = c;
						}
					}
					else
					{
						if(state == 1)
						{
							*ch++ = 13;
						}
						else
						{
							*ch++ = c;
						}
						state = 0;
					}
				}
				
				length -= l;
				ch += l;
				bufferBottom += l;
			}
		}
		return status;
	}

	TInt ReadOneOrMore(char* ch, int length)
	{
		TInt status = KErrNone;

		if(bufferTop == bufferBottom)
		{
			status = ReadIntoBuffer();
			if(status != KErrNone)
				return status;
		}

		if(bufferTop - bufferBottom >= length)
		{
			memcpy(ch, buffer + bufferBottom, length);
			bufferBottom += length;
		}
		else
		{
			int l = min(length, bufferTop - bufferBottom);
			memcpy(ch, buffer + bufferBottom, l);
			bufferBottom += l;
		}

		return status;
	}

};

class CConnectingSocket;
class CReadLineSocket;

struct IoSocket
{
  unsigned char color;
  IoValue *previous;
  IoValue *next;
  IoTag *tag;
  InternalSocket* socket;
  char* host;
  int port;
  bool isConnected;
  char* lineRead;

  CConnectingSocket *activeConnect;
  CReadLineSocket* activeReadLine;
};

IoValue *IoSocket_clone(IoSocket *self, IoValue *locals, IoMessage *m);
void IoSocket_free(IoSocket *self);
char *IoSocket_name(IoSocket *self); 
void IoSocket_mark(IoSocket *self);
IoValue *IoSocket_host(IoSocket *self, IoValue *locals, IoMessage *m);
IoValue *IoSocket_host_(IoSocket *self, IoValue *locals, IoMessage *m);
IoValue *IoSocket_port(IoSocket *self, IoValue *locals, IoMessage *m);
IoValue *IoSocket_port_(IoSocket *self, IoValue *locals, IoMessage *m);
IoValue *IoSocket_open(IoSocket *self, IoValue *locals, IoMessage *m);
IoValue *IoSocket_connect(IoSocket *self, IoValue *locals, IoMessage *m);
IoValue *IoSocket_asyncConnect(IoSocket *self, IoValue *locals, IoMessage *m);
IoValue *IoSocket_waitForConnect(IoSocket *self, IoValue *locals, IoMessage *m);
IoValue *IoSocket_isConnected(IoSocket *self, IoValue *locals, IoMessage *m);
IoValue *IoSocket_isConnected_(IoSocket *self, IoValue *locals, IoMessage *m);
IoValue *IoSocket_write(IoSocket *self, IoValue *locals, IoMessage *m);
IoValue *IoSocket_writeLine(IoSocket *self, IoValue *locals, IoMessage *m);
IoValue *IoSocket_read(IoSocket *self, IoValue *locals, IoMessage *m);
IoValue *IoSocket_readLine(IoSocket *self, IoValue *locals, IoMessage *m);
IoValue *IoSocket_close(IoSocket *self, IoValue *locals, IoMessage *m);

IoValue *IoSocket_isLineRead(IoSocket *self, IoValue *locals, IoMessage *m);
IoValue *IoSocket_asyncReadLine(IoSocket *self, IoValue *locals, IoMessage *m);
IoValue *IoSocket_waitForReadLine(IoSocket *self, IoValue *locals, IoMessage *m);
IoValue *IoSocket_getLineRead(IoSocket *self, IoValue *locals, IoMessage *m);

class CConnectingSocket : public CActive
{
private:
	InternalSocket* socket;
	IoSocket* ioSocket;
	int state;
	RHostResolver resolver;
	TNameEntry entry;


public:
	static CConnectingSocket* NewL(InternalSocket* s, IoSocket* ios)
	{
		CConnectingSocket* self = new (ELeave) CConnectingSocket(s, ios);
		self->ConstructL();
		return self;
	}

	CConnectingSocket(InternalSocket* s, IoSocket* ios) : 
		CActive(CActive::EPriorityStandard),
		socket(s),
		ioSocket(ios),
		state(0)
	{
	}

	~CConnectingSocket() 
	{
		Cancel();
	}

	void ConstructL()
	{
		CActiveScheduler::Add(this);
	}

	void StartConnectL()
	{
		IoState* state = (IoState*)ioSocket->tag->state;
		CConsoleControl* control = (CConsoleControl*)IoState_userData(state);
		TInt result = resolver.Open(control->socketServer, 2048, 17);
		if(result != 0)
		{
			Cancel();
			IoState_error_description_(state, "IoSocket.connect", "Could not open resolver: '%d'\n", result); 
		}
		else
		{
			TPtr16 ptr16 = stringToPtr16(ioSocket->host);
			resolver.GetByName(ptr16, entry, iStatus);
			SetActive();
		}
	}

	void RunL()
	{
		if(state == 0)
		{
			if(iStatus.Int() != 0)
			{
				Cancel();
				IoState_error_description_((IoState*)ioSocket->tag->state, "IoSocket.connect", "resolver.GetByName: '%d'\n", iStatus.Int()); 
			}
			state = 1;
			TNameRecord record(entry());
			TSockAddr addr(record.iAddr);
			addr.SetPort(ioSocket->port);
			resolver.Close();

			socket->socket.Connect(addr, iStatus);
			SetActive();
		}
		else if(state == 1)
		{
			ioSocket->isConnected = 1;
			if(iStatus.Int() != 0)
			{
				Cancel();
				IoState_error_description_((IoState*)ioSocket->tag->state, "IoSocket.connect", "socket.connect: '%d'\n", iStatus.Int()); 
			}
		}
	}

	void DoCancel()
	{
		if(state == 0)
		{
			resolver.Close();
		}
		socket->socket.CancelAll();
		socket->socket.Close();
	}
};

#define READ_STATE_BUFFER_UNDERFLOW 1
#define READ_STATE_BUFFER_READ 2
#define READ_STATE_BUFFER_POST_UNDERFLOW 3
#define READ_STATE_BUFFER_COMPLETE 4

class CReadLineSocket : public CActive
{
private:
	InternalSocket* socket;
	IoSocket* ioSocket;
	char* originalBuffer;
	char* buffer;
	int length;
	int state;
	int lineState;
	TSockXfrLength tlen;

public:
	static CReadLineSocket* NewL(InternalSocket* s, IoSocket* ios, char* buffer, int length)
	{
		CReadLineSocket* self = new (ELeave) CReadLineSocket(s, ios, buffer, length);
		self->ConstructL();
		return self;
	}

	CReadLineSocket(InternalSocket* s, IoSocket* ios, char* b, int len) : 
		CActive(CActive::EPriorityStandard),
		socket(s),
		ioSocket(ios),
		state(0),
		lineState(0),
		buffer(b),
		originalBuffer(b),
		length(len)
	{
	}

	~CReadLineSocket() 
	{
		Cancel();
	}

	void ConstructL()
	{
		CActiveScheduler::Add(this);
	}

	void CheckBuffer()
	{
		if(length <= 0)
		{
			state = READ_STATE_BUFFER_COMPLETE;
		}
		else if(socket->bufferTop == socket->bufferBottom)
		{
			state = READ_STATE_BUFFER_UNDERFLOW;
		}
		else
		{
			state = READ_STATE_BUFFER_READ;
		}
		TRequestStatus* tempStatus = &iStatus;
		User::RequestComplete(tempStatus, KErrNone);
	}

	void StartReadLineL()
	{
		SetActive();
		CheckBuffer();
	}

	void RunL()
	{
		switch(state)
		{
			case READ_STATE_BUFFER_UNDERFLOW:
			{
				TPtr8 ptr8((TUint8*)socket->buffer, SOCKET_BUFFER_SIZE, SOCKET_BUFFER_SIZE);
				socket->socket.RecvOneOrMore(ptr8, 0, iStatus, tlen);	
				state = READ_STATE_BUFFER_POST_UNDERFLOW;
				SetActive();
				break;
			}

			case READ_STATE_BUFFER_POST_UNDERFLOW:
			{
				socket->bufferTop = tlen();
				socket->bufferBottom = 0;
				state = READ_STATE_BUFFER_READ;
				SetActive();
				TRequestStatus* tempStatus = &iStatus;
				User::RequestComplete(tempStatus, KErrNone);
				break;
			}

			case READ_STATE_BUFFER_READ:
			{
				if(length == 0)
				{
					state = READ_STATE_BUFFER_COMPLETE;
					SetActive();
					TRequestStatus* tempStatus = &iStatus;
					User::RequestComplete(tempStatus, KErrNone);
					break;
				}
				else if(socket->bufferTop == socket->bufferBottom)
				{
					state = READ_STATE_BUFFER_UNDERFLOW;
					SetActive();
					TRequestStatus* tempStatus = &iStatus;
					User::RequestComplete(tempStatus, KErrNone);
					break;
				}
				else if(socket->bufferTop - socket->bufferBottom >= length)
				{
					char* start = socket->buffer + socket->bufferBottom;
					char* end = socket->buffer + length;

					while(start != end)
					{
						char c = *start++;
						if(c == 13)
						{
							lineState = 1;
						}
						else if(c == 10)
						{
							if(lineState == 1)
							{
								lineState = 2;
								socket->bufferBottom = start - socket->buffer;
								length = 0;
								state = READ_STATE_BUFFER_COMPLETE;
								SetActive();
								TRequestStatus* tempStatus = &iStatus;
								User::RequestComplete(tempStatus, KErrNone);
								return;
							}
							else
							{
								*buffer++ = c;
							}
						}
						else
						{
							if(lineState == 1)
							{
								*buffer++ = 13;
							}
							else
							{
								*buffer++ = c;
							}
							state = 0;
						}
					}
					socket->bufferBottom += length;
					length = 0;
					state = READ_STATE_BUFFER_COMPLETE;
					SetActive();
					TRequestStatus* tempStatus = &iStatus;
					User::RequestComplete(tempStatus, KErrNone);
					return;
				}
				else
				{
					int l = min(length, socket->bufferTop - socket->bufferBottom);
					char* start = socket->buffer + socket->bufferBottom;
					char* end = socket->buffer + l;
					while(lineState != 2 && start != end)
					{
						char c = *start++;
						if(c == 13)
						{
							lineState = 1;
						}
						else if(c == 10)
						{
							if(lineState == 1)
							{
								lineState = 2;
								socket->bufferBottom = start - socket->buffer;
								state = READ_STATE_BUFFER_COMPLETE;
								SetActive();
								TRequestStatus* tempStatus = &iStatus;
								User::RequestComplete(tempStatus, KErrNone);
								return;
							}
							else
							{
								*buffer++ = c;
							}
						}
						else
						{
							if(lineState == 1)
							{
								*buffer++ = 13;
							}
							else
							{
								*buffer++ = c;
							}
							lineState = 0;
						}
					}
				
					length -= l;
					buffer += l;
					socket->bufferBottom += l;
				}

				state = READ_STATE_BUFFER_READ;
				SetActive();
				TRequestStatus* tempStatus = &iStatus;
				User::RequestComplete(tempStatus, KErrNone);
				break;
			}

			case READ_STATE_BUFFER_COMPLETE:
			{
				ioSocket->lineRead = originalBuffer;
				break;
			}
		}
	}

	void DoCancel()
	{
		socket->socket.CancelAll();
	}
};


IoTag *IoSocket_initTagWithId_(void *ioState, int tagId)
{
  IoTag *tag = IoTag_new();
  tag->state = ioState;
  tag->freeCallback = (TagFreeCallback *)IoSocket_free;
  tag->nameCallback = (TagNameCallback *)IoSocket_name;
  tag->markCallback = (TagMarkCallback *)IoSocket_mark;

  Tag_addMethod(tag, "clone", (void*)IoSocket_clone);
  Tag_addMethod(tag, "setHost", (void*)IoSocket_host_);
  Tag_addMethod(tag, "host", (void*)IoSocket_host);
  Tag_addMethod(tag, "setPort", (void*)IoSocket_port_);
  Tag_addMethod(tag, "port", (void*)IoSocket_port);
  Tag_addMethod(tag, "open", (void*)IoSocket_open);
  Tag_addMethod(tag, "connect", (void*)IoSocket_connect);
  Tag_addMethod(tag, "asyncConnect", (void*)IoSocket_asyncConnect);
  Tag_addMethod(tag, "waitForConnect", (void*)IoSocket_waitForConnect);
  Tag_addMethod(tag, "isConnected", (void*)IoSocket_isConnected);
  Tag_addMethod(tag, "setIsConnected", (void*)IoSocket_isConnected_);
  Tag_addMethod(tag, "write", (void*)IoSocket_write);
  Tag_addMethod(tag, "writeLine", (void*)IoSocket_writeLine);
  Tag_addMethod(tag, "read", (void*)IoSocket_read);
  Tag_addMethod(tag, "readLine", (void*)IoSocket_readLine);
  Tag_addMethod(tag, "close", (void*)IoSocket_close);

  
  Tag_addMethod(tag, "isLineRead", (void*)IoSocket_isLineRead);
  Tag_addMethod(tag, "asyncReadLine", (void*)IoSocket_asyncReadLine);
  Tag_addMethod(tag, "waitForReadLine", (void*)IoSocket_waitForReadLine);
  Tag_addMethod(tag, "getLineRead", (void*)IoSocket_getLineRead);
  
  return tag;
}

IoSocket *IoSocket_new(void *state)
{
  IoTag *tag = IoState_tagWithInitFunction_((IoState*)state, IoSocket_initTagWithId_);
  IoSocket *self = (IoSocket *)malloc(sizeof(IoSocket));
  memset(self, 0x0, sizeof(IoSocket));
  self->tag = tag;
  self->color = IOVALUE_WHITE();
  self->socket = new InternalSocket();
  self->host = strdup("localhost");
  self->isConnected = false;
  self->lineRead = 0;
  self->activeConnect = 0;
  self->activeReadLine = 0;
  IoState_addValue_((IoState*)self->tag->state, (IoValue *)self);
  return self;
}

IoValue *IoSocket_clone(IoSocket *self, IoValue *locals, IoMessage *m)
{ 
  IoSocket *newSocket = IoSocket_new(self->tag->state);
  newSocket->host = strdup(self->host);
  return (IoValue *)newSocket; 
}

void IoSocket_free(IoSocket *self) 
{ 
	if(self->activeConnect)
	{
		delete self->activeConnect;
		self->activeConnect = 0;
	}
	self->socket->socket.Close();
	delete self->socket;
	self->socket = 0;
	free(self->host);
	free(self);
}

char *IoSocket_name(IoSocket *self) 
{ 
	return "Socket"; 
}

void IoSocket_mark(IoSocket *self) 
{ 
}

IoValue *IoSocket_host(IoSocket *self, IoValue *locals, IoMessage *m)
{ 
	return (IoValue *)USTRING(self->host); 
}

IoValue *IoSocket_host_(IoSocket *self, IoValue *locals, IoMessage *m)
{ 
  IoString *host = (IoString*)IoMessage_locals_stringArgAt_(m, locals, 0);
  self->host = strdup(CSTRING(host));
  return (IoValue *)self; 
}

IoValue *IoSocket_port(IoSocket *self, IoValue *locals, IoMessage *m)
{ 
	return (IoValue *)IONUMBER(self->port); 
}

IoValue *IoSocket_port_(IoSocket *self, IoValue *locals, IoMessage *m)
{ 
  IoNumber *port = (IoNumber*)IoMessage_locals_numberArgAt_(m, locals, 0);
  self->port = IoNumber_asInt(port);
  return (IoValue *)self; 
}

IoValue *IoSocket_isConnected(IoSocket *self, IoValue *locals, IoMessage *m)
{
	return (IoValue *)IONUMBER(self->isConnected); 
}

IoValue *IoSocket_isConnected_(IoSocket *self, IoValue *locals, IoMessage *m)
{
  IoNumber *isConnected = (IoNumber*)IoMessage_locals_numberArgAt_(m, locals, 0);
  self->isConnected = IoNumber_asInt(isConnected);
  return (IoValue *)self; 
}

IoValue *IoSocket_open(IoSocket *self, IoValue *locals, IoMessage *m)
{ 
	self->socket->ResetBuffer();
	IoState* state = (IoState*)self->tag->state;
	CConsoleControl* control = (CConsoleControl*)IoState_userData(state);
	
	return (IoValue *)IONUMBER(self->socket->socket.Open(control->socketServer, KAfInet, KSockStream, KProtocolInetTcp));
}

IoValue *IoSocket_connect(IoSocket *self, IoValue *locals, IoMessage *m)
{
	if(self->activeConnect)
	{
		delete self->activeConnect;
	}
	self->activeConnect = CConnectingSocket::NewL(self->socket, self);
	self->activeConnect->StartConnectL();
	IoState* state = (IoState*)self->tag->state;
	while(!self->isConnected)
	{	
		IoState_yield(state);
	}
	return (IoValue*)self;
}

IoValue *IoSocket_asyncConnect(IoSocket *self, IoValue *locals, IoMessage *m)
{
	if(self->activeConnect)
	{
		delete self->activeConnect;
	}
	self->activeConnect = CConnectingSocket::NewL(self->socket, self);
	self->activeConnect->StartConnectL();
	return (IoValue*)self;
}

IoValue *IoSocket_waitForConnect(IoSocket *self, IoValue *locals, IoMessage *m)
{
	IoState* state = (IoState*)self->tag->state;
	while(!self->isConnected)
	{	
		IoState_yield(state);
	}
	return (IoValue*)self;
}

IoValue *IoSocket_asyncReadLine(IoSocket *self, IoValue *locals, IoMessage *m)
{
	if(self->activeReadLine)
	{
		delete self->activeReadLine;
	}

    IoNumber *size = (IoNumber*)IoMessage_locals_numberArgAt_(m, locals, 0);
	int trueSize = IoNumber_asInt(size);
	char* buffer = (char*)malloc(trueSize + 1);
	memset(buffer, 0, trueSize + 1);
	self->activeReadLine = CReadLineSocket::NewL(self->socket, self, buffer, trueSize);
	self->activeReadLine->StartReadLineL();
	return (IoValue*)self;
}

IoValue *IoSocket_waitForReadLine(IoSocket *self, IoValue *locals, IoMessage *m)
{
	IoState* state = (IoState*)self->tag->state;
	while(!self->lineRead)
	{	
		IoState_yield(state);
	}
	IoValue* value = (IoValue*)USTRING(self->lineRead); 
	free(self->lineRead);
	self->lineRead = 0;
	return (IoValue*)value;
}

IoValue *IoSocket_isLineRead(IoSocket *self, IoValue *locals, IoMessage *m)
{
	bool value = self->lineRead != 0;
	return (IoValue *)IONUMBER(value); 
}

IoValue *IoSocket_getLineRead(IoSocket *self, IoValue *locals, IoMessage *m)
{
	return (IoValue*)USTRING(self->lineRead); 
}

IoValue *IoSocket_close(IoSocket *self, IoValue *locals, IoMessage *m)
{ 
	if(self->activeConnect)
	{
		delete self->activeConnect;
		self->activeConnect = 0;
	}
	if(self->activeReadLine)
	{
		delete self->activeReadLine;
		self->activeReadLine = 0;
	}
	self->socket->socket.Close();
	return (IoValue*)self;
}

IoValue *IoSocket_write(IoSocket *self, IoValue *locals, IoMessage *m)
{ 
	IoState* state = (IoState*)self->tag->state;
	IoString *ioText = (IoString*)IoMessage_locals_stringArgAt_(m, locals, 0);
	char* text = CSTRING(ioText);
	int len = strlen(text);
	TRequestStatus status;
	TPtr8 ptr8((TUint8*)text, len, len);
	self->socket->socket.Write(ptr8, status);
	User::WaitForRequest(status);
	if(status.Int() != 0)
	{
		IoState_error_description_(state, "IoSocket.write", "RSocket.Write: '%d'\n", status.Int()); 
	}

	return (IoValue*)self;
}

IoValue *IoSocket_writeLine(IoSocket *self, IoValue *locals, IoMessage *m)
{ 
	IoState* state = (IoState*)self->tag->state;
	IoString *ioText = (IoString*)IoMessage_locals_stringArgAt_(m, locals, 0);
	char* text = strdup(CSTRING(ioText));
	int len = strlen(text);
	text = (char*)realloc(text, len + 3);
	text[len] = 13;
	text[len + 1] = 10;
	text[len + 2] = 0;

	TRequestStatus status;
	TPtr8 ptr8((TUint8*)text, len + 2, len + 3);
	self->socket->socket.Write(ptr8, status);
	free(text);
	User::WaitForRequest(status);
	if(status.Int() != 0)
	{
		IoState_error_description_(state, "IoSocket.write", "RSocket.Write: '%d'\n", status.Int()); 
	}

	return (IoValue*)self;
}

IoValue *IoSocket_read(IoSocket *self, IoValue *locals, IoMessage *m)
{ 
    IoNumber *size = (IoNumber*)IoMessage_locals_numberArgAt_(m, locals, 0);
	IoState* state = (IoState*)self->tag->state;
	int trueSize = IoNumber_asInt(size);
	char* buffer = (char*)malloc(trueSize + 1);
	memset(buffer, 0, trueSize + 1);
	TInt status = self->socket->ReadOneOrMore(buffer, trueSize);
	if(status != 0)
	{
		IoState_error_description_(state, "IoSocket.read", "RSocket.Read: '%d'\n", status); 
	}

	IoValue* result = (IoValue*)USTRING(buffer);
	free(buffer);
	return result;
}

IoValue *IoSocket_readLine(IoSocket *self, IoValue *locals, IoMessage *m)
{ 
    IoNumber *size = (IoNumber*)IoMessage_locals_numberArgAt_(m, locals, 0);
	IoState* state = (IoState*)self->tag->state;
	if(self->activeReadLine)
	{
		delete self->activeReadLine;
		self->activeReadLine = 0;
	}
	int trueSize = IoNumber_asInt(size);
	char* buffer = (char*)malloc(trueSize + 1);
	memset(buffer, 0, trueSize + 1);
	self->activeReadLine = CReadLineSocket::NewL(self->socket, self, buffer, trueSize);
	self->activeReadLine->StartReadLineL();
	while(!self->lineRead)
	{	
		IoState_yield(state);
	}
	IoValue* value = (IoValue*)USTRING(self->lineRead); 
	free(self->lineRead);
	self->lineRead = 0;
	return (IoValue*)value;

}


void initSocketAddons(IoState* state)
{
  IoState_addTagWithInitFunc_(state, IoSocket_initTagWithId_);
  IoObject_setSlot_to_(state->lobby, IoState_stringWithCString_(state, "Socket"), IoSocket_new(state));
}

