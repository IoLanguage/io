
//--metadoc State copyright Steve Dekorte 2002
//--metadoc State license BSD revised

/*cmetadoc State description
VM lifecycle and proto registry. IoState_new_atAddress builds the whole
object graph in a deliberate order — Object, Coroutine and Sequence have
to exist before anything else because creating objects needs a retain
stack (which lives on the main coroutine) and defining methods needs
Symbols and CFunctions. After the three bootstrap protos are in place,
each remaining core proto registers itself via IoState_registerProtoWithId_
into the primitives PointerHash so IoState_protoWithId_ can look them up.
The function also installs singletons (nil/true/false/flow-control
markers), cached messages, the evaluation frame pool fields, and finally
runs IoVMCodeInit (the generated IoVMInit.c) to load the Io standard
library. IoState_markLazyArgsCFunctions_ is the last step: it stamps
isLazyArgs on every CFunction whose arguments the iterative evaluator
must leave un-pre-evaluated (if/while/for/callcc/method/block/do/foreach/
…). Tear-down is IoState_done + IoState_free.
*/

#define IOSTATE_C 1
#include "IoState.h"
#undef IOSTATE_C

#include "IoObject.h"
#include "IoCall.h"
#include "IoCoroutine.h"
#ifdef IO_CALLCC
#include "IoContinuation.h"
#endif
#include "IoEvalFrame.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoCFunction.h"
#include "IoBlock.h"
#include "IoList.h"
#include "IoMap.h"
//#include "IoRange.h"
#include "IoFile.h"
#include "IoDate.h"
#include "IoDuration.h"
#include "IoSeq.h"
#include "IoMessage_parser.h"
#include "IoWeakLink.h"
#include "IoError.h"

#include "IoSystem.h"
#include "IoCompiler.h"
#include "IoDebugger.h"
#include "IoCollector.h"
#include "IoSandbox.h"
#include "IoDirectory.h"
#include "IoProfiler.h"
//#include "IoEditLine.h"

#include <stdlib.h>

void IoVMCodeInit(IoObject *context);

// Mark CFunction objects whose arguments must not be pre-evaluated.
// Called once during init after all protos are registered.
// Aliases (e.g., false.elseif := Object getSlot("if")) automatically
// inherit the flag since they reference the same CFunction object.
/*cdoc State IoState_markSlotLazyArgs_(self, protoId, slotName)
Looks up a named slot on a proto and, if it resolves to a CFunction,
sets its isLazyArgs flag so the iterative evaluator will skip the
EVAL_ARGS pass and hand the raw message arguments to the callee. Used
to tag the special forms that need their arguments as un-evaluated
messages (if, while, for, method, block, foreach, …).
*/
static void IoState_markSlotLazyArgs_(IoState *self, const char *protoId,
                                      const char *slotName) {
    IoObject *proto = IoState_protoWithId_(self, protoId);
    if (!proto) return;
    IoObject *f = IoObject_rawGetSlot_(proto, SIOSYMBOL(slotName));
    if (f && ISCFUNCTION(f)) {
        ((IoCFunctionData *)IoObject_dataPointer(f))->isLazyArgs = 1;
    }
}

