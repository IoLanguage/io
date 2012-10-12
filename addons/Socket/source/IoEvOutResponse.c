//metadoc EvRequest copyright Steve Dekorte, 2004
//metadoc EvRequest license BSD revised
//metadoc EvRequest description Networking Event.
//metadoc EvRequest category Networking

#include "IoEvOutResponse.h"
#include "IoEvConnection.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoMap.h"
#include "IoSeq.h"

#define REQUEST(self) ((struct evhttp_request *)IoObject_dataPointer(self))

static const char *protoId = "EvOutResponse";

IoTag *IoEvOutResponse_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoEvOutResponse_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoEvOutResponse_free);
	return tag;
}

IoEvOutResponse *IoEvOutResponse_proto(void *state)
{
	IoObject *self = IoObject_new(state);

	IoObject_tag_(self, IoEvOutResponse_newTag(state));
	IoObject_setDataPointer_(self, 0x0);

	IoState_registerProtoWithId_((IoState *)state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"requestId", IoEvOutResponse_requestId},
		{"asyncSend", IoEvOutResponse_asyncSend},
		{NULL, NULL},
		};

		IoObject_addMethodTable_(self, methodTable);
	} 


	return self;
}

IoEvOutResponse *IoEvOutResponse_rawClone(IoEvOutResponse *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, 0x0);
	return self;
}

IoEvOutResponse *IoEvOutResponse_new(void *state)
{
	IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
	return IOCLONE(proto);
}

void IoEvOutResponse_free(IoEvOutResponse *self)
{
	if (REQUEST(self))
	{	
		// looks like evhttpd is responsible for freeing the request
		IoObject_setDataPointer_(self, 0x0);
	}
}

void IoEvOutResponse_rawSetRequest_(IoEvOutResponse *self, struct evhttp_request *req)
{
	IoObject_setDataPointer_(self, req);
}


void IoEvOutResponse_writeResponseHeaders(IoEvOutResponse *self)
{	
	struct evhttp_request *req = REQUEST(self);
	IoMap *headers  = IoObject_getSlot_(self, IOSYMBOL("headers"));
	assert(ISMAP(headers));
	
	{
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
			"Pragma",
			"Proxy-Authenticate",
			"Refresh",
			"Retry-After",
			"Server",
			"Set-Cookie",
			"Trailer",
			"Transfer-Encoding",
			"Vary",
			"Via",
			"Warning",
			"WWW-Authenticate",
			0x0
		};
	
		//struct evkeyvalq *h = req->input_headers;
		int i = 0;
		const char *name;
		
		//IoMap_rawEmpty(headers);
		
		while ((name = headerNames[i]))
		{
			IoSeq *v = IoMap_rawAt(headers, IOSYMBOL(name));
			
			if (v && ISSEQ(v)) 
			{
				evhttp_remove_header(req->output_headers, name);
				evhttp_add_header(req->output_headers, name, CSTRING(v));
			}

			i ++;
		}
	}
}

IoObject *IoEvOutResponse_asyncSend(IoEvOutResponse *self, IoObject *locals, IoMessage *m)
{
	IoEvOutResponse_writeResponseHeaders(self);

	{
		IoSeq *data = IoObject_seqGetSlot_(self, IOSYMBOL("data"));
		IoSeq *responseMessage = IoObject_seqGetSlot_(self, IOSYMBOL("responseMessage"));
		int statusCode = IoObject_doubleGetSlot_(self, IOSYMBOL("statusCode"));
		struct evhttp_request *req = REQUEST(self);

		struct evbuffer *buf = evbuffer_new();
		evbuffer_add(buf, IOSEQ_BYTES(data), IOSEQ_LENGTH(data));
		evhttp_send_reply(req, statusCode, CSTRING(responseMessage), buf);		
		//evhttp_send_reply_end(req);
		evbuffer_free(buf);
	}
	
	return self;
}

IoObject *IoEvOutResponse_requestId(IoEvOutResponse *self, IoObject *locals, IoMessage *m)
{
	struct evhttp_request *req = REQUEST(self);
	char s[64];
	sprintf(s, "%p", (void *)req);
	return IOSYMBOL(s);
}

