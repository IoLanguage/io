
// metadoc EvalFrame copyright Steve Dekorte 2002, 2025
// metadoc EvalFrame license BSD revised

#ifndef IOEVALFRAME_DEFINED
#define IOEVALFRAME_DEFINED 1

#include "Common.h"
#include "IoObject.h"
#include "IoMessage.h"
#include "IoCall.h"

#ifdef __cplusplus
extern "C" {
#endif

// IoEvalFrame is a GC-managed IoObject.
// Frames are allocated by the collector and freed by GC when unreferenced.
typedef IoObject IoEvalFrame;

// Frame states for the evaluation state machine
typedef enum {
    FRAME_STATE_START,           // Just pushed, need to begin evaluation
    FRAME_STATE_EVAL_ARGS,       // Evaluating arguments lazily
    FRAME_STATE_LOOKUP_SLOT,     // Ready to do slot lookup
    FRAME_STATE_ACTIVATE,        // Activating a block/method
    FRAME_STATE_CONTINUE_CHAIN,  // Continue to next message in chain
    FRAME_STATE_RETURN,          // Returning result to parent

    // Control flow states (for non-reentrant primitives)
    FRAME_STATE_IF_EVAL_CONDITION,    // Evaluating 'if' condition
    FRAME_STATE_IF_CONVERT_BOOLEAN,   // Converting condition result to boolean
    FRAME_STATE_IF_EVAL_BRANCH,       // Evaluating 'if' true/false branch

    // While loop states
    FRAME_STATE_WHILE_EVAL_CONDITION, // Evaluating 'while' condition
    FRAME_STATE_WHILE_CHECK_CONDITION,// Converting condition to boolean
    FRAME_STATE_WHILE_DECIDE,         // Deciding whether to continue or exit
    FRAME_STATE_WHILE_EVAL_BODY,      // Evaluating 'while' body

    // Loop (infinite) states
    FRAME_STATE_LOOP_EVAL_BODY,       // Starting 'loop' body
    FRAME_STATE_LOOP_AFTER_BODY,      // After 'loop' body, check break/continue

    // For loop states
    FRAME_STATE_FOR_EVAL_SETUP,       // Evaluating 'for' setup (range/collection)
    FRAME_STATE_FOR_EVAL_BODY,        // Starting 'for' body
    FRAME_STATE_FOR_AFTER_BODY,       // After 'for' body, increment and check

    // Foreach states (collection iteration)
    FRAME_STATE_FOREACH_EVAL_BODY,    // Set slots and push body frame
    FRAME_STATE_FOREACH_AFTER_BODY,   // After body, increment and check

    // Callcc states
    FRAME_STATE_CALLCC_EVAL_BLOCK,    // Evaluating callcc block body

    // Coroutine states
    FRAME_STATE_CORO_WAIT_CHILD,      // Waiting for child coroutine to complete
    FRAME_STATE_CORO_YIELDED,         // Yielded, will resume here when switched back

    // doString/doMessage/doFile states
    FRAME_STATE_DO_EVAL,              // Ready to evaluate compiled code
    FRAME_STATE_DO_WAIT               // Waiting for evaluation result
} IoFrameState;

// Frame data payload — stored in IoObject's dataPointer
typedef struct IoEvalFrameData {
    // Message chain being evaluated
    IoMessage *message;          // Current message in chain

    // Evaluation context
    IoObject *target;            // Current target (self for this message)
    IoObject *locals;            // Current locals (sender context)
    IoObject *cachedTarget;      // Original target (for semicolon reset)

    // Parent frame (for returning) — also an IoEvalFrame (IoObject)
    IoEvalFrame *parent;

    // Current state in the evaluation state machine
    IoFrameState state;

    // Argument evaluation state (for lazy evaluation)
    int argCount;                // Total number of args
    int currentArgIndex;         // Which arg we're currently evaluating
    IoObject **argValues;        // Evaluated argument values (array)

    // Result
    IoObject *result;            // Result of this frame

    // For slot lookup and activation
    IoObject *slotValue;         // Result of slot lookup
    IoObject *slotContext;       // Context where slot was found

    // Call object (for introspection - created during block activation)
    IoCall *call;                // Call object for this activation (or NULL)
    IoCall *savedCall;           // Original Call preserved across in-place if optimization

    // For block activation
    IoObject *blockLocals;       // Block's local context (NULL if not a block)
    int passStops;               // Whether to pass stop status (break/continue/return)
    int isNestedEvalRoot;        // True if this is a nested eval boundary
    uintptr_t retainPoolMark;    // Retain pool mark for block activation (0 if none)

    // Control flow continuation info (only one active at a time)
    union {
        struct {
            IoMessage *conditionMsg;      // Condition message to evaluate
            IoMessage *trueBranch;        // True branch message
            IoMessage *falseBranch;       // False branch message (may be NULL)
            int conditionResult;          // Result of condition evaluation
        } ifInfo;

        struct {
            IoMessage *conditionMsg;      // Condition message to evaluate
            IoMessage *bodyMsg;           // Body message to execute
            IoObject *lastResult;         // Last body result (for return value)
            int conditionResult;          // Result of condition evaluation
        } whileInfo;

        struct {
            IoMessage *bodyMsg;           // Body message to execute
            IoObject *lastResult;         // Last body result (for break value)
        } loopInfo;

        struct {
            IoMessage *bodyMsg;           // Body message to execute
            IoSymbol *counterName;        // Name of counter variable
            double startValue;            // Start value
            double endValue;              // End value
            double increment;             // Increment value
            double currentValue;          // Current counter value
            IoObject *lastResult;         // Last body result
            int initialized;              // Whether loop has started
        } forInfo;

        struct {
            IoObject *collection;         // The collection object (List/Map/Seq)
            IoMessage *bodyMsg;           // Body message to execute
            IoSymbol *indexName;          // Index variable name (NULL if not used)
            IoSymbol *valueName;          // Value variable name (NULL for each)
            int currentIndex;             // Current iteration index
            int collectionSize;           // Total size (may change if list mutated)
            IoObject *lastResult;         // Last body result
            int direction;                // 1 for forward, -1 for reverse
            int isEach;                   // 1 if "each" (send to element as target)
            IoObject *mapSource;          // Original Map (when collection is a keys list)
        } foreachInfo;

        struct {
            IoObject *continuation;       // The Continuation object
            IoObject *blockLocals;        // Block's local context
        } callccInfo;

        struct {
            IoObject *childCoroutine;     // Child coroutine we're waiting for
        } coroInfo;

        struct {
            IoMessage *codeMessage;       // The parsed/compiled message to evaluate
            IoObject *evalTarget;         // Target for evaluation (self)
            IoObject *evalLocals;         // Locals for evaluation (context)
        } doInfo;
    } controlFlow;
} IoEvalFrameData;

// Access the frame's data payload via IoObject's dataPointer
#define FRAME_DATA(frame) ((IoEvalFrameData *)IoObject_dataPointer(frame))

// Fast path: check if a body message is a cached literal with no chain
// (e.g., nil, a number, or a string). These can be resolved without
// pushing a child frame — just use the cached result directly.
#define BODY_IS_CACHED_LITERAL(bodyMsg) \
    (IOMESSAGEDATA(bodyMsg)->cachedResult && !IOMESSAGEDATA(bodyMsg)->next)

#define CACHED_LITERAL_RESULT(bodyMsg) \
    (IOMESSAGEDATA(bodyMsg)->cachedResult)

// Type check macro
#define ISEVALFRAME(self) \
    IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoEvalFrame_rawClone)