/*cdoc State IoState_markLazyArgsCFunctions_(self)
The canonical list of lazy-args CFunctions. Called once at the very end
of IoState_new_atAddress after all protos and Io-side slot aliases are
in place. Keep this list in sync with the special-form detection in
IoState_iterative.c — adding a new control-flow primitive that takes
un-evaluated message arguments means touching both places.
*/
static void IoState_markLazyArgsCFunctions_(IoState *self) {
    // Control flow
    IoState_markSlotLazyArgs_(self, "Object", "if");
    IoState_markSlotLazyArgs_(self, "Object", "while");
    IoState_markSlotLazyArgs_(self, "Object", "for");
    IoState_markSlotLazyArgs_(self, "Object", "loop");
#ifdef IO_CALLCC
    IoState_markSlotLazyArgs_(self, "Object", "callcc");
#endif
    // Block/method construction
    IoState_markSlotLazyArgs_(self, "Object", "method");
    IoState_markSlotLazyArgs_(self, "Object", "block");
    // Evaluation (body is lazy)
    IoState_markSlotLazyArgs_(self, "Object", "do");
    IoState_markSlotLazyArgs_(self, "Object", "lexicalDo");
    IoState_markSlotLazyArgs_(self, "Object", "message");
    IoState_markSlotLazyArgs_(self, "Object", "foreachSlot");
    // List
    IoState_markSlotLazyArgs_(self, "List", "foreach");
    IoState_markSlotLazyArgs_(self, "List", "reverseForeach");
    IoState_markSlotLazyArgs_(self, "List", "sortInPlace");
    // Number
    IoState_markSlotLazyArgs_(self, "Number", "repeat");
    // Date
    IoState_markSlotLazyArgs_(self, "Date", "cpuSecondsToRun");
    // Sequence
    IoState_markSlotLazyArgs_(self, "Sequence", "foreach");
    // Map
    IoState_markSlotLazyArgs_(self, "Map", "foreach");
    // File
    IoState_markSlotLazyArgs_(self, "File", "foreach");
    IoState_markSlotLazyArgs_(self, "File", "foreachLine");
}

