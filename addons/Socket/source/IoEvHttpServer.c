//metadoc EvRequest copyright Steve Dekorte, 2004
//metadoc EvRequest license BSD revised
//metadoc EvRequest description Networking Event.
//metadoc EvRequest category Networking

#include "IoEvHttpServer.h"
#include "IoEventManager.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
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

void IoEvHttpServer_handleRequest(struct evhttp_request *req, void *arg)
{
	IoEvHttpServer *self = arg;

	const char *uri = evhttp_request_uri(req);
	struct evbuffer *buf = 0x0;
	IoObject *request  = IoObject_getSlot_(self, IOSYMBOL("request"));

	IoSeq *postData = IOSEQ((const unsigned char *)EVBUFFER_DATA(req->input_buffer), (int)EVBUFFER_LENGTH(req->input_buffer));
	
	IoObject_setSlot_to_(request, IOSYMBOL("uri"), IOSYMBOL(uri));
	IoObject_setSlot_to_(request, IOSYMBOL("postData"), postData);

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

		IoSeq *data = IoObject_seqGetSlot_(response, IOSYMBOL("data"));
		int statusCode = IoObject_doubleGetSlot_(response, IOSYMBOL("statusCode"));

		buf = evbuffer_new();
		evbuffer_add_printf(buf, "%s", CSTRING(data));

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
