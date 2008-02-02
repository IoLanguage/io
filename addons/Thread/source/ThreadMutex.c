/*
copyright
	Trevor Fancher, 2006
license
	BSD revised
*/

#include "ThreadMutex.h"

#ifndef WIN32
# include <pthread.h>
#endif
#include <stdio.h>
#include <stdlib.h>

ThreadMutex *ThreadMutex_new(void)
{
	ThreadMutex *self;
	self = malloc(sizeof(ThreadMutex));
	if (self)
	{
	#ifdef WIN32
		self->mutex = CreateMutex(NULL, FALSE, NULL);
	#else
		pthread_mutex_init(&(self->mutex), NULL);
	#endif
		self->isLocked = 0;
	}
	return self;
}

void ThreadMutex_destroy(ThreadMutex* self)
{
	if (self->isLocked)
		return;

#ifdef WIN32
	CloseHandle(self->mutex);
#else
	pthread_mutex_destroy(&(self->mutex));
#endif

	free(self);
}

ThreadMutexReturnCode ThreadMutex_lock(ThreadMutex *self)
{
#ifdef WIN32
	WaitForSingleObject(self->mutex, INFINITE);
#else
	int err;
	err = pthread_mutex_lock(&(self->mutex));
	if (err)
	{
		return THREADMUTEX_FAILURE;
	}
#endif

	self->isLocked = 1;
	return THREADMUTEX_SUCCESS;
}

ThreadMutexReturnCode ThreadMutex_unlock(ThreadMutex *self)
{
#ifdef WIN32
	ReleaseMutex(self->mutex);
#else
	int err;
	err = pthread_mutex_unlock(&(self->mutex));
	if (err)
	{
		return THREADMUTEX_FAILURE;
	}
#endif
	self->isLocked = 0;
	return THREADMUTEX_SUCCESS;
}