/*cdoc State IoState_new_atAddress(address)
In-place VM constructor: initializes a caller-supplied IoState struct
rather than allocating one, so embedders can place the VM in a specific
arena. Follows a carefully-ordered bootstrap (see cmetadoc) — the
temporary Stack *currentIoStack exists only until the main coroutine
is constructed and its real retain stack takes over. Pauses the
collector across all proto allocation so partially-built objects are
never swept. Must be called exactly once per state; IoState_new wraps
it with a malloc.
*/
void IoState_new_atAddress(void *address) {
    IoState *self = (IoState *)address;
    IoCFunction *cFunctionProto;
    IoSeq *seqProto;

    self->randomGen = RandomGen_new();
    RandomGen_setSeed(self->randomGen,
                      754905204); // no particular reason for this seed
    // collector

    self->collector = Collector_new();
    IoState_pushCollectorPause(self);

    Collector_setMarkFunc_(self->collector, (CollectorMarkFunc *)IoObject_mark);
    Collector_setWillFreeFunc_(self->collector,
                               (CollectorWillFreeFunc *)IoObject_willFree);
    Collector_setFreeFunc_(self->collector, (CollectorFreeFunc *)IoObject_free);

    self->mainArgs = MainArgs_new();
    self->primitives = PointerHash_new();

    self->recycledObjects = List_new();
    self->maxRecycledObjects = IOSTATE_DEFAULT_MAX_RECYCLED_OBJECTS;

    // Sandbox

    self->messageCount = 0;
    self->messageCountLimit = 0;
    self->endTime = 0;

    // symbol table

    self->symbols = CHash_new();

    CHash_setEqualFunc_(self->symbols, (CHashEqualFunc *)UArray_equals_);
    CHash_setHash1Func_(self->symbols, (CHashHashFunc *)UArray_evenHash);
    CHash_setHash2Func_(self->symbols, (CHashHashFunc *)UArray_oddHash);

    /*
    Problem:
    - there are some interdependencies here:
    - creating instances requires a retain stack
    - we need a Coroutine to use for our retainStack
    - defining any primitive methods requires Strings and CFunctions

    Solution:
    - create a temporary fake stack
    - create Object, CFunction and String protos sans methods.
    - then add methods to Object, CFunction and String
    */

    self->currentIoStack = Stack_new(); // temp retain stack until coro is up

    self->objectProto = IoObject_proto(
        self); // need to do this first, so we have a retain stack
    // IoState_retain_(self, self->objectProto);

    self->mainCoroutine = IoCoroutine_proto(self);
    Stack_free(self->currentIoStack);
    self->currentIoStack = NULL;

    IoState_setCurrentCoroutine_(self, self->mainCoroutine);

    seqProto = IoSeq_proto(self);

    IoState_setupQuickAccessSymbols(self);

    IoObject_rawSetProto_(seqProto, self->objectProto);

    cFunctionProto = IoCFunction_proto(self);
    self->localsUpdateSlotCFunc = IoState_retain_(
        self, IoCFunction_newWithFunctionPointer_tag_name_(
                  self, IoObject_localsUpdateSlot, NULL, "localsUpdate"));

    IoSeq_protoFinish(seqProto);
    IoObject_protoFinish(self);
    IoCFunction_protoFinish(self);
    IoCoroutine_protoFinish(self->mainCoroutine);

    self->setSlotBlock = IoState_retain_(
        self, IoObject_getSlot_(self->objectProto, SIOSYMBOL("setSlot")));

    // setup lobby

    {
        IoObject *objectProto = self->objectProto;
        IoObject *protos = IOCLONE(objectProto);
        IoObject *core = IOCLONE(objectProto);

        self->core = core;
        self->lobby = IOCLONE(objectProto);
        IoState_retain_(self, self->lobby);
        IoState_retain_(self, self->core);

        // setup namespace

        IoObject_setSlot_to_(self->lobby, SIOSYMBOL("Lobby"), self->lobby);
        IoObject_setSlot_to_(self->lobby, SIOSYMBOL("Protos"), protos);
        IoObject_setSlot_to_(protos, SIOSYMBOL("Core"), core);
        IoObject_setSlot_to_(protos, SIOSYMBOL("Addons"), IOCLONE(objectProto));

        IoObject_setSlot_to_(core, SIOSYMBOL("Compiler"),
                             IoCompiler_proto(self));
        IoObject_setSlot_to_(core, SIOSYMBOL("Collector"),
                             IoCollector_proto(self));
        IoObject_setSlot_to_(core, SIOSYMBOL("Exception"),
                             IOCLONE(objectProto));

        // setup proto chain

        IoObject_rawSetProto_(objectProto, self->lobby);
        IoObject_rawSetProto_(self->lobby, protos);
        IoObject_rawSetProto_(protos, core);

        // add protos to namespace

        IoObject_setSlot_to_(core, SIOSYMBOL("Object"), objectProto);
        IoObject_setSlot_to_(core, SIOSYMBOL("Sequence"), seqProto);
        IoObject_setSlot_to_(core, SIOSYMBOL("Number"), IoNumber_proto(self));

        IoState_setupCachedNumbers(self);

        {
            IoObject *systemProto = IoSystem_proto(self);
            IoObject_setSlot_to_(core, SIOSYMBOL("System"), systemProto);
        }

        IoState_setupSingletons(self);
        IoState_setupCachedMessages(self);

        {
            self->debugger = IoState_retain_(self, IoDebugger_proto(self));
            IoObject_setSlot_to_(core, SIOSYMBOL("Debugger"), self->debugger);

            self->vmWillSendMessage =
                IoMessage_newWithName_(self, SIOSYMBOL("vmWillSendMessage"));
            IoMessage_rawSetCachedResult_(self->nilMessage, self->ioNil);
            IoState_retain_(self, self->vmWillSendMessage);
        }

        IoObject_setSlot_to_(core, SIOSYMBOL("Block"), IoBlock_proto(self));
        IoObject_setSlot_to_(core, SIOSYMBOL("List"), IoList_proto(self));
        IoObject_setSlot_to_(core, SIOSYMBOL("Map"), IoMap_proto(self));
        // IoObject_setSlot_to_(core, SIOSYMBOL("Range"), IoRange_proto(self));
        IoObject_setSlot_to_(core, SIOSYMBOL("Coroutine"), self->mainCoroutine);
#ifdef IO_CALLCC
        IoObject_setSlot_to_(core, SIOSYMBOL("Continuation"),
                             IoContinuation_proto(self));
#endif
        IoObject_setSlot_to_(core, SIOSYMBOL("EvalFrame"),
                             IoEvalFrame_proto(self));
        IoObject_setSlot_to_(core, SIOSYMBOL("Error"), IoError_proto(self));
        IoObject_setSlot_to_(core, SIOSYMBOL("File"), IoFile_proto(self));
        IoObject_setSlot_to_(core, SIOSYMBOL("Directory"),
                             IoDirectory_proto(self));
        IoObject_setSlot_to_(core, SIOSYMBOL("Date"), IoDate_proto(self));
        IoObject_setSlot_to_(core, SIOSYMBOL("Duration"),
                             IoDuration_proto(self));
        IoObject_setSlot_to_(core, SIOSYMBOL("WeakLink"),
                             IoWeakLink_proto(self));
        IoObject_setSlot_to_(core, SIOSYMBOL("Sandbox"), IoSandbox_proto(self));
        // IoObject_setSlot_to_(core, SIOSYMBOL("EditLine"),
        // IoEditLine_proto(self));


        // self->store =
        // IoObject_setSlot_to_(core, SIOSYMBOL("Store"),      self->store);
        IoObject_setSlot_to_(core, SIOSYMBOL("CFunction"), cFunctionProto);
        IoObject_setSlot_to_(core, SIOSYMBOL("Profiler"),
                             IoProfiler_proto(self));

        self->localsProto = IoState_retain_(self, IoObject_localsProto(self));
        IoObject_setSlot_to_(core, SIOSYMBOL("Locals"), self->localsProto);

        self->stopStatus = MESSAGE_STOP_STATUS_NORMAL;
        self->returnValue = self->ioNil;

        // Initialize iterative evaluation frame stack
        self->currentFrame = NULL;
        self->frameDepth = 0;
        self->maxFrameDepth = 10000;  // Default max depth
        self->framePoolCount = 0;
        memset(self->framePool, 0, sizeof(self->framePool));
        self->needsControlFlowHandling = 0;
        self->awaitingJsPromise = 0;
        self->suspendedCoro = NULL;
#ifdef IO_CALLCC
        self->continuationInvoked = 0;
#endif
        self->nestedEvalDepth = 0;
        self->errorRaised = 0;
        self->slotVersion = 0;

        IoState_clearRetainStack(self);

        IoState_popCollectorPause(self);

        // Collector_collect(self->collector);

        // io_show_mem("before IoVMCodeInit");
        IoVMCodeInit(core);

        // io_show_mem("after IoVMCodeInit");
        // Collector_collect(self->collector);
        // io_show_mem("after Collector_collect");

        // IoState_popCollectorPause(self);
        IoState_clearRetainStack(self);

        // Collector_check(self->collector);
        Collector_collect(self->collector);
        // io_show_mem("after IoState_clearRetainStack and Collector_collect");
        IoState_setupUserInterruptHandler(self);
        IoState_markLazyArgsCFunctions_(self);
    }
}

