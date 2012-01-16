
//metadoc EventManager copyright Steve Dekorte 2002
//metadoc EventManager license BSD revised
/*metadoc EventManager description
A binding for libevent.
*/
//metadoc EventManager category Networking

#include "IoEventManager.h"
//#include "event-internal.h"
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

static const char *protoId = "EventManager";

IoTag *IoEventManager_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
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

IoEventManager *IoEventManager_proto(void *vState)
{
	IoState *state = vState;
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoEventManager_newTag(state));

	IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoEventManagerData)));

	DATA(self)->handleEventMessageTrue = IoMessage_newWithName_(state, IOSYMBOL("handleEvent"));
	IoMessage_setCachedArg_to_(DATA(self)->handleEventMessageTrue, 0, state->ioTrue);

	DATA(self)->handleEventMessageFalse = IoMessage_newWithName_(state, IOSYMBOL("handleEvent"));
	IoMessage_setCachedArg_to_(DATA(self)->handleEventMessageFalse, 0, state->ioFalse);

	DATA(self)->activeEvents = List_new();

	IoState_registerProtoWithId_((IoState *)state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"addEvent", IoEventManager_addEvent},
		{"removeEvent", IoEventManager_removeEvent},
		{"resetEventTimeout", IoEventManager_resetEventTimeout},

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

/*
#if defined(__APPLE__)
	setenv("EVENT_NOKQUEUE", "1", 1);
	//printf("EventManager warning: disabling libevent kqueue support to avoid bug in OSX < 10.4\n");
	setenv("EVENT_NOPOLL", "1", 1);
	//setenv("EVENT_NOSELECT", "1", 1);
#endif
*/

	DATA(self)->eventBase = event_init();
	#ifdef USE_EVHTTP
	DATA(self)->evh = evhttp_new(DATA(self)->eventBase);
	#endif
	//IoEventManager_setDescriptorLimitToMax(self);
	Socket_SetDescriptorLimitToMax();

	return self;
}

// -----------------------------------------------------------

IoEventManager *IoEventManager_rawClone(IoEventManager *self)
{
	return self;
}

void IoEventManager_mark(IoEventManager *self)
{
	IoObject_shouldMark(DATA(self)->handleEventMessageTrue);
	IoObject_shouldMark(DATA(self)->handleEventMessageFalse);
	//printf("IoEventManager_mark %i events\n", List_size(DATA(self)->activeEvents));
	//List_do_(DATA(self)->activeEvents, (ListDoCallback *)IoObject_shouldMark);

	{
		const List *foreachList = DATA(self)->activeEvents;
		size_t index, foreachMax = foreachList->size;
		for (index = 0; index < foreachMax; index ++)
		{
			void *value = foreachList->items[index];

			//printf("	marking event %p\n", (void *)value);
			IoObject_shouldMark(value);
		}
	}

	// add code to walk event list and mark context values
}

void IoEventManager_free(IoEventManager *self)
{
	// we don't free libevent since it's global and there
	// may be other stuff (possibly other IoStates) in the process
	// using it
	List_free(DATA(self)->activeEvents);

	//printf("IoEventManager_free - skipping evhttp_free\n");
	#ifdef USE_EVHTTP
	evhttp_free(DATA(self)->evh);
	#endif
	io_free(IoObject_dataPointer(self));
}

void *IoEventManager_rawBase(IoEventManager *self)
{
	return DATA(self)->eventBase;
}

