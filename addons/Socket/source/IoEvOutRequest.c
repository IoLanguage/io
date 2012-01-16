//metadoc EvRequest copyright Steve Dekorte, 2004
//metadoc EvRequest license BSD revised
//metadoc EvRequest description Networking Event.
//metadoc EvRequest category Networking

#include "IoEvOutRequest.h"
#include "IoEvConnection.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoMap.h"
#include "IoSeq.h"

#define REQUEST(self) ((struct evhttp_request *)IoObject_dataPointer(self))

static const char *protoId = "EvOutRequest";


IoTag *IoEvOutRequest_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoEvOutRequest_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoEvOutRequest_free);
	return tag;
}

IoEvOutRequest *IoEvOutRequest_proto(void *state)
{
	IoObject *self = IoObject_new(state);

	IoObject_tag_(self, IoEvOutRequest_newTag(state));
	IoObject_setDataPointer_(self, 0x0);

	IoState_registerProtoWithId_((IoState *)state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"asyncSend", IoEvOutRequest_send},
		{"cancel", IoEvOutRequest_cancel},
		
		{"encodeUri", IoEvOutRequest_encodeUri},
		{"decodeUri", IoEvOutRequest_decodeUri},
		{"escapeHtml", IoEvOutRequest_htmlEscape},

		{NULL, NULL},
		};

		IoObject_addMethodTable_(self, methodTable);
	}


	return self;
}

IoEvOutRequest *IoEvOutRequest_rawClone(IoEvOutRequest *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, 0x0);
	return self;
}

IoEvOutRequest *IoEvOutRequest_new(void *state)
{
	IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
	return IOCLONE(proto);
}

void IoEvOutRequest_free(IoEvOutRequest *self)
{
	if (REQUEST(self))
	{	
		// looks like the connection or evhttp is responsible for freeing the request
		//evhttp_request_free(REQUEST(self));
		IoObject_setDataPointer_(self, 0x0);
	}
}

void IoEvOutRequest_RequestDoneCallback(struct evhttp_request *request, void *arg)
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
		struct evkeyvalq *headers = request->input_headers;
		struct evbuffer *b = request->input_buffer;
		int i = 0;
		const char *name;
		IoMap *responseHeaders = IoMap_new(IOSTATE);
		
		

		//IoObject_setSlot_to_(self, IOSYMBOL("data"), IOSEQ(b->buffer, b->off));
		size_t datlen = evbuffer_get_length(b);
		unsigned char *data = malloc(datlen);
		evbuffer_copyout(b, data, datlen);		
		IoSeq *dataSeq = IoSeq_newWithData_length_copy_(IOSTATE, data, datlen, 0);
		IoObject_setSlot_to_(self, IOSYMBOL("data"), dataSeq);
		
		IoObject_setSlot_to_(self, IOSYMBOL("responseHeaders"), responseHeaders);
		IoObject_setSlot_to_(self, IOSYMBOL("responseCode"), IONUMBER(request->response_code));
		
		while ((name = headerNames[i]))
		{
			const char *value = evhttp_find_header(headers, name);
			//printf("response header: '%s' : '%s'\n", name, value ? value : "nil");
			
			if (value) 
			{
				IoMap_rawAtPut(responseHeaders, IOSYMBOL(name), IOSYMBOL(value));
			}
			
			i ++;
		}
		
		IoMessage_locals_performOn_(IOSTATE->didFinishMessage, self, self); 
	}
}

IoObject *IoEvOutRequest_send(IoEvOutRequest *self, IoObject *locals, IoMessage *m)
{
	IoEvConnection *connection = IoObject_getSlot_(self, IOSYMBOL("connection"));
	IoSeq *requestType = IoObject_symbolGetSlot_(self, IOSYMBOL("requestType"));
	IoSeq *uri = IoObject_symbolGetSlot_(self, IOSYMBOL("uri"));
	IoMap *responseHeaders = IoObject_getSlot_(self, IOSYMBOL("requestHeaders"));
	int rtype = 0;
	int r;
	
	IOASSERT(REQUEST(self) == 0x0, "request already sent");
	IOASSERT(ISMAP(responseHeaders), "responseHeaders slot needs to be a Map");
	IOASSERT(ISEVCONNECTION(connection), "connection slot not set properly");
	IOASSERT(IoEvConnection_rawConnection(connection), "connection not open");
	
	if (IoSeq_rawEqualsCString_(requestType, "GET"))
	{
		rtype = EVHTTP_REQ_GET;
	}
	else if (IoSeq_rawEqualsCString_(requestType, "POST"))
	{
		IoSeq *postData = IoObject_seqGetSlot_(self, IOSYMBOL("postData"));
		rtype = EVHTTP_REQ_POST;
		evbuffer_add(REQUEST(self)->output_buffer, IOSEQ_BYTES(postData), IOSEQ_LENGTH(postData));
	}
	else
	{
		IOASSERT(0, "requestType must be GET or POST");
	}
	
	IoObject_setDataPointer_(self, evhttp_request_new(IoEvOutRequest_RequestDoneCallback, self));

	{
		PHash *rh = IoMap_rawHash(responseHeaders);
		PHASH_FOREACH(rh, k, v, 
			IOASSERT(ISSEQ(v), "responseHeader values must be Sequences");
			//printf("request header %s : %s\n", CSTRING(k), CSTRING(v));
			evhttp_add_header(REQUEST(self)->output_headers, CSTRING(k), CSTRING(v));
		)
	}
	
	r = evhttp_make_request(IoEvConnection_rawConnection(connection),	
		REQUEST(self), rtype, CSTRING(uri));
	
	//printf("send uri: %s\n", CSTRING(uri));
	return r == -1 ? IONIL(self) : self;
}

IoObject *IoEvOutRequest_cancel(IoEvOutRequest *self, IoObject *locals, IoMessage *m)
{
	IoEvOutRequest_free(self);
	return self;
}

IoObject *IoEvOutRequest_encodeUri(IoEvOutRequest *self, IoObject *locals, IoMessage *m)
{
	IoSeq *s = IoMessage_locals_seqArgAt_(m, locals, 0);
	return IoSeq_newWithCString_(IOSTATE, evhttp_encode_uri(CSTRING(s)));
}

IoObject *IoEvOutRequest_decodeUri(IoEvOutRequest *self, IoObject *locals, IoMessage *m)
{
	IoSeq *s = IoMessage_locals_seqArgAt_(m, locals, 0);
	return IoSeq_newWithCString_(IOSTATE, evhttp_decode_uri(CSTRING(s)));
}

IoObject *IoEvOutRequest_htmlEscape(IoEvOutRequest *self, IoObject *locals, IoMessage *m)
{
	IoSeq *s = IoMessage_locals_seqArgAt_(m, locals, 0);
	return IoSeq_newWithCString_(IOSTATE, evhttp_htmlescape(CSTRING(s)));
}