/*cdoc State IoState_new(void)
Heap-allocates an IoState and delegates to IoState_new_atAddress.
The common entry point for embedders that do not care where the VM
lives in memory.
*/
IoState *IoState_new(void) {
    IoState *self = (IoState *)io_calloc(1, sizeof(IoState));
    IoState_new_atAddress(self);
    return self;
}

/*cdoc State IoState_retainedSymbol(self, s)
Interns a C string as a Symbol and pins it against GC. Used during init
to cache hot-path names (self, call, type, …) that the VM dereferences
in inner loops and must never be collected.
*/
IoSymbol *IoState_retainedSymbol(IoState *self, char *s) {
    return IoState_retain_(self, SIOSYMBOL(s));
}

/*cdoc State IoState_setupQuickAccessSymbols(self)
Populates the pre-interned symbol fields on the IoState struct so the
evaluator and CFunction machinery can compare against cached IoSymbol
pointers instead of re-hashing strings on every call. Called early
during init, before any Io code runs.
*/
void IoState_setupQuickAccessSymbols(IoState *self) {
    self->activateSymbol = IoState_retainedSymbol(self, "activate");
    self->callSymbol = IoState_retainedSymbol(self, "call");
    self->forwardSymbol = IoState_retainedSymbol(self, "forward");
    self->noShufflingSymbol = IoState_retainedSymbol(self, "__noShuffling__");
    self->opShuffleSymbol = IoState_retainedSymbol(self, "opShuffle");
    // self->performSymbol    = IoState_retainedSymbol(self, "perform");
    // self->referenceIdSymbol    = IoState_retainedSymbol(self, "referenceId");
    self->semicolonSymbol = IoState_retainedSymbol(self, ";");
    self->selfSymbol = IoState_retainedSymbol(self, "self");
    self->setSlotSymbol = IoState_retainedSymbol(self, "setSlot");
    self->setSlotWithTypeSymbol =
        IoState_retainedSymbol(self, "setSlotWithType");
    self->stackSizeSymbol = IoState_retainedSymbol(self, "stackSize");
    self->typeSymbol = IoState_retainedSymbol(self, "type");
    self->updateSlotSymbol = IoState_retainedSymbol(self, "updateSlot");



    self->runTargetSymbol = IoState_retainedSymbol(self, "runTarget");
    self->runMessageSymbol = IoState_retainedSymbol(self, "runMessage");
    self->runLocalsSymbol = IoState_retainedSymbol(self, "runLocals");
    self->parentCoroutineSymbol =
        IoState_retainedSymbol(self, "parentCoroutine");
    self->resultSymbol = IoState_retainedSymbol(self, "result");
    self->exceptionSymbol = IoState_retainedSymbol(self, "exception");
}