/*
IoEventManager *IoEventManager_new(void)
{
	IoEventManager *self = (IoEventManager *)io_calloc(1, sizeof(IoEventManager));

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

int IoEventManager_rawHasActiveEvent_(IoEventManager *self, IoEvent *event)
{
	return List_contains_(DATA(self)->activeEvents, event);
}

void IoEventManager_rawRemoveEvent_(IoEventManager *self, IoEvent *event)
{
	if(!List_contains_(DATA(self)->activeEvents, event))
	{
		printf("WARNING: IoEventManager_rawRemoveEvent_: event not in active list\n");
	}
	//printf("NOT remove event %p\n", (void *)event);
	event_del(IoEvent_rawEvent(event));
	List_remove_(DATA(self)->activeEvents, event);
}

void IoEventManager_rawAddEvent_(IoEventManager *self, IoEvent *event)
{
	if(List_contains_(DATA(self)->activeEvents, event))
	{
		printf("ERROR: IoEventManager_addEvent: attempt to add same event twice\n");
		exit(-1);
	}

	//printf("add event %p\n", (void *)event);
	List_append_(DATA(self)->activeEvents, IOREF(event));
}

IoObject *IoEventManager_removeEvent(IoEventManager *self, IoObject *locals, IoMessage *m)
{
	IoEvent *event = IoMessage_locals_eventArgAt_(m, locals, 0);
	IoEventManager_rawRemoveEvent_(self, event);
	return self;
}

void IoEvent_handleEvent(int fd, short eventType, void *context)
{
	IoEvent *self = (IoEvent *)context;
	struct event *ev = IoEvent_rawEvent(self);
	IoEventManager *em = IoState_protoWithId_(IOSTATE, protoId);
	//printf("IoEvent_handleEvent type:%i descriptor:%i\n", eventType, fd);
	//printf("e: %i\n", List_size(DATA(em)->activeEvents));

	if(!IoEventManager_rawHasActiveEvent_(em, self))
	{
		printf("ERROR: got IoEvent_handleEvent for Event not in EventManager active list\n");
		exit(-1);
	}
	
	if (!ev)
	{
		printf("IoEventManager_addEvent: attempt to process an IoEvent with a 0x0 event struct - possible gc error");
		exit(1);
	}
	/*
	if (eventType !=  && !RawDescriptor_isValid(fd))
	{
		printf("IoEvent_handleEvent: handleEvent type %i on bad file descriptor\n", eventType);
	}
	*/

	IoState_pushRetainPool(IOSTATE);
	
	if(eventType == EV_TIMEOUT)
	{
		IoMessage *m = DATA(em)->handleEventMessageTrue;
		IoMessage_locals_performOn_(m, self, self);
	}
	else
	{
		IoMessage *m = DATA(em)->handleEventMessageFalse;
		IoMessage_locals_performOn_(m, self, self);
	}
	IoState_popRetainPool(IOSTATE);
	IoEventManager_rawRemoveEvent_(em, self);
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
		return IoError_newWithMessageFormat_(IOSTATE, "IoEventManager_addEvent: attempt to add bad file descriptor %i", fd);
	}

	IoEventManager_rawAddEvent_(self, event);

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

IoObject *IoEventManager_resetEventTimeout(IoEventManager *self, IoObject *locals, IoMessage *m)
{
	IoEvent *event = IoMessage_locals_eventArgAt_(m, locals, 0);
	struct event *ev = IoEvent_rawEvent(event);
	double timeout = IoMessage_locals_doubleArgAt_(m, locals, 1);
	struct timeval tv = timevalFromDouble(timeout);
	
	event_add(ev, &tv);

	return self;
}

// ------------------------------------

IoObject *IoEventManager_setListenTimeout(IoEventManager *self, IoObject *locals, IoMessage *m)
{
	double timeout = IoMessage_locals_doubleArgAt_(m, locals, 0);
	DATA(self)->listenTimeout = timevalFromDouble(timeout);
	return self;
}

IoObject *IoEventManager_listen(IoEventManager *self, IoObject *locals, IoMessage *m)
{
	int hadEvents;
	
	event_loopexit(&(DATA(self)->listenTimeout));
	hadEvents = event_base_loop(DATA(self)->eventBase, EVLOOP_NONBLOCK);
	
	//printf("IoEventManager_listen %p\n", (void *)self);
	if (hadEvents == -1)
	{
		return IoError_newWithMessageFormat_(IOSTATE, "EventManager: error in event_base_loop");
	}
	
	return self;
}

IoObject *IoEventManager_listenUntilEvent(IoEventManager *self, IoObject *locals, IoMessage *m)
{
	int hadEvents = event_base_loop(DATA(self)->eventBase, EVLOOP_ONCE);

	//printf("IoEventManager_listenUntilEvent\n");
	if (hadEvents == -1)
	{
		return IoError_newWithMessageFormat_(IOSTATE, "EventManager: error in event_base_loop");
	}

	return self;
}

// this may need to be updated on future releases of libevent

struct event_base_PROTO 
{
	void *evsel;
	void *evbase;
	int event_count;		/* counts number of total events */
	int event_count_active;	/* counts number of active events */
};

IoObject *IoEventManager_hasActiveEvents(IoEventManager *self, IoObject *locals, IoMessage *m)
{
	int count = ((struct event_base_PROTO *)DATA(self)->eventBase)->event_count;
	//int countActive = ((struct event_base_PROTO *)DATA(self)->eventBase)->event_count;
	//printf("count: %i countActive: %i\n", count, countActive);
	//return IOBOOL(self, count > 1);
	return IOBOOL(self, count);
}

IoObject *IoEventManager_activeEvents(IoEventManager *self, IoObject *locals, IoMessage *m)
{
	return IoList_newWithList_(IOSTATE, DATA(self)->activeEvents);
}

