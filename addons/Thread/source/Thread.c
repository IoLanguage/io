/*
copyright
	Trevor Fancher, 2006
license
	BSD revised
*/


#include "Thread.h"

#ifdef WIN32
# include <windows.h>
#else
# include <pthread.h>
# include <unistd.h>
#endif
#include <stdlib.h>
#include "List.h"

static List *threads;
static ThreadMutex *threads_mutex;

static Thread *mainThread;

#define LOCK_THEN_UNLOCK(var, code) do { \
	ThreadMutex_lock(var ## _mutex); \
	code; \
	ThreadMutex_unlock(var ## _mutex); \
	} while(0);

ThreadReturnCode Thread_Init(void)
{
#ifdef WIN32
	HANDLE rawMainThread = GetCurrentThread();
#else
	pthread_t rawMainThread = pthread_self();
#endif

	if (threads)
	{
		return THREAD_SUCCESS; // already initialized
	}

	threads = List_new();
	threads_mutex = ThreadMutex_new();

	mainThread = Thread_new();
	if (!mainThread)
	{
		return THREAD_FAILURE;
	}

	mainThread->thread = rawMainThread;
#ifdef WIN32
	mainThread->id = GetCurrentThreadId();
#endif

	return THREAD_SUCCESS;
}

void Thread_Shutdown(void)
{
	// need to shutdown threads
	//if (List_size(threads) > 0)

	if (mainThread)
	{
		LOCK_THEN_UNLOCK(threads,
			List_remove_(threads, (void *)mainThread);
		)
		Thread_destroy(mainThread);
	}
	else
	{
		return;
	}

	LOCK_THEN_UNLOCK(threads,
		List_free(threads);
	)

	ThreadMutex_destroy(threads_mutex);

	mainThread = NULL;
	threads = NULL;
	threads_mutex = NULL;
}


Thread *Thread_CurrentThread(void)
{
#ifdef WIN32
	HANDLE rawCurrentThread = GetCurrentThread();
#else
	pthread_t rawCurrentThread = pthread_self();
#endif
	int isSameThread;

	Thread *currentThread = NULL;
	#ifdef WIN32
	LOCK_THEN_UNLOCK(threads,
		LIST_FOREACH(threads, index, thread,
			isSameThread = (((Thread *)thread)->thread == rawCurrentThread);
			if (isSameThread)
			{
				currentThread = thread;
				break;
			}
		);
	);
	#else
	LOCK_THEN_UNLOCK(threads,
		LIST_FOREACH(threads, index, thread,
			isSameThread = pthread_equal(((Thread *)thread)->thread, rawCurrentThread);
			if (isSameThread)
			{
				currentThread = thread;
				break;
			}
		);
	);
	#endif

	if (!currentThread)
	{
		fflush(stdout);
		fflush(stderr);
		fprintf(stderr, "\nYou found a bug in libThread.  Please tell trevor on freenode or email trevor@fancher.org.\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	return currentThread;
}

ThreadReturnCode Thread_WaitOnThread_(Thread *thread)
{
	int err;
#ifdef WIN32
	WaitForSingleObject(thread->thread, INFINITE);
#else
	err = pthread_join(thread->thread, NULL);
	if (err)
	{
		return THREAD_FAILURE;
	}
#endif
	return THREAD_SUCCESS;
}

List *Thread_Threads(void)
{
	return List_clone(threads);
}

Thread *Thread_new(void)
{
	Thread *self = malloc(sizeof(Thread));
	if (self)
	{
		self->func = NULL;
		self->funcArg = NULL;
		self->active = 0;
		LOCK_THEN_UNLOCK(threads,
			List_append_(threads, (void *)self);
		);
	}
	return self;
}

Thread *Thread_newWithFunc_arg_(ThreadFunc func, void *funcArg)
{
	Thread *self = Thread_new();
	if (self)
	{
		Thread_setFunc_arg_(self, func, funcArg);
	}
	return self;
}

void Thread_destroy(Thread *self)
{
	LOCK_THEN_UNLOCK(threads,
		List_remove_(threads, (void *)self);
	);
	free(self);
}

void Thread_setFunc_(Thread *self, ThreadFunc func)
{
	self->func = func;
}

void Thread_setFuncArg_(Thread *self, void *funcArg)
{
	self->funcArg = funcArg;
}

void Thread_setFunc_arg_(Thread *self, ThreadFunc func, void *funcArg)
{
	Thread_setFunc_(self, func);
	Thread_setFuncArg_(self, funcArg);
}

ThreadReturnCode Thread_start(Thread *self)
{
#ifdef WIN32
	self->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(self->func), self->funcArg, 0, &(self->id));
#else
	int err;
	err = pthread_create(&(self->thread), NULL, self->func, self->funcArg);
	if (err)
	{
		return THREAD_FAILURE;
	}
#endif

	self->active = 1;
	return THREAD_SUCCESS;
}

ThreadReturnCode Thread_stop(Thread *self)
{
#ifdef WIN32
	TerminateThread(self->thread, 0);
#else
	pthread_cancel(self->thread);
#endif

	self->active = 0;
	return THREAD_SUCCESS;
}

void Thread_exitWithValue_(Thread *self, void *returnValue)
{
	self->returnValue = returnValue;
#ifdef WIN32
	ExitThread(0);
#else
	pthread_exit(NULL);
#endif
}

ThreadFunc Thread_func(Thread *self)
{
	return self->func;
}

void *Thread_funcArg(Thread *self)
{
	return self->funcArg;
}

void *Thread_userData(Thread *self)
{
	return self->userData;
}

void Thread_setUserData_(Thread *self, void *userData)
{
	self->userData = userData;
}