/*cdoc State IoState_setupSingletons(self)
Creates the shared singletons nil/true/false and the flow-control
markers Normal/Break/Continue/Return/Eol, registers them under
state->core, and retains each one so they survive GC. Also caches
the Message proto, Call proto and Call tag for the inline allocation
fast paths in the iterative evaluator. The flow-control markers are
compared by pointer identity in IoObject_flow.c to drive break/
continue/return semantics.
*/
void IoState_setupSingletons(IoState *self) {
    IoObject *core = self->core;
    // nil

    self->ioNil = IOCLONE(self->objectProto);
    IoObject_setSlot_to_(core, SIOSYMBOL("nil"), self->ioNil);
    // IoObject_setSlot_to_(core, self->noShufflingSymbol, self->ioNil);
    IoObject_setSlot_to_(core, SIOSYMBOL("Message"), IoMessage_proto(self));
    IoObject_setSlot_to_(core, SIOSYMBOL("Call"), IoCall_proto(self));

    // Cache Call tag/proto for inline allocation in block activation
    self->callProto = IoState_protoWithId_(self, "Call");
    self->callTag = IoObject_tag(self->callProto);
    self->blockLocalsPoolSize = 0;
    self->callPoolSize = 0;

    self->nilMessage = IoMessage_newWithName_(self, SIOSYMBOL("nil"));
    IoMessage_rawSetCachedResult_(self->nilMessage, self->ioNil);
    IoState_retain_(self, self->nilMessage);

    // true

    self->ioTrue = IoObject_new(self);
    IoObject_setSlot_to_(core, SIOSYMBOL("true"), self->ioTrue);
    IoObject_setSlot_to_(self->ioTrue, SIOSYMBOL("type"), SIOSYMBOL("true"));
    IoState_retain_(self, self->ioTrue);

    // false

    self->ioFalse = IoObject_new(self);
    IoObject_setSlot_to_(core, SIOSYMBOL("false"), self->ioFalse);
    IoObject_setSlot_to_(self->ioFalse, SIOSYMBOL("type"), SIOSYMBOL("false"));
    IoState_retain_(self, self->ioFalse);

    // Flow control: Normal
    self->ioNormal = IoObject_new(self);
    IoObject_setSlot_to_(core, SIOSYMBOL("Normal"), self->ioNormal);
    IoObject_setSlot_to_(self->ioNormal, SIOSYMBOL("type"),
                         SIOSYMBOL("Normal"));
    IoState_retain_(self, self->ioNormal);

    // Flow control: Break
    self->ioBreak = IoObject_new(self);
    IoObject_setSlot_to_(core, SIOSYMBOL("Break"), self->ioBreak);
    IoObject_setSlot_to_(self->ioBreak, SIOSYMBOL("type"), SIOSYMBOL("Break"));
    IoState_retain_(self, self->ioBreak);

    // Flow control: Continue
    self->ioContinue = IoObject_new(self);
    IoObject_setSlot_to_(core, SIOSYMBOL("Continue"), self->ioContinue);
    IoObject_setSlot_to_(self->ioContinue, SIOSYMBOL("type"),
                         SIOSYMBOL("Continue"));
    IoState_retain_(self, self->ioContinue);

    // Flow control: Return
    self->ioReturn = IoObject_new(self);
    IoObject_setSlot_to_(core, SIOSYMBOL("Return"), self->ioReturn);
    IoObject_setSlot_to_(self->ioReturn, SIOSYMBOL("type"),
                         SIOSYMBOL("Return"));
    IoState_retain_(self, self->ioReturn);

    // Flow control: Eol
    self->ioEol = IoObject_new(self);
    IoObject_setSlot_to_(core, SIOSYMBOL("Eol"), self->ioEol);
    IoObject_setSlot_to_(self->ioEol, SIOSYMBOL("type"), SIOSYMBOL("Eol"));
    IoState_retain_(self, self->ioEol);
}