// Proto and lifecycle
IOVM_API IoEvalFrame *IoEvalFrame_proto(void *state);
IOVM_API IoEvalFrame *IoEvalFrame_rawClone(IoEvalFrame *proto);
IOVM_API IoEvalFrame *IoEvalFrame_newWithState(void *state);
IOVM_API void IoEvalFrame_free(IoEvalFrame *self);
IOVM_API void IoEvalFrame_mark(IoEvalFrame *self);
IOVM_API void IoEvalFrame_reset(IoEvalFrame *self);
IOVM_API const char *IoEvalFrame_stateName(IoFrameState state);
IOVM_API IoFrameState IoEvalFrame_stateFromName(const char *name);

// Io-visible methods
IOVM_API IO_METHOD(IoEvalFrame, message);
IOVM_API IO_METHOD(IoEvalFrame, target);
IOVM_API IO_METHOD(IoEvalFrame, localContext);
IOVM_API IO_METHOD(IoEvalFrame, state);
IOVM_API IO_METHOD(IoEvalFrame, parent);
IOVM_API IO_METHOD(IoEvalFrame, result);
IOVM_API IO_METHOD(IoEvalFrame, depth);
IOVM_API IO_METHOD(IoEvalFrame, call);
IOVM_API IO_METHOD(IoEvalFrame, blockLocals);
IOVM_API IO_METHOD(IoEvalFrame, description);

#ifdef __cplusplus
}
#endif
#endif
