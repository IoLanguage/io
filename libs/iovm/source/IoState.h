/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IOSTATE_DEFINED
#define IOSTATE_DEFINED 1


#include "IoVMApi.h"

//#include "Collector.h"
#include "Stack.h"
#include "PHash.h"
#include "SHash.h"
#include "MainArgs.h"
#include "IoObject_struct.h"
#define COLLECTOROBJECTTYPE IoObjectData

#include "Collector.h"

#include "IoSeq.h"
#include "IoVersion.h"
#include "IoCoroutine.h"

#define IOMESSAGE_INLINE_PERFORM 1
//#define IO_BLOCK_LOCALS_RECYCLING 1
#define IOSTATE_DEFAULT_MAX_RECYCLED_OBJECTS 1000

#ifdef __cplusplus
extern "C" {
#endif

typedef struct IoState IoState;

#include "IoState_callbacks.h"

typedef IoObject *(IoStateProtoFunc)(void *);

struct IoState
{
	PHash *primitives;
	SHash *symbols;

	// coroutines

	IoObject *objectProto;
	IoObject *mainCoroutine;    // the object that represents the main "thread"
	IoObject *currentCoroutine; // the object whose coroutine is active
	Stack *currentIoStack;      // quick access to current coro's retain stack

	// quick access objects

	IoSymbol *activateSymbol;
	IoSymbol *forwardSymbol;
	IoSymbol *selfSymbol;
	IoSymbol *setSlotSymbol;
	IoSymbol *setSlotWithTypeSymbol;
	IoSymbol *callSymbol;
	IoSymbol *updateSlotSymbol;
	IoSymbol *typeSymbol;
	IoSymbol *opShuffleSymbol;
	IoSymbol *noShufflingSymbol;
	IoSymbol *semicolonSymbol;
	IoSymbol *stackSizeSymbol;

	IoObject *setSlotBlock;
	IoObject *localsUpdateSlotCFunc;
	IoObject *localsProto;

	IoMessage *asStringMessage;
	IoMessage *collectedLinkMessage;
	IoMessage *compareMessage;
	//IoMessage *doStringMessage;
	IoMessage *willFreeMessage;
	IoMessage *initMessage;
	IoMessage *mainMessage;
	IoMessage *nilMessage;
	IoMessage *runMessage;
	IoMessage *printMessage;
	IoMessage *opShuffleMessage;
	IoMessage *yieldMessage;

	List *cachedNumbers;

	// singletons

	IoObject *ioNil;
	IoObject *ioTrue;
	IoObject *ioFalse;

	// Flow control singletons

	IoObject *ioNormal;
	IoObject *ioBreak;
	IoObject *ioContinue;
	IoObject *ioReturn;
	IoObject *ioEol;

	// garbage collection

	Collector *collector;
	IoObject *lobby;
	IoObject *core;

	// recycling

	List *recycledObjects;
	size_t maxRecycledObjects;

	// startup environment

	MainArgs *mainArgs;

	// current execution state

	int stopStatus;
	void *returnValue;

	// embedding

	void *callbackContext;
	IoStateBindingsInitCallback *bindingsInitCallback;
	IoStatePrintCallback        *printCallback;
	IoStateExceptionCallback    *exceptionCallback;
	IoStateExitCallback         *exitCallback;
	IoStateActiveCoroCallback   *activeCoroCallback;

	// debugger

	int debugOn;
	IoObject *debugger;
	IoMessage *vmWillSendMessage;

	// SandBox limits

	int messageCountLimit;
	int messageCount;
	double timeLimit;
	double endTime;

	// tail calls

	IoMessage *tailCallMessage;

	// exiting

	int shouldExit;
	int exitResult;
};

#define IOSTATE_STRUCT_DEFINED

// setup

IOVM_API IoState *IoState_new(void);
IOVM_API void IoState_new_atAddress(void* address);
IOVM_API void IoState_init(IoState *self);

void IoState_setupQuickAccessSymbols(IoState *self);
void IoState_setupCachedMessages(IoState *self);
void IoState_setupSingletons(IoState *self);

// setup tags

IOVM_API void IoState_registerProtoWithFunc_(IoState *self, IoObject *proto, IoStateProtoFunc *func);
IOVM_API IoObject *IoState_protoWithInitFunction_(IoState *self, IoStateProtoFunc *func);
IOVM_API IoObject *IoState_protoWithName_(IoState *self, const char *name);

IOVM_API void IoState_done(IoState *self);
IOVM_API void IoState_free(IoState *self);

// lobby

IOVM_API IoObject *IoState_lobby(IoState *self);
IOVM_API void IoState_setLobby_(IoState *self, IoObject *obj);

// command line

IOVM_API void IoState_argc_argv_(IoState *self, int argc, const char *argv[]);
IOVM_API void IoState_runCLI(IoState *self);

// store

IOVM_API IoObject *IoState_objectWithPid_(IoState *self, PID_TYPE pid);
IOVM_API int IoState_exitResult(IoState *self);

#include "IoState_coros.h"
#include "IoState_debug.h"
#include "IoState_eval.h"
#include "IoState_symbols.h"
#include "IoState_exceptions.h"
#include "IoState_inline.h"

#ifdef __cplusplus
}
#endif
#endif
