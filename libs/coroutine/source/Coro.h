/*   
*/

#ifndef CORO_DEFINED
#define CORO_DEFINED 1

#include "Common.h"
#include "PortableUContext.h"

#if defined(__SYMBIAN32__)
	#define CORO_STACK_SIZE     8192
	#define CORO_STACK_SIZE_MIN 1024
#else
     //#define CORO_DEFAULT_STACK_SIZE     (65536/2)
     #define CORO_DEFAULT_STACK_SIZE  (65536*4)
	//128k needed on PPC due to parser
	#define CORO_STACK_SIZE_MIN 8192
#endif

#if !defined(__MINGW32__) && defined(WIN32)
#if defined(BUILDING_CORO_DLL) || defined(BUILDING_IOVMALL_DLL)
#define CORO_API __declspec(dllexport)
#else
#define CORO_API __declspec(dllimport)
#endif

#else
#define CORO_API
#endif

/*
#if defined(__amd64__) && !defined(__x86_64__)
	#define __x86_64__ 1
#endif
*/

#if defined(WIN32) && defined(HAS_FIBERS)
	#define CORO_IMPLEMENTATION "fibers"
#elif defined(HAS_UCONTEXT) && !defined(__x86_64__)
	#include <ucontext.h>
	#define CORO_IMPLEMENTATION "ucontext"
#else
	#include <setjmp.h>
	#define CORO_IMPLEMENTATION "setjmp"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Coro Coro;

struct Coro
{        
	size_t stackSize;
	void *stack;

#ifdef USE_VALGRIND
	unsigned int valgrindStackId;
#endif

#if defined(HAS_FIBERS)
    void *fiber;
#else
	#if defined(HAS_UCONTEXT) && !defined(__x86_64__)
	    ucontext_t env;
	#else
	    jmp_buf env;
	#endif
#endif

	unsigned char isMain;
};

CORO_API Coro *Coro_new(void);
CORO_API void Coro_free(Coro *self);

// stack

CORO_API void *Coro_stack(Coro *self);
CORO_API size_t Coro_stackSize(Coro *self);
CORO_API void Coro_setStackSize_(Coro *self, size_t sizeInBytes);
CORO_API size_t Coro_bytesLeftOnStack(Coro *self);
CORO_API int Coro_stackSpaceAlmostGone(Coro *self);

CORO_API void Coro_initializeMainCoro(Coro *self);

typedef void (CoroStartCallback)(void *);

CORO_API void Coro_startCoro_(Coro *self, Coro *other, void *context, CoroStartCallback *callback);
CORO_API void Coro_switchTo_(Coro *self, Coro *next);
CORO_API void Coro_setup(Coro *self, void *arg); // private

#ifdef __cplusplus
}
#endif
#endif