/*cdoc State IoState_setupCachedMessages(self)
Pre-builds and retains a set of Message objects the VM sends
frequently (asString, compare, init, main, opShuffle, print, run,
yield, …). Cached messages skip the per-send parse/construct cost and
give IoMessage_locals_performOn_ a stable pointer to hand to the
iterative evaluator.
*/
void IoState_setupCachedMessages(IoState *self) {
    self->asStringMessage = IoMessage_newWithName_(self, SIOSYMBOL("asString"));
    IoState_retain_(self, self->asStringMessage);

    self->collectedLinkMessage =
        IoMessage_newWithName_(self, SIOSYMBOL("collectedLink"));
    IoState_retain_(self, self->collectedLinkMessage);

    self->compareMessage = IoMessage_newWithName_(self, SIOSYMBOL("compare"));
    IoState_retain_(self, self->compareMessage);

    // self->doStringMessage = IoMessage_newWithName_(self,
    // SIOSYMBOL("doString")); IoState_retain_(self, self->doStringMessage);

    self->initMessage = IoMessage_newWithName_(self, SIOSYMBOL("init"));
    IoState_retain_(self, self->initMessage);

    self->mainMessage = IoMessage_newWithName_(self, SIOSYMBOL("main"));
    IoState_retain_(self, self->mainMessage);

    self->opShuffleMessage =
        IoMessage_newWithName_(self, self->opShuffleSymbol);
    IoState_retain_(self, self->opShuffleMessage);

    self->printMessage = IoMessage_newWithName_(self, SIOSYMBOL("print"));
    IoState_retain_(self, self->printMessage);

    self->referenceIdForObjectMessage =
        IoMessage_newWithName_(self, SIOSYMBOL("referenceIdForObject"));
    IoState_retain_(self, self->referenceIdForObjectMessage);

    self->objectForReferenceIdMessage =
        IoMessage_newWithName_(self, SIOSYMBOL("objectForReferenceId"));
    IoState_retain_(self, self->objectForReferenceIdMessage);

    self->runMessage = IoMessage_newWithName_(self, SIOSYMBOL("run"));
    IoState_retain_(self, self->runMessage);

    self->willFreeMessage = IoMessage_newWithName_(self, SIOSYMBOL("willFree"));
    IoState_retain_(self, self->willFreeMessage);

    self->yieldMessage = IoMessage_newWithName_(self, SIOSYMBOL("yield"));
    IoState_retain_(self, self->yieldMessage);

    self->didFinishMessage =
        IoMessage_newWithName_(self, SIOSYMBOL("didFinish"));
    IoState_retain_(self, self->didFinishMessage);

    self->asBooleanMessage =
        IoMessage_newWithName_(self, SIOSYMBOL("asBoolean"));
    IoState_retain_(self, self->asBooleanMessage);
}

IO_METHOD(IoObject, initBindings) {
    IOSTATE->bindingsInitCallback(IOSTATE, self);
    return self;
}

/*cdoc State IoState_init(self)
Second-stage init hook: invokes the embedder-supplied bindings
callback (set via IoState_setBindingsInitCallback) so additional
native protos can be installed after the core bootstrap. Wraps the
callback in a collector pause and clears the retain stack afterwards
so any objects the callback created are not held past the call.
*/
void IoState_init(IoState *self) {
    if (self->bindingsInitCallback) {
        IoState_pushCollectorPause(self);
        self->bindingsInitCallback(self, self->core);
        IoState_popCollectorPause(self);
        IoState_clearRetainStack(self);
    }
}

