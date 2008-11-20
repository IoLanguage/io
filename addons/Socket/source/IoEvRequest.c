//metadoc EvRequest copyright Steve Dekorte, 2004
//metadoc EvRequest license BSD revised
//metadoc EvRequest description Networking Event.
//metadoc EvRequest category Networking

#include "IoEvRequest.h"
#include "IoEvConnection.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoMap.h"
#include "IoSeq.h"

#define REQUEST(self) ((struct evhttp_request *)IoObject_dataPointer(self))

IoTag *IoEvRequest_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("EvRequest");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoEvRequest_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoEvRequest_free);
	return tag;
}

IoEvRequest *IoEvRequest_proto(void *state)
{
	IoObject *self = IoObject_new(state);

	IoObject_tag_(self, IoEvRequest_newTag(state));
	IoObject_setDataPointer_(self, (struct event *)calloc(1, sizeof(struct event)));

	IoState_registerProtoWithFunc_((IoState *)state, self, IoEvRequest_proto);

	{
		IoMethodTable methodTable[] = {
		{"asyncSend", IoEvRequest_send},
		{"cancel", IoEvRequest_cancel},
		{NULL, NULL},
		};

		IoObject_addMethodTable_(self, methodTable);
	}


	return self;
}

IoEvRequest *IoEvRequest_rawClone(IoEvRequest *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, 0x0);
	return self;
}

IoEvRequest *IoEvRequest_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoEvRequest_proto);
	return IOCLONE(proto);
}

void IoEvRequest_free(IoEvRequest *self)
{
	if (REQUEST(self))
	{	
		//printf("IoEvRequest_free skipping free?\n");
		//evhttp_request_free(REQUEST(self));
		IoObject_setDataPointer_(self, 0x0);
	}
}

void IoEvRequest_RequestDoneCallback(struct evhttp_request *request, void *arg)
{
	IoObject *self = arg;
	
	const char *headerNames[] = {
		"Accept-Ranges",
		"Age",
		"Allow",
		"Cache-Control",
		"Content-Encoding",
		"Content-Language",
		"Content-Length",
		"Content-Location",
		"Content-Disposition",
		"Content-MD5",
		"Content-Range",
		"Content-Type",
		"Date",
		"ETag",
		"Expires",
		"Last-Modified",
		"Location",
		"Server",
		"Set-Cookie", 
		0x0
	};
	
	if (REQUEST(self))
	{
		struct evkeyvalq *headers = request->output_headers;
		struct evbuffer *b = request->input_buffer;
		int i = 0;
		const char *name;
		IoMap *outputHeaders = IoMap_new(IOSTATE);
		
		IoObject_setSlot_to_(self, IOSYMBOL("data"), IOSEQ(b->buffer, b->off));
		IoObject_setSlot_to_(self, IOSYMBOL("inputHeaders"), outputHeaders);

		while ((name = headerNames[i]))
		{
			const char *value = evhttp_find_header(headers, name);
			if (value) IoMap_rawAtPut(outputHeaders, IOSYMBOL(name), IOSYMBOL(value));
			i ++;
		}
		
		//IoEvRequest_free(self);	
		IoMessage_locals_performOn_(IOSTATE->didFinishMessage, self, self); 
	}
}

IoObject *IoEvRequest_send(IoEvRequest *self, IoObject *locals, IoMessage *m)
{
	IoEvConnection *connection = IoObject_getSlot_(self, IOSYMBOL("connection"));
	IoSeq *requestType = IoObject_symbolGetSlot_(self, IOSYMBOL("requestType"));
	IoSeq *uri = IoObject_symbolGetSlot_(self, IOSYMBOL("uri"));
	IoMap *headers = IoObject_getSlot_(self, IOSYMBOL("inputHeaders"));
	int rt = 0;
	
	IOASSERT(REQUEST(self) == 0x0, "request already sent");
	IOASSERT(ISMAP(headers), "headers slot needs to be a Map");
	IOASSERT(ISEVCONNECTION(connection), "connection slot not set properly");
	IOASSERT(IoEvConnection_rawConnection(connection), "connection not open");
	
	if (IoSeq_rawEqualsCString_(requestType, "GET"))
	{
		rt = EVHTTP_REQ_GET;
	}
	else if (IoSeq_rawEqualsCString_(requestType, "POST"))
	{
		rt = EVHTTP_REQ_POST;
	}
	else
	{
		IOASSERT(0, "requestType must be GET or POST");
	}
	
	IoObject_setDataPointer_(self, evhttp_request_new(IoEvRequest_RequestDoneCallback, self));

	{
		PHash *h = IoMap_rawHash(headers);
		PHASH_FOREACH(h, k, v, 
			evhttp_add_header(REQUEST(self)->input_headers, CSTRING(k), CSTRING(v));
		)
	}
	
	int r = evhttp_make_request(IoEvConnection_rawConnection(connection),	
		REQUEST(self), rt, CSTRING(uri));
	
	return r == -1 ? IONIL(self) : self;
}


IoObject *IoEvRequest_cancel(IoEvRequest *self, IoObject *locals, IoMessage *m)
{
	IoEvRequest_free(self);
	return self;
}
