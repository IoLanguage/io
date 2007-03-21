/*#io
docCopyright("Trevor Fancher", 2006)
docLicense("BSD revised")
*/


#ifndef THREAD_DEFINED
#define THREAD_DEFINED 1

#include "Common.h"
#ifndef WIN32
# include <pthread.h>
# include <unistd.h>
#endif
#include "ThreadMutex.h"
#include "List.h"

typedef void *(*ThreadFunc)(void*);

enum ThreadReturnCode_
{
	THREAD_SUCCESS = 0,
	THREAD_FAILURE
};
typedef enum ThreadReturnCode_ ThreadReturnCode;

typedef struct Thread_ Thread;
struct Thread_
{
#ifdef WIN32
	HANDLE thread;
	DWORD id;
#else
	pthread_t thread;
#endif
	int active;
	ThreadFunc func;
	void *funcArg;
	void *returnValue;
	void *userData;
};

// --- class functions
ThreadReturnCode 	Thread_Init(void);
void             	Thread_Shutdown(void);
Thread *         	Thread_CurrentThread(void);
ThreadReturnCode 	Thread_WaitOnThread_(Thread *other);
List *           	Thread_Threads(void);

// --- constructors
Thread * 	Thread_new(void);
Thread * 	Thread_newWithFunc_arg_(ThreadFunc func, void *funcArg);

// --- destructor
void 	Thread_destroy(Thread* self);

// --- member functions
// -- getter functions
ThreadFunc 	Thread_func(Thread *self);
void *     	Thread_funcArg(Thread *self);
void *     	Thread_userData(Thread *self);

// -- setter functions
void Thread_setFunc_(Thread *self, ThreadFunc func);
void Thread_setFuncArg_(Thread *self, void *funcArg);
void Thread_setFunc_arg_(Thread *self, ThreadFunc, void *funcArg);
void Thread_setUserData_(Thread *self, void *userData);

// -- operational functions
ThreadReturnCode 	Thread_start(Thread *self);
ThreadReturnCode 	Thread_stop(Thread *self);
void             	Thread_exitWithValue_(Thread *self, void *returnValue);

#endif