/*
IoObject_registerAsProto(IoObject *self)
{
        IoState_registerProtoWithNamed_(IOSTATE, self, IoObject_name(self));
}

void IoState_registerProtoWithNamed_(IoState *self, IoObject *proto, const char
*name)
{
        if (PointerHash_at_(self->primitives, (void *)name))
        {
                printf("Error registering proto: %s\n", IoObject_name(proto));
                IoState_fatalError_(self, "IoState_registerProtoWithFunc_()
Error: attempt to add the same proto twice");
        }

        IoState_retain_(self, proto);
        PointerHash_at_put_(self->primitives, (void *)func, proto);
        //printf("registered %s\n", IoObject_name(proto));
}
*/

/*cdoc State IoState_registerProtoWithFunc_(self, proto, v)
Compatibility alias that forwards to IoState_registerProtoWithId_.
Older code in the Io ecosystem passed a function pointer as the key;
both forms now use the same PointerHash lookup.
*/
void IoState_registerProtoWithFunc_(IoState *self, IoObject *proto,
                                    const char *v) {
    IoState_registerProtoWithId_(self, proto, v);
}

/*cdoc State IoState_registerProtoWithId_(self, proto, v)
Records a proto in state->primitives keyed by a string id, and retains
it so the GC cannot collect it even when nothing else holds a reference.
Aborts via IoState_fatalError_ if the id is already registered: proto
ids are global singletons and double-registration usually signals a
mis-ordered bootstrap.
*/
IOVM_API void IoState_registerProtoWithId_(IoState *self, IoObject *proto,
                                           const char *v) {
    if (PointerHash_at_(self->primitives, (void *)v)) {
        printf("Error registering proto: %s\n", IoObject_name(proto));
        IoState_fatalError_(self, "IoState_registerProtoWithFunc_() Error: "
                                  "attempt to add the same proto twice");
    }

    IoState_retain_(self, proto);
    PointerHash_at_put_(self->primitives, (void *)v, proto);
    // printf("registered %s\n", IoObject_name(proto));
}

/*cdoc State IoState_protoWithName_(self, name)
Linear scan over registered protos matching IoObject_name. Unlike
IoState_protoWithId_ this looks at the runtime name slot rather than
the id key, so it can find protos registered under a different id
than their display name. Returns NULL when nothing matches.
*/
IoObject *IoState_protoWithName_(IoState *self, const char *name) {
    POINTERHASH_FOREACH(
        self->primitives, key, proto,
        if (!strcmp(IoObject_name(proto), name)) { return proto; });
    return NULL;
}

/*cdoc State IoState_tagList(self)
Returns a freshly allocated List of every registered proto's tag —
used by IoState_done to free the IoTag structures after the collector
has released the protos themselves. Caller owns the returned List and
must List_free it.
*/
List *IoState_tagList(IoState *self) // caller must io_free returned List
{
    List *tags = List_new();
    POINTERHASH_FOREACH(self->primitives, k, proto,
                        List_append_(tags, IoObject_tag((IoObject *)proto)));
    return tags;
}

/*cdoc State IoState_done(self)
Tears down a VM: snapshot the tag list first (IoObject_free does not
release tags), force the collector to free everything, then free the
tags, primitives hash, symbol table, recycled-object and cached-number
lists, random generator, and argv. Must be called from the main
coroutine with no live Io code running.
*/
void IoState_done(IoState *self) {
    // this should only be called from the main coro from outside of Io

    List *tags = IoState_tagList(
        self); // need to get the tags before we io_free the protos

    self->maxRecycledObjects = 0;

    Collector_freeAllValues(
        self->collector); // io_free all objects known to the collector
    Collector_free(self->collector);

    List_do_(tags, (ListDoCallback *)IoTag_free);
    List_free(tags);

    PointerHash_free(self->primitives);
    CHash_free(self->symbols);

    LIST_DO_(self->recycledObjects,
             IoObject_dealloc); // this does not work now that objects and marks
                                // are separate
    List_free(self->recycledObjects);
    List_free(self->cachedNumbers);

    RandomGen_free(self->randomGen);
    MainArgs_free(self->mainArgs);
}

/*cdoc State IoState_free(self)
Calls IoState_done and frees the IoState struct itself. Only safe to
call on a state that came from IoState_new (heap-allocated); in-place
states constructed via IoState_new_atAddress should call IoState_done
directly.
*/
void IoState_free(IoState *self) {
    IoState_done(self);
    io_free(self);
}

