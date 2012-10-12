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
#include "IoEvOutResponse.h"
#include <event.h>

#define HTTP_SERVERERROR 500
#define HTTP_SERVERERROR_MESSAGE "Internal Server Error"
#define HTTP_OK_MESSAGE	"OK"


#define HTTPD(self) ((struct evhttp *)IoObject_dataPointer(self))

static const char *protoId = "EvHttpServer";

IoTag *IoEvHttpServer_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
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

	IoState_registerProtoWithId_((IoState *)state, self, protoId);

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
	IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
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

void IoEvHttpServer_readRequestHeaders(IoEvHttpServer *self, IoObject *request, struct evhttp_request *req)
{	
	IoMap *headers;
	//IOASSERT(!ISNIL(request), "nil request");
	assert(!ISNIL(request));
	
	headers  = IoObject_getSlot_(request, IOSYMBOL("headers"));
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

void IoEvHttpServer_handleRequest(struct evhttp_request *req, void *arg)
{
	IoEvHttpServer *self = arg;

	const char *uri = evhttp_request_uri(req);
	IoObject *request  = IoObject_getSlot_(self, IOSYMBOL("requestProto"));
	char *httpMethod;
	switch (req->type)
	{
		case EVHTTP_REQ_GET:
			httpMethod = "GET";
			break;
		case EVHTTP_REQ_POST:
			httpMethod = "POST";
			break;
		case EVHTTP_REQ_HEAD:
			httpMethod = "HEAD";
			break;
		default:
			httpMethod = "UNSUPPORTED";
			break;
	}
	
	{
		IoSeq *postData = IOSEQ((const unsigned char *)EVBUFFER_DATA(req->input_buffer), (int)EVBUFFER_LENGTH(req->input_buffer));
		
		request = IOCLONE(request);
		IoObject_initClone_(self, request, IoMessage_newWithName_label_(IOSTATE, IOSYMBOL("IoEvHttpServer_handleRequest"), IOSYMBOL("IoEvHttpServer")), request);
		IoObject_setSlot_to_(request, IOSYMBOL("httpMethod"), IOSYMBOL(httpMethod));
		IoObject_setSlot_to_(request, IOSYMBOL("uri"), IOSYMBOL(uri));
		IoObject_setSlot_to_(request, IOSYMBOL("postData"), postData);
		IoEvHttpServer_readRequestHeaders(self, request, req);
		IoObject_setSlot_to_(self, IOSYMBOL("request"), request);
	}
	
	{
		IoObject *response  = IoObject_getSlot_(self, IOSYMBOL("responseProto"));
		response = IOCLONE(response);
		IoObject_initClone_(self, response, IoMessage_newWithName_label_(IOSTATE, IOSYMBOL("IoEvHttpServer_handleRequest"), IOSYMBOL("IoEvHttpServer")), response);
		assert(ISEVOUTRESPONSE(response));
		IoEvOutResponse_rawSetRequest_(response, req);
		IoObject_setSlot_to_(self, IOSYMBOL("response"), response);
	}
	
	{
		IoMessage *m = IoMessage_newWithName_label_(IOSTATE, IOSYMBOL("handleRequestCallback"), IOSYMBOL("IoEvHttpServer"));
		IoMessage_locals_performOn_(m, self, self);
	}
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
