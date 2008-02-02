/*
EventManager ioDoc(
//metadoc copyright Steve Dekorte 2002
*/
//metadoc license BSD revised
	/*metadoc description
A binding for libevent.")
//metadoc category Networking")
*/

#include "IoEventManager.h"
#include "IoNumber.h"
#include "IoList.h"
#include "Common.h"
#include <signal.h>
#include <math.h>
#if defined(__APPLE__) && defined(__MACH__)
#include <AvailabilityMacros.h>
#endif

#include <time.h>
#ifndef WIN32
#include <unistd.h>
#endif

#define DATA(self) ((IoEventManagerData *)IoObject_dataPointer(self))

IoTag *IoEventManager_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("EventManager");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoEventManager_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoEventManager_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoEventManager_mark);
	return tag;
}

#include <stdlib.h>

void IoEventManager_BrokenPipeSignalHandler(int v)
{
	printf("IoEventManager catching broken pipe signal %i\n", v);
}

IoEventManager *IoEventManager_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoEventManager_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoEventManagerData)));

	DATA(self)->handleEventMessage = IoMessage_newWithName_(state, IOSYMBOL("handleEvent"));
	DATA(self)->activeEvents = List_new();

	IoState_registerProtoWithFunc_((IoState *)state, self, IoEventManager_proto);

	{
		IoMethodTable methodTable[] = {
		{"addEvent", IoEventManager_addEvent},
		{"removeEvent", IoEventManager_removeEvent},

		{"listen", IoEventManager_listen},
		{"listenUntilEvent", IoEventManager_listenUntilEvent},
		{"setListenTimeout", IoEventManager_setListenTimeout},

		{"hasActiveEvents", IoEventManager_hasActiveEvents},
		{"activeEvents", IoEventManager_activeEvents},

		{NULL, NULL},
		};

		IoObject_addMethodTable_(self, methodTable);
	}


//#if !defined(AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER)
#if defined(__APPLE__)
	setenv("EVENT_NOKQUEUE", "1", 1);
	//printf("EventManager warning: disabling libevent kqueue support to avoid bug in OSX < 10.4\n");
	setenv("EVENT_NOPOLL", "1", 1);
	//setenv("EVENT_NOSELECT", "1", 1);
#endif

	DATA(self)->eventBase = event_init();

	//IoEventManager_setDescriptorLimitToMax(self);
	//Socket_SetDescriptorLimitToMax();

	return self;
}

// -----------------------------------------------------------

IoEventManager *IoEventManager_rawClone(IoEventManager *self)
{
	return self;
}

void IoEventManager_mark(IoEventManager *self)
{
	IoObject_shouldMark(DATA(self)->handleEventMessage);
	List_do_(DATA(self)->activeEvents, (ListDoCallback *)IoObject_shouldMark);

	// add code to walk event list and mark context values
}

void IoEventManager_free(IoEventManager *self)
{
	// we don't free libevent since it's global and there
	// may be other stuff (possibly other IoStates) in the process
	// using it
	List_free(DATA(self)->activeEvents);

	free(IoObject_dataPointer(self));
}


/*
IoEventManager *IoEventManager_new(void)
{
	IoEventManager *self = (IoEventManager *)calloc(1, sizeof(IoEventManager));

	DATA(self)->handleEventMessage = IoMessage_newWithName_(state, IOSYMBOL("handleEvent"));
	DATA(self)->activeEvents = List_new();
	event_init();

	//IoEventManager_setDescriptorLimitToMax(self);
	Socket_SetDescriptorLimitToMax();

	return self;
}
*/

/*
static int validEventType(int t)
{
	return (t == EV_READ || t == EV_WRITE || t == EV_TIMEOUT || t == EV_SIGNAL);
}
*/

