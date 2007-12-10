/*#io
docCopyright("Trevor Fancher", 2006)
docLicense("BSD revised")
*/


#ifndef THREADMUTEX_DEFINED
#define THREADMUTEX_DEFINED 1

#include "Common.h"
#ifndef WIN32
# include <pthread.h>
#endif

enum ThreadMutexReturnCode_
{
	THREADMUTEX_SUCCESS = 0,
	THREADMUTEX_FAILURE
};
typedef enum ThreadMutexReturnCode_ ThreadMutexReturnCode;

typedef struct ThreadMutex_ ThreadMutex;
struct ThreadMutex_
{
#ifdef WIN32
	HANDLE mutex;
#else
	pthread_mutex_t mutex;
#endif
	int isLocked;
};

// --- constructor
ThreadMutex * 	ThreadMutex_new(void);

// --- destructor
void 	ThreadMutex_destroy(ThreadMutex* self);

// --- member functions
// -- operational functions
ThreadMutexReturnCode 	ThreadMutex_lock(ThreadMutex *self);
ThreadMutexReturnCode 	ThreadMutex_unlock(ThreadMutex *self);

#endif
