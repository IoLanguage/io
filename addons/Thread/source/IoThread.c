//metadoc Thread copyright Steve Dekorte, 2006
//metadoc Thread license BSD revised
//metadoc Thread category Concurrency
/*metadoc Thread description
	For native threads.
Example use;
<pre>	
Thread createThread("1+1") // evals 1+1 in a new thread and an independent Io VM
</pre>
*/

#include "IoThread.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include <errno.h>
#include "Thread.h"
#include "List.h"

static const char *protoId = "Thread";

IoTag *IoThread_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoThread_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoThread_rawClone);
	return tag;
}

IoThread *IoThread_proto(void *state)
{
	IoThread *self = IoObject_new(state);
	IoObject_tag_(self, IoThread_newTag(state));

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"threadCount", IoThread_threadCount},
		{"createThread", IoThread_createThread},
		//{"endCurrentThread", IoThread_endCurrentThread},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	Thread_Init();

	return self;
}

IoThread *IoThread_rawClone(IoThread *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	return self;
}

IoThread *IoThread_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoThread_free(IoThread *self)
{
}

// --------------------------------------------

typedef struct
{
	IoState *state;
	Thread *thread;
	char *evalString;
} IoThreadInfo;

IoThreadInfo *IoThreadInfo_new(void)
{
	IoThreadInfo *self = (IoThreadInfo *)io_calloc(1, sizeof(IoThreadInfo));
	return self;
}

void IoThreadInfo_free(IoThreadInfo *self)
{
	if(self->evalString) io_free(self->evalString);
	io_free(self);
}

void IoThreadInfo_setState_(IoThreadInfo *self, IoState *state)
{
	self->state = state;
}

IoState *IoThreadInfo_state(IoThreadInfo *self)
{
	return self->state;
}

void IoThreadInfo_setThread_(IoThreadInfo *self, Thread *thread)
{
	self->thread = thread;
}

Thread *IoThreadInfo_thread(IoThreadInfo *self)
{
	return self->thread;
}

void IoThreadInfo_setEvalString_(IoThreadInfo *self, char *s)
{
	self->evalString = strcpy(io_malloc(strlen(s) + 1), s);
}

char *IoThreadInfo_evalString(IoThreadInfo *self)
{
	return self->evalString;
}

// ----------------------------------------------------

void *IoThread_BeginThread(void *vti)
{
	IoThreadInfo *ti = (IoThreadInfo *)vti;
	Thread *t = IoThreadInfo_thread(ti);
	IoState *state = IoThreadInfo_state(ti);

	Thread_setUserData_(t, state);
	IoState_doCString_(state, IoThreadInfo_evalString(ti));
	IoThreadInfo_free(ti);
	IoState_free(state);
	Thread_destroy(t);
	return NULL;
}

IoObject *IoThread_createThread(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Thread createThread(aSequence)
	Creates a new IoState and evals aSequence in it using a new OS thread. Returns self immediately.
	*/
	
	
	IoSeq *s = IoMessage_locals_seqArgAt_(m, locals, 0);
	IoState *newState = IoState_new();
	Thread *t;

	Thread_Init();

	t = Thread_new();

	IoThreadInfo *ti = IoThreadInfo_new();
	IoThreadInfo_setState_(ti, newState);
	IoThreadInfo_setThread_(ti, t);
	IoThreadInfo_setEvalString_(ti, CSTRING(s));

	Thread_setFunc_(t, IoThread_BeginThread);
	Thread_setFuncArg_(t, (void *)ti);
	Thread_start(t);

	return self;
}

IoObject *IoThread_threadCount(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Thread threadCount
	Returns the number of OS threads currently running in the process.
	*/
	
	Thread_Init();
	List *threads;
	size_t count;

	threads = Thread_Threads();
	count = List_size(threads);
	List_free(threads);

	return IONUMBER(count);
}

IoObject *IoThread_endCurrentThread(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Thread endCurrentThread
	Ends the currently running OS thread.
	*/
	
	// shutdown vm?
	Thread_destroy(Thread_CurrentThread());
	return self;
}