void IoEvent_handleEvent(int fd, short eventType, void *context)
{
	IoEvent *self = (IoEvent *)context;

	struct event *ev = IoEvent_rawEvent(self);
	IoEventManager *em = IoState_protoWithInitFunction_(IOSTATE, IoEventManager_proto);
	//printf("IoEvent_handleEvent type:%i descriptor:%i\n", eventType, fd);

	List_remove_(DATA(em)->activeEvents, self);
	//printf("e: %i\n", List_size(DATA(em)->activeEvents));

	if (!ev)
	{
		printf("IoEventManager_addEvent: attempt to process an IoEvent with a 0x0 event struct - possible gc error");
		exit(1);
	}

	event_del(ev);

	/*
	if (eventType !=  && !RawDescriptor_isValid(fd))
	{
		printf("IoEvent_handleEvent: handleEvent type %i on bad file descriptor\n", eventType);
	}
	*/

	IoState_pushRetainPool(IOSTATE);
	{
	IoMessage *m = DATA(em)->handleEventMessage;

	IoMessage_setCachedArg_to_(m, 0, IOBOOL(self, eventType == EV_TIMEOUT));
	IoMessage_locals_performOn_(m, self, self);
	}
	IoState_popRetainPool(IOSTATE);
	//printf("IoEvent_handleEvent %p done\n", (void *) context);
}

static struct timeval timevalFromDouble(double seconds)
{
	double ip;
	double fraction = modf(seconds, &ip);
	struct timeval tv;

	tv.tv_sec = ip;
	tv.tv_usec = fraction * 1000000.0;

	return tv;
}

IoObject *IoEventManager_addEvent(IoEventManager *self, IoObject *locals, IoMessage *m)
{
	IoEvent *event = IoMessage_locals_eventArgAt_(m, locals, 0);
	struct event *ev = IoEvent_rawEvent(event);

	int fd = IoMessage_locals_intArgAt_(m, locals, 1);

	int eventType = IoMessage_locals_intArgAt_(m, locals, 2);

	double timeout = IoMessage_locals_doubleArgAt_(m, locals, 3);
	struct timeval tv = timevalFromDouble(timeout);

	//printf("IoEventManager_addEvent type:%i descriptor:%i\n", eventType, fd);
	//printf("fcntl(fd, F_GETFL, NULL) = %i\n", fcntl(fd, F_GETFL, NULL));

	if (eventType != 0 && !RawDescriptor_isValid(fd))
	{
		return IoState_setErrorDescription_(IOSTATE, "IoEventManager_addEvent: attempt to add bad file descriptor %i", fd);
	}

	List_append_(DATA(self)->activeEvents, IOREF(event));

	event_set(ev, fd, eventType, IoEvent_handleEvent, event);
	event_base_set(DATA(self)->eventBase, ev);

	if (timeout < 0)
	{
		event_add(ev, NULL); // no timeout
	}
	else
	{
		event_add(ev, &tv);
	}

	return self;
}

IoObject *IoEventManager_removeEvent(IoEventManager *self, IoObject *locals, IoMessage *m)
{
	IoEvent *event = IoMessage_locals_eventArgAt_(m, locals, 0);
	event_del(IoEvent_rawEvent(event));
	return self;
}

// ------------------------------------

IoObject *IoEventManager_setListenTimeout(IoEventManager *self, IoObject *locals, IoMessage *m)
{
	double timeout = IoMessage_locals_doubleArgAt_(m, locals, 0);
	struct timeval tv = timevalFromDouble(timeout);
	event_loopexit(&tv);
	return self;
}

IoObject *IoEventManager_listen(IoEventManager *self, IoObject *locals, IoMessage *m)
{
	int hadEvents = event_base_loop(DATA(self)->eventBase, EVLOOP_NONBLOCK);
	
	if (hadEvents == -1)
	{
		return IoState_setErrorDescription_(IOSTATE, "EventManager: error in event_base_loop");
	}
	
	return self;
}

IoObject *IoEventManager_listenUntilEvent(IoEventManager *self, IoObject *locals, IoMessage *m)
{
	int hadEvents = event_base_loop(DATA(self)->eventBase, EVLOOP_ONCE);

	if (hadEvents == -1)
	{
		return IoState_setErrorDescription_(IOSTATE, "EventManager: error in event_base_loop");
	}

	return self;
}

IoObject *IoEventManager_hasActiveEvents(IoEventManager *self, IoObject *locals, IoMessage *m)
{
	return IOBOOL(self, List_size(DATA(self)->activeEvents) > 0);
}

IoObject *IoEventManager_activeEvents(IoEventManager *self, IoObject *locals, IoMessage *m)
{
	return IoList_newWithList_(IOSTATE, DATA(self)->activeEvents);
}
