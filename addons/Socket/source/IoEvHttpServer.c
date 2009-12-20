//metadoc EvRequest copyright Steve Dekorte, 2004
//metadoc EvRequest license BSD revised
//metadoc EvRequest description Networking Event.
//metadoc EvRequest category Networking

#include "IoEvHttpServer.h"
#include "IoEventManager.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include "IoMap.h"
#include <event.h>

#define HTTP_SERVERERROR 500
#define HTTP_SERVERERROR_MESSAGE "Internal Server Error"
#define HTTP_OK_MESSAGE	"OK"


#define HTTPD(self) ((struct evhttp *)IoObject_dataPointer(self))

IoTag *IoEvHttpServer_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("EvHttpServer");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoEvHttpServer_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoEvHttpServer_free);
	return tag;
}

IoEvHttpServer *IoEvHttpServer_proto(void *state)
{
	IoObject *self = IoObject_new(state);

	IoObject_tag_(self, IoEvHttpServer_newTag(state));
	IoObject_setDataPointer_(self, 0x0);

	IoState_registerProtoWithFunc_((IoState *)state, self, IoEvHttpServer_proto);

	{
		IoMethodTable methodTable[] = {
		{"start", IoEvHttpServer_start},
		{"stop", IoEvHttpServer_stop},
		{NULL, NULL},
		};

		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoEvHttpServer *IoEvHttpServer_rawClone(IoEvHttpServer *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, 0x0);
	return self;
}

IoEvHttpServer *IoEvHttpServer_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoEvHttpServer_proto);
	return IOCLONE(proto);
}

void IoEvHttpServer_rawStop(IoEvHttpServer *self)
{	
	if(HTTPD(self))
	{
		evhttp_free(HTTPD(self));
		IoObject_setDataPointer_(self, 0x0);
	}
}

void IoEvHttpServer_free(IoEvHttpServer *self)
{	
	IoEvHttpServer_rawStop(self);
}

void IoEvHttpServer_readRequestHeaders(IoEvHttpServer *self, struct evhttp_request *req)
{	
	IoObject *request  = IoObject_getSlot_(self, IOSYMBOL("request"));
	//IOASSERT(!ISNIL(request), "nil request");
	assert(!ISNIL(request));
	
	IoMap *headers  = IoObject_getSlot_(request, IOSYMBOL("headers"));
	//IOASSERT(ISMAP(headers), "request headers not a map");
	assert(ISMAP(headers));
	
	{
		struct evkeyval *header;

		TAILQ_FOREACH(header, req->input_headers, next)
		{
			UArray *key = UArray_newWithCString_copy_(header->key, 1);
			UArray_tolower(key);
			
			{
				IoSeq *mapKey = IoState_symbolWithUArray_copy_(IOSTATE, key, 0);

				if (header->value)
				{
					IoMap_rawAtPut(headers, mapKey, IOSYMBOL(header->value));
				}
				else
				{
					IoMap_rawAtPut(headers, mapKey, IONIL(self));
				}
			}
		}
	}
}

void IoEvHttpServer_writeResponseHeaders(IoEvHttpServer *self, struct evhttp_request *req)
{	
	IoObject *response  = IoObject_getSlot_(self, IOSYMBOL("response"));
	//IOASSERT(!ISNIL(response), "nil request");
	assert(!ISNIL(response));
	
	IoMap *headers  = IoObject_getSlot_(response, IOSYMBOL("headers"));
	//IOASSERT(ISMAP(headers), "request headers not a map");
	assert(ISMAP(headers));
	
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
	
	{
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

void IoEvHttpServer_handleRequest(struct evhttp_request *req, void *arg)
{
	IoEvHttpServer *self = arg;

	const char *uri = evhttp_request_uri(req);
	struct evbuffer *buf = 0x0;
	IoObject *request  = IoObject_getSlot_(self, IOSYMBOL("request"));

	IoSeq *postData = IOSEQ((const unsigned char *)EVBUFFER_DATA(req->input_buffer), (int)EVBUFFER_LENGTH(req->input_buffer));
	
	IoObject_setSlot_to_(request, IOSYMBOL("uri"), IOSYMBOL(uri));
	IoObject_setSlot_to_(request, IOSYMBOL("postData"), postData);
	IoEvHttpServer_readRequestHeaders(self, req);
	
	if (strcmp(uri, "/favicon.ico") == 0)
	{
		evhttp_send_reply(req, HTTP_OK, HTTP_OK_MESSAGE, buf);
	}
	else
	{
		IoObject *response = IoObject_getSlot_(self, IOSYMBOL("response"));
		
		//IoObject_perform(self, self, IOSYMBOL("handleRequestCallback"));
		IoMessage *m = IoMessage_newWithName_label_(IOSTATE, IOSYMBOL("handleRequestCallback"), IOSYMBOL("IoEvHttpServer"));
		IoMessage_locals_performOn_(m, self, self);

		IoEvHttpServer_writeResponseHeaders(self, req);
		
		IoSeq *data = IoObject_seqGetSlot_(response, IOSYMBOL("data"));
		int statusCode = IoObject_doubleGetSlot_(response, IOSYMBOL("statusCode"));

		buf = evbuffer_new();
		evbuffer_add_printf(buf, "%s", CSTRING(data));

/*
		IoMap *responseHeaders = IoObject_seqGetSlot_(response, IOSYMBOL("headers"));
		evhttp_remove_header(req->output_headers, k);
		evhttp_add_header(req->output_headers, k, v);

*/
		if (statusCode == 200)
		{
			evhttp_send_reply(req, HTTP_OK, HTTP_OK_MESSAGE, buf);
		}
		else
		{
			evhttp_send_reply(req, HTTP_SERVERERROR, HTTP_SERVERERROR_MESSAGE, buf);		
		}
	}
	
	evhttp_send_reply_end(req);
	if(buf) evbuffer_free(buf);
}

IoObject *IoEvHttpServer_start(IoEvHttpServer *self, IoObject *locals, IoMessage *m)
{
	IoEventManager *em = IoObject_getSlot_(self, IOSYMBOL("eventManager"));
	IoSeq *host = IoObject_seqGetSlot_(self, IOSYMBOL("host"));
	int port = IoObject_doubleGetSlot_(self, IOSYMBOL("port"));

	IOASSERT(HTTPD(self) == 0x0, "already running");
	IOASSERT(ISEEVENTMANAGER(em), "eventManager slot not set properly");
	
	IoObject_setDataPointer_(self, evhttp_new(IoEventManager_rawBase(em)));
	
	if(HTTPD(self))
	{
		evhttp_set_timeout(HTTPD(self), 10);
		evhttp_set_gencb(HTTPD(self), IoEvHttpServer_handleRequest, self);  
		evhttp_bind_socket(HTTPD(self), CSTRING(host), port);
		return self;
	}
	
	return IONIL(self);
}

IoObject *IoEvHttpServer_stop(IoEvHttpServer *self, IoObject *locals, IoMessage *m)
{
	IoEvHttpServer_rawStop(self);
	return self;
}