IoObject *IoState_lobby(IoState *self) { return self->lobby; }

void IoState_setLobby_(IoState *self, IoObject *obj) { self->lobby = obj; }

void MissingProtoError(void) { printf("missing proto\n"); }

/*cdoc State IoState_protoWithId_(self, v)
The standard proto lookup used throughout the VM's C source. Missing
ids are fatal — if a proto is missing at this point either a binding
forgot to register itself or code is running before the init sequence
got that far.
*/
IoObject *IoState_protoWithId_(IoState *self, const char *v) {
    IoObject *proto = PointerHash_at_(self->primitives, (void *)v);

    // printf("IoState_protoWithId_(self, %s)\n", v);

    if (!proto) {
        printf("IoState fatal error: missing proto '%s'", v);
        IoState_fatalError_(
            self, "IoState_protoWithId_() Error: missing proto with id");
    }

    return proto;
}

// command line ------------------------------------------------

/*cdoc State IoState_argc_argv_(self, argc, argv)
Exposes the command line to Io code via System args (skipping argv[0])
and stashes the raw argv in the MainArgs helper for re-retrieval. Called
by the REPL entry point before running user code.
*/
void IoState_argc_argv_(IoState *self, int argc, const char *argv[]) {
    IoList *args = IoList_new(self);
    int i;

    for (i = 1; i < argc; i++) {
        IoList_rawAppend_(args, SIOSYMBOL(argv[i]));
    }

    {
        IoObject *system = IoObject_getSlot_(self->lobby, SIOSYMBOL("System"));
        IoObject_setSlot_to_(system, SIOSYMBOL("args"), args);
    }

    MainArgs_argc_argv_(self->mainArgs, argc, argv);
}

// store -------------------------------------------------------

IoObject *IoState_objectWithPid_(IoState *self, PID_TYPE pid) {
    return self->ioNil;
}

// doString -------------------------------------------------------

/*cdoc State IoState_rawOn_doCString_withLabel_(self, target, s, label)
Parses a string as Io source and performs the resulting message on
target with no try/catch wrapper. Unlike IoState_on_doCString_withLabel_
this path does not guard the evaluation inside a try coroutine, so
uncaught exceptions propagate to whichever caller owns the current
coroutine. Used by the interactive prompt where the shell itself
displays errors.
*/
IoObject *IoState_rawOn_doCString_withLabel_(IoState *self, IoObject *target,
                                             const char *s, const char *label) {
    IoMessage *m = IoMessage_newFromText_label_(self, s, label);
    return IoMessage_locals_performOn_(m, target, target);
}

// CLI ---------------------------------------------------------

/*cdoc State IoState_rawPrompt(self)
Minimal built-in REPL: reads lines from stdin, evaluates each via
IoState_rawOn_doCString_withLabel_ and prints the result. Provided as
a fallback when the Io-side CLI.io loop isn't available; the normal
REPL entry point is IoState_runCLI.
*/
void IoState_rawPrompt(IoState *self) {
    int max = 1024 * 16;
    char *s = io_calloc(1, max);
    IoObject *result;

    for (;;) {
        fputs("Io> ", stdout);
        fflush(stdout);
        fgets(s, max, stdin);

        if (feof(stdin)) {
            break;
        }

        result = IoState_rawOn_doCString_withLabel_(self, self->lobby, s,
                                                    "IoState_rawPrompt()");

        fputs("==> ", stdout);
        IoObject_print(result);
        fputs("\n", stdout);
    }

    io_free(s);
}

/*cdoc State IoState_runCLI(self)
Launches the Io-implemented command-line loop by doing "CLI run" on
the lobby. If that run leaves an exception on the current coroutine,
records it in state->exitResult so the process exits non-zero.
*/
void IoState_runCLI(IoState *self) {
    IoObject *result = IoState_on_doCString_withLabel_(
        self, self->lobby, "CLI run", "IoState_runCLI()");
    IoObject *e = IoCoroutine_rawException(self->currentCoroutine);

    if (e != self->ioNil) {
        self->exitResult = -1;
    }
}

IOVM_API int IoState_exitResult(IoState *self) { return self->exitResult; }
